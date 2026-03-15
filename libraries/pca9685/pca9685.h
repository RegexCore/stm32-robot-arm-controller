/*********************************************************************
* Project     : Robot Arm Controller
* File        : pca9685.h
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   Control of the PCA9685 PWM controller.
*
* Copyright (c) 2026 Manuel Wiesinger
* All rights reserved.
*********************************************************************/

#pragma once
#include <stdint.h>

#ifndef PCA9685_OSC_HZ
#define PCA9685_OSC_HZ     25000000UL   // internal oscillator 25 MHz
#endif

#ifndef PCA9685_DEFAULT_ADDR
#define PCA9685_DEFAULT_ADDR  0x40      // A5 to A0 = GND
#endif

#ifndef PCA9685_DEFAULT_FREQUENCY
#define PCA9685_DEFAULT_FREQUENCY 50    // Servo frequency 50 Hz
#endif

#ifdef __cplusplus
extern "C" {
#endif

void PCA9685_Init(uint8_t addr, uint16_t pwm_freq_hz);
void PCA9685_SetPWM(uint8_t addr, uint8_t channel, uint16_t on, uint16_t off);
void PCA9685_SetServoUS(uint8_t addr, uint8_t channel, uint16_t pulse_us);

#ifdef __cplusplus
}
#endif