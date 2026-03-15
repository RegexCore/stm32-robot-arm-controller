/*********************************************************************
* Project     : Robot Arm Controller
* File        : systicktimer.h
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   SysTick-basierte Zeitfunktionen und Millisekunden-Delay.
*
* Copyright (c) 2026 Manuel Wiesinger
* All rights reserved.
*********************************************************************/

#ifndef SYSTICKTIMER_H_
#define SYSTICKTIMER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void systick_init(void);
uint32_t systick_millis(void);
void delay_ms(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif /* SYSTICKTIMER_H_ */
