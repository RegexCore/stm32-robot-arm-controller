/*********************************************************************
* Project     : Robot Arm Controller
* File        : joystick.cpp
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   Reading and providing the joystick states.
*
* Copyright (c) 2026 Manuel Wiesinger
* All rights reserved.
*********************************************************************/

#include "joystick.hpp"
#include "../../hardware/adc/hw_adc.h"
#include "../../hardware/gpio/hw_gpio.h"

namespace robotarm
{
    Joystick::Joystick()
    {

    }

    void Joystick::update()
    {
        joysticks[0].x = adc_values[0];    // left joystick x-axis
        joysticks[0].y = adc_values[1];    // left joystick y-axis
        joysticks[1].x = adc_values[2];    // right joystick x-axis
        joysticks[1].y = adc_values[3];    // right joystick y-axis

        joysticks[0].button = HW_PC10_Read();    // read left button
        joysticks[1].button = HW_PC11_Read();    // read right button
    }

    bool Joystick::isEmergencyStop() const
    {
        return HW_PC12_Read();
    }

    bool Joystick::isAutoModeOn() const
    {
        return HW_PA15_Read();
    }
}