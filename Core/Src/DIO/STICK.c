#include "DIO/STICK.h"
#include "main.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "LOG/LOG.h"

#define CHANNEL_COUNT 2
extern ADC_HandleTypeDef hadc1;
static tstStick STICK__instance = {0};
uint32_t adc_buffer[CHANNEL_COUNT];

typedef enum {
    Idle,
    Scanning,
    Scanned
} tenDMAStatus;

static tenDMAStatus status;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    if (hadc == &hadc1) {
        // adc_buffer[0..CHANNEL_COUNT-1] готовы
        STICK__instance.X = (int32_t)adc_buffer[0] - (int32_t)2048;
        STICK__instance.Y = (int32_t)adc_buffer[1] - (int32_t)2048;
    }
}

void STICK_Init()
{
    status = Idle;
}

void STICK_Process()
{
    HAL_ADC_Start(&hadc1);
    
    if (HAL_ADC_PollForConversion(&hadc1, 10) != HAL_OK)
    {
        LOG_Error("%s: Poll error", __FUNCTION__);
    }
    else 
    {
        STICK__instance.X = HAL_ADC_GetValue(&hadc1);
    }

    if (HAL_ADC_PollForConversion(&hadc1, 10) != HAL_OK)
    {
        LOG_Error("%s: Poll error", __FUNCTION__);
    }
    else 
    {
        STICK__instance.Y = HAL_ADC_GetValue(&hadc1);
    }

    HAL_ADC_Stop(&hadc1);
    
    STICK__instance.SW = HAL_GPIO_ReadPin(STICK_SW_GPIO_Port, STICK_SW_Pin);
    LOG_Debug("%s: x=%lu y=%lu sw=%u", __FUNCTION__, STICK__instance.X, STICK__instance.Y, STICK__instance.SW);
}

tstStick STICK_GetStick()
{
    return STICK__instance;
}