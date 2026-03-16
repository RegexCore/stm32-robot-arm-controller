/*********************************************************************
* Project     : Robot Arm Controller
* File        : logger.hpp
*
* Description :
*   Logger for diagnostic and debug output via USART.
*
* SPDX-License-Identifier: MIT
* Copyright (c) 2026 Manuel Wiesinger
*********************************************************************/

#pragma once

#include "logger.h"

/**
 * C++ wrapper for the C-based logger module.
 * Provides simple static functions for diagnostic and log output.
 */
namespace diagnostic
{
    class Logger
    {
    public:
        /// Initializes the logger system (must be called once before use).
        static void init()
        {
            logger_init();
        }

        /// Outputs a formatted diagnostic / log message.
        template<typename... Args>
        static void printf(const char *fmt, Args... args)
        {
            logger_printf(fmt, args...);
        }
    };
}