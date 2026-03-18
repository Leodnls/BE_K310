/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attentionleo
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include <stdlib.h>
#include <string.h>
#include <math.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
UART_HandleTypeDef huart1;
TIM_HandleTypeDef htim14;

/* USER CODE BEGIN PV */
volatile uint8_t rx_byte = 0;
volatile uint32_t rx_count = 0;
volatile uint32_t uart_err_count = 0;
volatile uint32_t uart_last_err = 0;

volatile int32_t coord_x = 0;
volatile int32_t coord_y = 0;
volatile float servo_angle_deg = 0.0f;
volatile uint16_t pwm_us = 600;

volatile uint32_t frame_count = 0;
volatile uint32_t parse_err_count = 0;

char rx_line[32];
volatile uint8_t rx_line_idx = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM14_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void UART_Start_Receive(void)
{
    HAL_UART_Receive_IT(&huart1, (uint8_t *)&rx_byte, 1);
}

void PWM_SetPulse_us(uint16_t us)
{
    if (us < 600)  us = 600;
    if (us > 2600) us = 2600;

    pwm_us = us;
    __HAL_TIM_SET_COMPARE(&htim14, TIM_CHANNEL_1, us);
}

void Servo_SetAngleFromXY(int32_t x, int32_t y)
{
    float angle_deg;
    uint16_t pulse;

    /* On refuse les Y négatifs */
    if (y < 0)
    {
        parse_err_count++;
        return;
    }

    /* Cas spécial : origine */
    if ((x == 0) && (y == 0))
    {
        servo_angle_deg = 0.0f;
        PWM_SetPulse_us(600);
        return;
    }

    /* Angle en degrés entre 0 et 180 si y >= 0 */
    angle_deg = atan2f((float)y, (float)x) * 57.2957795f;

    if (angle_deg < 0.0f)   angle_deg = 0.0f;
    if (angle_deg > 180.0f) angle_deg = 180.0f;

    servo_angle_deg = angle_deg;

    /* Mapping angle -> PWM : 0°=600us, 180°=2600us */
    pulse = (uint16_t)(600.0f + (angle_deg * (2000.0f / 180.0f)) + 0.5f);
    PWM_SetPulse_us(pulse);
}

void ProcessRxLine(char *line)
{
    char *comma;
    char *endptr1;
    char *endptr2;
    long x;
    long y;

    comma = strchr(line, ',');
    if (comma == NULL)
    {
        parse_err_count++;
        return;
    }

    *comma = '\0';

    x = strtol(line, &endptr1, 10);
    y = strtol(comma + 1, &endptr2, 10);

    if ((*endptr1 != '\0') || (*endptr2 != '\0'))
    {
        parse_err_count++;
        return;
    }

    coord_x = (int32_t)x;
    coord_y = (int32_t)y;
    frame_count++;

    Servo_SetAngleFromXY(coord_x, coord_y);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        rx_count++;

        if (rx_byte == '\r')
        {
            /* Ignore CR */
        }
        else if (rx_byte == '\n')
        {
            rx_line[rx_line_idx] = '\0';

            if (rx_line_idx > 0)
            {
                ProcessRxLine(rx_line);
            }

            rx_line_idx = 0;
        }
        else
        {
            if (rx_line_idx < (sizeof(rx_line) - 1))
            {
                rx_line[rx_line_idx++] = (char)rx_byte;
            }
            else
            {
                /* Overflow buffer -> on jette la ligne */
                rx_line_idx = 0;
                parse_err_count++;
            }
        }

        HAL_UART_Receive_IT(&huart1, (uint8_t *)&rx_byte, 1);
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        uart_last_err = HAL_UART_GetError(huart);
        uart_err_count++;

        rx_line_idx = 0;
        HAL_UART_Receive_IT(&huart1, (uint8_t *)&rx_byte, 1);
    }
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

  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_TIM14_Init();

  /* USER CODE BEGIN 2 */
  HAL_TIM_PWM_Start(&htim14, TIM_CHANNEL_1);

  /* PWM présent dès le démarrage : 1 ms à 50 Hz */
  PWM_SetPulse_us(600);

  /* Réception UART */
  UART_Start_Receive();
  /* USER CODE END 2 */

  while (1)
  {
    /* USER CODE BEGIN WHILE */

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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                              | RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM14 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM14_Init(void)
{
  TIM_OC_InitTypeDef sConfigOC = {0};

  htim14.Instance = TIM14;
  htim14.Init.Prescaler = 7;
  htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim14.Init.Period = 19999;
  htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim14) != HAL_OK)
  {
    Error_Handler();
  }

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 1000;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim14, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_TIM_MspPostInit(&htim14);
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
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
}
#endif /* USE_FULL_ASSERT */
