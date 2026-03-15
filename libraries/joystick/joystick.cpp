/*********************************************************************
* Project     : Robot Arm Controller
* File        : joystick.cpp
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   Einlesen und Bereitstellen der Joystick-Zustände.
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
         joysticks[0].x = adc_values[0];    //linker Joystick x-Achse
         joysticks[0].y = adc_values[1];    //linker Joystick y-Achse
         joysticks[1].x = adc_values[2];    //rechter Joystick x-Achse
         joysticks[1].y = adc_values[3];    //rechter Joystick y-Achse

         joysticks[0].button = HW_PC10_Read();    //linken Button einlesen
         joysticks[1].button = HW_PC11_Read();    //rechten Button einlesen
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