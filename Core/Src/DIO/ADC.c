#include "DIO/ADC.h"
#include "LOG/LOG.h"
#include "stdint.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"

#define CHANNEL_COUNT 2
extern ADC_HandleTypeDef hadc1;
uint16_t adc_buffer[CHANNEL_COUNT];
// osThreadId_t ADC_TaskHandle;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc == &hadc1)
    {
        // adc_buffer[0..CHANNEL_COUNT-1] ready
        
    }    
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc == &hadc1) 
    {
        // adc_buffer[0..CHANNEL_COUNT-1] ready
        LOG_Error("%s: ADC DMA error", __FUNCTION__);
    } 
}

void ADC_Init()
{
    // const osThreadAttr_t ADC_TaskAttr = {
    //     .name = "ADC_Task",
    //     .stack_size = 64 * 4,
    //     .priority = (osPriority_t) osPriorityLow,
    // };
    // ADC_TaskHandle = osThreadNew(ADC_Task, NULL,  &ADC_TaskAttr);
    // configASSERT(ADC_TaskHandle);

}

void ADC_Handle()
{
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, CHANNEL_COUNT);
}

void ADC_Task(void *pvParameters)
{
    LOG_Debug("%s", __FUNCTION__);
    for(;;)
    {
        ADC_Handle();
        osDelay(20);
    }
}

uint16_t ADC_GetValue(uint8_t channel)
{
    return adc_buffer[channel];
}


