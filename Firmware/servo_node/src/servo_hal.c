#include "servo_hal.h"
#include "node_core.h"

#include "hardware/gpio.h"
#include "hardware/pwm.h"

#define SERVO_FRAME_US     20000u   /* 20ms = 50Hz */
#define SERVO_MIN_PULSE_US   500u
#define SERVO_MAX_PULSE_US  2400u
#define SERVO_CENTER_DEG      90u

static uint g_pwm_slice;
static uint g_pwm_chan;
static bool g_pwm_enabled = false;
static uint16_t g_last_deg = SERVO_CENTER_DEG;

/* 1 tick = 1us にする */
static void servo_pwm_init_50hz(void) {
    gpio_set_function(SERVO_PWM_PIN, GPIO_FUNC_PWM);

    g_pwm_slice = pwm_gpio_to_slice_num(SERVO_PWM_PIN);
    g_pwm_chan  = pwm_gpio_to_channel(SERVO_PWM_PIN);

    pwm_config cfg = pwm_get_default_config();

    /*
     * clk_sys = 125MHz のとき、
     * divider = 125 にすると PWM カウンタ 1tick = 1us
     * wrap = 20000 - 1 で 20ms 周期
     */
    pwm_config_set_clkdiv(&cfg, 125.0f);
    pwm_config_set_wrap(&cfg, SERVO_FRAME_US - 1u);

    pwm_init(g_pwm_slice, &cfg, false);
    pwm_set_gpio_level(SERVO_PWM_PIN, 0u);
}

static uint16_t servo_clamp_deg(uint16_t deg) {
    return (deg > 180u) ? 180u : deg;
}

uint16_t servo_hal_deg_to_us(uint16_t deg) {
    uint32_t span_us;
    uint32_t pulse_us;

    deg = servo_clamp_deg(deg);

    span_us = (uint32_t)(SERVO_MAX_PULSE_US - SERVO_MIN_PULSE_US);
    pulse_us = (uint32_t)SERVO_MIN_PULSE_US + ((uint32_t)deg * span_us) / 180u;

    return (uint16_t)pulse_us;
}

void servo_hal_init(void) {
    servo_pwm_init_50hz();
    g_last_deg = SERVO_CENTER_DEG;
    g_pwm_enabled = false;
}

void servo_hal_set_enabled(bool enabled) {
    g_pwm_enabled = enabled;

    if (enabled) {
        /* 最後の角度で即出力再開 */
        pwm_set_gpio_level(SERVO_PWM_PIN, servo_hal_deg_to_us(g_last_deg));
        pwm_set_enabled(g_pwm_slice, true);
    } else {
        /* compare を 0 にしてから停止 */
        pwm_set_gpio_level(SERVO_PWM_PIN, 0u);
        pwm_set_enabled(g_pwm_slice, false);
    }
}

void servo_hal_write_deg_u16(uint16_t deg) {
    uint16_t pulse_us;

    g_last_deg = servo_clamp_deg(deg);
    pulse_us = servo_hal_deg_to_us(g_last_deg);

    /*
     * wrap=19999, 1tick=1us なので
     * level にそのまま pulse_us を入れればよい
     */
    pwm_set_gpio_level(SERVO_PWM_PIN, pulse_us);

    /*
     * 念のため、未 enable 状態でも次回 enable 時に反映されるよう
     * g_last_deg は常に更新しておく
     */
}