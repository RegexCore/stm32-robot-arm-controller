/*********************************************************************
* Project     : Robot Arm Controller
* File        : hw_usart.h
*
* Description :
*   Hardware driver for the USART2 interface.
*   PA2 = TX, PA3 = RX, AF7, USART2 TX only
*
* SPDX-License-Identifier: MIT
* Copyright (c) 2026 Manuel Wiesinger
*********************************************************************/

#ifndef HW_USART_H
#define HW_USART_H

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

#ifdef __cplusplus
}
#endif

#endif /* HW_USART_H */