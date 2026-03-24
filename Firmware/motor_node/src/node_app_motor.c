#include "node_app.h"
#include "node_profile.h"

typedef struct {
    bool run_active;
    uint8_t op_mode;

    /* latched active config */
    uint16_t cfg_setpoint0;
    uint16_t cfg_target_time;
    uint16_t cfg_limit0;
    uint16_t cfg_limit1;

    /* observed / published state */
    uint16_t pos;           /* pseudo integrated position */
    uint16_t output_level;  /* pseudo motor command level */
    uint16_t elapsed_ticks; /* 10ms ticks since RUN start */

    bool plan_valid;
} motor_app_state_t;

static motor_app_state_t g_motor;
static void motor_stop_output(void);
static void motor_stop_run(void);
static void motor_finish_target(void);

static void motor_tick_speed(void);
static void motor_tick_time(void);

/* ---------- helpers ---------- */

static uint8_t motor_resolve_mode(uint8_t op_mode) {
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

static uint16_t motor_normalize_output_level(uint16_t level) {
    if (level > 1000u) {
        return 1000u;
    }
    return level;
}

static uint16_t motor_normalize_target_time(uint16_t ticks) {
    if (ticks == 0u) {
        return 1u;
    }
    return ticks;
}

static uint16_t motor_step_from_output(uint16_t output_level) {
    uint16_t step;

    if (output_level == 0u) {
        return 0u;
    }

    step = (uint16_t)(output_level / 16u);
    if (step == 0u) {
        step = 1u;
    }
    return step;
}

static void motor_publish_state(void) {
    core_publish_fb_u16(
        g_motor.pos,
        g_motor.output_level,
        g_motor.elapsed_ticks
    );
}

/* ---------- reset / plan ---------- */

static void motor_reset_mode_state(void) {
    g_motor.output_level = 0u;
    g_motor.elapsed_ticks = 0u;
    g_motor.plan_valid = false;
}

static void motor_reset_all(void) {
    g_motor.run_active = false;
    g_motor.op_mode = OP_MODE_SPEED;

    /* profile-aligned defaults */
    g_motor.cfg_setpoint0 = 0u;
    g_motor.cfg_target_time = 0u;
    g_motor.cfg_limit0 = 0u;
    g_motor.cfg_limit1 = 0u;

    g_motor.pos = 0u;
    motor_reset_mode_state();
}

static bool motor_load_active_config(void) {
    uint8_t resolved_mode = motor_resolve_mode(core_get_op_mode());

    if (resolved_mode == 0xFFu) {
        return false;
    }

    g_motor.op_mode = resolved_mode;
    g_motor.cfg_setpoint0 = core_get_setpoint0();
    g_motor.cfg_target_time = core_get_target_time();
    g_motor.cfg_limit0 = core_get_limit0(); /* reserved */
    g_motor.cfg_limit1 = core_get_limit1(); /* reserved */

    return true;
}

static bool motor_validate_active_config(void) {
    switch (g_motor.op_mode) {
    case OP_MODE_SPEED:
        return true; /* SETPOINT0==0 is allowed */

    case OP_MODE_TIME:
        if (g_motor.cfg_target_time == 0u) {
            return false;
        }
        return true;

    default:
        return false;
    }
}

static void motor_replan_from_active(void) {
    motor_reset_mode_state();

    g_motor.output_level = motor_normalize_output_level(g_motor.cfg_setpoint0);

    switch (g_motor.op_mode) {
    case OP_MODE_SPEED:
        g_motor.plan_valid = true;
        break;

    case OP_MODE_TIME:
        g_motor.cfg_target_time =
            motor_normalize_target_time(g_motor.cfg_target_time);
        g_motor.plan_valid = true;
        break;

    default:
        g_motor.plan_valid = false;
        break;
    }
}

static bool motor_apply_active_config(void) {
    if (!motor_load_active_config()) {
        return false;
    }

    if (!motor_validate_active_config()) {
        return false;
    }

    motor_replan_from_active();
    return g_motor.plan_valid;
}

/* ---------- app callbacks ---------- */

static void app_init_impl(void) {
    motor_reset_all();
    motor_publish_state();
}

static void app_tick_10ms_impl(void) {
    if (!g_motor.run_active) {
        return;
    }

    if (!core_is_running() || core_is_estopped()) {
        motor_stop_run();
        motor_publish_state();
        return;
    }

    switch (g_motor.op_mode) {
    case OP_MODE_SPEED:
        motor_tick_speed();
        break;

    case OP_MODE_TIME:
        motor_tick_time();
        break;

    default:
        motor_stop_run();
        motor_publish_state();
        break;
    }
}

static void app_on_latch_apply_impl(void) {
    (void)motor_apply_active_config();
    motor_publish_state();
}

static bool app_on_run_start_impl(void) {
    if (!motor_apply_active_config()) {
        motor_stop_run();
        motor_publish_state();
        return false;
    }
    g_motor.elapsed_ticks = 0u;
    g_motor.run_active = true;
    motor_publish_state();
    return true;
}

static void app_on_run_stop_impl(void) {
    motor_stop_run();
    motor_publish_state();
}

static void app_on_estop_enter_impl(void) {
    motor_stop_run();
    motor_publish_state();
}

static void motor_stop_output(void) {
    g_motor.output_level = 0u;
}

static void motor_stop_run(void) {
    g_motor.run_active = false;
    motor_stop_output();
}

static void motor_finish_target(void) {
    motor_stop_run();
    motor_publish_state();
    core_finish_done_target();
}

/* ---------- tick handlers ---------- */

static void motor_tick_speed(void) {
    uint16_t step = motor_step_from_output(g_motor.output_level);

    g_motor.elapsed_ticks++;

    if (step > 0u) {
        g_motor.pos = (uint16_t)(g_motor.pos + step);
    }

    motor_publish_state();
}

static void motor_tick_time(void) {
    uint16_t step = motor_step_from_output(g_motor.output_level);

    g_motor.elapsed_ticks++;

    if (step > 0u) {
        g_motor.pos = (uint16_t)(g_motor.pos + step);
    }

    motor_publish_state();

    if (g_motor.elapsed_ticks >= g_motor.cfg_target_time) {
        motor_finish_target();
    }
}

const node_app_callbacks_t g_node_app_motor_callbacks = {
    .init = app_init_impl,
    .tick_10ms = app_tick_10ms_impl,
    .on_latch_apply = app_on_latch_apply_impl,
    .on_run_start = app_on_run_start_impl,
    .on_run_stop = app_on_run_stop_impl,
    .on_estop_enter = app_on_estop_enter_impl,
};