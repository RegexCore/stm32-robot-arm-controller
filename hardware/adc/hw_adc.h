/*********************************************************************
* Project     : Robot Arm Controller
* File        : hw_adc.h
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   ADC driver for acquisition of analog input values.
*   ADC1 with DMA (DMA2, Stream 0, Channel 0)
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
 Initializes ADC1 and DMA2 for the following channels:
 PA0 = ADC1_IN0
 PA1 = ADC1_IN1
 PA4 = ADC1_IN4
 PB0 = ADC1_IN8

 After initialization, the measured values are continuously written
 to the adc_values[4] array via DMA.
 */
void HW_ADC_Init(void);

/*
 Array containing the current ADC measurement values.
 Index 0: Channel ADC1_IN0 (PA0)
 Index 1: Channel ADC1_IN1 (PA1)
 Index 2: Channel ADC1_IN4 (PA4)
 Index 3: Channel ADC1_IN8 (PB0)
 */
extern volatile uint16_t adc_values[4];

#ifdef __cplusplus
}
#endif

#endif // HW_ADC_H