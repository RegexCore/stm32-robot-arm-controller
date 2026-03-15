/*********************************************************************
* Project     : Robot Arm Controller
* File        : joystick.hpp
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   Reading and providing the joystick states.
*
* Copyright (c) 2026 Manuel Wiesinger
* All rights reserved.
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