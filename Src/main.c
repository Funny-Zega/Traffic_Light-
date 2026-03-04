/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "input_reading.h"
#include "input_processing.h"

/* Private variables */
int mode = 1;
int countdown = 15;
int timer_flag[3] = {0, 0, 0};
int red_duration = 0;    // Default 15 seconds
int amber_duration = 1;   // Default 2 seconds
int green_duration = 1;  // Default 15 seconds
const int MAX_LED = 4;
int index_led = 0;
int led_buffer[4] = {0, 0, 0, 0};
int timer7SEG_counter = 0;
int timer7SEG_flag = 0;
int timerLED_counter = 0;
int timerLED_flag = 0;
const int TIMER_CYCLE = 10;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void display7Seg(int num) {
    uint8_t segments[10] = {
        0b1111110, // 0
        0b0110000, // 1
        0b1101101, // 2
        0b1111001, // 3
        0b0110011, // 4
        0b1011011, // 5
        0b1011111, // 6
        0b1110000, // 7
        0b1111111, // 8
        0b1111011  // 9
    };
    uint8_t data = segments[num % 10]; // Ensure single digit
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, !(data & 0x01)); // a
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, !(data & 0x02)); // b
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, !(data & 0x04)); // c
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, !(data & 0x08)); // d
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, !(data & 0x10)); // e
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, !(data & 0x20)); // f
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, !(data & 0x40)); // g
}



void update7SEG(int index) {
    if (index < 0 || index >= MAX_LED) return;
    switch (index) {
        case 0:
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, RESET); // EN1
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, SET);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, SET);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, SET);
            display7Seg(led_buffer[0]);
            break;
        case 1:
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, SET);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, RESET); // EN2
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, SET);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, SET);
            display7Seg(led_buffer[1]);
            break;
        case 2:
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, SET);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, SET);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, RESET); // EN3
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, SET);
            display7Seg(led_buffer[2]);
            break;
        case 3:
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, SET);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, SET);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, SET);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, RESET); // EN4
            display7Seg(led_buffer[3]);
            break;
        default:
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, SET);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, SET);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, SET);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, SET);
            break;
    }
}

void updateClockBuffer(int count, int mode) {
    if (mode == 1) {
        // Normal mode: LEDs 1 and 2 show countdown, LEDs 3 and 4 show '01'
        led_buffer[0] = count / 10;
        led_buffer[1] = count % 10;
        led_buffer[2] = 0;
        led_buffer[3] = 1;
    } else {
        // Modification modes: LEDs 1 and 2 show temp_duration, LEDs 3 and 4 show '0' and mode
        led_buffer[0] = (count < 10) ? 0 : count / 10; // Ensure leading zero if < 10
        led_buffer[1] = count % 10; // Units digit
        led_buffer[2] = 0; // Leading zero for mode
        led_buffer[3] = mode; // Mode number (2, 3, 4)
    }
}



void setTimerLED(int duration) {
    timerLED_counter = duration / TIMER_CYCLE;
    timerLED_flag = 0;
    timer_flag[0] = 0;
}

void setTimer7SEG(int duration) {
    timer7SEG_counter = duration / TIMER_CYCLE;
    timer7SEG_flag = 0;
    timer_flag[1] = 0;
}

void timer_run() {
    if (timerLED_counter > 0) {
        timerLED_counter--;
        if (timerLED_counter == 0) {
            timerLED_flag = 1;
            timer_flag[0] = 1;
        }
    }
    if (timer7SEG_counter > 0) {
        timer7SEG_counter--;
        if (timer7SEG_counter == 0) {
            timer7SEG_flag = 1;
            timer_flag[1] = 1;
        }
    }
    HAL_GPIO_TogglePin(LED_TIMER_GPIO_Port, LED_TIMER_Pin);
}

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
    MX_TIM2_Init();
    /* USER CODE BEGIN 2 */
    HAL_TIM_Base_Start_IT(&htim2);
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    setTimerLED(1000);
    setTimer7SEG(250);
    while (1) {
        fsm_for_input_processing();

        if (mode == 1) {
            fsm_mode_normal_run();
        }
        if(mode >= 2 && mode<=4){
			if (timer7SEG_flag == 1) {
		        set_blinking_led(mode);
				update7SEG(index_led);
				index_led = (index_led + 1) % MAX_LED;
				setTimer7SEG(250);
			}
        }
    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 7999;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 9;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOC, ENC_Pin|ENC2_Pin|DPC_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, LED_RED_Pin|LED_AMBER_Pin|LED_GREEN_Pin|LED_TIMER_Pin
                            |LED_REDVer_Pin|LED_AMBERVer_Pin|LED_GREENVer_Pin|EN1_Pin
                            |EN2_Pin|EN3_Pin|EN4_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, g_Pin|ENM_Pin|ENM2_Pin|DPM_Pin
                            |a_Pin|b_Pin|c_Pin|d_Pin
                            |e_Pin|f_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pins : ENC_Pin ENC2_Pin DPC_Pin */
    GPIO_InitStruct.Pin = ENC_Pin|ENC2_Pin|DPC_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /*Configure GPIO pins : LED_RED_Pin LED_AMBER_Pin LED_GREEN_Pin LED_TIMER_Pin
                             LED_REDVer_Pin LED_AMBERVer_Pin LED_GREENVer_Pin EN1_Pin
                             EN2_Pin EN3_Pin EN4_Pin */
    GPIO_InitStruct.Pin = LED_RED_Pin|LED_AMBER_Pin|LED_GREEN_Pin|LED_TIMER_Pin
                            |LED_REDVer_Pin|LED_AMBERVer_Pin|LED_GREENVer_Pin|EN1_Pin
                            |EN2_Pin|EN3_Pin|EN4_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pins : Button1_Pin Button2_Pin Button3_Pin */
    GPIO_InitStruct.Pin = Button1_Pin|Button2_Pin|Button3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /*Configure GPIO pins : g_Pin ENM_Pin ENM2_Pin DPM_Pin
                             a_Pin b_Pin c_Pin d_Pin
                             e_Pin f_Pin */
    GPIO_InitStruct.Pin = g_Pin|ENM_Pin|ENM2_Pin|DPM_Pin
                            |a_Pin|b_Pin|c_Pin|d_Pin
                            |e_Pin|f_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    timer_run();
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    __disable_irq();
    while (1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
