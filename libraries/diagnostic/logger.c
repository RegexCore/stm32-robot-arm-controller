/*********************************************************************
* Project     : Robot Arm Controller
* File        : logger.c
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   Logger for diagnostic and debug output via USART.
*
* Copyright (c) 2026 Manuel Wiesinger
* All rights reserved.
*********************************************************************/

#include "logger.h"
#include <stdio.h>
#include <stdarg.h>
#include "../../hardware/usart/hw_usart.h"

/**
 * Initializes the logger system and the USART interface
 */
void logger_init(void)
{
    HW_USART2_Init();
}

/**
 * Outputs a formatted log / diagnostic message.
 * The message is printed via printf as well as via the USART interface.
 *
 * @param fmt Format string (printf style)
 * @param ... Variadic arguments matching the format string
 */
void logger_printf(const char *fmt, ...)
{
    if (fmt == NULL)
        return;

    char buffer[128];
    va_list args;

    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    buffer[sizeof(buffer) - 1] = '\0';  // Safety: ensure string termination

    printf("%s", buffer);
    HW_USART2_SendMessage(buffer);
}