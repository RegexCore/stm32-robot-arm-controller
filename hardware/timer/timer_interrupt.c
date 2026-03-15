/*********************************************************************
* Project     : Robot Arm Controller
* File        : timer_interrupt.c
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   Initialization of the TIM7 interrupt timer (10 ms).
*
* Copyright (c) 2026 Manuel Wiesinger
* All rights reserved.
*********************************************************************/

#include "timer_interrupt.h"
#include <stm32f4xx.h>

void TIM7_Init_10ms(void)
{
    // Enable clock for TIM7 (APB1)
    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;

    // Stop timer if it is running
    TIM7->CR1 &= ~TIM_CR1_CEN;

    // Set prescaler and auto-reload value
    TIM7->PSC = 31;      // -> 500 kHz
    TIM7->ARR = 4999;    // -> 10 ms

    // Enable update interrupt
    TIM7->DIER |= TIM_DIER_UIE;

    // Clear update flag (if set)
    TIM7->SR &= ~TIM_SR_UIF;

    // Enable NVIC interrupt for TIM7
    NVIC_SetPriority(TIM7_IRQn, 5);   // Set priority
    NVIC_EnableIRQ(TIM7_IRQn);

    // Start timer
    TIM7->CR1 |= TIM_CR1_CEN;
}