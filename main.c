#include "stm32l476xx.h"
#include "Drivers/SysClock.h"
#include "Drivers/UART.h"
#include "Drivers/timer.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char RxComByte = 0;
uint8_t inBuffer[BufferSize];
uint8_t buffer[BufferSize];
int buffer_index=0;
int in_buffer_index = 0;

// Power On Self Test
int post(void)
{
	// Start timer
	timer_start();
	
	
	uint32_t time = timer_count();
	
	// Store times for two pulses
	uint32_t pulse_one = 0;
	uint32_t pulse_two = 0;
	
	// Run for 100 milliseconds
	while( (timer_count() - time) < 100000)
	{
		// Check for timer event (rising edge seen on pa1)
		if (timer_event() & 0xF)
		{
			if (pulse_one == 0)
				pulse_one = timer_capture();
			else if (pulse_two == 0)
				pulse_two = timer_capture();
		}
	}
	
	timer_stop();

	// Ensure we have seen a complete pulse (two rising edges) within 100ms
	//if (events_captured > 2)
	if ((pulse_two - pulse_one < 100000) && (pulse_two != 0))
		return 1;
	return 0;
	
}

int getInt(void)
{
	char rxByte;
	
	in_buffer_index = 0;
	while((rxByte != '\r') && (in_buffer_index < BufferSize - 2))
	{
		rxByte = USART_Read(USART2); //Read the input and store it into rxByte
		if((rxByte != '\r') && (rxByte >= '0') && (rxByte <= '9'))
		{
			USART_Write(USART2, (uint8_t *)&rxByte, 1);
			inBuffer[in_buffer_index] = rxByte;
			in_buffer_index++;
		}
	}
	inBuffer[in_buffer_index] = '\0';
	
	return atoi((char *)inBuffer);
}

char getChar(void)
{
	char rxByte;
	rxByte = USART_Read(USART2); //Read the input and store it into rxByte
	USART_Write(USART2, (uint8_t *)&rxByte, 1);
	return rxByte;
}

int main(void)
{
	System_Clock_Init(); // Switch System Clock = 80 MHz
	UART2_Init();
	timer_init();

	uint32_t min_bound = 950;
	
	int post_success = 0;
	while(post_success == 0)
	{
		sprintf((char *)buffer, "\r\nStarting POST...\r\n");
  	USART_Write(USART2,(uint8_t *)buffer, strlen((char *)buffer));
		post_success = post();
		if (post_success != 1)
		{
			sprintf((char *)buffer, "POST failed, retry? (Y/N)\r\n");
			USART_Write(USART2,(uint8_t *)buffer, strlen((char *)buffer));
			
			char reply = getChar();
			if(reply != 'y' && reply != 'Y')
				break;
		}
	}
	if(post_success)
	{
		sprintf((char *)buffer, "POST success!\r\n");
		USART_Write(USART2,(uint8_t *)buffer, strlen((char *)buffer));
		
		while(1)
		{
			uint32_t buckets[101] = {0};
			uint32_t bucket_num = 0;
			uint32_t reading_num = 0;
			// Tell the user what the current bounds are
			sprintf((char *)buffer, "The minimum bound is %d and the maximum bound is %d!\r\n", min_bound, min_bound +100);
			USART_Write(USART2,(uint8_t *)buffer, strlen((char *)buffer));
			
			// Prompt the user to accept or change bounds
			sprintf((char *)buffer, "Do you accept the bounds? (Y/N)\r\n");
			USART_Write(USART2,(uint8_t *)buffer, strlen((char *)buffer));
			
			// Receive user accept/decline
			char input = getChar();
			if(input  != 'y' && input !='Y')
			{ 
				// If declined, receive integer from user
				int lowerbound = 0;
				// Ensure integer is within max bounds
				while(lowerbound <50 || lowerbound > 9850)
				{
					sprintf((char *)buffer, "\n\rEnter the lower bound in the range of 50 to 9950\r\n");
					USART_Write(USART2,(uint8_t *)buffer, strlen((char *)buffer));
					lowerbound =	getInt();
					int upperbound= lowerbound + 100;		
				}
				
				sprintf((char *)buffer, "\n\rAccepted new lower bound.\r\n");
				USART_Write(USART2,(uint8_t *)buffer, strlen((char *)buffer));
				// Change min_bound
				min_bound = lowerbound;
			
			}
			
			// Begin Measurements
			sprintf((char *)buffer, "\n\rBeginning measurements...\r\n");
			USART_Write(USART2,(uint8_t *)buffer, strlen((char *)buffer));
			
			timer_start();
			// First Measurement
			// Wait for timer_event
			while((timer_event() & 0xF) == 0)
			{}
			// Read current TIM2 channel 2 counter value
			uint32_t prev_count = timer_capture();
			
			while (reading_num < 1000)
			{	
				// Wait for timer_event
				while((timer_event() & 0xF) == 0)
				{}
				// Read current TIM2 channel 2 counter value
				uint32_t count = timer_capture();
				
				// Pulse width in microseconds
				uint32_t pulse = count - prev_count;
					
				prev_count = count;
					
				if ((pulse > min_bound) && (pulse < min_bound + 100))
				{
					// Increment bucket for corresponding pulse width
					buckets[pulse - min_bound]++;
				}
				reading_num++;
			}
			
			timer_stop();
			
			sprintf((char *)buffer, "\n\rMeasurements complete!\r\n");
			USART_Write(USART2,(uint8_t *)buffer, strlen((char *)buffer));

			for(int i = 0; i < 101; ++i)
			{
				uint32_t bucket = buckets[i];
				if(bucket > 0)
				{
					sprintf((char *)buffer, "%d\t%d\n\r", i + min_bound, bucket);
					USART_Write(USART2,(uint8_t *)buffer, strlen((char *)buffer));
				}
			}
		}
	}
}

