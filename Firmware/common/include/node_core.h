#ifndef NODE_CORE_H
#define NODE_CORE_H

#include <stdbool.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/i2c_slave.h"

typedef struct node_profile node_profile_t;

// ============================================================
// GPIO mapping
// ============================================================
#define I2C_SDA_PIN 0
#define I2C_SCL_PIN 1
#define SERVO_PWM_PIN 2

#define DIP0_PIN 18
#define DIP1_PIN 19
#define DIP2_PIN 20
#define DIP3_PIN 21

#define ESTOP_BUS_PIN 17
#define ENABLE_LOCAL_ESTOP 1
#define ESTOP_LOCAL_PIN 16
#define ACK_PIN 15

#define LIMIT_DEBOUNCE_MS 30
#define ACK_DEBOUNCE_MS 30

#define I2C_BASE_ADDR 0x10

// ============================================================
// I2C register window (0x00..0x3F)
// ============================================================
#define REG_WINDOW_SIZE 64
#define REG_WINDOW_MASK (REG_WINDOW_SIZE - 1)

enum {
    SIGNATURE0 = 0x00,
    SIGNATURE1 = 0x01,
    MODULE_TYPE = 0x02,
    MODULE_ID_L = 0x03,
    MODULE_ID_H = 0x04,
    HW_REV = 0x05,
    FW_VER = 0x06,
    CAP_FLAGS0 = 0x07,
    PROFILE_ID = 0x08,
    CAP_FLAGS1 = 0x09,
    UNIT_FLAGS0 = 0x0A,
    UNIT_FLAGS1 = 0x0B,

    STATUS = 0x10,
    ERROR_CODE = 0x11,
    WARN_CODE = 0x12,
    UPDATE_CNT_H = 0x13,
    UPDATE_CNT_L = 0x14,
    ESTOP_SOURCE = 0x15,
    STATE_FLAGS0 = 0x16,
    STATE_FLAGS1 = 0x17,
    LAST_CMD = 0x18,
    LAST_CMD_RESULT = 0x19,
    FB_SEQ_H = 0x1A,
    FB_SEQ_L = 0x1B,
    COMPLETE_FLAGS = 0x1C,
    OP_MODE = 0x1D,

    FB_VALUE0_H = 0x20,
    FB_VALUE0_L = 0x21,
    FB_VALUE1_H = 0x22,
    FB_VALUE1_L = 0x23,
    FB_VALUE2_H = 0x24,
    FB_VALUE2_L = 0x25,
    FB_FLAGS0 = 0x26,
    FB_FLAGS1 = 0x27,
    CFG_VALUE0_H = 0x28,
    CFG_VALUE0_L = 0x29,
    CFG_VALUE1_H = 0x2A,
    CFG_VALUE1_L = 0x2B,
    CFG_FLAGS0 = 0x2C,
    CFG_FLAGS1 = 0x2D,
    EXT_VALUE0 = 0x2E,
    EXT_VALUE1 = 0x2F,

    CMD_FLAGS = 0x30,
    SETPOINT0_H = 0x31,
    SETPOINT0_L = 0x32,
    SETPOINT1_H = 0x33,
    SETPOINT1_L = 0x34,
    TARGET_TIME_H = 0x35,
    TARGET_TIME_L = 0x36,
    HOLD_TIME_H = 0x37,
    HOLD_TIME_L = 0x38,
    CMD_IMMEDIATE = 0x39,
    CMD_PARAM0 = 0x3A,
    CMD_PARAM1 = 0x3B,
    LIMIT0_H = 0x3C,
    LIMIT0_L = 0x3D,
    LIMIT1_H = 0x3E,
    LIMIT1_L = 0x3F,
};

// LAST_CMD_RESULT
#define CMDRES_OK               0x00
#define CMDRES_REJECT_BUSY      0x01
#define CMDRES_REJECT_UNLATCHED 0x02
#define CMDRES_REJECT_ESTOP     0x03
#define CMDRES_REJECT_STATE     0x04
#define CMDRES_ACCEPTED_NOOP    0x05

// STATUS bits
#define STATUS_READY      (1u << 0)
#define STATUS_BUSY       (1u << 1)
#define STATUS_ERROR      (1u << 2)
#define STATUS_WARN       (1u << 3)
#define STATUS_ESTOP      (1u << 4)
#define STATUS_DATA_READY (1u << 5)

// ESTOP_SOURCE bits
#define ESTOP_SRC_BUS   (1u << 0)
#define ESTOP_SRC_LOCAL (1u << 1)

// COMPLETE_FLAGS bits
#define DONE              (1u << 0)
#define DONE_BY_TIME      (1u << 1)
#define DONE_BY_TARGET    (1u << 2)
#define DONE_BY_CONDITION (1u << 3)
#define DONE_BY_ABORT     (1u << 4)
#define LATCHED           (1u << 5)

// CMD_FLAGS bits
#define CMD_RUN           (1u << 0)
#define CMD_DIR           (1u << 1)
#define CMD_MODE          (1u << 2)
#define CMD_ESTOP_RESET   (1u << 3)
#define CMD_LATCH_APPLY   (1u << 4)
#define CMD_USE_SOFTLIMIT (1u << 5)

typedef struct {
    void (*init)(void);
    void (*tick_10ms)(void);
    void (*on_latch_apply)(void);
    bool (*on_run_start)(void);
    void (*on_run_stop)(void);
    void (*on_estop_enter)(void);
} node_app_callbacks_t;

void node_core_init(const node_app_callbacks_t *callbacks);
void node_core_poll_1ms(void);
void node_core_tick_10ms(void);
uint8_t node_core_get_i2c_addr(void);
void node_core_i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event);

const node_profile_t *node_core_get_profile(void);

// core API for node_app.c
uint8_t core_get_op_mode(void);
uint16_t core_get_target_time(void);
uint16_t core_get_setpoint0(void);
uint16_t core_get_limit0(void);
uint16_t core_get_limit1(void);
uint8_t core_get_cmd_flags_level(void);
void core_publish_fb_u16(uint16_t v0, uint16_t v1, uint16_t v2);
void core_finish_done_target(void);
bool core_is_running(void);
bool core_is_estopped(void);
void core_finish_stopped(void);

#endif

