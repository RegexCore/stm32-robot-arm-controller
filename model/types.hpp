/*********************************************************************
* Project     : Robot Arm Controller
* File        : types.hpp
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   Datentypen für Gelenke, Joystick und Servos.
*
* Copyright (c) 2026 Manuel Wiesinger
* All rights reserved.
*********************************************************************/

#pragma once

#include <array>

namespace robotarm::model 
{
    // JointAngles: 6 Gelenkwinkel des Roboters
    // (IK Ergebnis oder FK Eingang)
    struct JointAngles
    {
        std::array<int, 6> targetAngles{};   // Soll-Winkel (was die Servos erreichen sollen)
        std::array<int, 6> currentAngles{};  // Ist-Winkel (aktuelle Position)
    };

    // JoystickState: Zustand des Steuer-Panels
    struct JoystickState
    {
        int x = 0;     // 0 … +4095
        int y = 0;     // 0 … +4095

        bool button = false;
    };

    // ServoLimits: Grenzen für jeden Servo-Motor
    // Notwendig um IK-Ausgabe auf reale Servowerte abzubilden
    struct ServoLimits
    {
        int limitMinAngle = 0;      // Grad
        int limitMaxAngle = 180;    // Grad

        int minAngle = 0;      // Grad
        int maxAngle = 180;    // Grad

        int minPulse = 500;    // µs
        int maxPulse = 2500;   // µs
    };

    // Servo Motor Zuordnung (enum class ServoID wäre typsicher)
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
