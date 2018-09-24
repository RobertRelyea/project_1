#ifndef TIMER_h
#define TIMER_h

#include "stm32l476xx.h"
#include "input_pa1.h"

// Sets up the TIM2 registers for Channel 2 on PA1
void timer_init(void);


void timer_start(void);

void timer_stop(void);

uint32_t timer_count(void);

uint32_t timer_capture(void);

uint32_t timer_event(void);


#endif
