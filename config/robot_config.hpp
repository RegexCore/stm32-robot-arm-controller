/*********************************************************************
* Project     : Robot Arm Controller
* File        : robot_config.hpp
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   Konfigurationsdaten für die Robotervarianten.
*
* Copyright (c) 2026 Manuel Wiesinger
* All rights reserved.
*********************************************************************/

#pragma once

#include <array>
#include "../model/types.hpp"

#define ROBOT_VARIANT_A
//#define ROBOT_VARIANT_B

namespace robotarm::config 
{
    // RobotConfig: Settings der jeweiligen Roboter
    struct RobotConfig 
    {
        float offsets[3];             // M0, M1, M2
        bool inverted[3];             // M0, M1, M2
        const robotarm::model::ServoLimits servoLimits[model::ServoID::Count]; // Limits
        std::array<int, model::ServoID::Count> initialAngles;       // Initial-Positionen
        float upperArmLength;                   // Oberarm (mm)
        float lowerArmLength;                   // Unterarm (mm)
    };

    // Roboter A Konfiguration
    // constexpr: Der Wert ist zur Compile-Zeit bekannt und kann für Arrays,
    // Templates und andere Compile-Time-Konstrukte verwendet werden.
    // inline: Erlaubt die Definition im Header, ohne Mehrfachdefinitionen
    // beim Linken zu verursachen (keine Text-Einfügung, sondern ODR-Regel)
    // ODR (One Definition Rule): Jede Variable/Funktion darf im gesamten Programm
    // nur einmal definiert sein; `inline` erlaubt mehrere *identische* Definitionen
    // über mehrere Übersetzungseinheiten hinweg.
    inline constexpr RobotConfig robotAConfig
    {
        // Offsets
        {98.0f, 170.0f, 32.0f},

        // Inverted
        {false, true, true},

        // Servo Limits
        {
            {20, 160, 0, 180, 500, 2500},       // Motor0
            {20, 170, 0, 180, 500, 2500},       // Motor1
            {30, 160, 0, 180, 500, 2500},       // Motor2
            {50, 130, 0, 180, 500, 2500},       // Motor3
            {10, 160, 0, 180, 500, 2500},       // Motor4
            {40, 120, 0, 180, 500, 2500}        // Motor5
        },

        // Startpositionen
        {90, 40, 140, 90, 80, 60},

        // Arm-Längen
        100.0f,   // Oberarm in mm
        100.0f    // Unterarm in mm
    };

    // Roboter B Konfiguration
    inline constexpr RobotConfig robotBConfig
    {
        {85.0f, 21.0f, 24.0f},

        {false, false, true},

        {
            { 0, 180, 0, 180, 500, 2500 },   // Motor0
            { 0, 180, 0, 180, 500, 2500 },   // Motor1
            { 0, 175, 0, 180, 500, 2500 },   // Motor2
            { 0, 180, 0, 180, 500, 2500 },   // Motor3
            { 0, 180, 0, 180, 500, 2500 },   // Motor4
            { 50, 125, 0, 180, 500, 2500 }   // Motor5
        },

        {90, 165, 170, 90, 90, 60},

        100.0f,
        100.0f
    };

    // Auswahl über Build-Flags
    // Das const beschreibt den Typ der Referenz, nicht die Referenzvariable. 
    // constexpr bezieht sich auf Referenzvariable nicht auf das Objekt dahinter
    // Eine constexpr-Referenz darf nur auf ein const Objekt zeigen.
    #if defined(ROBOT_VARIANT_A)
    inline constexpr const RobotConfig& activeConfig = robotAConfig;
    #elif defined(ROBOT_VARIANT_B)
    inline constexpr const RobotConfig& activeConfig = robotBConfig;
    #else
    inline constexpr const RobotConfig& activeConfig = robotAConfig; // Default
    #endif
} 
