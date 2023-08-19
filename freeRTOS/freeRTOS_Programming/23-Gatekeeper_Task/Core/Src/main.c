
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



void digital_sensor_task(void *pvParameters);
void analog_sensor_task(void *pvParameters);
void LCD_task(void *pvParameters);				//Gatekeeper Task


QueueHandle_t xPrintQueue;

int main(void)
{

  HAL_Init();		/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  SystemClock_Config();		/* Configure the system clock */
  MX_GPIO_Init();		    /* Initialize all configured peripherals */
  USART2_UART_TX_Init();


  printf("System Initializing...\n\r");


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

  xTaskCreate(LCD_task,					//Task 3
		  "GateKeeper",
		  128,
		  NULL,
		  0,
		  NULL);

  xPrintQueue=xQueueCreate(2,sizeof(int32_t));		//Create a queue



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

		xQueueSendToBack(xPrintQueue,&button_state,0);		//Send the button_state data to queue

		vTaskDelay(10);
	}

}

void analog_sensor_task(void *pvParameters)
{
	adc_init();

	while(1)
	{
		sensor_value=read_analog_sensor();

		xQueueSendToBack(xPrintQueue,&sensor_value,0);		//Send the sensor_value data to queue

		vTaskDelay(10);

	}

}

int value_to_print;

void LCD_task(void *pvParameters)				//Gatekeeper Task
{
	while(1)
	{
		xQueueReceive(xPrintQueue, &value_to_print, portMAX_DELAY);

		printf("New vale : %d \r\n",value_to_print);

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
