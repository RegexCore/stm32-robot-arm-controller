/*********************************************************************
* Project     : Robot Arm Controller
* File        : kinematics.cpp
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   Forward and inverse kinematics of the robotic arm.
*
* Copyright (c) 2026 Manuel Wiesinger
* All rights reserved.
*********************************************************************/

#include "kinematics.hpp"
#include <cmath>    // std::sin, std::cos, std::atan2, std::sqrt

namespace robotarm
{
    // visible only in this file (anonymous namespace)
    namespace
    {
        constexpr float PI      = 3.14159265358979323846f;
        constexpr float DEG2RAD = PI / 180.0f;
        constexpr float RAD2DEG = 180.0f / PI;
    }

    Kinematics::Kinematics(float link1, float link2)
        : m_L1(link1),
          m_L2(link2),
          m_offset0_deg(0.0f),
          m_offset1_deg(0.0f),
          m_offset2_deg(0.0f),
          m_inv0(false),
          m_inv1(false),
          m_inv2(false)
    { }

    void Kinematics::init()
    {
       
    }

    void Kinematics::setOffsets(float off0_deg, float off1_deg, float off2_deg)
    {
        m_offset0_deg = off0_deg;
        m_offset1_deg = off1_deg;
        m_offset2_deg = off2_deg;
    }

    void Kinematics::setInverted(bool inv0, bool inv1, bool inv2)
    {
        m_inv0 = inv0;
        m_inv1 = inv1;
        m_inv2 = inv2;
    }

    // Motor -> kinematics (all values in DEGREES)
    float Kinematics::motorToKinDeg(int idx, float q_motor_deg) const
    {
        float offset = 0.0f;
        bool inv = false;

        switch (idx)
        {
            case 0: 
                offset = m_offset0_deg; 
                inv = m_inv0; 
                break;
            case 1:
                offset = m_offset1_deg; 
                inv = m_inv1; 
                break;
            case 2: 
                offset = m_offset2_deg; 
                inv = m_inv2; 
                break;
            default: 
                break;
        }

        float q_rel = q_motor_deg - offset;  // remove offset
        
        if (inv)
            q_rel = -q_rel;                  // invert direction

        return q_rel; // kinematic angle in DEGREES
    }

    // Kinematics -> motor (all values in DEGREES)
    float Kinematics::kinToMotorDeg(int idx, float q_kin_deg) const
    {
        float offset = 0.0f;
        bool inv = false;

        switch (idx)
        {
            case 0: 
                offset = m_offset0_deg; 
                inv = m_inv0; 
                break;
            case 1: 
                offset = m_offset1_deg; 
                inv = m_inv1; 
                break;
            case 2: 
                offset = m_offset2_deg; 
                inv = m_inv2; 
                break;
            default: 
                break;
        }

        if (inv)
            q_kin_deg = -q_kin_deg;         // reverse direction again

        float q_motor_deg = offset + q_kin_deg; // add offset

        return q_motor_deg;
    }

    // Calculate x, y, z coordinates from motor angles using forward kinematics
    Vec3 Kinematics::forward(float q0_motor_deg, float q1_motor_deg, float q2_motor_deg) const
    {
        // Motor angles -> kinematic angles (DEGREES)
        float q0_deg = motorToKinDeg(0, q0_motor_deg);
        float q1_deg = motorToKinDeg(1, q1_motor_deg);
        float q2_deg = motorToKinDeg(2, q2_motor_deg);

        // Convert to radians for sin/cos
        float q0 = q0_deg * DEG2RAD;
        float q1 = q1_deg * DEG2RAD;
        float q2 = q2_deg * DEG2RAD;

        // Projection of the two segments into the arm plane (r = distance from base axis)
        float r = m_L1 * std::cos(q1) + m_L2 * std::cos(q1 + q2);

        Vec3 p;
        // Rotation around Z by q0 (base)
        p.x = std::cos(q0) * r;
        p.y = std::sin(q0) * r;

        // Height z
        p.z = m_L1 * std::sin(q1) + m_L2 * std::sin(q1 + q2);

        return p;
    }

    // Calculate motor angles from x, y, z coordinates using inverse kinematics
    IKResult Kinematics::inverse(float x, float y, float z, bool elbow_up) const
    {
        IKResult res;

        res.valid = false;
        res.q0 = 0.0f;
        res.q1 = 0.0f;
        res.q2 = 0.0f;

        // Base angle q0
        float q0 = std::atan2(y, x);

        // Distance in the XY plane
        float r = std::sqrt(x * x + y * y);

        // Law of cosines for elbow angle q2
        float num = r * r + z * z - m_L1 * m_L1 - m_L2 * m_L2;
        float den = 2.0f * m_L1 * m_L2;
        
        if (den == 0.0f)
        {
            return res; // safety check
        }

        float D = num / den;

        // Target outside reachable range?
        if (D < -1.0f || D > 1.0f)
        {
            return res;
        }

        float s2 = std::sqrt(1.0f - D * D);

        if (elbow_up)
        {
            s2 = -s2;
        }

        float q2 = std::atan2(s2, D);

        // Shoulder angle q1
        float phi = std::atan2(z, r);
        float psi = std::atan2(m_L2 * s2, m_L1 + m_L2 * D);
        float q1 = phi - psi;

        // Convert radians to kinematic angles in DEGREES
        float q0_deg_kin = q0 * RAD2DEG;
        float q1_deg_kin = q1 * RAD2DEG;
        float q2_deg_kin = q2 * RAD2DEG;

        // Convert kinematic angles (degrees) to motor angles (degrees)
        float q0_motor_deg = kinToMotorDeg(0, q0_deg_kin);
        float q1_motor_deg = kinToMotorDeg(1, q1_deg_kin);
        float q2_motor_deg = kinToMotorDeg(2, q2_deg_kin);

        res.valid = true;
        res.q0 = q0_motor_deg;
        res.q1 = q1_motor_deg;
        res.q2 = q2_motor_deg;

        return res;
    }
}