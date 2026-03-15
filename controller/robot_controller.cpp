/*********************************************************************
* Project     : Robot Arm Controller
* File        : robot_controller.cpp
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   Zentrale Steuerlogik des Roboterarms.
*
* Copyright (c) 2026 Manuel Wiesinger
* All rights reserved.
*********************************************************************/

#include "robot_controller.hpp"
#include "../model/types.hpp"
#include "../libraries/diagnostic/logger.hpp"
#include "../hardware/timer/systicktimer.h"
#include "../config/robot_config.hpp"

namespace robotarm 
{
    using namespace model;

    RobotController::RobotController(Joystick& js,
                                     Kinematics& kin,
                                     ServoController& servo)
        : m_joystick(js)
        , m_kinematics(kin)
        , m_servo(servo)
    {

    }

    void RobotController::init()
    {
        m_servo.init(servoPosition);

        m_kinematics.setOffsets
        (
            config::activeConfig.offsets[Motor0],    // M0 (Basis): Servo steht bei 98°, wenn Gelenk kinematisch 0° ist
            config::activeConfig.offsets[Motor1],    // M1 (Schulter): Servo steht bei 170°, wenn Gelenk kinematisch 0° ist
            config::activeConfig.offsets[Motor2]     // M2 (Ellbogen): Servo steht bei 32°,  wenn Gelenk kinematisch 0° ist
        );

        m_kinematics.setInverted
        (
            config::activeConfig.inverted[Motor0],  // M0 läuft mechanisch in richtiger Richtung zur Kinematik
            config::activeConfig.inverted[Motor1],  // M1 läuft mechanisch in entgegengesetzter Richtung zur Kinematik
            config::activeConfig.inverted[Motor2]   // M2 läuft mechanisch in entgegengesetzter Richtung zur Kinematik
        );

        writeLogData();
        diagnostic::Logger::printf("[Init] Servo Positions Initialized\n");
    }

    void RobotController::periodicUpdate()
    {
#ifdef DEBUG
        writeLogData();
#endif
        static bool autoMode = false;
        static bool lastToggleState = false;
        bool togglePressed = m_joystick.isAutoModeOn();

        if (togglePressed && !lastToggleState)
        {
            autoMode = !autoMode;
        }

        lastToggleState = togglePressed;

        if (!togglePressed)
        {
            if (!autoMode)
            {
                m_joystick.update();
                updateServoTargetsFromJoystick();
            }
            else
            {
                //servoPosition.targetAngles = {120, 60, 160, 60, 60, 80};
                //m_servo.moveAllToTargets(servoPosition, 1, 60);
                // servoPosition.targetAngles = {120, 60, 160, 60, 110, 41};
                //m_servo.moveAllToTargets(servoPosition, 1, 60);
                //servoPosition.targetAngles = {120, 60, 160, 60, 110, 80};
                //m_servo.moveAllToTargets(servoPosition, 1, 60);
                //servoPosition.targetAngles = {90, 40, 140, 90, 80, 60};
                //m_servo.moveAllToTargets(servoPosition, 1, 30);

                int position1X = 80;
                int position1Y = 70;
                int position1Z = 50;

                int position2X = 100;
                int position2Y = 0;
                int position2Z = 100;

                int position3X = 80;
                int position3Y = -70;
                int position3Z = 50;

#if defined(ROBOT_VARIANT_A)
                setGripperAngle(80);
                performObjectTransport(position1X, position1Y, position1Z,60,80);
                setGripperAngle(40);

                performObjectTransport(position2X, position2Y, position2Z,110,80);

                performObjectTransport(position3X, position3Y, position3Z,60,80);
                setGripperAngle(80);

                performObjectTransport(position2X, position2Y, position2Z,110,80);
#elif defined(ROBOT_VARIANT_B)
                setGripperAngle(50);
                performObjectTransport(position1X, position1Y, position1Z,75,80);
                setGripperAngle(125);

                performObjectTransport(position2X, position2Y, position2Z,100,80);

                performObjectTransport(position3X, position3Y, position3Z,75,80);
                setGripperAngle(50);

                performObjectTransport(position2X, position2Y, position2Z,110,80);
#endif
            }
        }
    }

    void RobotController::writeLogData()
    {
        static uint64_t nextLogTime = 0;
        uint64_t now = systick_millis();

        if (now < nextLogTime)
            return;

        nextLogTime = now + 5000;
          
        Vec3 p = m_kinematics.forward(servoPosition.currentAngles[Motor0],servoPosition.currentAngles[Motor1], servoPosition.currentAngles[Motor2]);
        
        diagnostic::Logger::printf("\n******* DEBUG *******\n");
        diagnostic::Logger::printf
        (
            "Forward Kinematics: Position (x, y, z) = (%dmm, %dmm, %dmm) -> Angles (q0, q1, q2) = (%d°, %d°, %d°)\n",
            (int)p.x, (int)p.y, (int)p.z,
            servoPosition.currentAngles[Motor0], servoPosition.currentAngles[Motor1], servoPosition.currentAngles[Motor2]
        );
        
        diagnostic::Logger::printf("Current Servo Positions: ");
        diagnostic::Logger::printf(
            "  M0: %d°  M1: %d°  M2: %d°  M3: %d°  M4: %d°  M5: %d°\n",
            servoPosition.currentAngles[Motor0],
            servoPosition.currentAngles[Motor1],
            servoPosition.currentAngles[Motor2],
            servoPosition.currentAngles[Motor3],
            servoPosition.currentAngles[Motor4],
            servoPosition.currentAngles[Motor5]
        );

        diagnostic::Logger::printf("Target Servo Positions: ");
        diagnostic::Logger::printf(
            "   M0: %d°  M1: %d°  M2: %d°  M3: %d°  M4: %d°  M5: %d°\n",
            servoPosition.targetAngles[Motor0],
            servoPosition.targetAngles[Motor1],
            servoPosition.targetAngles[Motor2],
            servoPosition.targetAngles[Motor3],
            servoPosition.targetAngles[Motor4],
            servoPosition.targetAngles[Motor5]
        );

        diagnostic::Logger::printf("Joystick Left:  x: %d  y: %d  btn: %d\n",
            m_joystick.joysticks[0].x,
            m_joystick.joysticks[0].y,
            m_joystick.joysticks[0].button
        );

        diagnostic::Logger::printf("Joystick Right: x: %d  y: %d  btn: %d\n",
            m_joystick.joysticks[1].x,
            m_joystick.joysticks[1].y,
            m_joystick.joysticks[1].button
        );
    }

    void RobotController::performObjectTransport(int x, int y, int z, int m3, int m4)
    {
        IKResult result = m_kinematics.inverse(x, y, z, true);

        diagnostic::Logger::printf("\n***** AUTOMODE *****\n");
        diagnostic::Logger::printf
        (
            "Inverse Kinematics: Target (x, y, z) = (%dmm, %dmm, %dmm) -> "
            "Angles (q0, q1, q2) = (%d°, %d°, %d°)\n",
            x, y, z,
            (int)result.q0, (int)result.q1, (int)result.q2
        );

        diagnostic::Logger::printf
        (
            "@IK(#99) x=%d y=%d z=%d\n",
            x, y, z
        );

        // Nur Achsen setzen, die geändert werden sollen
        servoPosition.targetAngles[Motor0] = (int)result.q0;  // M0
        servoPosition.targetAngles[Motor1] = (int)result.q1;  // M1
        servoPosition.targetAngles[Motor2] = (int)result.q2;  // M2
        servoPosition.targetAngles[Motor3] = m3;              // M3 (Handgelenk-Rotation)
        servoPosition.targetAngles[Motor4] = m4;              // M4 (Handgelenk-Neigung)
        // servoPosition.targetAngles[5] bleibt unverändert (Greifer M5)

        m_servo.moveAllToTargets(servoPosition, 1, 60);
    }

    void RobotController::setGripperAngle(int angle)
    {
        servoPosition.targetAngles[5] = angle;
        m_servo.moveAllToTargets(servoPosition, 1, 60);
    }

    // Begrenzt einen Winkel auf den gültigen Servo-Bereich
    float RobotController::clampTargetAngle(float angle, const model::ServoLimits& limits)
    {
        if (angle < limits.limitMinAngle)
            angle = limits.limitMinAngle;

        if (angle > limits.limitMaxAngle)
            angle = limits.limitMaxAngle;

        return angle;
    }

    void RobotController::updateServoTargetsFromJoystick()
    {
        static uint64_t nextUpdateTime = 0;
        uint64_t now = systick_millis();

        if (now < nextUpdateTime)
            return;
        
        if (m_joystick.isEmergencyStop())
            return;
        
        nextUpdateTime = now + 40; // ms Update Frequenz

        const auto &left= m_joystick.joysticks[0];
        const auto &right= m_joystick.joysticks[1];

        bool changed = false;

        if (left.x > 3000) 
        {
           servoPosition.targetAngles[Motor0] =
            clampTargetAngle(servoPosition.targetAngles[Motor0] + 1,
                                m_servo.servoLimits[Motor0]);
            changed = true;
        }
        else if (left.x < 1000) 
        {
            servoPosition.targetAngles[Motor0] =
            clampTargetAngle(servoPosition.targetAngles[Motor0] - 1,
                                m_servo.servoLimits[Motor0]);
            changed = true;
        }

        if (left.y > 3000) 
        {
            servoPosition.targetAngles[Motor1] =
            clampTargetAngle(servoPosition.targetAngles[Motor1] - 1,
                                m_servo.servoLimits[Motor1]);
            changed = true;
        }
        else if (left.y < 1000) 
        {
            servoPosition.targetAngles[Motor1] =
            clampTargetAngle(servoPosition.targetAngles[Motor1] + 1,
                                m_servo.servoLimits[Motor1]);
            changed = true;
        }

        if (right.y > 3000) 
        {
            servoPosition.targetAngles[Motor2] =
            clampTargetAngle(servoPosition.targetAngles[Motor2] + 1,
                                m_servo.servoLimits[Motor2]);
            changed = true;
        }
        else if (right.y < 1000) 
        {
            servoPosition.targetAngles[Motor2] =
            clampTargetAngle(servoPosition.targetAngles[Motor2] - 1,
                                m_servo.servoLimits[Motor2]);
            changed = true;
        }

        if (right.x > 3000) 
        {
            servoPosition.targetAngles[Motor3] =
            clampTargetAngle(servoPosition.targetAngles[Motor3] + 1,
                                m_servo.servoLimits[Motor3]);
            changed = true;
        }
        else if (right.x < 1000) 
        {
            servoPosition.targetAngles[Motor3] =
            clampTargetAngle(servoPosition.targetAngles[Motor3] - 1,
                                m_servo.servoLimits[Motor3]);
            changed = true;
        }
        
        // --- Gripper-Logik ---
        static bool gripper = true;
        static bool lastToggleState = false;   // Merkt sich, ob beide Buttons vorher gedrückt waren

        bool togglePressed = (right.button && left.button);

        // Umschalten nur beim Übergang: vorher NICHT gedrückt → jetzt gedrückt
        if (togglePressed && !lastToggleState)
        {
            gripper = !gripper;
        }

        // Zustand speichern
        lastToggleState = togglePressed;

        // Wenn beide gedrückt → NUR toggeln, KEINE Bewegung
        if (!togglePressed)
        {
            if (!gripper)
            {
                // Modus 1: Buttons steuern Servo 4
                if (right.button)
                {
                    servoPosition.targetAngles[Motor4] =
                                clampTargetAngle(servoPosition.targetAngles[Motor4] + 1,
                                m_servo.servoLimits[Motor4]);
                    changed = true;
                }
                else if (left.button)
                {
                    servoPosition.targetAngles[Motor4] =
                                clampTargetAngle(servoPosition.targetAngles[Motor4] - 1,
                                m_servo.servoLimits[Motor4]);
                    changed = true;
                }
            }
            else
            {
                // Modus 2: Buttons steuern Servo 5
                if (left.button)
                {
                    servoPosition.targetAngles[Motor5] =
                                clampTargetAngle(servoPosition.targetAngles[Motor5] + 1,
                                m_servo.servoLimits[Motor5]);
                    changed = true;
                }
                else if (right.button)
                {
                    servoPosition.targetAngles[Motor5] =
                                clampTargetAngle(servoPosition.targetAngles[Motor5] - 1,
                                m_servo.servoLimits[Motor5]);
                    changed = true;
                }
            }
        }

        if (changed)
            m_servo.moveAllToTargets(servoPosition, 1, 0);
    }
}