/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stdlib.h"
#include "uart.h"
#include <string.h>
#include <stdio.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

// Bucket structure to hold Bucket information
typedef struct Bucket {
	uint32_t period;
	uint32_t count;
} Bucket;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
// Private constants
const short MAX_PULSES_READ = 1000;
const uint32_t MAX_PERIOD = 1000000;
const uint16_t MIN_PERIOD = 50;
const uint16_t BUCKET_AMOUNT = 100;
const uint8_t  CARRAIGE_RETURN = 13;
const uint32_t DEFAULT_PERIOD = 1000;
const char MESSAGE[] = "Enter expected period or Press Enter if no change (DEFAULT 1000)";
// Private variables
char buffer[50];
struct Bucket buckets[100];

uint32_t prev_time = 0, curr_time, diff_time;
uint32_t period;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */
// runs the power on self-test. Returns true if the test passes, false otherwise
_Bool power_on_self_test(void);
// ask user for expected period, sets timer clock accordingly. Return period or 0 if invalid
int set_time_base(uint16_t base);
// Captures 1 line of text from console. Returns nul terminated string when \n is entered
void get_line (char *buffer, int max_length);
// Gets the amount of pulses specified and puts information into the given bucket array
void get_pulses(struct Bucket *buckets, int max_pulses);
// Adds a period to a bucket
void add_to_bucket(struct Bucket *buckets, uint32_t pulses);
// Prints out the buckets statistics to the terminal through UART
void print_buckets(struct Bucket *buckets);
// Prints out given String
void print(char *str);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// USER FUNCTIONS
_Bool power_on_self_test() {
	uint32_t first, second, difference;

	// Print post
	print("POST: Performing...");

	// Get the first capture
	while(!(TIM2->SR & TIM_SR_CC1IF));
	first = TIM2->CCR1;

	while(!(TIM2->SR & TIM_SR_CC1IF));
	second = TIM2->CCR1;

	difference = second - first;
	if (difference > (100 * 1000)) {
		print("POST: Failed, did not receive a pulse less than 100 milliseconds long");
		return 0;
	}

	print("POST: Successful");
	return 1;

}

int set_timer_base(uint16_t base) {
	char buff[30];

	if (base < MIN_PERIOD) return 0;
	else if (base > MAX_PERIOD ) return 0;
	else {
		for(int i = 0; i < BUCKET_AMOUNT; i++) {
			buckets[i].period = (base - 50) + i;
		}
		sprintf(buff, "Expected period set to %d", base);
		print(buff);
		return base;
	}
}

void get_line(char *buffer, int max_length) {
	uint16_t count = 0;

	do {
		// Get last entered Input
		buffer[count] = USART_Read(USART2);
		// Write it to the screen
		USART_Write(USART2, &buffer[count], 1);
		// increase count so we know amount of characters inputed
		count++;
	}
	// While we do no have an enter input
	while(buffer[count-1] != CARRAIGE_RETURN);
}

void get_pulses(struct Bucket *buckets, int max_pulses) {
	uint8_t  pulses = 0;

	print("Reading Periods...");

	while (pulses < MAX_PULSES_READ) {
		// Check the timer 2 capture flag (CCXIF flag (TIMx_SR register)) to go high and retrieve from CCR register
		if (TIM2->SR & TIM_SR_CC1IF) {
			// First pulse
			if (prev_time == 0) { prev_time = TIM2->CCR1; }
			// Past first pulse
			else {
				// Get the current time
				curr_time = TIM2->CCR1;
				// Get the difference time
				diff_time = curr_time - prev_time;
				// Set the current time to the previous time
				prev_time = curr_time;
				// Increase count of pulses
				pulses++;
				// Update count with frequency
				add_to_bucket(buckets, diff_time);
			}
		}
	}
}

void add_to_bucket(struct Bucket *buckets, uint32_t period) {
	for	(int i = 0; i < BUCKET_AMOUNT; i++) {
		if (buckets[i].period == period) {
			buckets[i].count = buckets[i].count + 1;
			return;
		}
	}
}

void print_buckets(struct Bucket *buckets) {
	char message[30];
	uint16_t n;

	print("\r\n BUCKETS");

	for (int i = 0; i < BUCKET_AMOUNT; i++) {
		if (buckets[i].count != 0) {
			n = sprintf(message, "Period: %d, Count: %d\r\n", buckets[i].period, buckets[i].count);
			USART_Write(USART2, message, n);
		}
	}
}

void print(char *str) {
	uint32_t n = sprintf(str, str);
	USART_Write(USART2, str, n);
	USART_Write(USART2, "\r\n", 2);
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
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  // Check to make sure power is on
  while( power_on_self_test() == 0);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		// 1. Print “Enter expected period or <CR> if no change�?. Wait for user response
		print(MESSAGE);
		get_line(buffer, 10);
		if (buffer[0] == CARRAIGE_RETURN) {
			period = DEFAULT_PERIOD;
		}
		else {
			period = atoi(buffer);
		}

		// 2. if yes, read new period then set up timer clock
		while( set_timer_base(period) == 0 );

		// 3. read 100 pulses
		get_pulses(buckets, MAX_PULSES_READ);

		// 4. print out results
		print_buckets(buckets);
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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
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

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 79;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4294967295;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */
  HAL_TIM_Base_Start(&htim2);
  HAL_TIM_IC_Start(&htim2, TIM_CHANNEL_1);
  /* USER CODE END TIM2_Init 2 */

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
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
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
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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
