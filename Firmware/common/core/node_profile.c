#include "node_profile.h"
#include "node_build_config.h"

extern const node_profile_t g_node_profile_dummy;
extern const node_profile_t g_node_profile_motor;
extern const node_profile_t g_node_profile_sensor;
extern const node_profile_t g_node_profile_servo;

const node_profile_t *node_profile_get(void) {
    if (ACTIVE_NODE_PROFILE == NODE_PROFILE_DUMMY) {
        return &g_node_profile_dummy;
    }
    if (ACTIVE_NODE_PROFILE == NODE_PROFILE_MOTOR_BASIC) {
        return &g_node_profile_motor;
    }
    if (ACTIVE_NODE_PROFILE == NODE_PROFILE_SENSOR_BASIC) {
        return &g_node_profile_sensor;
    }
    if (ACTIVE_NODE_PROFILE == NODE_PROFILE_SERVO_BASIC) {
        return &g_node_profile_servo;
    }
    return &g_node_profile_dummy;
}
