// L. Kiser Feb. 8, 2017

#include "stm32l476xx.h"
#include "input_pa0.h"
#include "LED.h"

// Turn on the peripheral clock for GPIOA
// and set PA0 to be input mode
void init_pa1_timer( void )
{
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN ;		// enable clock for A group of GPIO
	GPIOA->MODER &= ~0xFF;										  // clear out bits 0 and 1 for PA0
	// Enable alternate function mode (binary 10) for PA1
	GPIOA->MODER |= 0xA;								    
	GPIOA->AFR[0] &= ~0xF0;							    // Clear PA1 field in AFRL
	GPIOA->AFR[0] |= 0x10;							    // Sets AF1 as the alternate function for PA1
																					// TIM2_CH2
}

// Read the state of PA0 (high or low)
int read_pa0( void )
{
	return GPIOA->IDR & 1 ;				// read the bottom bit of the GPIOA input lines which is PA0.
																// returns 1 if high, 0 if low.
}

// This is an infinite loop that will show if PA0 is changing state
void monitor_pa0( void )
{
	while ( 1 )
		if ( read_pa0() )
			Red_LED_On();
		else
			Red_LED_Off() ;
}
