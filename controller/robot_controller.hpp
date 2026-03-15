/*********************************************************************
* Project     : Robot Arm Controller
* File        : robot_controller.hpp
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   Central control logic of the robotic arm.
*
* Copyright (c) 2026 Manuel Wiesinger
* All rights reserved.
*********************************************************************/

#pragma once

#include "../libraries/servo/servo.hpp"
#include "../libraries/joystick/joystick.hpp"
#include "../libraries/kinematics/kinematics.hpp"

namespace robotarm 
{
    class RobotController
    {
    public:
        RobotController(Joystick& js,
                        Kinematics& kin,
                        ServoController& servo);

        void init();
        void periodicUpdate();

    private:
        Joystick& m_joystick;
        Kinematics& m_kinematics;
        ServoController& m_servo;
        model::JointAngles servoPosition;
        void writeLogData();
        void updateServoTargetsFromJoystick();
        float clampTargetAngle(float angle, const model::ServoLimits& limits);
        void performObjectTransport(int x, int y, int z, int m3, int m4);
        void setGripperAngle(int angle);
    };
}