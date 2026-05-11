#include "node_profile.h"
#include "node_core.h"

/*
 * Sensor template mapping
 *
 * OP_MODE
 *   - DEFAULT    : fallback to SPEED
 *   - SPEED      : EMA filtering (LIMIT0)
 *   - TIME       : SMA filtering (TARGET_TIME)
 *
 * Mode support
 *   - CONTINUOUS / ONE_SHOT are unsupported and rejected
 *
 * Parameters
 *   - SETPOINT0   : threshold (filtered_value >= threshold → detect)
 *   - LIMIT0      : EMA coefficient (1..1000 = 0.1%..100.0%) [SPEED mode]
 *   - TARGET_TIME : SMA window length in 10ms ticks [TIME mode]
 *   - LIMIT1      : reserved for future
 *
 * Notes
 *   - In SPEED mode, LIMIT0==0 is normalized to 1 (minimum smoothing).
 *   - In SPEED mode, LIMIT0 is clamped to 1000 (100.0%).
 *
 *   - In TIME mode, TARGET_TIME==0 is rejected.
 *   - In TIME mode, TARGET_TIME is clamped to SMA_MAX.
 *
 *   - EMA uses Q15 fixed-point internally.
 *   - SMA uses circular buffer with running sum.
 *
 * Behavior model
 *   - SPEED : exponential smoothing (fast response / noise sensitive)
 *   - TIME  : moving average (stable / slower response)
 *
 * Default:
 *   LIMIT0      = 80 (8.0% EMA coefficient)
 *   TARGET_TIME = 8  (80ms SMA window)
 */

static void sensor_apply_defaults(uint8_t *regs, uint16_t reg_count) {

    (void)reg_count;

    regs[CFG_FLAGS0] = 0x01;
    regs[CFG_FLAGS1] = 0x00;

    regs[SETPOINT0_H] = 0x00;
    regs[SETPOINT0_L] = 0x00;
    regs[SETPOINT1_H] = 0x00;
    regs[SETPOINT1_L] = 0x00;

    regs[LIMIT0_H] = 0x00;
    regs[LIMIT0_L] = 0x50;
    regs[LIMIT1_H] = 0x00;
    regs[LIMIT1_L] = 0x00;

    regs[TARGET_TIME_H] = 0x00;
    regs[TARGET_TIME_L] = 0x08;
}

const node_profile_t g_node_profile_sensor = {
    .profile_id = NODE_PROFILE_SENSOR_BASIC,
    .name = "sensor_basic",
    .module_type = NODE_MODULE_SENSOR,
    .hw_rev = 0x01,
    .fw_ver = 0x01,
    .cap_flags0 = NODE_CAP_ESTOP |
                  NODE_CAP_FB_U16_3CH,
    .cap_flags1 = 0x00,
    .apply_defaults = sensor_apply_defaults,
};