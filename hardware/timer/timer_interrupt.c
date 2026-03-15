/*********************************************************************
* Project     : Robot Arm Controller
* File        : timer_interrupt.c
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   Initialisierung des TIM7-Interrupttimers (10 ms).
*
* Copyright (c) 2026 Manuel Wiesinger
* All rights reserved.
*********************************************************************/

#include "timer_interrupt.h"
#include <stm32f4xx.h>

void TIM7_Init_10ms(void)
{
    // Takt für TIM7 einschalten (APB1)
    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;

    // Timer stoppen, falls er läuft
    TIM7->CR1 &= ~TIM_CR1_CEN;

    // Prescaler und Auto-Reload setzen
    TIM7->PSC = 31;      // -> 500 kHz
    TIM7->ARR = 4999;    // -> 10 ms

    // Update-Interrupt erlauben
    TIM7->DIER |= TIM_DIER_UIE;

    // Update-Flag löschen (falls gesetzt)
    TIM7->SR &= ~TIM_SR_UIF;

    // NVIC-Interrupt für TIM7 einschalten
    NVIC_SetPriority(TIM7_IRQn, 5);   // Priorität setzen
    NVIC_EnableIRQ(TIM7_IRQn);

    // Timer starten
    TIM7->CR1 |= TIM_CR1_CEN;
}