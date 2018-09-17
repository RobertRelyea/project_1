#include "stm32l476xx.h"
#include "Drivers/SysClock.h"
#include "Drivers/UART.h"
#include "Drivers/input_pa0.h"

#include <string.h>
#include <stdio.h>

char RxComByte = 0;
uint8_t buffer[BufferSize];
char str[] = "Give Red LED control input (Y = On, N = off):\r\n";
char newline[]="\r\n";
int buffer_index=0;
int main(void){
	char rxByte;
	
	System_Clock_Init(); // Switch System Clock = 80 MHz
	
	init_pa0(); // Init PA0 to alternate function mode
	
	RCC->APB1ENR1 |= 0x0001; // Enable TIM2 in the APB1 clock enable register 1
	
	TIM2->PSC = 0x9C40; // Set prescaler for TIM2 to 40000
										 // Results in a sampling frequency of 2 KHz

	TIM2->EGR |=0X40; // Trigger an update event for timer 2
	
	TIM2->CCER &= ~0x1; // Turn off output enable for capture input
											// Ensures changes will take effect when capture input is reenabled
	
	// Set up CCMRx please
	
	UART2_Init();
		
	while (1){
		
		rxByte = USART_Read(USART2); //Read the input and store it into rxByte
		USART_Write(USART2, (uint8_t *)&rxByte, 1); //Pointing to rxBytes address
		if(rxByte=='\r')//If input is enter
		{
			USART_Write(USART2,(uint8_t *)newline, strlen(newline)); //move to a new line
			
			USART_Write(USART2, buffer, buffer_index);//write from buffer
			buffer_index=0;        //reset buffer to 0
			USART_Write(USART2,(uint8_t *)newline, strlen(newline)); //move to a new line again
		}
		else
		{
			buffer[buffer_index]=rxByte;   //put the input in buffer
			buffer_index++;                //increment buffer as input is being added
		}		
		
	}
}

