/*********************************************************************
* Project     : Robot Arm Controller
* File        : hw_gpio.c
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   GPIO-Treiber für Taster- und Schalteingänge.
*   PC10 = Joystick Left
*   PC11 = Joystick Right
*   PC12 = Button 1 (Notaus)
*   PA15 = Button 2 (Auto Mode on/off)
*
* Copyright (c) 2026 Manuel Wiesinger
* All rights reserved.
*********************************************************************/

#include "hw_gpio.h"
#include <stm32f4xx.h>

void HW_PC10_PC11_Init(PC_Pull_t pull)
{
    /* Clock für GPIOC aktivieren */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    (void)RCC->AHB1ENR;     // sicherstellen, dass der GPIO-Takt aktiv ist

    /* PC10 und PC11 als Eingang */
    GPIOC->MODER &= ~(GPIO_MODER_MODE10 | GPIO_MODER_MODE11);

    /* Pull-Up / Pull-Down löschen */
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
    /* Clock aktivieren */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    (void)RCC->AHB1ENR;

    /* PC12 als Eingang */
    GPIOC->MODER &= ~GPIO_MODER_MODE12;

    /* Pull löschen */
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
    /* Clock für GPIOA aktivieren */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    (void)RCC->AHB1ENR;

    /* PA15 als Eingang */
    GPIOA->MODER &= ~GPIO_MODER_MODE15;

    /* Pull löschen */
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
    // Taste zieht auf GND -> invertiert zurückgeben
    return ((GPIOC->IDR & GPIO_IDR_ID10) ? 0U : 1U);
}

uint8_t HW_PC11_Read(void)
{
    // Taste zieht auf GND -> invertiert zurückgeben
    return ((GPIOC->IDR & GPIO_IDR_ID11) ? 0U : 1U);
}

uint8_t HW_PC12_Read(void)
{
    // Taste zieht auf GND -> invertiert zurückgeben
    return ((GPIOC->IDR & GPIO_IDR_ID12) ? 0U : 1U);
}

uint8_t HW_PA15_Read(void)
{
    // Taste zieht auf GND -> invertiert zurückgeben
    return ((GPIOA->IDR & GPIO_IDR_ID15) ? 0U : 1U);
}