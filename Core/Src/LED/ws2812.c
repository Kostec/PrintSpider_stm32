/*
 * ws2812.c
 *
 *  Created on: Aug 17, 2025
 *      Author: rad-k
 */

//#include "ws2812.h"
//
//uint16_t pwmBuffer[LED_COUNT * BIT_PER_LED];
//
//void WS2812_PrepareLED(uint32_t color, uint16_t *buf)
//{
//    for (int i = 0; i < BIT_PER_LED; i++) {
//        // бит 23 первым → GRB упакован в цвете
//        if (color & (1UL << (23 - i))) {
//            buf[i] = DUTY1;
//        } else {
//            buf[i] = DUTY0;
//        }
//    }
//}
//
//void WS2812_FillBuffer(uint32_t *colors, int len)
//{
//    for (int i = 0; i < len; i++) {
//        WS2812_PrepareLED(colors[i], &pwmBuffer[i * BIT_PER_LED]);
//    }
//}

