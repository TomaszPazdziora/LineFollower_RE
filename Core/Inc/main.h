/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include <stdbool.h>
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_CTR_Pin GPIO_PIN_13
#define LED_CTR_GPIO_Port GPIOC
#define DRIVR2_Pin GPIO_PIN_15
#define DRIVR2_GPIO_Port GPIOA
#define DRIVR1_Pin GPIO_PIN_3
#define DRIVR1_GPIO_Port GPIOB
#define DRIVL2_Pin GPIO_PIN_4
#define DRIVL2_GPIO_Port GPIOB
#define DRIVL1_Pin GPIO_PIN_5
#define DRIVL1_GPIO_Port GPIOB
#define LED_SENSOR_Pin GPIO_PIN_8
#define LED_SENSOR_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define SENSOR_NUMBER 14
#define TRESHOLD 250
#define CLEARED_READING 2048
#define SENSOR_DELAY 200

#define PRESC_VAL 1
#define ARR_VAL 999
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
