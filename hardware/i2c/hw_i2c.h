/*********************************************************************
* Project     : Robot Arm Controller
* File        : hw_i2c.h
*
* Description :
*   Hardware driver for I2C1 communication.
*   Low-level I2C1 driver (PB8 = SCL, PB9 = SDA, 100 kHz, AF4)
*
* SPDX-License-Identifier: MIT
* Copyright (c) 2026 Manuel Wiesinger
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