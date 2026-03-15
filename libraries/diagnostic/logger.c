/*********************************************************************
* Project     : Robot Arm Controller
* File        : logger.c
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   Logger für Diagnose- und Debugausgaben über USART.
*
* Copyright (c) 2026 Manuel Wiesinger
* All rights reserved.
*********************************************************************/

#include "logger.h"
#include <stdio.h>
#include <stdarg.h>
#include "../../hardware/usart/hw_usart.h"

/**
 * Initialisiert das Logger-System bzw. die USART Schnittstelle
 */
void logger_init(void)
{
    HW_USART2_Init();
}

/**
 * Gibt eine formatierte Log-/Diagnose-Nachricht aus.
 * Die Ausgabe erfolgt sowohl über printf als auch über die USART-Schnittstelle.
 *
 * @param fmt Format-String (printf-Stil)
 * @param ... Variadische Argumente passend zum Format-String
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

    buffer[sizeof(buffer) - 1] = '\0';  // Sicherheit: String terminieren

    printf("%s", buffer);
    HW_USART2_SendMessage(buffer);
}