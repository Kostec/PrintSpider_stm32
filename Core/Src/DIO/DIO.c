/*
 Copyright 2025 Kostec
 SPDX-License-Identifier: Apache-2.0
*/

#include "DIO/DIO.h"

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include <semphr.h>
#include "LOG/LOG.h"
#include "DIO/STICK.h"
#include "DIO/ADC.h"
#include "EXHD/EXHD.h"

extern I2C_HandleTypeDef PCF8574_I2C_PORT;
osThreadId_t DIO__thread;

static DIO_tstPCF8574_Data DIO__data;

void DIO_Init()
{
    STICK_Init();

    const osThreadAttr_t DIO_attributes = {
        .name = "DIO_Task",
        .stack_size = 256 * 4,
        .priority = (osPriority_t) osPriorityHigh,
      };
    DIO__thread = osThreadNew(DIO_Task, NULL, &DIO_attributes);
    configASSERT(DIO__thread);
}

void DIO_WritePins(DIO_tstPCF8574_Data data)
{   
    while (HAL_I2C_GetState(&PCF8574_I2C_PORT) != HAL_I2C_STATE_READY) {
        vTaskDelay(1);
    }

    if (HAL_I2C_Master_Transmit(&PCF8574_I2C_PORT, PCF8574_I2C_ADDR, &data.byte, 1, 100) != HAL_OK)
    {
        EXHD_Raise(EXHD_DIO_Write);
        return; // HAL_ERROR
    }

    uint32_t start = HAL_GetTick();
    while (HAL_I2C_GetState(&PCF8574_I2C_PORT) != HAL_I2C_STATE_READY) {
        if ((HAL_GetTick() - start) > 100) {
            EXHD_Raise(EXHD_DIO_Write);
            EXHD_Raise(EXHD_DIO_Timeout);
            return; // HAL_TIMEOUT;
        }
        vTaskDelay(1);
    }

    EXHD_Reset(EXHD_DIO_Write);
    EXHD_Reset(EXHD_DIO_Timeout);

    LOG_Error("%s: Transmit OK", __FUNCTION__);
}

void DIO_ReadPins()
{
    while (HAL_I2C_GetState(&PCF8574_I2C_PORT) != HAL_I2C_STATE_READY) {
        osDelay(1);
    }
  
    if (HAL_I2C_Master_Receive(&PCF8574_I2C_PORT, PCF8574_I2C_ADDR, &DIO__data.byte, 1, 500) != HAL_OK)
    {
        EXHD_Raise(EXHD_DIO_Read);
        return; // HAL_ERROR
    }

    uint32_t start = HAL_GetTick();
    while (HAL_I2C_GetState(&PCF8574_I2C_PORT) != HAL_I2C_STATE_READY) {
        if ((HAL_GetTick() - start) > 100) {
            EXHD_Raise(EXHD_DIO_Read);
            EXHD_Raise(EXHD_DIO_Timeout);
            return; // HAL_TIMEOUT;
        }
        osDelay(1);
    }

    EXHD_Reset(EXHD_DIO_Read);
    EXHD_Reset(EXHD_DIO_Timeout);
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
    DIO__data.byte = 0x00;
    // DIO_WritePins(DIO__data);
    for (;;) {
        DIO_ReadPins();
        ADC_Handle();
        // LOG_Debug("%s: 0x%x", __FUNCTION__, DIO__data.byte);
        STICK_Process();
        osDelay(20);
    }
}

uint8_t DIO_ReadPin(DIO_tenPin pin)
{
    return (DIO__data.byte >> pin) & 1;
}

void DIO_WritePin(DIO_tenPin pin, uint8_t value)
{
    DIO__data.byte = (DIO__data.byte & ((0xFF) & (pin << 0))) & (pin << value);
}

DIO_tstPCF8574_Data DIO_ReadAll()
{
    return DIO__data;
}
