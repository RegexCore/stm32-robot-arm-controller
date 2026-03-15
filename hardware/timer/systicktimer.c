/*********************************************************************
* Project     : Robot Arm Controller
* File        : systicktimer.c
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   SysTick-based time functions and millisecond delay.
*
* Copyright (c) 2026 Manuel Wiesinger
* All rights reserved.
*********************************************************************/

#include "systicktimer.h"
#include <stm32f4xx.h>
#include <stdio.h>

volatile uint32_t systick_time_ms = 0; // Changed to 32-bit for atomic access

void systick_init(void)
{
    // Update current core clock value
    SystemCoreClockUpdate();

    // Output the current system frequency
    printf("[SysTick] Initialized with Core Clock: %lu Hz\r\n",
           (unsigned long)SystemCoreClock);

    // Configure SysTick: Reload = SystemCoreClock / 1000
    // 1 interrupt per millisecond (1000 interrupts per second)
    if (SysTick_Config(SystemCoreClock / 1000) != 0)
    {
        printf("\n[SysTick] ERROR: Configuration failed!");
        while (1);
    }

    systick_time_ms = 0;
}

uint32_t systick_millis(void)
{
    return systick_time_ms;
}

void SysTick_Handler(void)
{
    // ISR is called every 1 ms
    systick_time_ms++;
}

void delay_ms(uint32_t ms)
{
    uint32_t start = systick_millis();

    // Wait for the specified delay time
    while ((systick_millis() - start) < ms)
    {

    }
}