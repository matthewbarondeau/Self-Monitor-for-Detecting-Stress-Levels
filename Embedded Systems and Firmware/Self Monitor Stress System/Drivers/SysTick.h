/*
 * SysTick.h
 *
 *  Created on: Oct 17, 2019
 *      Author: Matthew Barondeau
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_

// Enables SysTick timer with interrupts
// Input: period is SysTick reload value
void SysTick_Init(int period);

// Disables SysTick timer
void SysTick_Stop(void);

void SysTick_Restart(void);

#endif /* SYSTICK_H_ */
