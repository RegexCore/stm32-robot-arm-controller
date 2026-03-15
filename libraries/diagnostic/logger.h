/*********************************************************************
* Project     : Robot Arm Controller
* File        : logger.h
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   Logger for diagnostic and debug output via USART.
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
 * Initializes the logger.
 * This function MUST be called before any other logger functions.
 */
void logger_init(void);

/**
 * printf-like log output.
 * Formats the message into an internal buffer and sends it via UART and printf.
 */
void logger_printf(const char *fmt, ...);
         
#ifdef __cplusplus
} // extern "C"
#endif

#endif // LOGGER_H