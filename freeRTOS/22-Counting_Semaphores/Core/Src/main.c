
#include "main.h"
#include "cmsis_os.h"
#include <stdio.h>

#include "uart_driver.h"
#include "exti_driver.h"
#include "adc_driver.h"

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);

int __io_putchar(int ch);

uint8_t button_state;
uint32_t sensor_value;

SemaphoreHandle_t xSerialSemaphore;		//This semaphore is used to access the serial port


void digital_sensor_task(void *pvParameters);
void analog_sensor_task(void *pvParameters);


int main(void)
{

  HAL_Init();		/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  SystemClock_Config();		/* Configure the system clock */
  MX_GPIO_Init();		    /* Initialize all configured peripherals */
  USART2_UART_TX_Init();


  printf("System Initializing...\n\r");

  xSerialSemaphore=xSemaphoreCreateCounting(1,0);		//Create a Counting Semaphore with max_count=1 and min_count=0

  xTaskCreate(digital_sensor_task,		//Task 1
		  "ButtonRead",
		  128,
		  NULL,
		  1,
		  NULL);

  xTaskCreate(analog_sensor_task,		//Task 2
		  "SensorRead",
		  128,
		  NULL,
		  1,
		  NULL);

  xSemaphoreGive(xSerialSemaphore);		//First give the semaphore

  /*
   * At first you can give the semaphore or if you put the max_count and min_count the same number,
   * then the semaphore will be automatically given to the task.
   * Then there is no need to add the command xSemaphoreGive(xSerialSemaphore); at the beginning.
   *
   */


  vTaskStartScheduler();

  while (1)
  {

  }
}

void digital_sensor_task(void *pvParameters)
{
	gpio_init();

	while(1)
	{
		button_state=read_digital_sensor();

		/*Difference between Binary semaphore, Counting semaphore and a Mutex  is, at the beginning of mutex we don't need to give the semaphore,
		 *we just check if the semaphore is available or not.
		 *Whereas in binary semaphore and Counting Semaphore, someone has to give the semaphore at the beginning or else it will end up in DEADLOCK.
		 */


		if(xSemaphoreTake(xSerialSemaphore,(TickType_t)5)==pdTRUE)
			/*
			 * See if we can obtain or 'Take' the Serial Semaphore.
			 * If the semaphore is not available,wait 5 ticks for scheduler to see if it becomes free
			*/
		{
			printf("The button state is : %d  \r\n",button_state);

			xSemaphoreGive(xSerialSemaphore);		//Now free or give the serial port to others

		}
		vTaskDelay(1);
	}

}

void analog_sensor_task(void *pvParameters)
{
	adc_init();

	while(1)
	{
		sensor_value=read_analog_sensor();

		if(xSemaphoreTake(xSerialSemaphore,(TickType_t)5)==pdTRUE)
		{
			printf("The sensor value is : %ld  \r\n",sensor_value);

			xSemaphoreGive(xSerialSemaphore);
		}
		vTaskDelay(1);

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
