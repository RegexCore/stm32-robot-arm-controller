/*********************************************************************
* Project     : Robot Arm Controller
* File        : hw_usart.h
*
* Description :
*   Hardware driver for the USART2 interface.
*   PA2 = TX, PA3 = RX, AF7, USART2 with interrupt-driven line reception
*
* SPDX-License-Identifier: MIT
* Copyright (c) 2026 Manuel Wiesinger
*********************************************************************/

#ifndef HW_USART_H
#define HW_USART_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HW_USART2_PCLK_HZ
#define HW_USART2_PCLK_HZ  16000000UL   // APB1 clock
#endif

#ifndef HW_USART2_BAUD
#define HW_USART2_BAUD     9600UL       // Desired baud rate
#endif

void HW_USART2_Init(void);
void HW_USART2_SendMessage(const char *message);

#define HW_USART2_RX_LINE_SIZE 96U
#define HW_USART2_RX_QUEUE_DEPTH 8U

typedef enum
{
    HW_USART2_RX_OK,
    HW_USART2_RX_TOO_LONG,
    HW_USART2_RX_INVALID_BYTE,
    HW_USART2_RX_UART_ERROR
} HW_USART2_RxStatus;

typedef struct
{
    HW_USART2_RxStatus status;
    uint8_t motion_allowed;
    char text[HW_USART2_RX_LINE_SIZE];
} HW_USART2_RxLine;

// Reception is always active; motion permission is captured with each line.
void HW_USART2_SetMotionAllowed(int allowed);
int HW_USART2_ReadLine(HW_USART2_RxLine *line);
uint32_t HW_USART2_GetRxQueueCount(void);
uint32_t HW_USART2_TakeRxOverflowCount(void);

#ifdef __cplusplus
}
#endif

#endif /* HW_USART_H */