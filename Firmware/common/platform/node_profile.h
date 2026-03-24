#ifndef NODE_PROFILE_H
#define NODE_PROFILE_H

#include <stdint.h>

/*
 * Minimal profile layer.
 * This file is intentionally small so it can be introduced
 * without disturbing the existing node_core / node_app behavior.
 */

/*
 * OP_MODE is a common 1-byte enum shared by all node types.
 *
 * The semantic meaning of each value is common, but concrete
 * parameter interpretation and support/reject/fallback behavior
 * are defined per node implementation.
 *
 * Default behavior:
 *   - Unsupported modes shall be rejected.
 *   - Fallback behavior (if any) must be explicitly defined
 *     by each node implementation.
 */

typedef enum {
    NODE_PROFILE_DUMMY        = 0x00,
    NODE_PROFILE_MOTOR_BASIC  = 0x01,
    NODE_PROFILE_SENSOR_BASIC = 0x02,
    NODE_PROFILE_SERVO_BASIC  = 0x03,
} node_profile_id_t;

typedef enum {
    NODE_MODULE_GENERIC = 0x00,
    NODE_MODULE_MOTOR   = 0x02,
    NODE_MODULE_SENSOR  = 0x03,
    NODE_MODULE_SERVO   = 0x04,
} node_module_type_t;

enum {
    NODE_CAP_STAGED_16BIT = (1u << 0),
    NODE_CAP_RUN_STOP     = (1u << 1),
    NODE_CAP_ESTOP        = (1u << 2),
    NODE_CAP_FB_U16_3CH   = (1u << 3),
};

typedef struct node_profile {
    node_profile_id_t profile_id;
    const char *name;
    uint8_t module_type;
    uint8_t hw_rev;
    uint8_t fw_ver;
    uint8_t cap_flags0;
    uint8_t cap_flags1;
    void (*apply_defaults)(uint8_t *regs, uint16_t reg_count);
} node_profile_t;

typedef enum {
    OP_MODE_DEFAULT    = 0x00,

    OP_MODE_SPEED      = 0x01,
    OP_MODE_TIME       = 0x02,

    OP_MODE_CONTINUOUS = 0x10,
    OP_MODE_ONE_SHOT   = 0x11,
} node_op_mode_t;

const node_profile_t *node_profile_get(void);

#endif
