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
int main(void){
	char rxByte;
	
	System_Clock_Init(); // Switch System Clock = 80 MHz
	
	LED_Init();
	
	init_pa0(); // Init PA0 to alternate function mode
	monitor_pa0();
	
	RCC->APB1ENR1 |= 0x0001; // Enable TIM2 in the APB1 clock enable register 1
	
	TIM2->PSC = 0x9C40; // Set prescaler for TIM2 to 40000
										 // Results in a sampling frequency of 2 KHz

	TIM2->EGR |=0x1; // Trigger an update event for timer 2
	
	TIM2->CCER &= ~0x1; // Turn off output enable for capture input
											// Ensures changes will take effect when capture input is reenabled
	
	// Set up CCMRx
	TIM2->CCMR1 |= 0x1; // Set CC1 channel as input, IC1 mapped on TI2
	TIM2->CCMR1 &= ~0xF0; // Clear input capture 1 filter
	
	TIM2->CCER &= ~0xD; // Set our capture trigger to rising edges
	
	TIM2->CCER |= 0x1; // Turn on output enable for capture input
	
	
	
	TIM2->CR1 |= 0x1; // Enable input capture
	
	UART2_Init();
		
	while (1)
	{	
		if (TIM2->SR & 0X2) // Capture 1 interrupt flag
		{
			// Read current TIM2 channel 1 counter value
			unsigned int count = TIM2->CCR1;
			
			sprintf((char *)buffer, "CCR1 Value = %d", count);
			
			USART_Write(USART2,(uint8_t *)buffer, strlen((char *)buffer)); //move to a new line
			USART_Write(USART2,(uint8_t *)newline, strlen(newline)); //move to a new line
			
			USART_Write(USART2, buffer, buffer_index);//write from buffer
			USART_Write(USART2,(uint8_t *)newline, strlen(newline)); //move to a new line
		}			
	}
}

