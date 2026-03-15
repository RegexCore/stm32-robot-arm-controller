/*********************************************************************
* Project     : Robot Arm Controller
* File        : hw_adc.h
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   ADC-Treiber zur Erfassung analoger Eingangswerte.
*   ADC1 mit DMA (DMA2, Stream 0, Channel 0)
*
* Copyright (c) 2026 Manuel Wiesinger
* All rights reserved.
*********************************************************************/

#ifndef HW_ADC_H
#define HW_ADC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 Initialisiert ADC1 und DMA2 für die Kanäle
 PA0 = ADC1_IN0
 PA1 = ADC1_IN1
 PA4 = ADC1_IN4
 PB0 = ADC1_IN8
 Nach Aufruf werden die Messwerte kontinuierlich per DMA
 in das Array adc_values[4] geschrieben.
 */
void HW_ADC_Init(void);

/*
 Array mit den aktuellen ADC-Messwerten
 Index 0: Kanal ADC1_IN0 (PA0)
 Index 1: Kanal ADC1_IN1 (PA1)
 Index 2: Kanal ADC1_IN4 (PA4)
 Index 3: Kanal ADC1_IN8 (PB0)
 */
extern volatile uint16_t adc_values[4];

#ifdef __cplusplus
}
#endif

#endif // HW_ADC_H
