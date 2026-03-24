#ifndef SERVO_HAL_H
#define SERVO_HAL_H

#include <stdbool.h>
#include <stdint.h>

void servo_hal_init(void);
void servo_hal_set_enabled(bool enabled);
void servo_hal_write_deg_u16(uint16_t deg);
uint16_t servo_hal_deg_to_us(uint16_t deg);

#endif
