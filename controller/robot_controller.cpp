/*********************************************************************
* Project     : Robot Arm Controller
* File        : robot_controller.cpp
*
* Description :
*   Central control logic of the robotic arm.
*
* SPDX-License-Identifier: MIT
* Copyright (c) 2026 Manuel Wiesinger
*********************************************************************/

#include "robot_controller.hpp"
#include "../model/types.hpp"
#include "../libraries/diagnostic/logger.hpp"
#include "../hardware/timer/systicktimer.h"
#include "../config/robot_config.hpp"
#include "../hardware/usart/hw_usart.h"
#include <cmath>

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
            config::activeConfig.offsets[Motor0],    // M0 (Base): servo is at 98° when the joint is kinematically at 0°
            config::activeConfig.offsets[Motor1],    // M1 (Shoulder): servo is at 170° when the joint is kinematically at 0°
            config::activeConfig.offsets[Motor2]     // M2 (Elbow): servo is at 32° when the joint is kinematically at 0°
        );

        m_kinematics.setInverted
        (
            config::activeConfig.inverted[Motor0],   // M0 moves mechanically in the same direction as the kinematics
            config::activeConfig.inverted[Motor1],   // M1 moves mechanically in the opposite direction to the kinematics
            config::activeConfig.inverted[Motor2]    // M2 moves mechanically in the opposite direction to the kinematics
        );

        writeLogData();
        diagnostic::Logger::printf("[Init] Servo positions initialized\n");
    }

    void RobotController::periodicUpdate()
    {
        bool togglePressed = m_joystick.isAutoModeOn();

        if (togglePressed && !m_lastToggleState)
        {
            m_autoMode = !m_autoMode;
            if (m_autoMoving)
                servoPosition.targetAngles = servoPosition.currentAngles;
            m_autoMoving = false;
            m_autoStep = 0;
        }

        m_lastToggleState = togglePressed;
        bool stopped = m_joystick.isEmergencyStop();
        if (stopped)
        {
            if (m_autoMoving)
                servoPosition.targetAngles = servoPosition.currentAngles;
            m_autoMoving = false;
            m_autoStep = 0;
        }
        m_joystick.update();
        bool remoteAllowed = !m_autoMode && !togglePressed && !stopped;
        HW_USART2_SetMotionAllowed(remoteAllowed);
        const char* rejection = stopped ? "ESTOP" : (m_autoMode ? "AUTO_MODE" : "MODE_BUTTON");
        if (processRemoteCommands(remoteAllowed, rejection) || stopped)
            return;

#ifdef DEBUG
        writeLogData();
#endif

        if (!togglePressed)
        {
            if (!m_autoMode)
            {
                updateServoTargetsFromJoystick();
            }
            else
            {
                updateAutomaticTransport();
            }
        }
    }

    void RobotController::receiveCommand(bool allowed, const char* rejection)
    {
        using diagnostic::Logger;
        uint32_t overflow = HW_USART2_TakeRxOverflowCount();
        if (overflow != 0)
            Logger::printf("@ERR(#0) code=RX_QUEUE_FULL count=%lu\n", (unsigned long)overflow);

        HW_USART2_RxLine line;
        if (!HW_USART2_ReadLine(&line))
            return;

        if (line.status != HW_USART2_RX_OK)
        {
            const char* error = "UART_ERROR";
            if (line.status == HW_USART2_RX_TOO_LONG)
                error = "LINE_TOO_LONG";
            else if (line.status == HW_USART2_RX_INVALID_BYTE)
                error = "INVALID_BYTE";
            Logger::printf("@ERR(#0) code=%s\n", error);
            return;
        }

        int statusId;
        if (parseStatusCommand(line.text, statusId))
        {
            writeStatus(statusId);
            return;
        }

        MotionCommand command;
        if (!parseMotionCommand(line.text, command))
        {
            Logger::printf("@ERR(#0) code=INVALID_COMMAND\n");
            return;
        }
        if (!allowed || !line.motion_allowed)
        {
            Logger::printf("@ERR(#%d) code=%s\n", command.id, allowed ? "RX_DISABLED" : rejection);
            return;
        }
        if (m_motionCount == MotionQueueCapacity)
        {
            Logger::printf("@ERR(#%d) code=MOTION_QUEUE_FULL\n", command.id);
            return;
        }

        m_motionQueue[(m_motionHead + m_motionCount) % MotionQueueCapacity] = command;
        ++m_motionCount;
        Logger::printf("@QUEUED(#%d)\n", command.id);
    }

    void RobotController::abortRemoteCommands(const char* reason)
    {
        if (m_remoteActive)
        {
            servoPosition.targetAngles = servoPosition.currentAngles;
            m_remoteActive = false;
            diagnostic::Logger::printf("@ERR(#%d) code=%s\n", m_remoteId, reason);
        }
        while (m_motionCount != 0)
        {
            int id = m_motionQueue[m_motionHead].id;
            m_motionHead = (m_motionHead + 1U) % MotionQueueCapacity;
            --m_motionCount;
            diagnostic::Logger::printf("@ERR(#%d) code=%s\n", id, reason);
        }
    }

    bool RobotController::processRemoteCommands(bool allowed, const char* rejection)
    {
        using diagnostic::Logger;
        if (!allowed)
            abortRemoteCommands(rejection);

        receiveCommand(allowed, rejection);
        if (!allowed)
            return false;

        if (m_remoteActive)
        {
            uint32_t now = systick_millis();
            if ((int32_t)(now - m_nextRemoteStep) >= 0)
            {
                auto status = m_servo.stepToTargets(servoPosition, 1);
                m_nextRemoteStep = now + 60;
                if (status == ServoController::MoveStatus::Complete)
                {
                    m_remoteActive = false;
                    Logger::printf("@DONE(#%d)\n", m_remoteId);
                }
                else if (status == ServoController::MoveStatus::Stopped)
                {
                    HW_USART2_SetMotionAllowed(0);
                    abortRemoteCommands("ESTOP");
                }
            }
            return true;
        }

        if (m_motionCount == 0)
            return false;

        const MotionCommand command = m_motionQueue[m_motionHead];
        m_motionHead = (m_motionHead + 1U) % MotionQueueCapacity;
        --m_motionCount;
        IKResult result = m_kinematics.inverse(command.x, command.y, command.z, true);
        float angles[] = {
            result.q0, result.q1, result.q2,
            static_cast<float>(command.tilt.value_or(servoPosition.currentAngles[Motor3])),
            static_cast<float>(command.rotation.value_or(servoPosition.currentAngles[Motor4])),
            static_cast<float>(command.gripper.value_or(servoPosition.currentAngles[Motor5]))
        };
        const char* error = result.valid ? nullptr : "UNREACHABLE";
        for (unsigned int motor = 0; motor < ServoID::Count && !error; ++motor)
        {
            const auto& limits = m_servo.servoLimits[motor];
            if (!std::isfinite(angles[motor]) ||
                angles[motor] < limits.limitMinAngle || angles[motor] > limits.limitMaxAngle)
                error = "SERVO_LIMIT";
        }
        if (error)
        {
            Logger::printf("@ERR(#%d) code=%s\n", command.id, error);
            return true;
        }

        for (unsigned int motor = 0; motor < ServoID::Count; ++motor)
            servoPosition.targetAngles[motor] = (int)angles[motor];
        m_remoteId = command.id;
        m_remoteActive = true;
        m_nextRemoteStep = systick_millis();
        Logger::printf("@ACK(#%d)\n", command.id);
        return true;
    }

    void RobotController::writeStatus(int id)
    {
        using diagnostic::Logger;
        const auto& current = servoPosition.currentAngles;
        const auto& target = servoPosition.targetAngles;
        const Vec3 position = m_kinematics.forward(current[Motor0], current[Motor1], current[Motor2]);
        const Vec3 targetPosition = m_kinematics.forward(target[Motor0], target[Motor1], target[Motor2]);
        const bool stopped = m_joystick.isEmergencyStop();
        const uint32_t rxCount = HW_USART2_GetRxQueueCount();
        const uint32_t time = systick_millis();
        Logger::printf("@STATUS(#%d) mode=%s estop=%d mode_button=%d busy=%d active=%d auto_step=%d\n",
            id, m_autoMode ? "AUTO" : "MANUAL", stopped, m_lastToggleState,
            m_remoteActive || m_autoMoving, m_remoteActive ? m_remoteId : 0,
            m_autoMode ? (int)m_autoStep : -1);
        Logger::printf("@POSITION(#%d) x=%d y=%d z=%d tx=%d ty=%d tz=%d\n", id,
            (int)position.x, (int)position.y, (int)position.z,
            (int)targetPosition.x, (int)targetPosition.y, (int)targetPosition.z);
        Logger::printf("@ANGLES(#%d) m0=%d m1=%d m2=%d tilt=%d rotation=%d gripper=%d\n", id,
            current[Motor0], current[Motor1], current[Motor2], current[Motor3], current[Motor4], current[Motor5]);
        Logger::printf("@TARGETS(#%d) m0=%d m1=%d m2=%d tilt=%d rotation=%d gripper=%d\n", id,
            target[Motor0], target[Motor1], target[Motor2], target[Motor3], target[Motor4], target[Motor5]);
        const auto& left = m_joystick.joysticks[0];
        const auto& right = m_joystick.joysticks[1];
        Logger::printf("@INPUTS(#%d) lx=%d ly=%d lb=%d rx=%d ry=%d rb=%d buttons=%s\n", id,
            left.x, left.y, left.button, right.x, right.y, right.button,
            m_controlGripper ? "GRIPPER" : "ROTATION");
        Logger::printf("@QUEUE(#%d) motion=%u motion_cap=%u rx=%lu rx_cap=%u uptime_ms=%lu\n", id,
            m_motionCount, MotionQueueCapacity, (unsigned long)rxCount,
            HW_USART2_RX_QUEUE_DEPTH, (unsigned long)time);
        Logger::printf("@STATUS_END(#%d)\n", id);
    }

    void RobotController::updateAutomaticTransport()
    {
        if (!m_autoMoving)
        {
#if defined(ROBOT_VARIANT_B) && !defined(ROBOT_VARIANT_A)
            constexpr int open = 50, closed = 125, tilt = 75, transitTilt = 100;
#else
            constexpr int open = 80, closed = 40, tilt = 60, transitTilt = 110;
#endif
            switch (m_autoStep)
            {
                case 0: setGripperAngle(open); break;
                case 1: performObjectTransport(80, 70, 50, tilt, 80); break;
                case 2: setGripperAngle(closed); break;
                case 3: performObjectTransport(100, 0, 100, transitTilt, 80); break;
                case 4: performObjectTransport(80, -70, 50, tilt, 80); break;
                case 5: setGripperAngle(open); break;
                case 6: performObjectTransport(100, 0, 100, 110, 80); break;
            }
            m_autoMoving = true;
            m_nextAutoStep = systick_millis();
        }
        const uint32_t now = systick_millis();
        if ((int32_t)(now - m_nextAutoStep) < 0)
            return;

        const auto status = m_servo.stepToTargets(servoPosition, 1);
        m_nextAutoStep = now + 60;
        if (status == ServoController::MoveStatus::Complete)
        {
            m_autoMoving = false;
            m_autoStep = (m_autoStep + 1U) % 7U;
        }
        else if (status == ServoController::MoveStatus::Stopped)
        {
            m_autoMoving = false;
            m_autoStep = 0;
            diagnostic::Logger::printf("[Auto] Movement stopped by emergency stop\n");
        }
    }

    void RobotController::writeLogData()
    {
        static uint64_t nextLogTime = 0;
        uint64_t now = systick_millis();

        if (now < nextLogTime)
            return;

        nextLogTime = now + 5000;
          
        Vec3 p = m_kinematics.forward(
            servoPosition.currentAngles[Motor0],
            servoPosition.currentAngles[Motor1],
            servoPosition.currentAngles[Motor2]
        );
        
        diagnostic::Logger::printf("\n******* DEBUG *******\n");
        diagnostic::Logger::printf
        (
            "Forward kinematics: Position (x, y, z) = (%d mm, %d mm, %d mm) -> Angles (q0, q1, q2) = (%d°, %d°, %d°)\n",
            (int)p.x, (int)p.y, (int)p.z,
            servoPosition.currentAngles[Motor0], servoPosition.currentAngles[Motor1], servoPosition.currentAngles[Motor2]
        );
        
        diagnostic::Logger::printf("Current servo positions: ");
        diagnostic::Logger::printf(
            "  M0: %d°  M1: %d°  M2: %d°  M3: %d°  M4: %d°  M5: %d°\n",
            servoPosition.currentAngles[Motor0],
            servoPosition.currentAngles[Motor1],
            servoPosition.currentAngles[Motor2],
            servoPosition.currentAngles[Motor3],
            servoPosition.currentAngles[Motor4],
            servoPosition.currentAngles[Motor5]
        );

        diagnostic::Logger::printf("Target servo positions: ");
        diagnostic::Logger::printf(
            "   M0: %d°  M1: %d°  M2: %d°  M3: %d°  M4: %d°  M5: %d°\n",
            servoPosition.targetAngles[Motor0],
            servoPosition.targetAngles[Motor1],
            servoPosition.targetAngles[Motor2],
            servoPosition.targetAngles[Motor3],
            servoPosition.targetAngles[Motor4],
            servoPosition.targetAngles[Motor5]
        );

        diagnostic::Logger::printf("Left joystick:  x: %d  y: %d  btn: %d\n",
            m_joystick.joysticks[0].x,
            m_joystick.joysticks[0].y,
            m_joystick.joysticks[0].button
        );

        diagnostic::Logger::printf("Right joystick: x: %d  y: %d  btn: %d\n",
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
            "Inverse kinematics: Target (x, y, z) = (%d mm, %d mm, %d mm) -> "
            "Angles (q0, q1, q2) = (%d°, %d°, %d°)\n",
            x, y, z,
            (int)result.q0, (int)result.q1, (int)result.q2
        );

        diagnostic::Logger::printf
        (
            "@IK(#99) x=%d y=%d z=%d\n",
            x, y, z
        );

        // Only set the axes that should be changed
        servoPosition.targetAngles[Motor0] = (int)result.q0;  // M0
        servoPosition.targetAngles[Motor1] = (int)result.q1;  // M1
        servoPosition.targetAngles[Motor2] = (int)result.q2;  // M2
        servoPosition.targetAngles[Motor3] = m3;              // M3 (tool tilt)
        servoPosition.targetAngles[Motor4] = m4;              // M4 (wrist rotation)
        // servoPosition.targetAngles[5] remains unchanged (gripper M5)

    }

    void RobotController::setGripperAngle(int angle)
    {
        servoPosition.targetAngles[5] = angle;
    }

    // Clamps an angle to the valid servo range
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
        
        nextUpdateTime = now + 40; // ms update frequency

        const auto &left = m_joystick.joysticks[0];
        const auto &right = m_joystick.joysticks[1];

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
        
        // --- Gripper logic ---
        bool togglePressed = (right.button && left.button);

        // Toggle only on transition: previously NOT pressed -> now pressed
        if (togglePressed && !m_lastGripperToggleState)
        {
            m_controlGripper = !m_controlGripper;
        }

        // Store state
        m_lastGripperToggleState = togglePressed;

        // If both are pressed -> only toggle, NO movement
        if (!togglePressed)
        {
            if (!m_controlGripper)
            {
                // Mode 1: Buttons control servo 4
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
                // Mode 2: Buttons control servo 5
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