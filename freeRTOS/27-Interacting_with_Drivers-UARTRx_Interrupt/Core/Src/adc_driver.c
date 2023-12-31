
#include "stm32f4xx_hal.h"

void adc_init(void)
{
	RCC->AHB1ENR |=(1u<<0);		//Enable clock access for GPIOA

	RCC->APB2ENR |=(1u<<8);		//Enable clock access for ADC1

	GPIOA->MODER |= (3U<<2);			//Set PA1 as analog

	ADC1->CR2=0;		//SE Trigger
	ADC1->SQR3=1;		//Conversion sequence starts at channel 1
	ADC1->SQR3=1;		//Conversion sequence starts at channel 1
	ADC1->CR2|=1;		//enable ADC1

}

uint32_t read_analog_sensor(void)
{
	ADC1->CR2 |=(1U<<30);		//Start conversion

	while(!(ADC1->SR & 2)){}	//Wait for conversion to complete

	return ADC1->DR;		//Return results

}

