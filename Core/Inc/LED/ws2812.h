#ifndef WS8212_H
#define WS8212_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

#define LED_COUNT 16
#define BIT_PER_LED  24
#define PWM_FREQ     800000UL           // 800 kHz
#define TIMER_CLK    16000000UL   // APB1 timer clock (16 MHz)

#define PWM_PERIOD   (TIMER_CLK / PWM_FREQ - 1)

// формируем длительность "1" и "0" в тиках:
// 1: 0.8 µs × 16 МГц = 12.8 ≈ 13 тиков
// 0: 0.4 µs × 16 МГц = 6.4 ≈ 6 тиков
// #define DUTY1        ((PWM_PERIOD + 1) * 64 / 100)
// #define DUTY0        ((PWM_PERIOD + 1) * 32 / 100)

#define DUTY1        12
#define DUTY0        6

extern uint16_t pwmBuffer[LED_COUNT * BIT_PER_LED];


void WS2812_PrepareLED(uint32_t color, uint16_t *buf);
void WS2812_FillBuffer(uint32_t *colors, int len);

#endif // WS8212_H