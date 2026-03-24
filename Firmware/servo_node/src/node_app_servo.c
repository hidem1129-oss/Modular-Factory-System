#include "node_app.h"
#include "node_profile.h"
#include "servo_hal.h"

typedef struct {
    /* physical / observed state */
    uint16_t cur_deg;
    uint16_t target_deg;
    bool run_active;

    /* resolved / active mode */
    uint8_t op_mode;
    uint8_t prev_op_mode;

    /* cached active config */
    uint16_t cfg_setpoint0;
    uint16_t cfg_target_time;
    uint16_t cfg_limit0;
    uint16_t cfg_limit1;

    /* SPEED plan */
    uint16_t speed_step_deg_per_tick;

    /* TIME plan */
    uint16_t time_total_ticks;
    uint16_t time_elapsed_ticks;
    uint16_t time_start_deg;
    uint16_t time_step_q8;
    uint16_t time_accum_q8;

    bool plan_valid;
} servo_app_state_t;

static servo_app_state_t g_servo;

/* ---------- small helpers ---------- */

static uint16_t servo_abs_diff_u16(uint16_t a, uint16_t b) {
    return (a > b) ? (uint16_t)(a - b) : (uint16_t)(b - a);
}

static uint16_t servo_clamp_target_deg(uint16_t deg) {
    return (deg > 180u) ? 180u : deg;
}

static uint16_t servo_normalize_speed_step(uint16_t step) {
    if (step == 0u) return 1u;
    if (step > 30u) return 30u;
    return step;
}

static uint16_t servo_normalize_target_time(uint16_t ticks) {
    if (ticks == 0u) return 1u;
    return ticks;
}

static uint8_t servo_resolve_mode(uint8_t op_mode) {
    switch (op_mode) {
    case OP_MODE_DEFAULT:
        return OP_MODE_SPEED;
    case OP_MODE_SPEED:
    case OP_MODE_TIME:
        return op_mode;
    default:
        return 0xFFu;
    }
}

static void servo_publish_state(void) {
    uint16_t pulse_us = servo_hal_deg_to_us(g_servo.cur_deg);
    uint16_t error = servo_abs_diff_u16(g_servo.cur_deg, g_servo.target_deg);

    core_publish_fb_u16(g_servo.cur_deg, pulse_us, error);
}

static void servo_stop_output(void) {
    servo_hal_set_enabled(false);
}
static void servo_stop_run(void) {
    g_servo.run_active = false;
    servo_stop_output();
}

static void servo_finish_target(void) {
    servo_stop_run();
    core_finish_done_target();
    servo_publish_state();
}

/* ---------- reset / plan ---------- */

static void servo_reset_mode_state(void) {
    g_servo.speed_step_deg_per_tick = 0u;

    g_servo.time_total_ticks = 0u;
    g_servo.time_elapsed_ticks = 0u;
    g_servo.time_start_deg = g_servo.cur_deg;
    g_servo.time_step_q8 = 0u;
    g_servo.time_accum_q8 = 0u;

    g_servo.plan_valid = false;
}

static void servo_reset_all(void) {
    g_servo.cur_deg = 90u;
    g_servo.target_deg = 90u;
    g_servo.run_active = false;

    g_servo.op_mode = OP_MODE_SPEED;
    g_servo.prev_op_mode = OP_MODE_SPEED;

    /* profile-aligned defaults */
    g_servo.cfg_setpoint0 = 90u;
    g_servo.cfg_target_time = 0u;
    g_servo.cfg_limit0 = 5u;
    g_servo.cfg_limit1 = 0u;

    servo_reset_mode_state();
}

static bool servo_load_active_config(void) {
    uint8_t resolved_mode = servo_resolve_mode(core_get_op_mode());

    if (resolved_mode == 0xFFu) {
        return false;
    }

    g_servo.op_mode = resolved_mode;
    g_servo.cfg_setpoint0 = core_get_setpoint0();
    g_servo.cfg_target_time = core_get_target_time();
    g_servo.cfg_limit0 = core_get_limit0();
    g_servo.cfg_limit1 = core_get_limit1();

    return true;
}

static bool servo_validate_active_config(void) {
    if (g_servo.cfg_setpoint0 > 180u) {
        return false;
    }

    switch (g_servo.op_mode) {
    case OP_MODE_SPEED:
        return true; /* LIMIT0==0 is normalized later */

    case OP_MODE_TIME:
        if (g_servo.cfg_target_time == 0u) {
            return false;
        }
        return true;

    default:
        return false;
    }
}

static void servo_replan_from_active(void) {
    uint16_t delta_deg;

    g_servo.target_deg = servo_clamp_target_deg(g_servo.cfg_setpoint0);

    servo_reset_mode_state();

    switch (g_servo.op_mode) {
    case OP_MODE_SPEED:
        g_servo.speed_step_deg_per_tick =
            servo_normalize_speed_step(g_servo.cfg_limit0);
        g_servo.plan_valid = true;
        break;

    case OP_MODE_TIME:
        g_servo.time_start_deg = g_servo.cur_deg;
        g_servo.time_total_ticks =
            servo_normalize_target_time(g_servo.cfg_target_time);
        g_servo.time_elapsed_ticks = 0u;
        g_servo.time_accum_q8 = 0u;

        delta_deg = servo_abs_diff_u16(g_servo.target_deg, g_servo.time_start_deg);

        if (delta_deg == 0u) {
            g_servo.time_step_q8 = 0u;
        } else {
            g_servo.time_step_q8 =
                (uint16_t)(((uint32_t)delta_deg << 8) /
                           (uint32_t)g_servo.time_total_ticks);

            if (g_servo.time_step_q8 == 0u) {
                g_servo.time_step_q8 = 1u;
            }
        }

        g_servo.plan_valid = true;
        break;

    default:
        g_servo.plan_valid = false;
        break;
    }

    g_servo.prev_op_mode = g_servo.op_mode;
}

static bool servo_apply_active_config(void) {
    if (!servo_load_active_config()) {
        return false;
    }

    if (!servo_validate_active_config()) {
        return false;
    }

    servo_replan_from_active();
    return g_servo.plan_valid;
}

/* ---------- tick handlers ---------- */

static void servo_tick_speed(void) {
    uint16_t diff;
    uint16_t step;

    if (g_servo.cur_deg < g_servo.target_deg) {
        diff = (uint16_t)(g_servo.target_deg - g_servo.cur_deg);
        step = (diff < g_servo.speed_step_deg_per_tick)
             ? diff
             : g_servo.speed_step_deg_per_tick;
        g_servo.cur_deg = (uint16_t)(g_servo.cur_deg + step);
    } else if (g_servo.cur_deg > g_servo.target_deg) {
        diff = (uint16_t)(g_servo.cur_deg - g_servo.target_deg);
        step = (diff < g_servo.speed_step_deg_per_tick)
             ? diff
             : g_servo.speed_step_deg_per_tick;
        g_servo.cur_deg = (uint16_t)(g_servo.cur_deg - step);
    }
}

static void servo_tick_time(void) {
    uint16_t step_deg;

    if (g_servo.cur_deg == g_servo.target_deg) {
        return;
    }

    if (g_servo.time_elapsed_ticks >= g_servo.time_total_ticks) {
        g_servo.cur_deg = g_servo.target_deg;
        return;
    }

    g_servo.time_elapsed_ticks++;
    g_servo.time_accum_q8 =
        (uint16_t)(g_servo.time_accum_q8 + g_servo.time_step_q8);

    step_deg = (uint16_t)(g_servo.time_accum_q8 >> 8);
    g_servo.time_accum_q8 &= 0x00FFu;

    if (step_deg == 0u) {
        step_deg = 1u;
    }

    if (g_servo.target_deg >= g_servo.cur_deg) {
        uint16_t remain = (uint16_t)(g_servo.target_deg - g_servo.cur_deg);
        if (step_deg > remain) {
            step_deg = remain;
        }
        g_servo.cur_deg = (uint16_t)(g_servo.cur_deg + step_deg);
    } else {
        uint16_t remain = (uint16_t)(g_servo.cur_deg - g_servo.target_deg);
        if (step_deg > remain) {
            step_deg = remain;
        }
        g_servo.cur_deg = (uint16_t)(g_servo.cur_deg - step_deg);
    }

    if (g_servo.time_elapsed_ticks >= g_servo.time_total_ticks) {
        g_servo.cur_deg = g_servo.target_deg;
    }
}

/* ---------- app callbacks ---------- */

static void app_init_impl(void) {
    servo_hal_init();
    servo_hal_set_enabled(false);

    servo_reset_all();
    servo_publish_state();
}

static void app_tick_10ms_impl(void) {
    if (!g_servo.run_active) {
        return;
    }

    if (core_is_estopped()) {
        servo_stop_run();
        servo_publish_state();
        return;
    }

    switch (g_servo.op_mode) {
    case OP_MODE_SPEED:
        servo_tick_speed();
        break;

    case OP_MODE_TIME:
        servo_tick_time();
        break;

    default:
        servo_stop_run();
        servo_publish_state();
        return;
    }

    servo_hal_write_deg_u16(g_servo.cur_deg);
    servo_publish_state();

    if (g_servo.cur_deg == g_servo.target_deg) {
        servo_finish_target();
    }
}

static void app_on_latch_apply_impl(void) {
    (void)servo_apply_active_config();
    servo_publish_state();
}

static bool app_on_run_start_impl(void) {
    if (!servo_apply_active_config()) {
        servo_stop_run();
        servo_publish_state();
        return false;
    }
    servo_hal_set_enabled(true);
    servo_hal_write_deg_u16(g_servo.cur_deg);
    g_servo.run_active = true;
    servo_publish_state();
    return true;
}

static void app_on_run_stop_impl(void) {
    servo_stop_run();
    servo_publish_state();
}

static void app_on_estop_enter_impl(void) {
    servo_stop_run();
    servo_publish_state();
}

const node_app_callbacks_t g_node_app_servo_callbacks = {
    .init = app_init_impl,
    .tick_10ms = app_tick_10ms_impl,
    .on_latch_apply = app_on_latch_apply_impl,
    .on_run_start = app_on_run_start_impl,
    .on_run_stop = app_on_run_stop_impl,
    .on_estop_enter = app_on_estop_enter_impl,
};