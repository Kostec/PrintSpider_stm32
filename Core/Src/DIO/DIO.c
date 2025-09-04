#include "DIO/DIO.h"

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include <semphr.h>
#include "LOG/LOG.h"
#include "DIO/STICK.h"
#include "DIO/ADC.h"

extern I2C_HandleTypeDef PCF8574_I2C_PORT;
osThreadId_t DIO_Thread;

static PCF8574_Data _data;

void DIO_Init()
{
    STICK_Init();

    const osThreadAttr_t DIO_attributes = {
        .name = "DIO_Task",
        .stack_size = 256 * 4,
        .priority = (osPriority_t) osPriorityHigh,
      };
    DIO_Thread = osThreadNew(DIO_Task, NULL, &DIO_attributes);
    configASSERT(DIO_Thread);
}

void DIO_WritePins(PCF8574_Data data)
{   
    while (HAL_I2C_GetState(&PCF8574_I2C_PORT) != HAL_I2C_STATE_READY) {
        vTaskDelay(1);
    }

    if (HAL_I2C_Master_Transmit(&PCF8574_I2C_PORT, PCF8574_I2C_ADDR, &data.byte, 1, 100) != HAL_OK)
    {
        LOG_Error("%s: Transmit error", __FUNCTION__);
        return; // HAL_ERROR
    }

    uint32_t start = HAL_GetTick();
    while (HAL_I2C_GetState(&PCF8574_I2C_PORT) != HAL_I2C_STATE_READY) {
        if ((HAL_GetTick() - start) > 100) {
            LOG_Error("%s: Transmit timeout", __FUNCTION__);
            return; // HAL_TIMEOUT;
        }
        vTaskDelay(1);
    }

    LOG_Error("%s: Transmit OK", __FUNCTION__);
}

void DIO_ReadPins()
{
    while (HAL_I2C_GetState(&PCF8574_I2C_PORT) != HAL_I2C_STATE_READY) {
        osDelay(1);
    }
  
    if (HAL_I2C_Master_Receive(&PCF8574_I2C_PORT, PCF8574_I2C_ADDR, &_data.byte, 1, 500) != HAL_OK)
    {
        LOG_Error("%s: Receive error", __FUNCTION__);
        return; // HAL_ERROR
    }

    uint32_t start = HAL_GetTick();
    while (HAL_I2C_GetState(&PCF8574_I2C_PORT) != HAL_I2C_STATE_READY) {
        if ((HAL_GetTick() - start) > 100) {
            LOG_Error("%s: Receive timeout", __FUNCTION__);
            return; // HAL_TIMEOUT;
        }
        osDelay(1);
    }
    // LOG_Debug("%s: Receive OK", __FUNCTION__);
}

void DIO_Task(void *pvParameters)
{
    osDelay(1000);
    LOG_Debug("%s", __FUNCTION__);
    /*
    0xFF all pins are inputs
    0x00 all pins are outputs
    */
    _data.byte = 0x00;
    // DIO_WritePins(_data);
    for (;;) {
        DIO_ReadPins();
        ADC_Handle();
        // LOG_Debug("%s: 0x%x", __FUNCTION__, _data.byte);
        STICK_Process();
        osDelay(20);
    }
}

uint8_t DIO_ReadPin(tenDIOPin pin)
{
    return (_data.byte >> pin) & 1;
}

void DIO_WritePin(tenDIOPin pin, uint8_t value)
{
    _data.byte = (_data.byte & ((0xFF) & (pin << 0))) & (pin << value);
}

PCF8574_Data DIO_ReadAll()
{
    return _data;
}
