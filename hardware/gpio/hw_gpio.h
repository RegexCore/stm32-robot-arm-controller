/*********************************************************************
* Project     : Robot Arm Controller
* File        : hw_gpio.h
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   GPIO driver for push-button and switch inputs.
*
* Copyright (c) 2026 Manuel Wiesinger
* All rights reserved.
*********************************************************************/

#ifndef HW_GPIO_H
#define HW_GPIO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Pull configuration for GPIO inputs */
typedef enum
{
    PC_NO_PULL   = 0U,   /* No internal pull resistor */
    PC_PULL_UP   = 1U,   /* Internal pull-up */
    PC_PULL_DOWN = 2U    /* Internal pull-down */
} PC_Pull_t;

/* Initializes PC10 and PC11 as inputs with optional pull resistor */
void HW_PC10_PC11_Init(PC_Pull_t pull);

/* Initializes PC12 as input with optional pull resistor */
void HW_PC12_Init(PC_Pull_t pull);

/* Initializes PA15 as input with optional pull resistor */
void HW_PA15_Init(PC_Pull_t pull);

/* Reads current logic level of the inputs (inverted button logic) */
uint8_t HW_PC10_Read(void);
uint8_t HW_PC11_Read(void);
uint8_t HW_PC12_Read(void);
uint8_t HW_PA15_Read(void);

#ifdef __cplusplus
}
#endif

#endif // HW_GPIO_H