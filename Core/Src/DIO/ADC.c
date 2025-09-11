/*
 Copyright 2025 Kostec
 SPDX-License-Identifier: Apache-2.0
*/

#include "DIO/ADC.h"
#include "LOG/LOG.h"
#include "stdint.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"

#define ADC__CHANNEL_COUNT 2
extern ADC_HandleTypeDef hadc1;
uint16_t adc_buffer[ADC__CHANNEL_COUNT];

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc == &hadc1)
    {
        // adc_buffer[0..ADC__CHANNEL_COUNT-1] ready
        
    }    
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc == &hadc1) 
    {
        // adc_buffer[0..ADC__CHANNEL_COUNT-1] ready
        LOG_Error("%s: ADC DMA error", __FUNCTION__);
    } 
}

void ADC_Init()
{

}

void ADC_Handle()
{
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, ADC__CHANNEL_COUNT);
}

uint16_t ADC_GetValue(uint8_t channel)
{
    return adc_buffer[channel];
}


