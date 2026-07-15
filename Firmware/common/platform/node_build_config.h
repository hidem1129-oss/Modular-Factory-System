#ifndef NODE_BUILD_CONFIG_H
#define NODE_BUILD_CONFIG_H

#include "node_profile.h"

/*
 * Enable exactly one build profile by setting its value to 1.
 * All other profiles must remain 0.
 */
#define BUILD_PROFILE_DUMMY  1
#define BUILD_PROFILE_MOTOR  0
#define BUILD_PROFILE_SENSOR 0
#define BUILD_PROFILE_SERVO  0

#if (BUILD_PROFILE_DUMMY + \
     BUILD_PROFILE_MOTOR + \
     BUILD_PROFILE_SENSOR + \
     BUILD_PROFILE_SERVO) != 1
#error "Exactly one node profile must be enabled."
#endif

#if BUILD_PROFILE_DUMMY
#define ACTIVE_NODE_PROFILE NODE_PROFILE_DUMMY
#elif BUILD_PROFILE_MOTOR
#define ACTIVE_NODE_PROFILE NODE_PROFILE_MOTOR_BASIC
#elif BUILD_PROFILE_SENSOR
#define ACTIVE_NODE_PROFILE NODE_PROFILE_SENSOR_BASIC
#elif BUILD_PROFILE_SERVO
#define ACTIVE_NODE_PROFILE NODE_PROFILE_SERVO_BASIC
#endif

#endif