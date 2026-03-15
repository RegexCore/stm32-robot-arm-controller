/*********************************************************************
* Project     : Robot Arm Controller
* File        : hw_i2c.c
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   Hardwaretreiber für die I2C1-Kommunikation.
*   Low-Level I2C1 Treiber (PB8 = SCL, PB9 = SDA, 100kHz, AF4)
*
* Copyright (c) 2026 Manuel Wiesinger
* All rights reserved.
*********************************************************************/

#include <stm32f4xx.h>
#include "hw_i2c.h"

#define I2C_TIMEOUT_COUNT 10000U    // Timeout zum Schutz gegen Deadlocks
#define I2C_SPEED_HZ   100000U      // Zielgeschwindigkeit 100 kHz Standardmode
#define HW_I2C_PCLK1_MHZ 16U        // Frequenz an PCLK1/APB1 in MHZ

void HW_I2C_Init(void)
{
    // GPIOB Clock aktivieren
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; 

    // Alternate Function Mode aktivieren
    GPIOB->MODER &= ~(GPIO_MODER_MODER8 | GPIO_MODER_MODER9);
    GPIOB->MODER |=  (GPIO_MODER_MODER8_1 | GPIO_MODER_MODER9_1);

    // Open Drain aktivieren
    GPIOB->OTYPER |= (GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9);

    // High Speed-Modus (saubere Signalflanken)
    GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR8 | GPIO_OSPEEDER_OSPEEDR9);

    // Interne Pull Ups Nicht aktivieren, da Servo Shield PCA9685 bereits Pull-Ups enthält
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR8 | GPIO_PUPDR_PUPDR9);

    // PB8 und PB9 Alternate Function 4 (I2C1)
    // Mit Maske für AF8 + AF9 in AFR[1] löschen
    GPIOB->AFR[1] &= ~(GPIO_AFRH_AFSEL8_Msk | GPIO_AFRH_AFSEL9_Msk);

    // AF4 für beide Pins setzen
    GPIOB->AFR[1] |=  GPIO_AFRH_AFSEL8_2 | GPIO_AFRH_AFSEL9_2;

    // I2C1-Peripherie aktivieren bzw. Clock aktivieren
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    // I2C1 Reset für sauberen Zustand
    RCC->APB1RSTR |= RCC_APB1RSTR_I2C1RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C1RST;

    // I2C1 deaktivieren, um Register gefahrlos zu ändern
    I2C1->CR1 &= ~I2C_CR1_PE;

    // APB1-Frequenz
    uint32_t pclk1_mhz = HW_I2C_PCLK1_MHZ;

    // Register konfigurieren
    I2C1->CR2 = pclk1_mhz;  // Taktfrequenz in MHz
    I2C1->CCR = (pclk1_mhz * 1000000U) / (2U * I2C_SPEED_HZ); // Standardmode (100 kHz)
    I2C1->TRISE = pclk1_mhz + 1; // Max. Anstiegszeit (Datenblatt)

    // I2C1 aktivieren
    I2C1->CR1 |= I2C_CR1_PE;
}

void HW_I2C_Transmit(uint8_t device_address, const uint8_t *data, uint8_t size)
{
    uint32_t timeout;

    // Startbedingung erzeugen
    I2C1->CR1 |= I2C_CR1_START;

    timeout = I2C_TIMEOUT_COUNT;

    // Auf Startbit warten
    while (!(I2C1->SR1 & I2C_SR1_SB))
    {
        if (--timeout == 0) 
            return;
    }

    // Slave Adresse + Write Bit senden
    I2C1->DR = (device_address << 1);

    timeout = I2C_TIMEOUT_COUNT;
    
    // Auf Adress Acknowledge warten
    while (!(I2C1->SR1 & I2C_SR1_ADDR))
    {
        if (--timeout == 0) 
            return;
    }

    (void)I2C1->SR2; // ADDR-Flag löschen durch Lesen von SR2

    // Datenbytes senden
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
    
    // Auf Abschluss warten
    while (!(I2C1->SR1 & I2C_SR1_BTF))
    {
        if (--timeout == 0) 
            return;
    }

    // Stop Bedingung setzen
    I2C1->CR1 |= I2C_CR1_STOP;
}

void HW_I2C_Receive(uint8_t device_address, uint8_t *data, uint8_t size)
{
    uint32_t timeout;

    // Startbedingung erzeugen
    I2C1->CR1 |= I2C_CR1_START;

    timeout = I2C_TIMEOUT_COUNT;
    
    // Auf Startbit warten
    while (!(I2C1->SR1 & I2C_SR1_SB))
    {
        if (--timeout == 0) 
            return;
    }

    // Slave Adresse und Read-Bit senden
    I2C1->DR = (device_address << 1) | 0x01;

    timeout = I2C_TIMEOUT_COUNT;
   
    // Auf ACK der Adresse warten
    while (!(I2C1->SR1 & I2C_SR1_ADDR))
    {
        if (--timeout == 0) 
            return;
    }

    (void)I2C1->SR2; // ADDR Flag löschen durch Lesen von SR2

    // Byteweise Daten empfangen
    for (uint8_t i = 0; i < size; i++)
    {
        if (i == (size - 1))
        {
            // Letztes Byte: NACK + STOP
            I2C1->CR1 &= ~I2C_CR1_ACK;
            I2C1->CR1 |= I2C_CR1_STOP;
        }
        else
        {
            // Weitere Bytes: ACK senden
            I2C1->CR1 |= I2C_CR1_ACK;
        }

        timeout = I2C_TIMEOUT_COUNT;

        // Warten bis Daten im Datenregister bereitstehen
        while (!(I2C1->SR1 & I2C_SR1_RXNE))
        {
             if (--timeout == 0) 
                return;
        }
           
        data[i] = I2C1->DR; //Daten auslesen
    }
}
