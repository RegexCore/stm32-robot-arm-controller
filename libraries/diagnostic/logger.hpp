/*********************************************************************
* Project     : Robot Arm Controller
* File        : logger.hpp
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   Logger für Diagnose- und Debugausgaben über USART.
*
* Copyright (c) 2026 Manuel Wiesinger
* All rights reserved.
*********************************************************************/

#pragma once

#include "logger.h"

/**
 * C++-Wrapper für das C-basierte Logger-Modul.
 * Stellt einfache statische Funktionen für Diagnose- und Log-Ausgaben bereit.
 */
namespace diagnostic
{
    class Logger
    {
    public:
        /// Initialisiert das Logger-System (muss einmal vor der Nutzung aufgerufen werden).
        static void init()
        {
            logger_init();
        }

        /// Gibt eine formatierte Diagnose-/Log-Nachricht aus.
        template<typename... Args>
        static void printf(const char *fmt, Args... args)
        {
            logger_printf(fmt, args...);
        }
    };
}