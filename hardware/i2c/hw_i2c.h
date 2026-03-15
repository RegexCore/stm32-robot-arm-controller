/*********************************************************************
* Project     : Robot Arm Controller
* File        : hw_i2c.h
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

#ifndef HW_I2C_H
#define HW_I2C_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void HW_I2C_Init(void);
void HW_I2C_Transmit(uint8_t device_address, const uint8_t *data, uint8_t size);
void HW_I2C_Receive(uint8_t device_address, uint8_t *data, uint8_t size);

#ifdef __cplusplus
}
#endif

#endif
