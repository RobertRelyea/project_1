#include "stm32l476xx.h"
#include "Drivers/SysClock.h"
#include "Drivers/UART.h"
#include "Drivers/LED.h"
#include "Drivers/input_pa0.h"

#include <string.h>
#include <stdio.h>

char RxComByte = 0;
uint8_t buffer[BufferSize];
char goodCount[] = "Count > 0";
char badCount[] = "Count <= 0";
char newline[]="\r\n";
int buffer_index=0;

void timer_init()
{
	init_pa0();

	// Enable TIM2 in the APB1 clock enable register 1
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN; 
	
	TIM2->PSC = 80;    // Set prescaler value for TIM2
	TIM2->EGR |= TIM_EGR_UG; 	  // Trigger an update event for timer 2
	
	TIM2->CCER &= ~(0xFFFFFFFF);
	
	// Set up CCMRx
	TIM2->CCMR1 |= 0x0100; // Set CC2 channel as input, IC2 mapped on TI2
	
	TIM2->CCER |= TIM_CCER_CC2E; // Turn on output enable for capture input
}

void timer_start()
{
	TIM2->CR1 = 0x1;
}

void timer_stop()
{
	TIM2->CR1 = 0x0;
}

uint32_t timer_count()
{
	return TIM2->CCR2; // Timer 2 Channel 2
}

uint32_t timer_event()
{
	return TIM2->SR & 0x4;
}

int main(void)
{
	char rxByte;
	
	System_Clock_Init(); // Switch System Clock = 80 MHz
	UART2_Init();
	timer_init();

	//uint32_t prev_count = 0;
	uint32_t readings[101];
	uint32_t reading_num = 0;
	
	
	sprintf((char *)buffer, "Starting...");
		
	USART_Write(USART2,(uint8_t *)buffer, strlen((char *)buffer)); //move to a new line
	USART_Write(USART2,(uint8_t *)newline, strlen(newline)); //move to a new line

	
	timer_start();
	while (reading_num < 101)
	{	
		// Read current TIM2 channel 2 counter value
		while((timer_event() & 0xF) == 0)
		{}
   	uint32_t prev_count = timer_count();
			
		while((timer_event() & 0xF) == 0)
		{}
		uint32_t count = timer_count();
		
		readings[reading_num] = count - prev_count;
		reading_num++;
		
		//timer_stop();
		
		//sprintf((char *)buffer, "CCR2 Value = %d", count);
		
		//USART_Write(USART2,(uint8_t *)buffer, strlen((char *)buffer)); //move to a new line
		//USART_Write(USART2,(uint8_t *)newline, strlen(newline)); //move to a new line	
	}
	
	timer_stop();
	uint32_t sum = 0;
	
	for(uint32_t i = 0; i < 101; ++i)
	{
		sum += readings[i];
	}
	
	sprintf((char *)buffer, "CCR2 Avg. Count = %d", sum / 101);
		
	USART_Write(USART2,(uint8_t *)buffer, strlen((char *)buffer)); //move to a new line
	USART_Write(USART2,(uint8_t *)newline, strlen(newline)); //move to a new line

}

