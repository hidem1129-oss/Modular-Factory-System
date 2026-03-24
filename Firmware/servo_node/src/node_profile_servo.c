#include "node_profile.h"
#include "node_core.h"

/*
 * Servo template mapping
 *
 * OP_MODE
 *   - DEFAULT    : fallback to SPEED
 *   - SPEED      : SETPOINT0 + LIMIT0
 *   - TIME       : SETPOINT0 + TARGET_TIME
 *
 * Mode support
 *   - CONTINUOUS / ONE_SHOT are unsupported and rejected
 *
 * Parameters
 *   - SETPOINT0   : target angle [deg]
 *   - TARGET_TIME : target move time in 10ms ticks [TIME mode]
 *   - LIMIT0      : max angle step per 10ms tick [SPEED mode]
 *   - LIMIT1      : reserved for future
 *
 * Notes
 *   - In SPEED mode, LIMIT0==0 is normalized to 1 (minimum step).
 *   - In TIME mode, TARGET_TIME==0 is rejected.
 *
 *   - In TIME mode, LIMIT0 is unused and reserved for future extension.
 *
 *   - Mode switching re-plans motion from the current position.
 *
 * Behavior model
 *   - SPEED : step-limited convergence
 *   - TIME  : time-constrained interpolation
 *
 * Default:
 *   SETPOINT0   = 90  (center position)
 *   TARGET_TIME = 0   (TIME mode requires explicit configuration)
 *   LIMIT0      = 5   (5 deg per 10ms tick)
 *   LIMIT1      = 0   (reserved)
 */

static void servo_apply_defaults(uint8_t *regs, uint16_t reg_count) {
    (void)reg_count;

    regs[CFG_FLAGS0] = 0x00;
    regs[CFG_FLAGS1] = 0x00;

    regs[SETPOINT0_H] = 0x00;
    regs[SETPOINT0_L] = 90;   /* center */
    regs[SETPOINT1_H] = 0x00;
    regs[SETPOINT1_L] = 0x00;

    regs[TARGET_TIME_H] = 0x00;
    regs[TARGET_TIME_L] = 0x00;
    regs[HOLD_TIME_H]   = 0x00;
    regs[HOLD_TIME_L]   = 0x00;

    regs[LIMIT0_H] = 0x00;
    regs[LIMIT0_L] = 5;      /* 5 deg / 10ms */
    regs[LIMIT1_H] = 0x00;
    regs[LIMIT1_L] = 0x00;
}

const node_profile_t g_node_profile_servo = {
    .profile_id = NODE_PROFILE_SERVO_BASIC,
    .name = "servo_basic",
    .module_type = NODE_MODULE_SERVO,
    .hw_rev = 0x01,
    .fw_ver = 0x01,
    .cap_flags0 = NODE_CAP_STAGED_16BIT |
                  NODE_CAP_RUN_STOP |
                  NODE_CAP_ESTOP |
                  NODE_CAP_FB_U16_3CH,
    .cap_flags1 = 0x00,
    .apply_defaults = servo_apply_defaults,
};
