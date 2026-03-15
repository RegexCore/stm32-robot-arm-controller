/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*                        The Embedded Experts                        *
**********************************************************************

File        : main.cpp
Purpose     : Generic application start

Project     : Robot Arm Controller
Author      : Manuel Wiesinger
Created     : 24.02.2026
Description : Main application entry for robotic arm control firmware

* Copyright (c) 2026 Manuel Wiesinger
* All rights reserved.
*********************************************************************/

#include <stdio.h>
#include <stm32f4xx.h>
#include "hardware/i2c/hw_i2c.h"
#include "hardware/adc/hw_adc.h"
#include "hardware/gpio/hw_gpio.h"
#include "hardware/timer/systicktimer.h"
#include "hardware/timer/timer_interrupt.h"
#include "libraries/pca9685/pca9685.h"
#include "libraries/diagnostic/logger.hpp"
#include "libraries/joystick/joystick.hpp"
#include "libraries/servo/servo.hpp"
#include "libraries/kinematics/kinematics.hpp"
#include "controller/robot_controller.hpp"
#include "config/robot_config.hpp"

using namespace diagnostic;
using namespace robotarm;

volatile bool control_tick = true;

int main(void) 
{
    printf("[Init] Steps:\n");
    SystemCoreClockUpdate();
    Logger::init();
    TIM7_Init_10ms();
    systick_init();
    HW_I2C_Init();
    HW_ADC_Init();
    HW_PC10_PC11_Init(PC_NO_PULL);
    HW_PC12_Init(PC_NO_PULL);
    HW_PA15_Init(PC_NO_PULL);
    PCA9685_Init(PCA9685_DEFAULT_ADDR, PCA9685_DEFAULT_FREQUENCY);

    uint32_t mhz   = SystemCoreClock / 1000000;
    uint32_t centi = (SystemCoreClock % 1000000) / 10000;
    
    Logger::printf("****** SYSTEM ******\n");
    Logger::printf("[Clock] System Core Clock: %d Hz (%d.%d MHz)\n",
         SystemCoreClock, mhz, centi);
    
    Logger::printf("[Init] Peripheral setup completed\n");
    Logger::printf("[Main] Program start\n");

    Joystick joystick;
    Kinematics kinematics(config::activeConfig.upperArmLength, config::activeConfig.lowerArmLength);  // Upper arm = 100, forearm = 100 (lengths in mm)
    ServoController servocontroller(joystick);
    RobotController robotcontroller(joystick, kinematics, servocontroller);
    
    robotcontroller.init();

    while (1)
    { 
        if(control_tick)
        {
            control_tick = false;
            robotcontroller.periodicUpdate();
        }
    }
}

void TIM7_IRQHandler(void)
{
    if (TIM7->SR & TIM_SR_UIF)   // Update interrupt?
    {
        control_tick = true;
        TIM7->SR &= ~TIM_SR_UIF; // Clear flag
    }
}