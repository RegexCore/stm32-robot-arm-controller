/*********************************************************************
* Project     : Robot Arm Controller
* File        : hw_usart.c
*
* Description :
*   Hardware driver for the USART2 interface.
*
* SPDX-License-Identifier: MIT
* Copyright (c) 2026 Manuel Wiesinger
*********************************************************************/

#include <stm32f4xx.h>
#include "hw_usart.h"
#include <string.h>

static HW_USART2_RxLine rx_queue[HW_USART2_RX_QUEUE_DEPTH + 1U];
static volatile uint32_t rx_head;
static volatile uint32_t rx_tail;
static volatile uint32_t rx_overflows;
static volatile int motion_allowed;
static HW_USART2_RxLine rx_line;
static uint32_t rx_length;

void HW_USART2_SetMotionAllowed(int allowed)
{
    uint32_t mask = __get_PRIMASK();
    __disable_irq();
    motion_allowed = allowed;
    if (!allowed)
    {
        rx_line.motion_allowed = 0;
        for (uint32_t i = rx_tail; i != rx_head; i = (i + 1U) % (HW_USART2_RX_QUEUE_DEPTH + 1U))
            rx_queue[i].motion_allowed = 0;
    }
    __set_PRIMASK(mask);
}

int HW_USART2_ReadLine(HW_USART2_RxLine *line)
{
    uint32_t mask = __get_PRIMASK();
    __disable_irq();
    int available = rx_tail != rx_head;
    if (available)
    {
        *line = rx_queue[rx_tail];
        rx_tail = (rx_tail + 1U) % (HW_USART2_RX_QUEUE_DEPTH + 1U);
    }
    __set_PRIMASK(mask);
    return available;
}

uint32_t HW_USART2_TakeRxOverflowCount(void)
{
    uint32_t mask = __get_PRIMASK();
    __disable_irq();
    uint32_t count = rx_overflows;
    rx_overflows = 0;
    __set_PRIMASK(mask);
    return count;
}

uint32_t HW_USART2_GetRxQueueCount(void)
{
    uint32_t mask = __get_PRIMASK();
    __disable_irq();
    uint32_t count = (rx_head + HW_USART2_RX_QUEUE_DEPTH + 1U - rx_tail) %
                     (HW_USART2_RX_QUEUE_DEPTH + 1U);
    __set_PRIMASK(mask);
    return count;
}

void USART2_IRQHandler(void)
{
    uint32_t status = USART2->SR;
    if (!(status & (USART_SR_RXNE | USART_SR_ORE | USART_SR_FE | USART_SR_NE | USART_SR_PE)))
        return;

    // Reading SR followed by DR clears RXNE and the receive error flags.
    uint8_t byte = (uint8_t)USART2->DR;
    if (status & (USART_SR_ORE | USART_SR_FE | USART_SR_NE | USART_SR_PE))
    {
        rx_line.status = HW_USART2_RX_UART_ERROR;
        return; // Resynchronize at the next error-free LF, never execute a fragment.
    }

    if (rx_length == 0U && rx_line.status == HW_USART2_RX_OK)
        rx_line.motion_allowed = (uint8_t)motion_allowed;
    else if (!motion_allowed)
        rx_line.motion_allowed = 0;

    if (byte == '\n')
    {
        if (rx_length > 0U && rx_line.text[rx_length - 1U] == '\r')
            --rx_length;
        rx_line.text[rx_length] = '\0';
        if (rx_length != 0U || rx_line.status != HW_USART2_RX_OK)
        {
            uint32_t next = (rx_head + 1U) % (HW_USART2_RX_QUEUE_DEPTH + 1U);
            if (next != rx_tail)
            {
                rx_queue[rx_head] = rx_line;
                rx_head = next;
            }
            else if (rx_overflows != UINT32_MAX)
            {
                ++rx_overflows;
            }
        }
        rx_length = 0;
        rx_line.status = HW_USART2_RX_OK;
    }
    else if (rx_line.status == HW_USART2_RX_OK)
    {
        if (byte != '\r' && (byte < 32U || byte > 126U))
            rx_line.status = HW_USART2_RX_INVALID_BYTE;
        else if (rx_length == HW_USART2_RX_LINE_SIZE - 1U)
            rx_line.status = HW_USART2_RX_TOO_LONG;
        else
            rx_line.text[rx_length++] = (char)byte;
    }
}

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
    NVIC_DisableIRQ(USART2_IRQn);
    rx_head = rx_tail = rx_overflows = rx_length = 0;
    motion_allowed = 0;
    memset(&rx_line, 0, sizeof(rx_line));

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

    GPIOA->MODER = (GPIOA->MODER & ~GPIO_MODER_MODER3_Msk) | GPIO_MODER_MODER3_1;
    GPIOA->PUPDR = (GPIOA->PUPDR & ~GPIO_PUPDR_PUPD3_Msk) | GPIO_PUPDR_PUPD3_0;
    GPIOA->AFR[0] = (GPIOA->AFR[0] & ~GPIO_AFRL_AFSEL3_Msk) |
                   GPIO_AFRL_AFRL3_0 | GPIO_AFRL_AFRL3_1 | GPIO_AFRL_AFRL3_2;

    // Configure USART2 (oversampling 16, 8N1, TX and RX)
    USART2->CR1 = 0;                 // UE = 0, disable everything
    USART2->CR2 = 0;                 // 1 stop bit
    USART2->CR3 = USART_CR3_EIE;      // receive error interrupts, no flow control

    // Baud rate
    USART2->BRR = usart_brr(HW_USART2_PCLK_HZ, HW_USART2_BAUD);

    (void)USART2->SR;
    (void)USART2->DR;
    NVIC_ClearPendingIRQ(USART2_IRQn);
    NVIC_SetPriority(USART2_IRQn, 6);
    NVIC_EnableIRQ(USART2_IRQn);
    USART2->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE | USART_CR1_PEIE;
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