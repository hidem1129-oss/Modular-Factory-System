#include "node_app.h"
#include "node_build_config.h"
#include "node_profile.h"

extern const node_app_callbacks_t g_node_app_dummy_callbacks;
extern const node_app_callbacks_t g_node_app_motor_callbacks;
extern const node_app_callbacks_t g_node_app_sensor_callbacks;
extern const node_app_callbacks_t g_node_app_servo_callbacks;

const node_app_callbacks_t *node_app_get_callbacks(void) {
    if (ACTIVE_NODE_PROFILE == NODE_PROFILE_DUMMY) {
        return &g_node_app_dummy_callbacks;
    }
    if (ACTIVE_NODE_PROFILE == NODE_PROFILE_MOTOR_BASIC) {
        return &g_node_app_motor_callbacks;
    }
    if (ACTIVE_NODE_PROFILE == NODE_PROFILE_SENSOR_BASIC) {
        return &g_node_app_sensor_callbacks;
    }
    if (ACTIVE_NODE_PROFILE == NODE_PROFILE_SERVO_BASIC) {
        return &g_node_app_servo_callbacks;
    }
    return &g_node_app_dummy_callbacks;
}
