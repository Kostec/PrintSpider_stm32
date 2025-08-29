#ifndef ADC_H
#define ADC_H
#include "stdint.h"

void ADC_Init();
void ADC_Task(void *pvParameters);

uint16_t ADC_GetValue(uint8_t channel);
void ADC_Handle();
#endif // ADC_H