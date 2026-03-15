/*********************************************************************
* Project     : Robot Arm Controller
* File        : hw_usart.c
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   Hardware driver for the USART2 interface.
*
* Copyright (c) 2026 Manuel Wiesinger
* All rights reserved.
*********************************************************************/

#include <stm32f4xx.h>
#include "hw_usart.h"

// Helper function to calculate BRR from PCLK1 and baud rate (oversampling 16)
static uint16_t usart_brr(uint32_t pclk_hz, uint32_t baud)
{
    float usartdiv = (float)pclk_hz / (16.0f * (float)baud);

    uint32_t mantissa = (uint32_t)usartdiv;   // Truncate integer part
    uint32_t fraction = (uint32_t)((usartdiv - mantissa) * 16.0f + 0.5f); // Round

    // If fraction becomes 16, carry over to mantissa
    if (fraction > 15U)
    {
        mantissa++;
        fraction = 0;
    }

    return (uint16_t)((mantissa << 4) | fraction);
}

void HW_USART2_Init(void)
{
    // Enable clocks for GPIOA and USART2 (APB1)
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    // Configure PA2 as AF7 (USART2_TX)
    GPIOA->MODER &= ~(GPIO_MODER_MODER2_Msk);  // Clear MODER2
    GPIOA->MODER |= GPIO_MODER_MODER2_1;       // Set PA2 to alternate function mode

    // OTYPER: 0 = push-pull
    GPIOA->OTYPER &= ~(GPIO_OTYPER_OT2);

    // OSPEEDR: fast speed for clean signal edges
    GPIOA->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED2_Msk);
    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED2_1;  // 10: fast speed

    // PUPDR: no pull-up and no pull-down
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD2_Msk;

    // AFRL: AF7 for PA2 USART2_TX
    GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL2_Msk);   // Clear AF selection
    GPIOA->AFR[0] |= (GPIO_AFRL_AFRL2_0 | GPIO_AFRL_AFRL2_1 | GPIO_AFRL_AFRL2_2);

    // Configure USART2 (oversampling 16, 8N1, TX only)
    USART2->CR1 = 0;                 // UE = 0, disable everything
    USART2->CR2 = 0;                 // 1 stop bit
    USART2->CR3 = 0;                 // no flow control

    // Baud rate
    USART2->BRR = usart_brr(HW_USART2_PCLK_HZ, HW_USART2_BAUD);

    // Enable transmitter and USART
    USART2->CR1 = USART_CR1_TE;      // Enable transmitter only
    USART2->CR1 |= USART_CR1_UE;     // Enable USART

    // Optionally wait for first TXE flag
    while ((USART2->SR & USART_SR_TXE) == 0)
    {

    }
}

void HW_USART2_SendMessage(const char *message)
{
    // Check if pointer is NULL
    if (!message)
        return;

    while (*message)
    {
        // Wait until data register is empty (TXE = 1)
        while ((USART2->SR & USART_SR_TXE) == 0)
        {

        }

        USART2->DR = (uint8_t)(*message++);
    }

    // Wait for transmission complete (TC)
    while ((USART2->SR & USART_SR_TC) == 0)
    {

    }
}