/*********************************************************************
* Project     : Robot Arm Controller
* File        : robot_config.hpp
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   Configuration data for the robot variants.
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
    // RobotConfig: configuration settings for the supported robot variants
    struct RobotConfig
    {
        float offsets[3];             // M0, M1, M2
        bool inverted[3];             // M0, M1, M2
        const robotarm::model::ServoLimits servoLimits[model::ServoID::Count]; // Limits
        std::array<int, model::ServoID::Count> initialAngles;                  // Initial positions
        float upperArmLength;                                               // Upper arm length (mm)
        float lowerArmLength;                                               // Forearm length (mm)
    };

    // Robot A configuration
    // constexpr: the value is known at compile time and can be used for arrays,
    // templates, and other compile-time constructs.
    // inline: allows the definition in a header without causing multiple
    // definition linker errors (this is not textual inlining, but an ODR rule).
    // ODR (One Definition Rule): each variable/function may only be defined once
    // across the entire program; `inline` allows multiple *identical* definitions
    // across translation units.
    inline constexpr RobotConfig robotAConfig
    {
        // Offsets
        {98.0f, 170.0f, 32.0f},

        // Inverted
        {false, true, true},

        // Servo limits
        {
            {20, 160, 0, 180, 500, 2500},       // Motor0
            {20, 170, 0, 180, 500, 2500},       // Motor1
            {30, 160, 0, 180, 500, 2500},       // Motor2
            {50, 130, 0, 180, 500, 2500},       // Motor3
            {10, 160, 0, 180, 500, 2500},       // Motor4
            {40, 120, 0, 180, 500, 2500}        // Motor5
        },

        // Initial positions
        {90, 40, 140, 90, 80, 60},

        // Arm lengths
        100.0f,   // Upper arm in mm
        100.0f    // Forearm in mm
    };

    // Robot B configuration
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

    // Selection via build flags
    // const describes the referenced type, not the reference variable itself.
    // constexpr applies to the reference variable, not the referenced object.
    // A constexpr reference may only refer to a const object.
    #if defined(ROBOT_VARIANT_A)
    inline constexpr const RobotConfig& activeConfig = robotAConfig;
    #elif defined(ROBOT_VARIANT_B)
    inline constexpr const RobotConfig& activeConfig = robotBConfig;
    #else
    inline constexpr const RobotConfig& activeConfig = robotAConfig; // Default
    #endif
}