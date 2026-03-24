#include <string.h>
#include "pico/time.h"
#include "node_core.h"
#include "node_profile.h"

typedef struct {
    uint8_t hi;
    uint8_t lo;
} reg16_stage_t;

static uint8_t regs[256];
static uint16_t update_cnt = 0;
static volatile bool expecting_reg = true;
static volatile uint8_t reg_index = 0;

static reg16_stage_t stg_setpoint0;
static reg16_stage_t stg_setpoint1;
static reg16_stage_t stg_target_time;
static reg16_stage_t stg_hold_time;
static reg16_stage_t stg_limit0;
static reg16_stage_t stg_limit1;

static uint16_t act_setpoint0 = 0;
static uint16_t act_setpoint1 = 0;
static uint16_t act_target_time = 0;
static uint16_t act_hold_time = 0;
static uint16_t act_limit0 = 0;
static uint16_t act_limit1 = 0;
static uint8_t act_cmd_flags_level = 0;
static uint8_t act_op_mode = 0;

static volatile bool estop_latched = false;
static uint8_t estop_low_cnt = 0;
static absolute_time_t estop_boot_ignore_until;
static bool estop_monitor_armed = false;
static uint8_t estop_release_cnt = 0;
static uint8_t g_i2c_addr = I2C_BASE_ADDR;

static const node_app_callbacks_t *g_app = NULL;
static const node_profile_t *g_profile = NULL;

static inline uint8_t wrap_reg(uint8_t r) { return (uint8_t)(r & REG_WINDOW_MASK); }
static inline void reg_advance(void) { reg_index = wrap_reg((uint8_t)(reg_index + 1)); }
static inline uint16_t make_u16(uint8_t hi, uint8_t lo) { return (uint16_t)(((uint16_t)hi << 8) | (uint16_t)lo); }

static void call_app_init(void) { if (g_app && g_app->init) g_app->init(); }
static void call_app_tick_10ms(void) { if (g_app && g_app->tick_10ms) g_app->tick_10ms(); }
static void call_app_on_latch_apply(void) { if (g_app && g_app->on_latch_apply) g_app->on_latch_apply(); }
static bool call_app_on_run_start(void) {
    if (g_app && g_app->on_run_start) {
        return g_app->on_run_start();
    }
    return true;
}
static void call_app_on_run_stop(void) { if (g_app && g_app->on_run_stop) g_app->on_run_stop(); }
static void call_app_on_estop_enter(void) { if (g_app && g_app->on_estop_enter) g_app->on_estop_enter(); }

static inline void bump_update_counter_and_data_ready(void) {
    update_cnt++;
    regs[UPDATE_CNT_H] = (uint8_t)((update_cnt >> 8) & 0xFF);
    regs[UPDATE_CNT_L] = (uint8_t)(update_cnt & 0xFF);
    regs[STATUS] |= STATUS_DATA_READY;
}

static inline void clear_latched_flag(void) {
    regs[COMPLETE_FLAGS] &= (uint8_t)~LATCHED;
}

static inline void commit_staged_16bit(uint8_t reg_h, uint8_t reg_l, reg16_stage_t *stg) {
    bool changed = (regs[reg_h] != stg->hi) || (regs[reg_l] != stg->lo);
    regs[reg_h] = stg->hi;
    regs[reg_l] = stg->lo;
    if (changed) {
        clear_latched_flag();
        bump_update_counter_and_data_ready();
    }
}

static inline bool handle_staged_write(uint8_t cur, uint8_t b) {
    switch (cur) {
        case SETPOINT0_H: stg_setpoint0.hi = b; return true;
        case SETPOINT0_L: stg_setpoint0.lo = b; commit_staged_16bit(SETPOINT0_H, SETPOINT0_L, &stg_setpoint0); return true;
        case SETPOINT1_H: stg_setpoint1.hi = b; return true;
        case SETPOINT1_L: stg_setpoint1.lo = b; commit_staged_16bit(SETPOINT1_H, SETPOINT1_L, &stg_setpoint1); return true;
        case TARGET_TIME_H: stg_target_time.hi = b; return true;
        case TARGET_TIME_L: stg_target_time.lo = b; commit_staged_16bit(TARGET_TIME_H, TARGET_TIME_L, &stg_target_time); return true;
        case HOLD_TIME_H: stg_hold_time.hi = b; return true;
        case HOLD_TIME_L: stg_hold_time.lo = b; commit_staged_16bit(HOLD_TIME_H, HOLD_TIME_L, &stg_hold_time); return true;
        case LIMIT0_H: stg_limit0.hi = b; return true;
        case LIMIT0_L: stg_limit0.lo = b; commit_staged_16bit(LIMIT0_H, LIMIT0_L, &stg_limit0); return true;
        case LIMIT1_H: stg_limit1.hi = b; return true;
        case LIMIT1_L: stg_limit1.lo = b; commit_staged_16bit(LIMIT1_H, LIMIT1_L, &stg_limit1); return true;
        default: return false;
    }
}

static inline void set_last_cmd_result(uint8_t cmd, uint8_t result) {
    regs[LAST_CMD] = cmd;
    regs[LAST_CMD_RESULT] = result;
}

static inline void clear_done_flags(void) {
    regs[COMPLETE_FLAGS] &= (uint8_t)~(DONE | DONE_BY_TIME | DONE_BY_TARGET | DONE_BY_CONDITION | DONE_BY_ABORT);
}

static inline void set_idle_status(void) {
    regs[STATUS] &= (uint8_t)~(STATUS_BUSY | STATUS_ESTOP);
    regs[STATUS] |= STATUS_READY;
}

static inline void set_running_status(void) {
    regs[STATUS] &= (uint8_t)~(STATUS_READY | STATUS_ESTOP);
    regs[STATUS] |= STATUS_BUSY;
}

static inline void set_estopped_status(void) {
    regs[STATUS] &= (uint8_t)~(STATUS_READY | STATUS_BUSY);
    regs[STATUS] |= STATUS_ESTOP;
}

static inline bool is_busy(void) { return (regs[STATUS] & STATUS_BUSY) != 0; }
static inline bool is_estopped(void) { return (regs[STATUS] & STATUS_ESTOP) != 0; }
static inline bool is_latched(void) { return (regs[COMPLETE_FLAGS] & LATCHED) != 0; }

static inline void apply_active_from_regs(void) {
    act_setpoint0 = make_u16(regs[SETPOINT0_H], regs[SETPOINT0_L]);
    act_setpoint1 = make_u16(regs[SETPOINT1_H], regs[SETPOINT1_L]);
    act_target_time = make_u16(regs[TARGET_TIME_H], regs[TARGET_TIME_L]);
    act_hold_time = make_u16(regs[HOLD_TIME_H], regs[HOLD_TIME_L]);
    act_limit0 = make_u16(regs[LIMIT0_H], regs[LIMIT0_L]);
    act_limit1 = make_u16(regs[LIMIT1_H], regs[LIMIT1_L]);
    act_cmd_flags_level = (uint8_t)(regs[CMD_FLAGS] & (CMD_DIR | CMD_MODE | CMD_USE_SOFTLIMIT));
    act_op_mode = regs[OP_MODE];

    (void)act_setpoint1;
    (void)act_target_time;
    (void)act_hold_time;
    (void)act_limit1;
    (void)act_cmd_flags_level;
}

static inline void finish_as_stopped(void) {
    regs[COMPLETE_FLAGS] |= (DONE | DONE_BY_ABORT);
    set_idle_status();
}

static void dip_init_pins(void) {
    const uint dip_pins[4] = {DIP0_PIN, DIP1_PIN, DIP2_PIN, DIP3_PIN};
    for (int i = 0; i < 4; i++) {
        gpio_init(dip_pins[i]);
        gpio_set_dir(dip_pins[i], GPIO_IN);
        gpio_disable_pulls(dip_pins[i]);
    }
}

static uint8_t read_dip_value_once(void) {
    uint8_t b0 = (uint8_t)(gpio_get(DIP0_PIN) ? 1 : 0);
    uint8_t b1 = (uint8_t)(gpio_get(DIP1_PIN) ? 1 : 0);
    uint8_t b2 = (uint8_t)(gpio_get(DIP2_PIN) ? 1 : 0);
    uint8_t b3 = (uint8_t)(gpio_get(DIP3_PIN) ? 1 : 0);
    return (uint8_t)((b3 << 3) | (b2 << 2) | (b1 << 1) | b0);
}

static uint8_t read_dip_value_stable(void) {
    sleep_ms(20);
    uint8_t a = read_dip_value_once();
    sleep_ms(5);
    uint8_t b = read_dip_value_once();
    if (a == b) return a;
    sleep_ms(5);
    uint8_t c = read_dip_value_once();
    if (b == c) return b;
    if (a == c) return a;
    return c;
}

static void init_registers(void) {
    memset(regs, 0, sizeof(regs));
    regs[SIGNATURE0] = 0xC0;
    regs[SIGNATURE1] = 0xDE;
    regs[STATUS] = STATUS_READY;

    memset(&stg_setpoint0, 0, sizeof(stg_setpoint0));
    memset(&stg_setpoint1, 0, sizeof(stg_setpoint1));
    memset(&stg_target_time, 0, sizeof(stg_target_time));
    memset(&stg_hold_time, 0, sizeof(stg_hold_time));
    memset(&stg_limit0, 0, sizeof(stg_limit0));
    memset(&stg_limit1, 0, sizeof(stg_limit1));

    act_setpoint0 = 0;
    act_setpoint1 = 0;
    act_target_time = 0;
    act_hold_time = 0;
    act_limit0 = 0;
    act_limit1 = 0;
    act_cmd_flags_level = 0;
    act_op_mode = 0;
    update_cnt = 0;
    expecting_reg = true;
    reg_index = 0;
    estop_latched = false;
    estop_low_cnt = 0;
    estop_monitor_armed = false;
    estop_release_cnt = 0;

    if (g_profile) {
        regs[MODULE_TYPE] = g_profile->module_type;
        regs[HW_REV]      = g_profile->hw_rev;
        regs[FW_VER]      = g_profile->fw_ver;
        regs[CAP_FLAGS0]  = g_profile->cap_flags0;
        regs[PROFILE_ID]  = (uint8_t)g_profile->profile_id;
        regs[CAP_FLAGS1]  = g_profile->cap_flags1;

        if (g_profile->apply_defaults) {
            g_profile->apply_defaults(regs, REG_WINDOW_SIZE);
        }
    }
}

static inline void estop_init_pins(void) {
    gpio_init(ESTOP_BUS_PIN);
    gpio_set_dir(ESTOP_BUS_PIN, GPIO_IN);
    gpio_pull_up(ESTOP_BUS_PIN);
#if ENABLE_LOCAL_ESTOP
    gpio_init(ESTOP_LOCAL_PIN);
    gpio_set_dir(ESTOP_LOCAL_PIN, GPIO_IN);
    gpio_pull_up(ESTOP_LOCAL_PIN);
#endif
}

static inline bool estop_inputs_released(void) {
    bool bus_ok = (gpio_get(ESTOP_BUS_PIN) == 1);
#if ENABLE_LOCAL_ESTOP
    bool local_ok = (gpio_get(ESTOP_LOCAL_PIN) == 1);
#else
    bool local_ok = true;
#endif
    return bus_ok && local_ok;
}

static inline void estop_init_runtime_state(void) {
    estop_boot_ignore_until = make_timeout_time_ms(100);
    estop_monitor_armed = false;
    estop_release_cnt = 0;
    estop_low_cnt = 0;
    estop_latched = false;
}

static inline void estop_latch(uint8_t source_bits) {
    if (estop_latched) {
        regs[ESTOP_SOURCE] |= source_bits;
        return;
    }
    estop_latched = true;
    regs[ESTOP_SOURCE] |= source_bits;
    set_estopped_status();
    regs[COMPLETE_FLAGS] |= (DONE | DONE_BY_ABORT);
    regs[CMD_FLAGS] &= (uint8_t)~CMD_RUN;
    call_app_on_estop_enter();
    bump_update_counter_and_data_ready();
}

static inline void estop_try_clear(void) {
    if (!estop_latched) return;
    if (!estop_inputs_released()) return;
    estop_latched = false;
    estop_low_cnt = 0;
    regs[ESTOP_SOURCE] = 0;
    set_idle_status();
    estop_monitor_armed = true;
    estop_release_cnt = 0;
}

static inline void poll_estop_and_latch_if_needed(void) {
    if (estop_latched) return;
    if (absolute_time_diff_us(get_absolute_time(), estop_boot_ignore_until) > 0) {
        return;
    }

    bool bus_low = (gpio_get(ESTOP_BUS_PIN) == 0);
#if ENABLE_LOCAL_ESTOP
    bool local_low = (gpio_get(ESTOP_LOCAL_PIN) == 0);
#else
    bool local_low = false;
#endif
    bool any_low = bus_low || local_low;

    if (!estop_monitor_armed) {
        if (!any_low) {
            if (estop_release_cnt < 50) estop_release_cnt++;
        } else {
            estop_release_cnt = 0;
        }
        if (estop_release_cnt >= 20) {
            estop_monitor_armed = true;
            estop_low_cnt = 0;
        }
        return;
    }

    if (any_low) {
        if (estop_low_cnt < 10) estop_low_cnt++;
    } else {
        estop_low_cnt = 0;
    }

    if (estop_low_cnt >= 5) {
        uint8_t src = 0;
        if (bus_low) src |= ESTOP_SRC_BUS;
        if (local_low) src |= ESTOP_SRC_LOCAL;
        estop_latch(src);
    }
}

static inline void handle_cmd_flags_write(uint8_t new_value) {
    uint8_t old_flags = regs[CMD_FLAGS];
    uint8_t level_mask = (CMD_RUN | CMD_DIR | CMD_MODE | CMD_USE_SOFTLIMIT);
    uint8_t oneshot_mask = (CMD_ESTOP_RESET | CMD_LATCH_APPLY);

    uint8_t old_level = (uint8_t)(old_flags & level_mask);
    uint8_t new_level_bits = (uint8_t)(new_value & level_mask);
    uint8_t oneshot_bits = (uint8_t)(new_value & oneshot_mask);
    uint8_t effective_level = (new_level_bits == 0 && oneshot_bits != 0) ? old_level : new_level_bits;

    bool old_run = (old_flags & CMD_RUN) != 0;
    bool new_run = (effective_level & CMD_RUN) != 0;
    bool req_run_rise = (!old_run && new_run);
    bool req_run_fall = (old_run && !new_run);
    bool req_estop_reset = (new_value & CMD_ESTOP_RESET) != 0;
    bool req_latch_apply = (new_value & CMD_LATCH_APPLY) != 0;

    bool handled = false;
    uint8_t result = CMDRES_ACCEPTED_NOOP;
    uint8_t last_cmd = 0x00;

    regs[CMD_FLAGS] &= (uint8_t)~level_mask;
    regs[CMD_FLAGS] |= effective_level;

    if (req_estop_reset) {
        last_cmd = CMD_ESTOP_RESET;
        if (is_estopped() && !is_busy()) {
            if (estop_inputs_released()) {
                estop_try_clear();
                result = CMDRES_OK;
            } else {
                result = CMDRES_REJECT_STATE;
            }
        } else if (is_busy()) {
            result = CMDRES_REJECT_BUSY;
        } else {
            result = CMDRES_REJECT_STATE;
        }
        handled = true;
    }

    if (req_latch_apply) {
        last_cmd = CMD_LATCH_APPLY;
        if (!is_busy() && !is_estopped()) {
            apply_active_from_regs();
            regs[COMPLETE_FLAGS] |= LATCHED;
            call_app_on_latch_apply();
            result = CMDRES_OK;
        } else if (is_busy()) {
            result = CMDRES_REJECT_BUSY;
        } else {
            result = CMDRES_REJECT_ESTOP;
        }
        handled = true;
    }

    if (req_run_rise) {
        last_cmd = CMD_RUN;
        if (is_busy()) {
            result = CMDRES_REJECT_BUSY;
        } else if (is_estopped()) {
            result = CMDRES_REJECT_ESTOP;
        } else if (!is_latched()) {
            result = CMDRES_REJECT_UNLATCHED;
        } else {
            clear_done_flags();

            if (call_app_on_run_start()) {
                set_running_status();
                result = CMDRES_OK;
            } else {
                set_idle_status();
                regs[CMD_FLAGS] &= (uint8_t)~CMD_RUN;
                result = CMDRES_REJECT_STATE;
            }
        }
        handled = true;
    } else if (req_run_fall) {
        last_cmd = CMD_RUN;
        if (is_busy()) {
            call_app_on_run_stop();
            core_finish_stopped();
            result = CMDRES_OK;
        } else {
            result = CMDRES_ACCEPTED_NOOP;
        }
        handled = true;
    }

    regs[CMD_FLAGS] &= (uint8_t)~(CMD_ESTOP_RESET | CMD_LATCH_APPLY);

    if (handled) {
        set_last_cmd_result(last_cmd, result);
        bump_update_counter_and_data_ready();
    }
}

void node_core_i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event) {
    switch (event) {
        case I2C_SLAVE_RECEIVE: {
            uint8_t b = i2c_read_byte_raw(i2c);
            if (expecting_reg) {
                reg_index = wrap_reg(b);
                expecting_reg = false;
            } else {
                uint8_t cur = reg_index;
                if (cur == STATUS) {
                    if (b & STATUS_DATA_READY) {
                        regs[STATUS] &= (uint8_t)~STATUS_DATA_READY;
                    }
                } else if (cur == CMD_FLAGS) {
                    handle_cmd_flags_write(b);
                } else if (handle_staged_write(cur, b)) {
                    // handled
                } else {
                    if (regs[cur] != b) {
                        regs[cur] = b;
                        bump_update_counter_and_data_ready();
                    } else {
                        regs[cur] = b;
                    }
                }
                reg_advance();
            }
            break;
        }
        case I2C_SLAVE_REQUEST:
            i2c_write_byte_raw(i2c, regs[reg_index]);
            reg_advance();
            break;
        case I2C_SLAVE_FINISH:
            expecting_reg = true;
            break;
        default:
            break;
    }
}

void node_core_init(const node_app_callbacks_t *callbacks) {
    g_app = callbacks;
    dip_init_pins();
    uint8_t dip = read_dip_value_stable();
    g_i2c_addr = (uint8_t)(I2C_BASE_ADDR + dip);
    g_profile = node_profile_get();

    init_registers();
    estop_init_pins();
    estop_init_runtime_state();
    call_app_init();

    i2c_init(i2c0, 100 * 1000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
    i2c_slave_init(i2c0, g_i2c_addr, &node_core_i2c_slave_handler);
}

void node_core_poll_1ms(void) {
    poll_estop_and_latch_if_needed();
}

void node_core_tick_10ms(void) {
    call_app_tick_10ms();
}

uint8_t node_core_get_i2c_addr(void) {
    return g_i2c_addr;
}

uint8_t core_get_op_mode(void) { return act_op_mode; }
uint16_t core_get_target_time(void) { return act_target_time; }
uint16_t core_get_setpoint0(void) { return act_setpoint0; }
uint16_t core_get_limit0(void) { return act_limit0; }
uint16_t core_get_limit1(void) { return act_limit1; }
bool core_is_running(void) { return is_busy(); }
bool core_is_estopped(void) { return is_estopped(); }

void core_publish_fb_u16(uint16_t v0, uint16_t v1, uint16_t v2) {
    regs[FB_VALUE0_H] = (uint8_t)((v0 >> 8) & 0xFF);
    regs[FB_VALUE0_L] = (uint8_t)(v0 & 0xFF);
    regs[FB_VALUE1_H] = (uint8_t)((v1 >> 8) & 0xFF);
    regs[FB_VALUE1_L] = (uint8_t)(v1 & 0xFF);
    regs[FB_VALUE2_H] = (uint8_t)((v2 >> 8) & 0xFF);
    regs[FB_VALUE2_L] = (uint8_t)(v2 & 0xFF);
    bump_update_counter_and_data_ready();
}

void core_finish_done_target(void) {
    regs[COMPLETE_FLAGS] |= (DONE | DONE_BY_TARGET);
    set_idle_status();
    regs[CMD_FLAGS] &= (uint8_t)~CMD_RUN;
    bump_update_counter_and_data_ready();
}

void core_finish_stopped(void) {
    finish_as_stopped();
    regs[CMD_FLAGS] &= (uint8_t)~CMD_RUN;
    bump_update_counter_and_data_ready();
}

const node_profile_t *node_core_get_profile(void) {
    return g_profile;
}