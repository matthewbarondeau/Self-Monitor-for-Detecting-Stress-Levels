/*
 * Timer.c
 *
 *  Created on: Oct 17, 2019
 *      Author: Matthew Barondeau
 */

#include "msp.h"

extern int Timer_32_Flag;

// Initialize 32-bit timer
void Timer32Init(int period){
    // Timer32 set up in periodic mode, 32-bit, no pre-scale
    TIMER32_1->CONTROL = TIMER32_CONTROL_SIZE | TIMER32_CONTROL_MODE;

    // Load Period
    TIMER32_1->LOAD= period;

    // Enable the Timer32 interrupt in NVIC
    NVIC->ISER[0] = 1 << ((T32_INT1_IRQn) & 31);

    // Start Timer32 with interrupt enabled
    TIMER32_1->CONTROL |= TIMER32_CONTROL_ENABLE | TIMER32_CONTROL_IE;

    // Enable sleep on exit from ISR
    // SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;
}

// Disables 32-bit timer
void Timer32Stop(void){
    TIMER32_1->CONTROL &= ~(TIMER32_CONTROL_ENABLE | TIMER32_CONTROL_IE);
}

// 32-bit timer interrupt handler
void T32_INT1_IRQHandler(void){
    TIMER32_1->INTCLR |= BIT0;              // Clear Timer32 interrupt flag
    Timer_32_Flag = 1;
}
