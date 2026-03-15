/*********************************************************************
* Project     : Robot Arm Controller
* File        : hw_usart.c
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   Hardwaretreiber für die USART2-Schnittstelle.
*
* Copyright (c) 2026 Manuel Wiesinger
* All rights reserved.
*********************************************************************/

#include <stm32f4xx.h>
#include "hw_usart.h"

// Hilfsfunktion BRR aus PCLK1 und Baud (mit Oversampling 16) berechnen
static uint16_t usart_brr(uint32_t pclk_hz, uint32_t baud)
{
    float usartdiv = (float)pclk_hz / (16.0f * (float)baud);

    uint32_t mantissa = (uint32_t)usartdiv;   // Ganzzahlteil abschneiden
    uint32_t fraction = (uint32_t)((usartdiv - mantissa) * 16.0f + 0.5f); // runden

    // Falls die Fraktion 16 wird, Übertrag auf Mantisse
    if (fraction > 15U)
    {
        mantissa++;
        fraction = 0;
    }

    return (uint16_t)((mantissa << 4) | fraction);
}

void HW_USART2_Init(void)
{
    // Takte einschalten GPIOA und USART2 (APB1)
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    // PA2 auf AF7 (USART2_TX) konfigurieren
    GPIOA->MODER &= ~(GPIO_MODER_MODER2_Msk);  // lösche MODER2
    GPIOA->MODER |= GPIO_MODER_MODER2_1;       // PA2 auf Alternate-Function-Modus setzen

    // OTYPER: 0 = Push-Pull
    GPIOA->OTYPER &= ~(GPIO_OTYPER_OT2);

    // OSPEEDR: Fast speed für schöne Flanken
    GPIOA->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED2_Msk);  // Register Löschen
    GPIOA->OSPEEDR |=  GPIO_OSPEEDR_OSPEED2_1;      // 10: Fast speed

    // PUPDR: kein pull up und pull down
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD2_Msk;

    // AFRL: AF7 für PA2 USART2_TX
    GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL2_Msk);   // AFSEL löschen - Alternate Function Selection
    GPIOA->AFR[0] |=  (GPIO_AFRL_AFRL2_0 | GPIO_AFRL_AFRL2_1 | GPIO_AFRL_AFRL2_2);

    // USART2 konfigurieren (Oversampling 16, 8N1, nur TX)
    USART2->CR1 = 0;                 // UE=0, alles aus
    USART2->CR2 = 0;                 // 1 Stopbit
    USART2->CR3 = 0;                 // kein Flow Control

    // Baudrate
    USART2->BRR = usart_brr(HW_USART2_PCLK_HZ, HW_USART2_BAUD);

    // TX enable, UE enable
    USART2->CR1 = USART_CR1_TE;      // nur Sender aktiv
    USART2->CR1 |= USART_CR1_UE;     // UART einschalten

    // Optional: erstes TXE-Flag abwarten
    while ((USART2->SR & USART_SR_TXE) == 0) 
    {

    }
}

void HW_USART2_SendMessage(const char *message)
{
    // Abfrage ob Zeiger NULL ist, dann return
    if (!message) 
      return;

    while (*message) 
    {
        // Warten bis Datenregister leer (TXE=1)
        while ((USART2->SR & USART_SR_TXE) == 0) 
        {  

        }

        USART2->DR = (uint8_t)(*message++);
    }

    // Am Ende auf Transmission Complete (TC) warten
    while ((USART2->SR & USART_SR_TC) == 0)
    {

    }
}