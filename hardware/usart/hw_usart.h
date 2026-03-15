/*********************************************************************
* Project     : Robot Arm Controller
* File        : hw_usart.h
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   Hardwaretreiber für die USART2-Schnittstelle.
*   PA2 = TX, PA3 = RX, AF7, USART2 TX only
*
* Copyright (c) 2026 Manuel Wiesinger
* All rights reserved.
*********************************************************************/

#ifndef HW_USART_H
#define HW_USART_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HW_USART2_PCLK_HZ
#define HW_USART2_PCLK_HZ  16000000UL   //APB1-Takt
#endif

#ifndef HW_USART2_BAUD
#define HW_USART2_BAUD     9600UL   //Gewünschte Baudrate
#endif

void HW_USART2_Init(void);
void HW_USART2_SendMessage(const char *message);

#ifdef __cplusplus
}
#endif

#endif /* HW_USART_H */