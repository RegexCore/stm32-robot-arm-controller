/*********************************************************************
* Project     : Robot Arm Controller
* File        : servo.hpp
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   Control and regulation of the servo motors.
*
* Copyright (c) 2026 Manuel Wiesinger
* All rights reserved.
*********************************************************************/

#pragma once

#include "../../model/types.hpp"
#include "../joystick/joystick.hpp"

namespace robotarm
{
    class ServoController
    {
    public:
        ServoController(Joystick& js);
  
        void init(model::JointAngles& servoPosition);
        void moveAllToTargets(model::JointAngles& servoPosition,
                              int stepDeg,
                              int stepDelayMs);
        const model::ServoLimits servoLimits[model::ServoID::Count];
    private:
        void moveToPosition(uint8_t motorNumber, int angle);
        int angleToPulse(float angleDeg, const model::ServoLimits& servoLimit);
        Joystick& m_joystick;
    };
}