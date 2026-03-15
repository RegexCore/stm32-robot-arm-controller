/*********************************************************************
* Project     : Robot Arm Controller
* File        : hw_i2c.c
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   Hardware driver for I2C1 communication.
*   Low-level I2C1 driver (PB8 = SCL, PB9 = SDA, 100 kHz, AF4)
*
* Copyright (c) 2026 Manuel Wiesinger
* All rights reserved.
*********************************************************************/

#include <stm32f4xx.h>
#include "hw_i2c.h"

#define I2C_TIMEOUT_COUNT 10000U    // Timeout protection against deadlocks
#define I2C_SPEED_HZ      100000U   // Target speed: 100 kHz standard mode
#define HW_I2C_PCLK1_MHZ  16U       // Frequency on PCLK1/APB1 in MHz

void HW_I2C_Init(void)
{
    // Enable GPIOB clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; 

    // Enable alternate function mode
    GPIOB->MODER &= ~(GPIO_MODER_MODER8 | GPIO_MODER_MODER9);
    GPIOB->MODER |=  (GPIO_MODER_MODER8_1 | GPIO_MODER_MODER9_1);

    // Enable open-drain mode
    GPIOB->OTYPER |= (GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9);

    // Enable high-speed mode (clean signal edges)
    GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR8 | GPIO_OSPEEDER_OSPEEDR9);

    // Do not enable internal pull-ups because the PCA9685 servo shield
    // already provides pull-up resistors
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR8 | GPIO_PUPDR_PUPDR9);

    // Configure PB8 and PB9 for alternate function 4 (I2C1)
    // Clear AF8 and AF9 bits in AFR[1] using a mask
    GPIOB->AFR[1] &= ~(GPIO_AFRH_AFSEL8_Msk | GPIO_AFRH_AFSEL9_Msk);

    // Set AF4 for both pins
    GPIOB->AFR[1] |= GPIO_AFRH_AFSEL8_2 | GPIO_AFRH_AFSEL9_2;

    // Enable I2C1 peripheral clock
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    // Reset I2C1 to ensure a clean state
    RCC->APB1RSTR |= RCC_APB1RSTR_I2C1RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C1RST;

    // Disable I2C1 so registers can be modified safely
    I2C1->CR1 &= ~I2C_CR1_PE;

    // APB1 frequency
    uint32_t pclk1_mhz = HW_I2C_PCLK1_MHZ;

    // Configure registers
    I2C1->CR2   = pclk1_mhz;                                      // Clock frequency in MHz
    I2C1->CCR   = (pclk1_mhz * 1000000U) / (2U * I2C_SPEED_HZ);   // Standard mode (100 kHz)
    I2C1->TRISE = pclk1_mhz + 1;                                  // Maximum rise time (datasheet)

    // Enable I2C1
    I2C1->CR1 |= I2C_CR1_PE;
}

void HW_I2C_Transmit(uint8_t device_address, const uint8_t *data, uint8_t size)
{
    uint32_t timeout;

    // Generate start condition
    I2C1->CR1 |= I2C_CR1_START;

    timeout = I2C_TIMEOUT_COUNT;

    // Wait for start bit
    while (!(I2C1->SR1 & I2C_SR1_SB))
    {
        if (--timeout == 0)
            return;
    }

    // Send slave address + write bit
    I2C1->DR = (device_address << 1);

    timeout = I2C_TIMEOUT_COUNT;
    
    // Wait for address acknowledge
    while (!(I2C1->SR1 & I2C_SR1_ADDR))
    {
        if (--timeout == 0)
            return;
    }

    (void)I2C1->SR2; // Clear ADDR flag by reading SR2

    // Transmit data bytes
    for (uint8_t i = 0; i < size; i++)
    {
        timeout = I2C_TIMEOUT_COUNT;
        
        while (!(I2C1->SR1 & I2C_SR1_TXE))
        {
            if (--timeout == 0)
                return;
        }
            
        I2C1->DR = data[i];
    }

    timeout = I2C_TIMEOUT_COUNT;
    
    // Wait for transfer completion
    while (!(I2C1->SR1 & I2C_SR1_BTF))
    {
        if (--timeout == 0)
            return;
    }

    // Generate stop condition
    I2C1->CR1 |= I2C_CR1_STOP;
}

void HW_I2C_Receive(uint8_t device_address, uint8_t *data, uint8_t size)
{
    uint32_t timeout;

    // Generate start condition
    I2C1->CR1 |= I2C_CR1_START;

    timeout = I2C_TIMEOUT_COUNT;
    
    // Wait for start bit
    while (!(I2C1->SR1 & I2C_SR1_SB))
    {
        if (--timeout == 0)
            return;
    }

    // Send slave address + read bit
    I2C1->DR = (device_address << 1) | 0x01;

    timeout = I2C_TIMEOUT_COUNT;
   
    // Wait for address acknowledge
    while (!(I2C1->SR1 & I2C_SR1_ADDR))
    {
        if (--timeout == 0)
            return;
    }

    (void)I2C1->SR2; // Clear ADDR flag by reading SR2

    // Receive data byte by byte
    for (uint8_t i = 0; i < size; i++)
    {
        if (i == (size - 1))
        {
            // Last byte: send NACK + STOP
            I2C1->CR1 &= ~I2C_CR1_ACK;
            I2C1->CR1 |= I2C_CR1_STOP;
        }
        else
        {
            // More bytes to come: send ACK
            I2C1->CR1 |= I2C_CR1_ACK;
        }

        timeout = I2C_TIMEOUT_COUNT;

        // Wait until data is available in the data register
        while (!(I2C1->SR1 & I2C_SR1_RXNE))
        {
            if (--timeout == 0)
                return;
        }
           
        data[i] = I2C1->DR; // Read data
    }
}