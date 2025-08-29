
#include "OLED/OLED.h"

#include "stm32f4xx_hal.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "ssd1306_tests.h"
#include "cmsis_os.h"

#include "LOG/LOG.h"
#include "main.h" // ErrorHandler declaration
#include "MAIN_MENU.h"

osThreadId_t OLED_TaskHandle;
const osThreadAttr_t OLED_TaskAttributes = {
  .name = "OLED_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

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
  static uint8_t b = 0;
  ssd1306_Fill(Black);
  MAIN_MENU_Draw();
  // if (b == 0)
  // {
  //   LOG_Debug("%s: Hello, OLDED! Fill", __FUNCTION__);
  //   ssd1306_SetCursor(3, 3);
  //   ssd1306_WriteString("Hello, OLED!", Font_7x10, White);
  //   ssd1306_FillRectangle(110, 0, 125, 15, White);
  //   ssd1306_FillCircle(20, 28, 10, White);
  // }
  // else if (b == 1)
  // {
  //   ssd1306_SetCursor(3, 3);
  //   LOG_Debug("%s: Hello, OLDED! Empty", __FUNCTION__);
  //   ssd1306_WriteString("Hello, OLED!", Font_7x10, White);
  //   ssd1306_DrawRectangle(110, 0, 125, 15, White);
  //   ssd1306_DrawCircle(20, 28, 10, White);
  // }
  // else if (b == 2)
  // {
  //   // ssd1306_TestDrawBitmap();
  //   LOG_Debug("%s: Garfield", __FUNCTION__);
  //   ssd1306_Fill(White);
  //   ssd1306_DrawBitmap(0,16,garfield_128x64,128,64,Black);
  //   ssd1306_UpdateScreen();
  // }

  b++;
  b = b > 2 ? 0 : b;
  ssd1306_UpdateScreen();
}

void OLED_Init()
{
    OLED_TaskHandle = osThreadNew(OLED_Task, NULL, &OLED_TaskAttributes);
    MAIN_MENU_Init();
}

void OLED_Task(void *pvParameters)
{
    LOG_Debug("%s", __FUNCTION__);
    ssd1306_Init();
    for(;;)
    {
        ProcessOLED();
        osDelay(1000);
    }
}