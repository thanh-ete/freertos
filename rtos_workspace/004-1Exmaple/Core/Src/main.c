/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

#include "string.h"
#include "stdlib.h"
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
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
xTaskHandle HPT_Handler;
xTaskHandle MPT_Handler;
xTaskHandle LPT_Handler;
xTaskHandle VLPT_Handler;

void HighTask(void *argument);
void MediumTask(void *argument);
void LowTask(void *argument);
void VeryLowTask(void *argument);
SemaphoreHandle_t CountingSemaphore;

int source[3] = {11,22,33};
int idx = 0;
uint8_t rx_data = 0;
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
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&huart2, &rx_data, 1);
  CountingSemaphore = xSemaphoreCreateCounting(3,0);

  if(CountingSemaphore == NULL) HAL_UART_Transmit(&huart2, (uint8_t*)"Unable create Semaphore\n",24 , 1000);
  else HAL_UART_Transmit(&huart2, (uint8_t *) "Counting Semaphore created successfully\n\n", 41, 1000);

  xTaskCreate(HighTask, "HighTask", 128, NULL, 3, &HPT_Handler);
  xTaskCreate(MediumTask, "MediumTask", 128, NULL, 2, &MPT_Handler);
  xTaskCreate(LowTask, "LowTask", 128, NULL, 1, &LPT_Handler);
  xTaskCreate(VeryLowTask, "VLTask", 128, NULL, 0, &VLPT_Handler);
  vTaskStartScheduler();
  /* USER CODE END 2 */



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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
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
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
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
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HighTask(void *argument) {

	char ssource[3];
	char ssecount[2];
	int secount = 0;

	xSemaphoreGive(CountingSemaphore);
	xSemaphoreGive(CountingSemaphore);
	xSemaphoreGive(CountingSemaphore);

	while(1) {
				char str[150];
				strcpy(str, " Enter Medium Task and acquire Semaphore\n");
				secount = uxSemaphoreGetCount(CountingSemaphore);
				itoa(secount, ssecount, 10 );
				strcat(str, "Token available: ");
				strcat(str, ssecount);
				strcat (str, "\n\n");
				HAL_UART_Transmit(&huart2, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);

				xSemaphoreTake(CountingSemaphore, portMAX_DELAY);

				itoa(source[idx], ssource, 10);
				strcpy(str, "Leave Medium Task and data access: ");
				strcat(str, ssource);
				strcat(str,"\n Not release the Semaphore\n\n");
				HAL_UART_Transmit(&huart2, (uint8_t *)str, strlen (str), HAL_MAX_DELAY);

				idx++;
				if(idx > 2) idx = 0;
				vTaskDelay(3000);
	}
}

void MediumTask(void *argument) {
	char ssource[3];
	char ssecount[2];
	int secount = 0;


	while(1) {
		char str[150];
		strcpy(str, " Enter Medium Task and acquire Semaphore\n");
		secount = uxSemaphoreGetCount(CountingSemaphore);
		itoa(secount, ssecount, 10 );
		strcat(str, "Token available: ");
		strcat(str, ssecount);
		strcat (str, "\n\n");
		HAL_UART_Transmit(&huart2, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);

		xSemaphoreTake(CountingSemaphore, portMAX_DELAY);

		itoa(source[idx], ssource, 10);
		strcpy(str, "Leave Medium Task and data access: ");
		strcat(str, ssource);
		strcat(str,"\n Not release the Semaphore\n\n");
		HAL_UART_Transmit(&huart2, (uint8_t *)str, strlen (str), HAL_MAX_DELAY);

		idx++;
		if(idx > 2) idx = 0;
		vTaskDelay(2000);
	}
}

void LowTask(void *argument) {

	char ssource[3];
	char ssecount[2];
	int secount = 0;


	while(1) {
		char str[150];
		strcpy(str, " Enter Low Task and acquire Semaphore\n");
		secount = uxSemaphoreGetCount(CountingSemaphore);
		itoa(secount, ssecount, 10 );
		strcat(str, "Token available: ");
		strcat(str, ssecount);
		strcat (str, "\n\n");
		HAL_UART_Transmit(&huart2, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);

		xSemaphoreTake(CountingSemaphore, portMAX_DELAY);

		itoa(source[idx], ssource, 10);
		strcpy(str, "Leave Low Task and data access: ");
		strcat(str, ssource);
		strcat(str,"\n Not release the Semaphore\n\n");
		HAL_UART_Transmit(&huart2, (uint8_t *)str, strlen (str), HAL_MAX_DELAY);
		idx++;
		if(idx > 2) idx = 0;
		vTaskDelay(1000);
	}
}

void VeryLowTask(void *argument) {
	char ssource[3];
	char ssecount[2];
	int secount = 0;

	while(1) {
		char str[150];
		strcpy(str, " Enter VL Task and acquire Semaphore\n");
		secount = uxSemaphoreGetCount(CountingSemaphore);
		itoa(secount, ssecount, 10 );
		strcat(str, "Token available: ");
		strcat(str, ssecount);
		strcat (str, "\n\n");
		HAL_UART_Transmit(&huart2, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);

		xSemaphoreTake(CountingSemaphore, portMAX_DELAY);

		itoa(source[idx], ssource, 10);
		strcpy(str, "Leave VL Task and data access: ");
		strcat(str, ssource);
		strcat(str,"\n Not release the Semaphore\n\n");
		HAL_UART_Transmit(&huart2, (uint8_t *)str, strlen (str), HAL_MAX_DELAY);
		idx++;
		if(idx > 2) idx = 0;
		vTaskDelay(1000);
	}	while(1) {

	}
}

void  HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	HAL_UART_Receive_IT(huart, &rx_data, 1);
	if (rx_data == 'r')
	{
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		xSemaphoreGiveFromISR(CountingSemaphore, &xHigherPriorityTaskWoken);
		xSemaphoreGiveFromISR(CountingSemaphore, &xHigherPriorityTaskWoken);
		xSemaphoreGiveFromISR(CountingSemaphore, &xHigherPriorityTaskWoken);
		portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
	}

}
/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
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
  /* User can add his own implementation to report the HAL error return state */
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
