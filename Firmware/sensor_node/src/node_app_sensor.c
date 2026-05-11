#include "node_app.h"
#include "node_profile.h"

#include <stdbool.h>
#include <stdint.h>

#include "hardware/adc.h"
#include "pico/stdlib.h"

#define PHOTO_ADC_GPIO          26u
#define PHOTO_ADC_CH            0u
#define PHOTO_ADC_MAX           4095u
#define PHOTO_OVERSAMPLE_N      16u

#define EMA_PERMILLE_MAX        1000u
#define EMA_Q15_ONE             32768u
#define SMA_MAX                 64u

#define PHOTO_LED_GPIO 2u

/* FB2 / sensor_state bit assignment */
#define PHOTO_STATE_LEVEL_BIT       (1u << 0)
#define PHOTO_STATE_RISING_BIT      (1u << 1)
#define PHOTO_STATE_FALLING_BIT     (1u << 2)
#define PHOTO_STATE_SATURATED_BIT   (1u << 3)

/* saturated judgement copied from the previous standalone experiment */
#define PHOTO_SAT_LOW_THRESH    5u
#define PHOTO_SAT_HIGH_THRESH   4090u

typedef struct {
    bool run_active;
    bool adc_initialized;
    uint8_t op_mode;

    /* latched active config */
    uint16_t cfg_setpoint0;     /* threshold */
    uint16_t cfg_target_time;   /* SMA window when TIME */
    uint16_t cfg_limit0;        /* EMA coefficient permille when SPEED */
    uint16_t cfg_limit1;        /* hysteresis */

    /* observed / published state */
    uint16_t raw_value;
    uint16_t filtered_value;
    uint16_t sensor_state;
    bool level_last;

    /* SPEED (EMA) runtime state */
    uint32_t ema_q15;

    /* TIME (SMA) runtime state */
    uint16_t sma_window;
    uint16_t sma_buf[SMA_MAX];
    uint16_t sma_index;
    uint16_t sma_count;
    uint32_t sma_sum;

    bool plan_valid;
} photosensor_app_state_t;

static photosensor_app_state_t g_photo;

/* ---------- helpers ---------- */

static uint8_t photosensor_resolve_mode(uint8_t op_mode) {
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

static uint16_t photosensor_normalize_ema_permille(uint16_t k) {
    if (k == 0u) {
        return 1u;
    }
    if (k > EMA_PERMILLE_MAX) {
        return EMA_PERMILLE_MAX;
    }
    return k;
}

static uint16_t photosensor_normalize_sma_window(uint16_t w) {
    if (w == 0u) {
        return 1u;
    }
    if (w > SMA_MAX) {
        return SMA_MAX;
    }
    return w;
}

static void photosensor_publish_state(void) {
    core_publish_fb_u16(
        g_photo.raw_value,
        g_photo.filtered_value,
        g_photo.sensor_state
    );
}

static void photosensor_adc_init_once(void) {
    if (g_photo.adc_initialized) {
        return;
    }

    adc_init();
    adc_gpio_init(PHOTO_ADC_GPIO);
    adc_select_input(PHOTO_ADC_CH);

    g_photo.adc_initialized = true;
}

static uint16_t photosensor_read_raw_adc(void) {
    uint32_t acc = 0u;
    uint16_t i;

    adc_select_input(PHOTO_ADC_CH);

    for (i = 0u; i < PHOTO_OVERSAMPLE_N; i++) {
        acc += (uint32_t)adc_read();
    }

    return (uint16_t)(acc / PHOTO_OVERSAMPLE_N);
}

static void photosensor_update_state_flags(uint16_t filtered, uint16_t raw) {
    bool level = g_photo.level_last;
    bool rising;
    bool falling;
    bool saturated;
    uint16_t th_on;
    uint32_t th_off32;
    uint16_t th_off;
    uint16_t state = 0u;

    /* previous standalone experiment used "work present when <= threshold" */
    th_on = g_photo.cfg_setpoint0;
    th_off32 = (uint32_t)g_photo.cfg_setpoint0 + (uint32_t)g_photo.cfg_limit1;
    if (th_off32 > PHOTO_ADC_MAX) {
        th_off = PHOTO_ADC_MAX;
    } else {
        th_off = (uint16_t)th_off32;
    }

    if (!g_photo.level_last) {
        if (filtered <= th_on) {
            level = true;
        }
    } else {
        if (filtered >= th_off) {
            level = false;
        }
    }

    rising = (!g_photo.level_last && level);
    falling = (g_photo.level_last && !level);
    saturated = (raw <= PHOTO_SAT_LOW_THRESH) || (raw >= PHOTO_SAT_HIGH_THRESH);

    if (level) {
        state |= PHOTO_STATE_LEVEL_BIT;
    }
    if (rising) {
        state |= PHOTO_STATE_RISING_BIT;
    }
    if (falling) {
        state |= PHOTO_STATE_FALLING_BIT;
    }
    if (saturated) {
        state |= PHOTO_STATE_SATURATED_BIT;
    }

    g_photo.sensor_state = state;
    g_photo.level_last = level;
}

/* ---------- reset / plan ---------- */

static void photosensor_led_init(void) {
    gpio_init(PHOTO_LED_GPIO);
    gpio_set_dir(PHOTO_LED_GPIO, GPIO_OUT);
    gpio_put(PHOTO_LED_GPIO, 1);  // 常時ON
}

static void photosensor_reset_mode_state(void) {
    uint16_t i;

    g_photo.ema_q15 = 0u;

    g_photo.sma_window = 1u;
    g_photo.sma_index = 0u;
    g_photo.sma_count = 0u;
    g_photo.sma_sum = 0u;

    for (i = 0u; i < SMA_MAX; i++) {
        g_photo.sma_buf[i] = 0u;
    }

    g_photo.sensor_state = 0u;
    g_photo.level_last = false;
    g_photo.plan_valid = false;
}

static void photosensor_reset_all(void) {
    g_photo.run_active = false;
    g_photo.adc_initialized = false;
    g_photo.op_mode = OP_MODE_SPEED;

    /* defaults derived from previous photo sensor experiment */
    g_photo.cfg_setpoint0 = 3800u;
    g_photo.cfg_target_time = 8u;
    g_photo.cfg_limit0 = 250u;
    g_photo.cfg_limit1 = 120u;

    g_photo.raw_value = 0u;
    g_photo.filtered_value = 0u;
    g_photo.sensor_state = 0u;
    g_photo.level_last = false;

    photosensor_reset_mode_state();
}

static bool photosensor_load_active_config(void) {
    uint8_t resolved_mode = photosensor_resolve_mode(core_get_op_mode());

    if (resolved_mode == 0xFFu) {
        return false;
    }

    g_photo.op_mode = resolved_mode;
    g_photo.cfg_setpoint0 = core_get_setpoint0();
    g_photo.cfg_target_time = core_get_target_time();
    g_photo.cfg_limit0 = core_get_limit0();
    g_photo.cfg_limit1 = core_get_limit1();

    return true;
}

static bool photosensor_validate_active_config(void) {
    switch (g_photo.op_mode) {
    case OP_MODE_SPEED:
        return true;

    case OP_MODE_TIME:
        if (g_photo.cfg_target_time == 0u) {
            return false;
        }
        return true;

    default:
        return false;
    }
}

static void photosensor_replan_from_active(void) {
    photosensor_reset_mode_state();

    switch (g_photo.op_mode) {
    case OP_MODE_SPEED:
        g_photo.cfg_limit0 =
            photosensor_normalize_ema_permille(g_photo.cfg_limit0);
        g_photo.plan_valid = true;
        break;

    case OP_MODE_TIME:
        g_photo.sma_window =
            photosensor_normalize_sma_window(g_photo.cfg_target_time);
        g_photo.plan_valid = true;
        break;

    default:
        g_photo.plan_valid = false;
        break;
    }
}

static bool photosensor_apply_active_config(void) {
    if (!photosensor_load_active_config()) {
        return false;
    }

    if (!photosensor_validate_active_config()) {
        return false;
    }

    photosensor_replan_from_active();
    return g_photo.plan_valid;
}

/* ---------- tick handlers ---------- */

static void photosensor_tick_speed(void) {
    uint16_t raw = photosensor_read_raw_adc();
    uint32_t alpha_q15;
    uint64_t acc;

    g_photo.raw_value = raw;
    alpha_q15 =
        ((uint32_t)g_photo.cfg_limit0 * EMA_Q15_ONE) / EMA_PERMILLE_MAX;

    if (g_photo.ema_q15 == 0u) {
        g_photo.ema_q15 = ((uint32_t)raw << 15);
    } else {
        acc =
            ((uint64_t)(EMA_Q15_ONE - alpha_q15) * (uint64_t)g_photo.ema_q15) +
            ((uint64_t)alpha_q15 * ((uint64_t)raw << 15));
        g_photo.ema_q15 = (uint32_t)(acc >> 15);
    }

    g_photo.filtered_value = (uint16_t)(g_photo.ema_q15 >> 15);
    photosensor_update_state_flags(g_photo.filtered_value, raw);
    photosensor_publish_state();
}

static void photosensor_tick_time(void) {
    uint16_t raw = photosensor_read_raw_adc();

    g_photo.raw_value = raw;

    if (g_photo.sma_count < g_photo.sma_window) {
        g_photo.sma_buf[g_photo.sma_index] = raw;
        g_photo.sma_sum += raw;
        g_photo.sma_count++;
    } else {
        g_photo.sma_sum -= g_photo.sma_buf[g_photo.sma_index];
        g_photo.sma_buf[g_photo.sma_index] = raw;
        g_photo.sma_sum += raw;
    }

    g_photo.sma_index++;
    if (g_photo.sma_index >= g_photo.sma_window) {
        g_photo.sma_index = 0u;
    }

    g_photo.filtered_value =
        (uint16_t)(g_photo.sma_sum / g_photo.sma_count);

    photosensor_update_state_flags(g_photo.filtered_value, raw);
    photosensor_publish_state();
}

/* ---------- app callbacks ---------- */

static void app_init_impl(void) {
    photosensor_led_init();   // ★追加

    photosensor_reset_all();
    photosensor_adc_init_once();

    g_photo.raw_value = photosensor_read_raw_adc();
    g_photo.filtered_value = g_photo.raw_value;
    photosensor_update_state_flags(g_photo.filtered_value, g_photo.raw_value);
    photosensor_publish_state();
}

static void app_tick_10ms_impl(void) {
    if (!g_photo.run_active) {
        return;
    }

    if (!core_is_running() || core_is_estopped()) {
        g_photo.run_active = false;
        photosensor_publish_state();
        return;
    }

    switch (g_photo.op_mode) {
    case OP_MODE_SPEED:
        photosensor_tick_speed();
        break;

    case OP_MODE_TIME:
        photosensor_tick_time();
        break;

    default:
        g_photo.run_active = false;
        photosensor_publish_state();
        break;
    }
}

static void app_on_latch_apply_impl(void) {
    (void)photosensor_apply_active_config();
    photosensor_publish_state();
}

static bool app_on_run_start_impl(void) {
    photosensor_adc_init_once();

    if (!photosensor_apply_active_config()) {
        g_photo.run_active = false;
        photosensor_publish_state();
        return false;
    }

    g_photo.run_active = true;
    photosensor_publish_state();
    return true;
}

static void app_on_run_stop_impl(void) {
    g_photo.run_active = false;
    photosensor_publish_state();
}

static void app_on_estop_enter_impl(void) {
    g_photo.run_active = false;
    photosensor_publish_state();
}

const node_app_callbacks_t g_node_app_sensor_callbacks = {
    .init = app_init_impl,
    .tick_10ms = app_tick_10ms_impl,
    .on_latch_apply = app_on_latch_apply_impl,
    .on_run_start = app_on_run_start_impl,
    .on_run_stop = app_on_run_stop_impl,
    .on_estop_enter = app_on_estop_enter_impl,
};
