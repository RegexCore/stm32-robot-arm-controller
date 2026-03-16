/*********************************************************************
* Project     : Robot Arm Controller
* File        : timer_interrupt.h
*
* Description :
*   Initialization of the 16-bit TIM7 interrupt timer (10 ms).
*
* SPDX-License-Identifier: MIT
* Copyright (c) 2026 Manuel Wiesinger
*********************************************************************/

#ifndef TIMER_INTERRUPT_H_
#define TIMER_INTERRUPT_H_

#ifdef __cplusplus
extern "C" {
#endif

void TIM7_Init_10ms(void);
void TIM7_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* TIMER_INTERRUPT_H_ */