/*********************************************************************
* Project     : Robot Arm Controller
* File        : hw_adc.c
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   ADC-Treiber zur Erfassung analoger Eingangswerte.
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
    // GPIO Clocks
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;  // IO port A clock enable
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;  // IO port B clock enable

    // DMA2 Clock enable
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;

    // ADC1 Clock
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    // ADC common prescaler PCLK2/4
    ADC->CCR &= ~ADC_CCR_ADCPRE;
    ADC->CCR |=  ADC_CCR_ADCPRE_0;

    // Analog Mode für PA0, PA1, PA4, PB0
    GPIOA->MODER |= GPIO_MODER_MODER0    // PA0 analog
                 |  GPIO_MODER_MODER1    // PA1 analog
                 |  GPIO_MODER_MODER4;   // PA4 analog
    GPIOB->MODER |= GPIO_MODER_MODER0;   // PB0 analog

    // Pull Ups und Downs aus
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR0 | GPIO_PUPDR_PUPDR1 | GPIO_PUPDR_PUPDR4);
    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR0;

    // Sample Times für CH0,1,4,8 maximale Sample Time
    ADC1->SMPR2 |=
        (ADC_SMPR2_SMP0_2 | ADC_SMPR2_SMP0_1 | ADC_SMPR2_SMP0_0) |  // CH0
        (ADC_SMPR2_SMP1_2 | ADC_SMPR2_SMP1_1 | ADC_SMPR2_SMP1_0) |  // CH1
        (ADC_SMPR2_SMP4_2 | ADC_SMPR2_SMP4_1 | ADC_SMPR2_SMP4_0) |  // CH4
        (ADC_SMPR2_SMP8_2 | ADC_SMPR2_SMP8_1 | ADC_SMPR2_SMP8_0);   // CH8

    // Reset SQR-Register
    ADC1->SQR1 = 0;
    ADC1->SQR2 = 0;
    ADC1->SQR3 = 0;

    // Anzahl der Conversions: L = 4-1 = 3  weil 0 == 1 Conversion
    ADC1->SQR1 |= (3 << ADC_SQR1_L_Pos); // L[3:0] in Bits 23:20

    // Sequenz definieren SQx sind je 5 Bit
    ADC1->SQR3 |= (0 << ADC_SQR3_SQ1_Pos);  // SQ1 = Channel 0
    ADC1->SQR3 |= (1 << ADC_SQR3_SQ2_Pos);  // SQ2 = Channel 1
    ADC1->SQR3 |= (4 << ADC_SQR3_SQ3_Pos);  // SQ3 = Channel 4
    ADC1->SQR3 |= (8 << ADC_SQR3_SQ4_Pos);  // SQ4 = Channel 8

    // Scan Mode + Continuous Mode
    ADC1->CR1 |= ADC_CR1_SCAN;
    ADC1->CR2 |= ADC_CR2_CONT;

    // DMA im ADC einschalten, End of Sequence (EOCS=0 ist default)
    ADC1->CR2 |= ADC_CR2_DMA | ADC_CR2_DDS;

    // DMA2 Stream 0 Channel 0 für ADC1->DR, adc_values[]
    DMA2_Stream0->CR = 0;                       // Stream reset EN=0
    while (DMA2_Stream0->CR & DMA_SxCR_EN) {}   // sicherstellen dass er wirklich aus ist

    DMA2_Stream0->PAR  = (uint32_t)&ADC1->DR;   // Peripherieadresse
    DMA2_Stream0->M0AR = (uint32_t)adc_values;  // Speicheradresse
    DMA2_Stream0->NDTR = 4;                     // vier Werte

    // Konfiguration: Periph-to-Mem, 16 Bit, Memory increment, Circular
    DMA2_Stream0->CR |= DMA_SxCR_MINC | DMA_SxCR_CIRC;   // Memory increment + Circular
    DMA2_Stream0->CR |= (0 << DMA_SxCR_CHSEL_Pos);       // Channel 0
    DMA2_Stream0->CR |= DMA_SxCR_PSIZE_0;                // 16 Bit Peripherie
    DMA2_Stream0->CR |= DMA_SxCR_MSIZE_0;                // 16 Bit Memory
    // DIR bleibt 00 = Periph-to-Mem

    // DMA Stream einschalten
    DMA2_Stream0->CR |= DMA_SxCR_EN;

    // ADC einschalten
    ADC1->CR2 |= ADC_CR2_ADON;

    // Start der Conversion
    ADC1->CR2 |= ADC_CR2_SWSTART;
}
