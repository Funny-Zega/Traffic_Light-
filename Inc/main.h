/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32f1xx_hal.h"
extern int red_duration;
extern int amber_duration;
extern int green_duration;
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
#include "input_reading.h"
#include "input_processing.h"

/* Exported variables */
#define NO_OF_BUTTONS 3
#define BUTTON_IS_PRESSED GPIO_PIN_RESET
#define BUTTON_IS_RELEASED GPIO_PIN_SET
#define DURATION_FOR_AUTO_INCREASING 100

// External variables
extern int mode;
extern int countdown;
extern int timer_flag[3];
extern int red_duration;
extern int amber_duration;
extern int green_duration;
extern const int MAX_LED;
extern int index_led;
extern int timer7SEG_counter;
extern int timer7SEG_flag;
extern int led_buffer[4];

// Function prototypes
void update7SEG(int index);
void updateClockBuffer(int count, int mode);
void setTimerLED(int duration);
void setTimer7SEG(int duration);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ENC_Pin GPIO_PIN_13
#define ENC_GPIO_Port GPIOC
#define ENC2_Pin GPIO_PIN_14
#define ENC2_GPIO_Port GPIOC
#define DPC_Pin GPIO_PIN_15
#define DPC_GPIO_Port GPIOC
#define LED_RED_Pin GPIO_PIN_1
#define LED_RED_GPIO_Port GPIOA
#define LED_AMBER_Pin GPIO_PIN_2
#define LED_AMBER_GPIO_Port GPIOA
#define LED_GREEN_Pin GPIO_PIN_3
#define LED_GREEN_GPIO_Port GPIOA
#define LED_TIMER_Pin GPIO_PIN_4
#define LED_TIMER_GPIO_Port GPIOA
#define LED_REDVer_Pin GPIO_PIN_5
#define LED_REDVer_GPIO_Port GPIOA
#define LED_AMBERVer_Pin GPIO_PIN_6
#define LED_AMBERVer_GPIO_Port GPIOA
#define LED_GREENVer_Pin GPIO_PIN_7
#define LED_GREENVer_GPIO_Port GPIOA
#define Button1_Pin GPIO_PIN_0
#define Button1_GPIO_Port GPIOB
#define Button2_Pin GPIO_PIN_1
#define Button2_GPIO_Port GPIOB
#define Button3_Pin GPIO_PIN_2
#define Button3_GPIO_Port GPIOB
#define g_Pin GPIO_PIN_12
#define g_GPIO_Port GPIOB
#define ENM_Pin GPIO_PIN_13
#define ENM_Pin GPIO_PIN_13
#define ENM_GPIO_Port GPIOB
#define ENM2_Pin GPIO_PIN_14
#define ENM2_GPIO_Port GPIOB
#define DPM_Pin GPIO_PIN_15
#define DPM_GPIO_Port GPIOB
#define EN1_Pin GPIO_PIN_11
#define EN1_GPIO_Port GPIOA
#define EN2_Pin GPIO_PIN_12
#define EN2_GPIO_Port GPIOA
#define EN3_Pin GPIO_PIN_13
#define EN3_GPIO_Port GPIOA
#define EN4_Pin GPIO_PIN_14
#define EN4_GPIO_Port GPIOA
#define a_Pin GPIO_PIN_4
#define a_GPIO_Port GPIOB
#define b_Pin GPIO_PIN_5
#define b_GPIO_Port GPIOB
#define c_Pin GPIO_PIN_6
#define c_GPIO_Port GPIOB
#define d_Pin GPIO_PIN_7
#define d_GPIO_Port GPIOB
#define e_Pin GPIO_PIN_8
#define e_GPIO_Port GPIOB
#define f_Pin GPIO_PIN_9
#define f_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
