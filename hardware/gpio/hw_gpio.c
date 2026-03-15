/*********************************************************************
* Project     : Robot Arm Controller
* File        : hw_gpio.c
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   GPIO driver for push-button and switch inputs.
*   PC10 = Joystick Left
*   PC11 = Joystick Right
*   PC12 = Button 1 (Emergency Stop)
*   PA15 = Button 2 (Auto mode on/off)
*
* Copyright (c) 2026 Manuel Wiesinger
* All rights reserved.
*********************************************************************/

#include "hw_gpio.h"
#include <stm32f4xx.h>

void HW_PC10_PC11_Init(PC_Pull_t pull)
{
    /* Enable clock for GPIOC */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    (void)RCC->AHB1ENR;     // Ensure GPIO clock is active

    /* Configure PC10 and PC11 as input */
    GPIOC->MODER &= ~(GPIO_MODER_MODE10 | GPIO_MODER_MODE11);

    /* Clear pull-up / pull-down configuration */
    GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD10 | GPIO_PUPDR_PUPD11);

    switch (pull)
    {
        case PC_PULL_UP:
            GPIOC->PUPDR |= GPIO_PUPDR_PUPD10_0 | GPIO_PUPDR_PUPD11_0;
            break;

        case PC_PULL_DOWN:
            GPIOC->PUPDR |= GPIO_PUPDR_PUPD10_1 | GPIO_PUPDR_PUPD11_1;
            break;

        case PC_NO_PULL:
        default:
            break;
    }
}

void HW_PC12_Init(PC_Pull_t pull)
{
    /* Enable clock for GPIOC */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    (void)RCC->AHB1ENR;

    /* Configure PC12 as input */
    GPIOC->MODER &= ~GPIO_MODER_MODE12;

    /* Clear pull configuration */
    GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD12;

    switch (pull)
    {
        case PC_PULL_UP:
            GPIOC->PUPDR |= GPIO_PUPDR_PUPD12_0;
            break;

        case PC_PULL_DOWN:
            GPIOC->PUPDR |= GPIO_PUPDR_PUPD12_1;
            break;

        case PC_NO_PULL:
        default:
            break;
    }
}

void HW_PA15_Init(PC_Pull_t pull)
{
    /* Enable clock for GPIOA */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    (void)RCC->AHB1ENR;

    /* Configure PA15 as input */
    GPIOA->MODER &= ~GPIO_MODER_MODE15;

    /* Clear pull configuration */
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD15;

    switch (pull)
    {
        case PC_PULL_UP:
            GPIOA->PUPDR |= GPIO_PUPDR_PUPD15_0;
            break;

        case PC_PULL_DOWN:
            GPIOA->PUPDR |= GPIO_PUPDR_PUPD15_1;
            break;

        case PC_NO_PULL:
        default:
            break;
    }
}

uint8_t HW_PC10_Read(void)
{
    // Button pulls the line to GND -> return inverted logic level
    return ((GPIOC->IDR & GPIO_IDR_ID10) ? 0U : 1U);
}

uint8_t HW_PC11_Read(void)
{
    // Button pulls the line to GND -> return inverted logic level
    return ((GPIOC->IDR & GPIO_IDR_ID11) ? 0U : 1U);
}

uint8_t HW_PC12_Read(void)
{
    // Button pulls the line to GND -> return inverted logic level
    return ((GPIOC->IDR & GPIO_IDR_ID12) ? 0U : 1U);
}

uint8_t HW_PA15_Read(void)
{
    // Button pulls the line to GND -> return inverted logic level
    return ((GPIOA->IDR & GPIO_IDR_ID15) ? 0U : 1U);
}