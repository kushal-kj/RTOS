/* This project is to read how to set the event bits and how to read the event bits
 *
 */

#include "main.h"
#include "cmsis_os.h"
#include <stdio.h>

#include "uart_driver.h"
#include "exti_driver.h"
#include "adc_driver.h"

#define TASK1_BIT (1UL<<0UL)
#define TASK2_BIT (1UL<<1UL)


void SystemClock_Config(void);
static void MX_GPIO_Init(void);

int __io_putchar(int ch);

EventGroupHandle_t xEventGroup;

void vEventBitSettingTask(void *pvParameters);
void vEventBitReadingTask(void *pvParameters);



int main(void)
{

  HAL_Init();		/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  SystemClock_Config();		/* Configure the system clock */
  MX_GPIO_Init();		    /* Initialize all configured peripherals */

  USART2_UART_TX_Init();

  xEventGroup =xEventGroupCreate();

  xTaskCreate(vEventBitSettingTask,"BitSetter",100,NULL,1,NULL);
  xTaskCreate(vEventBitReadingTask,"BitReader",100,NULL,1,NULL);

  vTaskStartScheduler();



  while (1)
  {

  }

}

void vEventBitSettingTask(void *pvParameters)
{
	const TickType_t xDelay500ms=pdMS_TO_TICKS(500UL);

	while(1)
	{
		vTaskDelay(xDelay500ms);
		printf("Bit setting task -\t about to set bit 0. \r\n");

		xEventGroupSetBits(xEventGroup,TASK1_BIT);

		vTaskDelay(xDelay500ms);
		printf("Bit setting task -\t about to set bit 1. \r\n");

		xEventGroupSetBits(xEventGroup,TASK2_BIT);

	}
}

void vEventBitReadingTask(void *pvParameters)
{
	EventBits_t xEventGroupValue;
	const EventBits_t xBitsToWaitFOr=(TASK1_BIT | TASK2_BIT);
	//const TickType_t xDelay200ms = pdMS_TO_TICKS(200UL);

	while(1)
	{
		xEventGroupValue = xEventGroupWaitBits(xEventGroup, xBitsToWaitFOr, pdTRUE, pdFALSE, portMAX_DELAY);

		if ((xEventGroupValue & TASK1_BIT)!=0)
		{
			printf("BIT Reading Task : Event bit 0 set - Task 1 \r\n");
		}
		if ((xEventGroupValue & TASK2_BIT)!=0)
		{
			printf("BIT Reading Task : Event bit 0 set - Task 2 \r\n");
		}

	}
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */


/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */


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