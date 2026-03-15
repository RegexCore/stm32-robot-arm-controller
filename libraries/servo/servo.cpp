/*********************************************************************
* Project     : Robot Arm Controller
* File        : servo.cpp
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   Steuerung und Regelung der Servomotoren.
*
* Copyright (c) 2026 Manuel Wiesinger
* All rights reserved.
*********************************************************************/

#include "servo.hpp"
#include "../pca9685/pca9685.h"
#include "../../model/types.hpp"
#include "../../hardware/timer/systicktimer.h"
#include "../../config/robot_config.hpp"

namespace robotarm 
{
    using namespace robotarm::model;

    ServoController::ServoController(Joystick& js)
        : m_joystick(js)
        , servoLimits
        {
            robotarm::config::activeConfig.servoLimits[Motor0],   // Motor0
            robotarm::config::activeConfig.servoLimits[Motor1],   // Motor1
            robotarm::config::activeConfig.servoLimits[Motor2],   // Motor2
            robotarm::config::activeConfig.servoLimits[Motor3],   // Motor3
            robotarm::config::activeConfig.servoLimits[Motor4],   // Motor4
            robotarm::config::activeConfig.servoLimits[Motor5]    // Motor5
        }
    { }

    void ServoController::init(model::JointAngles& servoPosition)
    {
        servoPosition.targetAngles = robotarm::config::activeConfig.initialAngles;
        servoPosition.currentAngles  = servoPosition.targetAngles;

        moveToPosition(Motor2, servoPosition.targetAngles[Motor2]);
        moveToPosition(Motor1, servoPosition.targetAngles[Motor1]);
        delay_ms(500);
        moveToPosition(Motor0, servoPosition.targetAngles[Motor0]);
        moveToPosition(Motor3, servoPosition.targetAngles[Motor3]);
        moveToPosition(Motor4, servoPosition.targetAngles[Motor4]);
        moveToPosition(Motor5, servoPosition.targetAngles[Motor5]);
    }

    void ServoController::moveAllToTargets(model::JointAngles& servoPosition,
                                       int stepDeg,
                                       int stepDelayMs)
    {
        // zur Sicherheit minimale Werte vorgeben
        if (stepDeg < 1)      stepDeg = 1;
        if (stepDelayMs < 1)  stepDelayMs = 1;

        while (true)
        {
            bool anyMoving = false;

            for (uint8_t motor = 0; motor < ServoID::Count; ++motor)
            {
                int current = servoPosition.currentAngles[motor];
                int target  = servoPosition.targetAngles[motor];

                if (target < servoLimits[motor].limitMinAngle) 
                    target = servoLimits[motor].limitMinAngle;

                if (target > servoLimits[motor].limitMaxAngle) 
                    target = servoLimits[motor].limitMaxAngle;

                int diff = target - current;

                if (diff == 0)
                    continue; // dieser Motor ist schon am Ziel

                anyMoving = true;

                int step;

                if (diff > 0)
                    step = (diff > stepDeg) ? stepDeg : diff;
                else
                    step = (diff < -stepDeg) ? -stepDeg : diff;

                current += step;
                servoPosition.currentAngles[motor] = current;

                moveToPosition(motor, current);
            }

            if (!anyMoving)
                break;

            delay_ms(stepDelayMs);
        }
    }

    void ServoController::moveToPosition(uint8_t motorNumber, int angle)
    {
        if (m_joystick.isEmergencyStop()) 
        {
            return;
        }

        int i = angleToPulse(angle, servoLimits[motorNumber]);
        PCA9685_SetServoUS(PCA9685_DEFAULT_ADDR, motorNumber, i);
    }

    int ServoController::angleToPulse(float angleDeg, const model::ServoLimits& servoLimit)
    {
        // Winkel auf erlaubten Bereich begrenzen
        if (angleDeg < servoLimit.limitMinAngle) 
            angleDeg = servoLimit.limitMinAngle;

        if (angleDeg > servoLimit.limitMaxAngle) 
            angleDeg = servoLimit.limitMaxAngle;

        float angleSpan = servoLimit.maxAngle - servoLimit.minAngle;

        // 0 kein gültiger Bereich für Berechnung nachher
        if (angleSpan == 0.0f)
            return static_cast<int>(servoLimit.minPulse);

        // Lineare Gleichung kx + d
        float k = (servoLimit.maxPulse - servoLimit.minPulse) / angleSpan;
        float d = servoLimit.minPulse - k * servoLimit.minAngle;

        float pulse = k * angleDeg + d;

        return static_cast<int>(pulse + 0.5f);
    }
}