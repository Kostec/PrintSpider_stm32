/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include "semphr.h"
#include <Print/print.h>
#include <LED/ws2812.h>
#include "LOG/LOG.h"
#include "DIO/DIO.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "ssd1306_tests.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c3;
DMA_HandleTypeDef hdma_i2c3_tx;

SPI_HandleTypeDef hspi3;
DMA_HandleTypeDef hdma_spi3_rx;
DMA_HandleTypeDef hdma_spi3_tx;

UART_HandleTypeDef huart2;
UART_HandleTypeDef huart6;
DMA_HandleTypeDef hdma_usart2_tx;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for printTask */
osThreadId_t printTaskHandle;
const osThreadAttr_t printTask_attributes = {
  .name = "printTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for DisplayTask */
osThreadId_t DisplayTaskHandle;
const osThreadAttr_t DisplayTask_attributes = {
  .name = "DisplayTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for myQueue01 */
osMessageQueueId_t myQueue01Handle;
const osMessageQueueAttr_t myQueue01_attributes = {
  .name = "myQueue01"
};
/* Definitions for spi3_sem */
osSemaphoreId_t spi3_semHandle;
const osSemaphoreAttr_t spi3_sem_attributes = {
  .name = "spi3_sem"
};
/* USER CODE BEGIN PV */

SemaphoreHandle_t i2c3TxSem;
const osSemaphoreAttr_t i2c3_sem_attributes = {
  .name = "i2c3_sem"
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART6_UART_Init(void);
static void MX_SPI3_Init(void);
static void MX_I2C3_Init(void);
void StartDefaultTask(void *argument);
void StartPrintTask02(void *argument);
void StartDisplayTask(void *argument);

/* USER CODE BEGIN PFP */
HAL_StatusTypeDef I2C_MemWrite(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
{
    while (HAL_I2C_GetState(&SSD1306_I2C_PORT) != HAL_I2C_STATE_READY) {
      osDelay(1);
    }

    if (HAL_I2C_Mem_Write(&SSD1306_I2C_PORT, DevAddress, MemAddress, MemAddSize, pData, Size, HAL_MAX_DELAY) != HAL_OK)
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

//==============================MicroSD BEGIN==============================
#define SD_CS_LOW()  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET)
#define SD_CS_HIGH() HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET)

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    BaseType_t xHigherPriorityWoken = pdFALSE;
    if (hspi == &hspi3) 
    {
      xSemaphoreGiveFromISR(spi3_semHandle, &xHigherPriorityWoken);
      portYIELD_FROM_ISR(xHigherPriorityWoken);
    }
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
    BaseType_t xHigherPriorityWoken = pdFALSE;
    if (hspi == &hspi3) 
    {
      xSemaphoreGiveFromISR(spi3_semHandle, &xHigherPriorityWoken);
      portYIELD_FROM_ISR(xHigherPriorityWoken);
    }
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
    BaseType_t xHigherPriorityWoken = pdFALSE;
    if (hspi == &hspi3) 
    {
      xSemaphoreGiveFromISR(spi3_semHandle, &xHigherPriorityWoken);
      portYIELD_FROM_ISR(xHigherPriorityWoken);
    }
}

HAL_StatusTypeDef SPI3_Transmit_DMA(const uint8_t *txBuf, uint16_t len)
{
  SD_CS_LOW();
  vPortEnterCritical();
  HAL_StatusTypeDef res = HAL_SPI_Transmit_DMA(&hspi3, txBuf, len);
  vPortExitCritical();
  if (res != HAL_OK)
  {
    SD_CS_LOW();
    return HAL_ERROR;
  }

  // Wait DMA finish
  if (osSemaphoreAcquire(spi3_semHandle, osWaitForever) != osOK) 
  {
    SD_CS_HIGH();
    return HAL_TIMEOUT;
  }

  SD_CS_HIGH();
  return HAL_OK;
}

HAL_StatusTypeDef SPI3_Receive_DMA(uint8_t *txBuf, uint16_t len)
{
  SD_CS_LOW();
  
  vPortEnterCritical();
  HAL_StatusTypeDef res = HAL_SPI_Receive_DMA(&hspi3, txBuf, len);
  vPortExitCritical();
  if (res != HAL_OK)
  {
    SD_CS_LOW();
    return HAL_ERROR;
  }

  // Wait DMA finish
  if (osSemaphoreAcquire(spi3_semHandle, osWaitForever) != osOK) 
  {
    SD_CS_HIGH();
    return HAL_TIMEOUT;
  }

  SD_CS_HIGH();
  return HAL_OK;
}

HAL_StatusTypeDef SPI3_TransmitReceive_DMA(const uint8_t *txBuf, uint8_t *rxBuf, uint16_t len, uint32_t timeout) {
  SD_CS_LOW();

  vPortEnterCritical();
  HAL_StatusTypeDef res = HAL_SPI_TransmitReceive_DMA(&hspi3, txBuf, rxBuf, len);
  vPortExitCritical();
  if (res != HAL_OK) {
      SD_CS_HIGH();
      return HAL_ERROR;
  }

  // Wait DMA finish
  if (osSemaphoreAcquire(spi3_semHandle, timeout) != osOK) {
      SD_CS_HIGH();
      return HAL_TIMEOUT;
  }

  SD_CS_HIGH();
  return HAL_OK;
}
//==============================MicroSD END==============================

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_USART6_UART_Init();
  MX_SPI3_Init();
  MX_FATFS_Init();
  MX_I2C3_Init();
  /* USER CODE BEGIN 2 */
  LOG_Init();
  DIO_Init();
  // LOG_Debug("Init RTOS");
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  // LOG_Debug("USER RTOS_MUTEX definitions");
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of spi3_sem */
  spi3_semHandle = osSemaphoreNew(1, 0, &spi3_sem_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  // LOG_Debug("USER RTOS_SEMAPHORES definitions");
  i2c3TxSem = osSemaphoreNew(1, 0, &i2c3_sem_attributes);
  configASSERT(i2c3TxSem);
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  // LOG_Debug("USER RTOS_TIMERS definitions");
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of myQueue01 */
  myQueue01Handle = osMessageQueueNew (16, sizeof(uint16_t), &myQueue01_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  // LOG_Debug("USER RTOS_QUEUES definitions");
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of printTask */
  printTaskHandle = osThreadNew(StartPrintTask02, NULL, &printTask_attributes);

  /* creation of DisplayTask */
  DisplayTaskHandle = osThreadNew(StartDisplayTask, NULL, &DisplayTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  // LOG_Debug("USER BEGIN definitions");
  /* add threads, ... */
  // LOG_Debug("Start scheduler");
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 96;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.ClockSpeed = 100000;
  hi2c3.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

}

/**
  * @brief SPI3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI3_Init 0 */

  /* USER CODE END SPI3_Init 0 */

  /* USER CODE BEGIN SPI3_Init 1 */

  /* USER CODE END SPI3_Init 1 */
  /* SPI3 parameter configuration*/
  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI3_Init 2 */

  /* USER CODE END SPI3_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 115200;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
  /* DMA1_Stream4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
  /* DMA1_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4
                          |GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1|LED_GREEN_Pin|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_10
                          |GPIO_PIN_12|GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI3_CS_SD_GPIO_Port, SPI3_CS_SD_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC1 PC2 PC3 PC4
                           PC5 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4
                          |GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA1 LED_GREEN_Pin PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|LED_GREEN_Pin|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 PB10
                           PB12 PB13 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_10
                          |GPIO_PIN_12|GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : SPI3_CS_SD_Pin */
  GPIO_InitStruct.Pin = SPI3_CS_SD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SPI3_CS_SD_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */

void SD_loop()
{
  static uint8_t mounted = 0;
  if (!mounted)
  {
    FRESULT res = f_mount(&USERFatFS, USERPath, 1);
    if (res != FR_OK)
    {
      LOG_Debug("%s: SD card not found. Error: %d", __FUNCTION__, res);
    }
    else
    {
      LOG_Debug("%s: SD card is available", __FUNCTION__);
      mounted = 1;
    }
  }
  else
  {
    // uint32_t freeClusters;
    // FATFS* pUSERFatFS = &USERFatFS;
    // LOG_Debug("%s: read free clusters", __FUNCTION__);
    // f_getfree(USERPath, &freeClusters, &pUSERFatFS);
    // osDelay(50);
    // LOG_Debug("%s: Free clusters=%lu", __FUNCTION__, freeClusters);
    DIR dir;
    FILINFO fno;
    LOG_Debug("%s: opendir", __FUNCTION__);
    FRESULT res = f_opendir(&dir, USERPath);
    osDelay(50);
    if (res == FR_OK) {
        LOG_Debug("%s: readdir", __FUNCTION__);
        while (f_readdir(&dir, &fno) == FR_OK && fno.fname[0]) {
          LOG_Debug("%s: filename=%s", __FUNCTION__, fno.fname);
          osDelay(50);
        }
        osDelay(50);
        f_closedir(&dir);
        osDelay(50);
    }
    else {
      mounted = 0;
    }
  }
}
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  osDelay(2000);
  for(;;)
  {
    SD_loop();
    osDelay(3000);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartPrintTask02 */
/**
* @brief Function implementing the printTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartPrintTask02 */
void StartPrintTask02(void *argument)
{
  /* USER CODE BEGIN StartPrintTask02 */
  /* Infinite loop */
  for(;;)
  {
    LOG_Debug("%s: TogglePin", __FUNCTION__);
    HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
    // PRINT_setup();
    // PRINT_print();
    osDelay(1000);
  }
  /* USER CODE END StartPrintTask02 */
}

/* USER CODE BEGIN Header_StartDisplayTask */
extern const unsigned char garfield_128x64 [];

void ProcessOLED()
{
  static uint8_t b = 0;
  ssd1306_Fill(Black);
  if (b == 0)
  {
    LOG_Debug("%s: Hello, OLDED! Fill", __FUNCTION__);
    ssd1306_SetCursor(3, 3);
    ssd1306_WriteString("Hello, OLED!", Font_7x10, White);
    ssd1306_FillRectangle(110, 0, 125, 15, White);
    ssd1306_FillCircle(20, 28, 10, White);
  }
  else if (b == 1)
  {
    ssd1306_SetCursor(3, 3);
    LOG_Debug("%s: Hello, OLDED! Empty", __FUNCTION__);
    ssd1306_WriteString("Hello, OLED!", Font_7x10, White);
    ssd1306_DrawRectangle(110, 0, 125, 15, White);
    ssd1306_DrawCircle(20, 28, 10, White);
  }
  else if (b == 2)
  {
    // ssd1306_TestDrawBitmap();
    LOG_Debug("%s: Garfield", __FUNCTION__);
    ssd1306_Fill(White);
    ssd1306_DrawBitmap(0,16,garfield_128x64,128,64,Black);
    ssd1306_UpdateScreen();
  }

  b++;
  b = b > 2 ? 0 : b;
  ssd1306_UpdateScreen();
}

/**
* @brief Function implementing the DisplayTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDisplayTask */
void StartDisplayTask(void *argument)
{
  /* USER CODE BEGIN StartDisplayTask */
  LOG_Debug("%s", __FUNCTION__);
  ssd1306_Init();
  /* Infinite loop */
  for(;;)
  {
    ProcessOLED();
    osDelay(1000);
  }
  /* USER CODE END StartDisplayTask */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM11 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM11)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  // printf("%s", __FUNCTION__);
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
