/*
 * Timer32.h
 *
 *  Created on: Oct 28, 2019
 *      Author: Matthew Barondeau
 */

#ifndef DRIVERS_TIMER32_H_
#define DRIVERS_TIMER32_H_

void Timer32Init(int period);

void Timer32Stop(void);

uint64_t getOS_MsTime(void);

void T32_INT1_IRQHandler(void);

#endif /* DRIVERS_TIMER32_H_ */
