#include "DIO/DIO.h"

#include "stm32f4xx_hal.h"
#include <FreeRTOS.h>
#include <semphr.h>
#include "LOG/LOG.h"
#include "DIO/STICK.h"

SemaphoreHandle_t i2cTxSem;
SemaphoreHandle_t i2cRxSem;
// static QueueHandle_t logQueueHandle;
extern I2C_HandleTypeDef PCF8574_I2C_PORT;

static PCF8574_Data _data;

void DIO_Init()
{
    STICK_Init();
    i2cTxSem = xSemaphoreCreateBinary();
    configASSERT(i2cTxSem);
    xSemaphoreGive(i2cTxSem);

    i2cRxSem = xSemaphoreCreateBinary();
    configASSERT(i2cRxSem);
    xSemaphoreGive(i2cRxSem);

    xTaskCreate(DIO_Task, 
        "DIO_Task", 
        128, 
        NULL, 
        tskIDLE_PRIORITY + 1, 
        NULL);
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
        vTaskDelay(1);
    }
  
    if (HAL_I2C_Master_Receive(&PCF8574_I2C_PORT, PCF8574_I2C_ADDR, &_data.byte, 1, 100) != HAL_OK)
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
        vTaskDelay(1);
    }
    // LOG_Debug("%s: Receive OK", __FUNCTION__);
}

void DIO_Task(void *pvParameters)
{
    /*
    0xFF all pins are inputs
    0x00 all pins are outputs
    */
    _data.byte = 0x00;
    DIO_WritePins(_data);
    for (;;) {
        DIO_ReadPins();
        // LOG_Debug("%s: 0x%x", __FUNCTION__, _data.byte);
        STICK_Process();
        vTaskDelay(20);
    }
}