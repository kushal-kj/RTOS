
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

//The periods assigned to the one-shot and auto-reload timers respectively
#define mainONE_ONE_SHOT_TIMER_PERIOD	(pdMS_TO_TICKS(4000UL))		//execute after every 4000ms i.e. 4000ms = 1 tick
#define mainAUTO_RELOAD_TIMER_PERIOD	(pdMS_TO_TICKS(500UL))		//execute after every 500ms i.e. 500ms = 1 tick

TimerHandle_t xAutoReloadTimer,xOneShotTimer; 		//TImer handles

BaseType_t xTimer1Started,xTimer2Started;

void prvOneShotTimerCallback(TimerHandle_t xTimer);
void prvAutoReloadTimerCallback(TimerHandle_t xTimer);


int main(void)
{

  HAL_Init();		/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  SystemClock_Config();		/* Configure the system clock */
  MX_GPIO_Init();		    /* Initialize all configured peripherals */
  USART2_UART_TX_Init();


  printf("System Initializing...\n\r");

  xOneShotTimer=xTimerCreate("OnShot", 				//Creating One-shot Timer
		  mainONE_ONE_SHOT_TIMER_PERIOD,
		  pdFALSE,
		  0,
		  prvOneShotTimerCallback);

  xAutoReloadTimer=xTimerCreate("AutoReload", 		//Creating Auto-Reload Timer
		  mainAUTO_RELOAD_TIMER_PERIOD,
		  pdTRUE,
		  0,
		  prvAutoReloadTimerCallback);

  xTimer1Started=xTimerStart(xOneShotTimer,0);
  xTimer2Started=xTimerStart(xAutoReloadTimer,0);

  vTaskStartScheduler();



  while (1)
  {

  }
}

void prvOneShotTimerCallback(TimerHandle_t xTimer)
{
	static TickType_t xTimeNow;

	xTimeNow = xTaskGetTickCount();		//obtain the current tick count

	printf("One-shot timer executing : %d \n\r",(int)xTimeNow);

}

uint32_t timeout_Count=0;
const uint32_t STOP_MARK=10;

void prvAutoReloadTimerCallback(TimerHandle_t xTimer)
{
	static TickType_t xTimeNow;

	xTimeNow = xTaskGetTickCount();		//obtain the current tick count

	printf("Auto-reload timer executing : %d \n\r",(int)xTimeNow);

	timeout_Count++;

	if(timeout_Count==STOP_MARK)
	{
		printf("STOP MARK reached %d \n\r ", (int)xTimeNow);
		xTimerStop(xAutoReloadTimer,0);
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
