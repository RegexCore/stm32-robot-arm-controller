/*********************************************************************
* Project     : Robot Arm Controller
* File        : kinematics.hpp
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   Forward and inverse kinematics of the robotic arm.
*
* Copyright (c) 2026 Manuel Wiesinger
* All rights reserved.
*********************************************************************/

#pragma once

namespace robotarm
{
    struct Vec3
    {
        float x;
        float y;
        float z;
    };

    struct IKResult
    {
        bool valid;
        // Joint angles in DEGREES including offset and inversion
        float q0;  // M0
        float q1;  // M1
        float q2;  // M2
    };

    class Kinematics
    {
    public:
        // Lengths of the upper arm (L1) and forearm (L2) in mm
        Kinematics(float link1, float link2);

        void init();

        // Forward kinematics: convert motor joint angles (q0, q1, q2) in DEGREES into position (x, y, z)
        Vec3 forward(float q0_motor_deg, float q1_motor_deg, float q2_motor_deg) const;

        // Inverse kinematics: convert (x, y, z) into motor joint angles (q0, q1, q2) in DEGREES
        IKResult inverse(float x, float y, float z, bool elbow_up = false) const;

        // Motor offsets for calculations
        void setOffsets(float off0_deg, float off1_deg, float off2_deg);

        // Take motor inversion due to mounting direction into account
        void setInverted(bool inv0, bool inv1, bool inv2);

    private:
        float m_L1; // Upper arm length
        float m_L2; // Forearm length

        // Motor offsets in DEGREES
        float m_offset0_deg; // for M0
        float m_offset1_deg; // for M1
        float m_offset2_deg; // for M2

        // Motor inversion due to mounting direction
        bool  m_inv0;
        bool  m_inv1;
        bool  m_inv2;

        // Helper functions: motor angle / kinematic angle in degrees
        float motorToKinDeg(int idx, float q_motor_deg) const;
        float kinToMotorDeg(int idx, float q_kin_deg) const;
    };
}