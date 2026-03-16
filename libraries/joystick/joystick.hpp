/*********************************************************************
* Project     : Robot Arm Controller
* File        : joystick.hpp
*
* Description :
*   Reading and providing the joystick states.
*
* SPDX-License-Identifier: MIT
* Copyright (c) 2026 Manuel Wiesinger
*********************************************************************/

#pragma once

#include "../../model/types.hpp"

namespace robotarm
{
    class Joystick
    {
    public:
        Joystick();
        model::JoystickState joysticks[2];
        void init();
        void update();
        bool isEmergencyStop() const;
        bool isAutoModeOn() const;
    private:

    };
}