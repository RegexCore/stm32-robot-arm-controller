/*********************************************************************
* Project     : Robot Arm Controller
* File        : systicktimer.c
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   SysTick-basierte Zeitfunktionen und Millisekunden-Delay.
*
* Copyright (c) 2026 Manuel Wiesinger
* All rights reserved.
*********************************************************************/

#include "systicktimer.h"
#include <stm32f4xx.h>
#include <stdio.h>

volatile uint32_t systick_time_ms = 0; // auf 32Bit geändert wegen Atomarer zugriff!!!!

void systick_init(void)
{
    // Aktuelle Core Clock ermitteln
    SystemCoreClockUpdate();

    // Ausgabe der aktuellen Systemfrequenz
    printf("[SysTick] Initialized with Core Clock: %lu Hz\r\n", (unsigned long)SystemCoreClock);

    // SysTick konfigurieren: Reload = SystemCoreClock / 1000
    // 1 Interrupt pro Millisekunde (1000 Interrupts pro Sekunde)
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
    // ISR wird alle 1 ms aufgerufen
    systick_time_ms++;
}

void delay_ms(uint32_t ms)
{
    uint32_t start = systick_millis();

    // Wartezeit
    while ((systick_millis() - start) < ms) 
    {

    }
}
