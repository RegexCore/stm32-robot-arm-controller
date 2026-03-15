/*********************************************************************
* Project     : Robot Arm Controller
* File        : timer_interrupt.h
* Author      : Manuel Wiesinger
* Created     : 24.02.2026
*
* Description :
*   Initialization of the 16-bit TIM7 interrupt timer (10 ms).
*
* Copyright (c) 2026 Manuel Wiesinger
* All rights reserved.
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