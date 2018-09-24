// L. Kiser Feb. 8, 2017
#include "input_pa1.h"

// Turn on the peripheral clock for GPIOA
// and set PA1 to be in Alternate Function 
// mode for use with TIM2_CH2.
void init_pa1_timer( void )
{
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN ;		// enable clock for A group of GPIO
	GPIOA->MODER &= ~0xC; // clear out bits 0 and 1 for PA1
	// Enable alternate function mode (binary 10) for PA1
	GPIOA->MODER |= 0x8;								    
	GPIOA->AFR[0] &= ~0xF0;		// Clear PA1 field in AFRL
	GPIOA->AFR[0] |= 0x10;		// Sets AF1 as the alternate function for PA1
														// TIM2_CH2
}