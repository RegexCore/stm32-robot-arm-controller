/*********************************************************************
* Project     : Robot Arm Controller
* File        : logger.h
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   Logger für Diagnose- und Debugausgaben über USART.
*
* Copyright (c) 2026 Manuel Wiesinger
* All rights reserved.
*********************************************************************/

#ifndef LOGGER_H
#define LOGGER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialisiert den Logger
 * Diese Funktion MUSS vor allen anderen Logger-Funktionen aufgerufen werden.
 */
void logger_init(void);

/**
 * printf-ähnliche Log-Ausgabe.
 * Formatiert in einen internen Buffer und sendet ihn über UART und printf als Log
 */
void logger_printf(const char *fmt, ...);
         
#ifdef __cplusplus
} // extern "C"
#endif

#endif // LOGGER_H