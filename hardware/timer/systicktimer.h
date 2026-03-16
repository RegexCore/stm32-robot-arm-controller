/*********************************************************************
* Project     : Robot Arm Controller
* File        : systicktimer.h
*
* Description :
*   SysTick-based time functions and millisecond delay.
*
* SPDX-License-Identifier: MIT
* Copyright (c) 2026 Manuel Wiesinger
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