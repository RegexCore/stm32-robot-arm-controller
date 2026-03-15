/*********************************************************************
* Project     : Robot Arm Controller
* File        : hw_adc.c
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   ADC driver for acquisition of analog input values.
*
* Copyright (c) 2026 Manuel Wiesinger
* All rights reserved.
*********************************************************************/

#include "hw_adc.h"
#include <stm32f4xx.h>
#include <stdint.h>

volatile uint16_t adc_values[4];

void HW_ADC_Init(void)
{
    // GPIO clocks
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;  // Enable I/O port A clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;  // Enable I/O port B clock

    // Enable DMA2 clock
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;

    // Enable ADC1 clock
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    // ADC common prescaler PCLK2/4
    ADC->CCR &= ~ADC_CCR_ADCPRE;
    ADC->CCR |=  ADC_CCR_ADCPRE_0;

    // Configure PA0, PA1, PA4, PB0 as analog inputs
    GPIOA->MODER |= GPIO_MODER_MODER0    // PA0 analog
                 |  GPIO_MODER_MODER1    // PA1 analog
                 |  GPIO_MODER_MODER4;   // PA4 analog
    GPIOB->MODER |= GPIO_MODER_MODER0;   // PB0 analog

    // Disable pull-up and pull-down resistors
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR0 | GPIO_PUPDR_PUPDR1 | GPIO_PUPDR_PUPDR4);
    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR0;

    // Set maximum sample time for channels CH0, CH1, CH4, CH8
    ADC1->SMPR2 |=
        (ADC_SMPR2_SMP0_2 | ADC_SMPR2_SMP0_1 | ADC_SMPR2_SMP0_0) |  // CH0
        (ADC_SMPR2_SMP1_2 | ADC_SMPR2_SMP1_1 | ADC_SMPR2_SMP1_0) |  // CH1
        (ADC_SMPR2_SMP4_2 | ADC_SMPR2_SMP4_1 | ADC_SMPR2_SMP4_0) |  // CH4
        (ADC_SMPR2_SMP8_2 | ADC_SMPR2_SMP8_1 | ADC_SMPR2_SMP8_0);   // CH8

    // Reset SQR registers
    ADC1->SQR1 = 0;
    ADC1->SQR2 = 0;
    ADC1->SQR3 = 0;

    // Number of conversions: L = 4 - 1 = 3 (0 means 1 conversion)
    ADC1->SQR1 |= (3 << ADC_SQR1_L_Pos);

    // Define conversion sequence (each SQx field is 5 bits)
    ADC1->SQR3 |= (0 << ADC_SQR3_SQ1_Pos);  // SQ1 = Channel 0
    ADC1->SQR3 |= (1 << ADC_SQR3_SQ2_Pos);  // SQ2 = Channel 1
    ADC1->SQR3 |= (4 << ADC_SQR3_SQ3_Pos);  // SQ3 = Channel 4
    ADC1->SQR3 |= (8 << ADC_SQR3_SQ4_Pos);  // SQ4 = Channel 8

    // Enable scan mode and continuous conversion mode
    ADC1->CR1 |= ADC_CR1_SCAN;
    ADC1->CR2 |= ADC_CR2_CONT;

    // Enable DMA for ADC (EOCS = 0 by default → end of sequence)
    ADC1->CR2 |= ADC_CR2_DMA | ADC_CR2_DDS;

    // Configure DMA2 Stream0 Channel0 for ADC1->DR → adc_values[]
    DMA2_Stream0->CR = 0;                       // Reset stream (EN = 0)
    while (DMA2_Stream0->CR & DMA_SxCR_EN) {}   // Ensure stream is really disabled

    DMA2_Stream0->PAR  = (uint32_t)&ADC1->DR;   // Peripheral address
    DMA2_Stream0->M0AR = (uint32_t)adc_values;  // Memory address
    DMA2_Stream0->NDTR = 4;                     // Transfer four values

    // Configuration: Peripheral-to-memory, 16-bit width, memory increment, circular mode
    DMA2_Stream0->CR |= DMA_SxCR_MINC | DMA_SxCR_CIRC;
    DMA2_Stream0->CR |= (0 << DMA_SxCR_CHSEL_Pos); // Channel 0
    DMA2_Stream0->CR |= DMA_SxCR_PSIZE_0;          // 16-bit peripheral
    DMA2_Stream0->CR |= DMA_SxCR_MSIZE_0;          // 16-bit memory
    // DIR remains 00 → Peripheral-to-memory

    // Enable DMA stream
    DMA2_Stream0->CR |= DMA_SxCR_EN;

    // Enable ADC
    ADC1->CR2 |= ADC_CR2_ADON;

    // Start conversion
    ADC1->CR2 |= ADC_CR2_SWSTART;
}