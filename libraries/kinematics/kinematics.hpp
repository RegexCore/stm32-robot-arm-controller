/*********************************************************************
* Project     : Robot Arm Controller
* File        : kinematics.hpp
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   Vorwärts- und Inverskinematik des Roboterarms.
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
        // Gelenkwinkel in GRAD inklusive Offset und Invertierung
        float q0;  // M0
        float q1;  // M1
        float q2;  // M2
    };

    class Kinematics
    {
    public:
        // Längen des Oberarms (L1) und Unterarms (L2) in mm
        Kinematics(float link1, float link2);

        void init();

        // Forward Kinematics Motor-Gelenkwinkeln (q0, q1, q2) in GRAD umrechnung in Position (x, y, z)
        Vec3 forward(float q0_motor_deg, float q1_motor_deg, float q2_motor_deg) const;

        // Inverse Kinematics von (x, y, z) umrechnung in Motor-Gelenkwinkel (q0, q1, q2) in GRAD 
        IKResult inverse(float x, float y, float z, bool elbow_up = false) const;

        // Motor Offsets für Berechnugen
        void setOffsets(float off0_deg, float off1_deg, float off2_deg);

        // Invertierung wegen Motoreinbaurichtung berücksichtigen
        void setInverted(bool inv0, bool inv1, bool inv2);

    private:
        float m_L1; // Oberarm-Länge
        float m_L2; // Unterarm-Länge 

        // Motor Offsets in GRAD
        float m_offset0_deg; // für M0
        float m_offset1_deg; // für M1
        float m_offset2_deg; // für M2

        // Motor Invertierung wegen Einbaurichtung
        bool  m_inv0;
        bool  m_inv1;
        bool  m_inv2;

        // Hilfsfunktionen: Motorwinkel/Kinematik-Winkel in Grad
        float motorToKinDeg(int idx, float q_motor_deg) const;
        float kinToMotorDeg(int idx, float q_kin_deg) const;
    };
}
