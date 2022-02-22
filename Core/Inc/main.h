/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define RGB5_B_Pin GPIO_PIN_13
#define RGB5_B_GPIO_Port GPIOC
#define RGB5_G_Pin GPIO_PIN_14
#define RGB5_G_GPIO_Port GPIOC
#define RGB5_R_Pin GPIO_PIN_15
#define RGB5_R_GPIO_Port GPIOC
#define RGB4_B_Pin GPIO_PIN_0
#define RGB4_B_GPIO_Port GPIOC
#define RGB4_G_Pin GPIO_PIN_1
#define RGB4_G_GPIO_Port GPIOC
#define RGB4_R_Pin GPIO_PIN_2
#define RGB4_R_GPIO_Port GPIOC
#define SEQ_RST_Pin GPIO_PIN_0
#define SEQ_RST_GPIO_Port GPIOA
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define SSD1351_NSS_Pin GPIO_PIN_4
#define SSD1351_NSS_GPIO_Port GPIOA
#define SSD1351_SCK_Pin GPIO_PIN_5
#define SSD1351_SCK_GPIO_Port GPIOA
#define SSD1351_DC_Pin GPIO_PIN_6
#define SSD1351_DC_GPIO_Port GPIOA
#define SSD1351_MOSI_Pin GPIO_PIN_7
#define SSD1351_MOSI_GPIO_Port GPIOA
#define SSD1351_NRST_Pin GPIO_PIN_4
#define SSD1351_NRST_GPIO_Port GPIOC
#define RGB3_B_Pin GPIO_PIN_6
#define RGB3_B_GPIO_Port GPIOC
#define RGB3_G_Pin GPIO_PIN_7
#define RGB3_G_GPIO_Port GPIOC
#define RGB3_R_Pin GPIO_PIN_8
#define RGB3_R_GPIO_Port GPIOC
#define RGB2_B_Pin GPIO_PIN_9
#define RGB2_B_GPIO_Port GPIOC
#define RGB2_G_Pin GPIO_PIN_8
#define RGB2_G_GPIO_Port GPIOA
#define RGB2_R_Pin GPIO_PIN_9
#define RGB2_R_GPIO_Port GPIOA
#define RGB1_B_Pin GPIO_PIN_10
#define RGB1_B_GPIO_Port GPIOA
#define RGB1_G_Pin GPIO_PIN_11
#define RGB1_G_GPIO_Port GPIOA
#define RGB1_R_Pin GPIO_PIN_12
#define RGB1_R_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define BTN1_Pin GPIO_PIN_4
#define BTN1_GPIO_Port GPIOB
#define BTN1_EXTI_IRQn EXTI4_IRQn
#define BTN2_Pin GPIO_PIN_5
#define BTN2_GPIO_Port GPIOB
#define BTN2_EXTI_IRQn EXTI9_5_IRQn
#define BTN3_Pin GPIO_PIN_6
#define BTN3_GPIO_Port GPIOB
#define BTN3_EXTI_IRQn EXTI9_5_IRQn
#define BTN4_Pin GPIO_PIN_7
#define BTN4_GPIO_Port GPIOB
#define BTN4_EXTI_IRQn EXTI9_5_IRQn
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
