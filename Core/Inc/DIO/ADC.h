/*
 Copyright 2025 Kostec
 SPDX-License-Identifier: Apache-2.0
*/

#ifndef ADC_H
#define ADC_H
#include "stdint.h"

/*
 * The module handles ADC ports
*/

void ADC_Init();

uint16_t ADC_GetValue(uint8_t channel);
void ADC_Handle();
#endif // ADC_H