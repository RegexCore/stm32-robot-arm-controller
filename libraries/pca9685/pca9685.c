/*********************************************************************
* Project     : Robot Arm Controller
* File        : pca9685.c
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   Control of the PCA9685 PWM controller.
*
* Copyright (c) 2026 Manuel Wiesinger
* All rights reserved.
*********************************************************************/

#include "pca9685.h"
#include "../../hardware/i2c/hw_i2c.h"

#define REG_MODE1       0x00
#define REG_MODE2       0x01
#define REG_LED0_ON_L   0x06
#define REG_PRESCALE    0xFE

#define MODE1_RESTART   (1u << 7)
#define MODE1_AI        (1u << 5)
#define MODE1_SLEEP     (1u << 4)
#define MODE1_ALLCALL   (1u << 0)
#define MODE2_OUTDRV    (1u << 2)   // 1 = totem-pole, 0 = open-drain

static void write_register(uint8_t addr, uint8_t reg, uint8_t val)
{
    uint8_t buf[2] = { reg, val };
    HW_I2C_Transmit(addr, buf, 2);
}

static void read_register(uint8_t addr, uint8_t reg, uint8_t *val)
{
    HW_I2C_Transmit(addr, &reg, 1);
    HW_I2C_Receive(addr, val, 1);
}

static void tiny_delay(void)
{
    for (volatile uint32_t i = 0; i < 40000; ++i)
        __asm__("nop");
}

static uint8_t calculate_prescale(uint32_t osc_hz, uint16_t freq_hz)
{
    if (freq_hz < 40)
        freq_hz = 40;

    if (freq_hz > 1000)
        freq_hz = 1000;

    uint64_t denom = (uint64_t)4096u * (uint64_t)freq_hz;
    uint64_t prescale_times256 = ((uint64_t)osc_hz << 8) / denom;
    uint64_t prescale = (prescale_times256 + 128) >> 8;

    if (prescale < 1)
        prescale = 1;
    
    if (prescale > 256)
        prescale = 256;

    return (uint8_t)(prescale - 1);
}

void PCA9685_Init(uint8_t addr, uint16_t pwm_freq_hz)
{
    uint8_t mode1 = 0;
    read_register(addr, REG_MODE1, &mode1);

    write_register(addr, REG_MODE1, (uint8_t)((mode1 | MODE1_SLEEP) & ~MODE1_RESTART));
    tiny_delay();

    write_register(addr, REG_PRESCALE, calculate_prescale(PCA9685_OSC_HZ, pwm_freq_hz));
    write_register(addr, REG_MODE2, MODE2_OUTDRV);

    write_register(addr, REG_MODE1, (uint8_t)((mode1 & ~MODE1_SLEEP) | MODE1_AI | MODE1_ALLCALL));
    tiny_delay();

    write_register(addr, REG_MODE1, (uint8_t)(MODE1_RESTART | MODE1_AI | MODE1_ALLCALL));
}

void PCA9685_SetPWM(uint8_t addr, uint8_t channel, uint16_t on, uint16_t off)
{
    if (channel > 15)
        return;

    on  &= 0x0FFF;
    off &= 0x0FFF;

    uint8_t reg = (uint8_t)(REG_LED0_ON_L + 4u * channel);
    uint8_t buf[5] = {
        reg,
        (uint8_t)(on & 0xFF),
        (uint8_t)((on >> 8) & 0x0F),
        (uint8_t)(off & 0xFF),
        (uint8_t)((off >> 8) & 0x0F)
    };

    HW_I2C_Transmit(addr, buf, 5);
}

void PCA9685_SetServoUS(uint8_t addr, uint8_t channel, uint16_t pulse_us)
{
    const uint32_t period_us = 20000UL;   // at 50 Hz

    uint16_t ticks = (uint16_t)(((uint64_t)pulse_us * 4096ULL + (period_us / 2)) / period_us);

    if (ticks > 0)
        ticks -= 1;

    if (ticks > 4095)
        ticks = 4095;

    PCA9685_SetPWM(addr, channel, 0, ticks);
}