/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define SD_SPI_HANDLE hspi3
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_GREEN_Pin GPIO_PIN_5
#define LED_GREEN_GPIO_Port GPIOA
#define MOTOR_Y_DIR_Pin GPIO_PIN_6
#define MOTOR_Y_DIR_GPIO_Port GPIOA
#define MOTOR_Y_STEP_Pin GPIO_PIN_7
#define MOTOR_Y_STEP_GPIO_Port GPIOA
#define S4_Pin GPIO_PIN_1
#define S4_GPIO_Port GPIOB
#define S5_Pin GPIO_PIN_2
#define S5_GPIO_Port GPIOB
#define S1_Pin GPIO_PIN_13
#define S1_GPIO_Port GPIOB
#define S2_Pin GPIO_PIN_14
#define S2_GPIO_Port GPIOB
#define S3_Pin GPIO_PIN_15
#define S3_GPIO_Port GPIOB
#define MOTOR_X_STEP_Pin GPIO_PIN_6
#define MOTOR_X_STEP_GPIO_Port GPIOC
#define MOTOR_EN_Pin GPIO_PIN_7
#define MOTOR_EN_GPIO_Port GPIOC
#define MOTOR_X_DIR_Pin GPIO_PIN_8
#define MOTOR_X_DIR_GPIO_Port GPIOC
#define PWRB_Pin GPIO_PIN_3
#define PWRB_GPIO_Port GPIOB
#define PWRA_Pin GPIO_PIN_4
#define PWRA_GPIO_Port GPIOB
#define D3_Pin GPIO_PIN_5
#define D3_GPIO_Port GPIOB
#define D2_Pin GPIO_PIN_6
#define D2_GPIO_Port GPIOB
#define D1_Pin GPIO_PIN_7
#define D1_GPIO_Port GPIOB
#define DCLK_Pin GPIO_PIN_8
#define DCLK_GPIO_Port GPIOB
#define CSYNC_Pin GPIO_PIN_9
#define CSYNC_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
