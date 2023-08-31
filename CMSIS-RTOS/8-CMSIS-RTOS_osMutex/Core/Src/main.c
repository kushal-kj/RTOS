
#include "main.h"
#include "cmsis_os.h"
#include <stdio.h>
#include <stdbool.h>

UART_HandleTypeDef huart2;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);

int __io_putchar(int ch);

void vGreenLedControllerTask(void *pvParameters);
void vRedLedControllerTask(void *pvParameters);
void vOrangeLedControllerTask(void *pvParameters);
void vBlueLedControllerTask(void *pvParameters);


#define STACK_SIZE 512

typedef uint32_t TaskProfiler_t;

TaskProfiler_t Orange_TaskProfiler,Red_TaskProfiler,Green_TaskProfiler,Blue_TaskProfiler;

osThreadId_t  red_id,green_id,blue_id,orange_id;

osTimerId_t timer1_id,timer2_id;

TaskProfiler_t timer1_Taskprofiler,timer2_Taskprofiler;

void Timer1_callback(void *arg);
void Timer2_callback(void *arg);

const osMutexAttr_t uartMutexAtrribs={
		.name="Uart Mutex"
};

osMutexId_t uart_mut_id;


int main(void)
{

  HAL_Init();		/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  SystemClock_Config();		/* Configure the system clock */
  MX_GPIO_Init();		    /* Initialize all configured peripherals */
  MX_USART2_UART_Init();

  //Initialize the kernel
  osKernelInitialize();

  //Create timers
  timer1_id=osTimerNew(Timer1_callback,osTimerOnce, NULL, NULL);			//One-shot timer
  timer2_id=osTimerNew(Timer2_callback,osTimerPeriodic, NULL, NULL);		//Periodic timer


  //Create Threads

  osThreadAttr_t greenThreadAtrribs={
									  .name="GreenTask",
									  .stack_size = STACK_SIZE,
									  .priority=osPriorityNormal

  	  	  	  	  	  	  	  	  	};

  green_id = osThreadNew(vGreenLedControllerTask, NULL, &greenThreadAtrribs);

  osThreadAttr_t redThreadAtrribs={
									  .name="RedTask",
									  .stack_size = STACK_SIZE,
									  .priority=osPriorityNormal1

  	  	  	  	  	  	  	  	  	};

  red_id = osThreadNew(vRedLedControllerTask, NULL, &redThreadAtrribs);

  osThreadAttr_t orangeThreadAtrribs={
									  .name="OrangeTask",
									  .stack_size = STACK_SIZE,
									  .priority=osPriorityNormal2

  	  	  	  	  	  	  	  	  	};

  orange_id = osThreadNew(vOrangeLedControllerTask, NULL, &orangeThreadAtrribs);

  osThreadAttr_t blueThreadAtrribs={
									  .name="BlueTask",
									  .stack_size = STACK_SIZE,
									  .priority=osPriorityNormal3

  	  	  	  	  	  	  	  	  	};

  blue_id = osThreadNew(vBlueLedControllerTask, NULL, &blueThreadAtrribs);


  uart_mut_id = osMutexNew(&uartMutexAtrribs);		//Create mutex

  //Start the kernel
  osKernelStart();

  while (1)
  {

  }

}


void vGreenLedControllerTask(void *pvParameters)
{
	osTimerStart(timer1_id, 5000);		//Set timer 1 period to 5000 ticks
	osTimerStart(timer2_id, 1000);		//Set timer 2 period to 1000 ticks


	while(1)
	{
		Green_TaskProfiler++;

		osMutexAcquire(uart_mut_id, osWaitForever);
		printf("This is BlueTask \r\n");
		osMutexRelease(uart_mut_id);
		osDelay(100);
	}
}


void vRedLedControllerTask(void *pvParameters)
{
	while(1)
	{
		Red_TaskProfiler++;
		osMutexAcquire(uart_mut_id, osWaitForever);
		printf("This is RedTask \r\n");
		osMutexRelease(uart_mut_id);
		osDelay(100);


	}
}

void vOrangeLedControllerTask(void *pvParameters)
{
	int i;
	while(1)
	{
		Orange_TaskProfiler++;
		for(i=0;i<800000;i++){}
		osDelay(100);



	}
}

void vBlueLedControllerTask(void *pvParameters)
{
	int i;
	while(1)
	{
		Blue_TaskProfiler++;
		for(i=0;i<800000;i++){}
		osDelay(100);



	}
}

void Timer1_callback(void *arg)				//You should not put while(1) loop in timer callback function
{
	timer1_Taskprofiler++;
}

void Timer2_callback(void *arg)
{
	timer2_Taskprofiler++;
}


int USART2_write(int ch)
{
	while(!(USART2->SR & 0x0080)){}
	USART2->DR = (ch & 0xFF);
	return ch;
}

int __io_putchar(int ch)
{
	USART2_write(ch);
	return ch;
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
  __HAL_RCC_GPIOA_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
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