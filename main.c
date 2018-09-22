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
	UART2_Init();
	
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	
	GPIOA->MODER &= 0xFFFFFFFF;				  // Clear port A mode
	GPIOA->MODER |= 0x2;								// Enable alternate function mode (binary 10) for PA0
	GPIOA->AFR[0] |= 0x01;							// Sets AF1 as the alternate function for PA0
	
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN; // Enable TIM2 in the APB1 clock enable register 1
	
	//TIM2->PSC = 0x10; // Set prescaler for TIM2 to 40000
										 // Results in a sampling frequency of 2 KHz

	TIM2->PSC = 80;    // Set prescaler value for TIM2
	TIM2->EGR |= TIM_EGR_UG; 	  // Trigger an update event for timer 2
	
	TIM2->CCER &= ~0xFFFFFFFF;
	//TIM2->CCER &= ~0x1; // Turn off output enable for capture input
											// Ensures changes will take effect when capture input is reenabled
	//TIM2->CCER &= ~0xE; // Set our capture trigger to rising edges
	
	// Set up CCMRx
	TIM2->CCMR1 |= 0x01; // Set CC1 channel as input, IC1 mapped on TI1
	//TIM2->CCMR1 &= ~0xF0; // Clear input capture 1 filter
	//TIM2->CCMR1 &= ~(TIM_CCMR1_IC1PSC); // Set capture prescaler to zero
			
	TIM2->CCER |= TIM_CCER_CC1E; // Turn on output enable for capture input
	TIM2->CR1 = 0x1; // Enable input capture
		
	while (1)
	{	

//		USART_Write(USART2,(uint8_t *)badCount, strlen((char *)badCount)); //move to a new line
//		USART_Write(USART2,(uint8_t *)newline, strlen(newline)); //move to a new line
		if ((TIM2->SR & 0x2) != 0) // Capture 1 interrupt flag
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

