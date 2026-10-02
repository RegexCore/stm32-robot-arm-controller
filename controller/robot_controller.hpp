/*********************************************************************
* Project     : Robot Arm Controller
* File        : robot_controller.hpp
*
* Description :
*   Central control logic of the robotic arm.
*
* SPDX-License-Identifier: MIT
* Copyright (c) 2026 Manuel Wiesinger
*********************************************************************/

#pragma once

#include "../libraries/servo/servo.hpp"
#include "../libraries/joystick/joystick.hpp"
#include "../libraries/kinematics/kinematics.hpp"
#include "../libraries/protocol/motion_command.hpp"

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
        bool m_autoMode = false;
        bool m_lastToggleState = false;
        bool m_remoteActive = false;
        int m_remoteId = 0;
        uint32_t m_nextRemoteStep = 0;
        static constexpr unsigned int MotionQueueCapacity = 8;
        MotionCommand m_motionQueue[MotionQueueCapacity];
        unsigned int m_motionHead = 0;
        unsigned int m_motionCount = 0;
        bool m_autoMoving = false;
        unsigned int m_autoStep = 0;
        uint32_t m_nextAutoStep = 0;
        bool m_controlGripper = true;
        bool m_lastGripperToggleState = false;
        void receiveCommand(bool allowed, const char* rejection);
        void abortRemoteCommands(const char* reason);
        void writeStatus(int id);
        void updateAutomaticTransport();
        bool processRemoteCommands(bool allowed, const char* rejection);
        void writeLogData();
        void updateServoTargetsFromJoystick();
        float clampTargetAngle(float angle, const model::ServoLimits& limits);
        void performObjectTransport(int x, int y, int z, int m3, int m4);
        void setGripperAngle(int angle);
    };
}