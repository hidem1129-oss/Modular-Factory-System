#include "node_profile.h"
#include "node_core.h"

/*
 * Motor template mapping
 *
 * OP_MODE
 *   - DEFAULT    : fallback to SPEED
 *   - SPEED      : SETPOINT0
 *   - TIME       : SETPOINT0 + TARGET_TIME
 *
 * Mode support
 *   - CONTINUOUS / ONE_SHOT are unsupported and rejected
 *
 * Parameters
 *   - SETPOINT0   : output level (PWM duty or equivalent)
 *   - TARGET_TIME : operation duration in 10ms ticks [TIME mode]
 *   - LIMIT0      : reserved for future
 *   - LIMIT1      : reserved for future
 *
 * Notes
 *   - In SPEED mode, motor runs continuously with specified output level.
 *   - In TIME mode, motor runs for TARGET_TIME then stops automatically.
 *
 *   - In TIME mode, TARGET_TIME==0 is rejected.
 *
 *   - LIMIT0 and LIMIT1 are currently unused and reserved.
 *
 * Behavior model
 *   - SPEED : continuous output
 *   - TIME  : timed operation with auto-stop
 *
 * Default:
 *   SETPOINT0   = 0   (stopped)
 *   TARGET_TIME = 0   (TIME mode requires explicit configuration)
 *   LIMIT0      = 0   (reserved)
 *   LIMIT1      = 0   (reserved)
 */

static void motor_apply_defaults(uint8_t *regs, uint16_t reg_count) {
    (void)reg_count;

    regs[CFG_FLAGS0] = 0x00;
    regs[CFG_FLAGS1] = 0x00;

    regs[SETPOINT0_H] = 0x00;
    regs[SETPOINT0_L] = 0x00;
    regs[SETPOINT1_H] = 0x00;
    regs[SETPOINT1_L] = 0x00;

    regs[LIMIT0_H] = 0x00;
    regs[LIMIT0_L] = 0x00;
    regs[LIMIT1_H] = 0x00;
    regs[LIMIT1_L] = 0x00;

    regs[TARGET_TIME_H] = 0x00;
    regs[TARGET_TIME_L] = 0x00;
}

const node_profile_t g_node_profile_motor = {
    .profile_id = NODE_PROFILE_MOTOR_BASIC,
    .name = "motor_basic",
    .module_type = NODE_MODULE_MOTOR,
    .hw_rev = 0x01,
    .fw_ver = 0x01,
    .cap_flags0 = NODE_CAP_STAGED_16BIT |
                  NODE_CAP_RUN_STOP |
                  NODE_CAP_ESTOP |
                  NODE_CAP_FB_U16_3CH,
    .cap_flags1 = 0x00,
    .apply_defaults = motor_apply_defaults,
};