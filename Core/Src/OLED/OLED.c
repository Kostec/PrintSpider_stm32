
#include "OLED/OLED.h"

#include "stm32f4xx_hal.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "ssd1306_tests.h"
#include "cmsis_os.h"

#include "LOG/LOG.h"
#include "main.h" // ErrorHandler declaration
#include "OLED/MENU/MENU.h"
#include "OLED/MENU/MAIN_MENU.h"

static osThreadId_t OLED_TaskHandle;

HAL_StatusTypeDef I2C_MemWrite(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
{
    while (HAL_I2C_GetState(&SSD1306_I2C_PORT) != HAL_I2C_STATE_READY) {
      osDelay(1);
    }

    if (HAL_I2C_Mem_Write(&SSD1306_I2C_PORT, DevAddress, MemAddress, MemAddSize, pData, Size, 100) != HAL_OK)
    {
      return HAL_ERROR;
    }

    uint32_t start = HAL_GetTick();
    while (HAL_I2C_GetState(&SSD1306_I2C_PORT) != HAL_I2C_STATE_READY) {
        if ((HAL_GetTick() - start) > 100) {
            return HAL_TIMEOUT;
        }
        osDelay(1);
    }
    // Wait DMA finish
    // if (osSemaphoreAcquire(i2c3TxSem, 100) != osOK) 
    // {
    //   return HAL_TIMEOUT;
    // }
    // osDelay(5);
    return HAL_OK;
}

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c == &SSD1306_I2C_PORT)
    {
      // osSemaphoreRelease(i2c3TxSem);
    }
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
  if (hi2c == &SSD1306_I2C_PORT)
  {
    // osSemaphoreRelease(i2c3TxSem);
  }

  /* здесь ловим таймаут, NACK, ошибки шины и т.д. */
  Error_Handler();
}

extern const unsigned char garfield_128x64 [];

void ProcessOLED()
{
  static uint32_t lastUpdate = 0;
  if (MENU_IsUpdateNeeded())
  {
    ssd1306_Fill(Black);
    MENU_Draw();
    ssd1306_UpdateScreen();
    lastUpdate = HAL_GetTick();
  }

  if (HAL_GetTick() - lastUpdate > 1000)
  {
    MENU_Update();
  }
}

void OLED_Init()
{  
  const osThreadAttr_t OLED_TaskAttributes = {
    .name = "OLED_Task",
    .stack_size = 256 * 4,
    .priority = (osPriority_t) osPriorityLow,
  };
  OLED_TaskHandle = osThreadNew(OLED_Task, NULL, &OLED_TaskAttributes);
  configASSERT(OLED_TaskHandle);
}

void OLED_Task(void *pvParameters)
{
    LOG_Debug("%s", __FUNCTION__);
    osDelay(1000);
    MENU_Init();
    MAIN_MENU_Init();
    MENU_SetActiveMenu(MAIN_MENU_GetMenu(), false);
    ssd1306_Init();
    for(;;)
    {
        ProcessOLED();
        osDelay(50);
    }
}
