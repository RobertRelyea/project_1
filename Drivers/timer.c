#include "timer.h"

void timer_init()
{
	init_pa1_timer();

	// Enable TIM2 in the APB1 clock enable register 1
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN; 
	
	TIM2->PSC = 80;    // Set prescaler value for TIM2
	TIM2->EGR |= TIM_EGR_UG; 	  // Trigger an update event for timer 2
	
	TIM2->CCER &= ~(TIM_CCER_CC2E); // Disable channel 2 output
	TIM2->CCER &= 0xA0; // Set CC2NP and CC2P to rising edge trigger
	
	// Set up CCMRx
	TIM2->CCMR1 &= ~(0xF000);
	TIM2->CCMR1 |= 0x0100; // Set CC2 channel as input, IC2 mapped on TI2
	TIM2->CCER |= TIM_CCER_CC2E; // Turn on output enable for capture input
}

void timer_start()
{
	TIM2->CR1 |= 0x1;
}

void timer_stop()
{
	TIM2->CR1 |= 0x0;
}

uint32_t timer_count()
{
	return TIM2->CNT; // Timer 2
}

uint32_t timer_capture()
{
	return TIM2->CCR2; // Timer 2 Channel 2
}

uint32_t timer_event()
{
	return (TIM2->SR & 0x4);
}
