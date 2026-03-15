/*********************************************************************
* Project     : Robot Arm Controller
* File        : hw_gpio.h
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   GPIO-Treiber für Taster- und Schalteingänge.
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

// Pull-Konfiguration für die GPIO-Eingänge
typedef enum
{
    PC_NO_PULL   = 0U,   // kein interner Pull-Widerstand
    PC_PULL_UP   = 1U,   // interner Pull-Up
    PC_PULL_DOWN = 2U    // interner Pull-Down
} PC_Pull_t;

// Initialisiert PC10 und PC11 als Eingänge mit optionalem Pull-Widerstand
void HW_PC10_PC11_Init(PC_Pull_t pull);

// Initialisiert PC12 als Eingang mit optionalem Pull-Widerstand
void HW_PC12_Init(PC_Pull_t pull);

// Initialisiert PA15 als Eingang mit optionalem Pull-Widerstand
void HW_PA15_Init(PC_Pull_t pull);

uint8_t HW_PC10_Read(void);
uint8_t HW_PC11_Read(void);
uint8_t HW_PC12_Read(void);
uint8_t HW_PA15_Read(void);

#ifdef __cplusplus
}
#endif

#endif // HW_GPIO_H