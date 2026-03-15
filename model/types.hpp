/*********************************************************************
* Project     : Robot Arm Controller
* File        : types.hpp
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   Data types for joints, joystick and servos.
*
* Copyright (c) 2026 Manuel Wiesinger
* All rights reserved.
*********************************************************************/

#pragma once

#include <array>

namespace robotarm::model
{
    // JointAngles: 6 joint angles of the robot
    // (IK result or FK input)
    struct JointAngles
    {
        std::array<int, 6> targetAngles{};   // Target angles (what the servos should reach)
        std::array<int, 6> currentAngles{};  // Actual angles (current position)
    };

    // JoystickState: state of the control panel
    struct JoystickState
    {
        int x = 0;     // 0 … +4095
        int y = 0;     // 0 … +4095

        bool button = false;
    };

    // ServoLimits: limits for each servo motor
    // Required to map IK output to real servo values
    struct ServoLimits
    {
        int limitMinAngle = 0;      // degrees
        int limitMaxAngle = 180;    // degrees

        int minAngle = 0;           // degrees
        int maxAngle = 180;         // degrees

        int minPulse = 500;         // µs
        int maxPulse = 2500;        // µs
    };

    // Servo motor mapping (enum class ServoID would be type-safe)
    enum ServoID : uint8_t
    {
        Motor0 = 0,
        Motor1 = 1,
        Motor2 = 2,
        Motor3 = 3,
        Motor4 = 4,
        Motor5 = 5,
        Count = 6
    };
}