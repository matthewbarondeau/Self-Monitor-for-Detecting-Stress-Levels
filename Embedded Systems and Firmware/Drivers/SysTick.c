/*
 * SysTick.c
 *
 *  Created on: Oct 17, 2019
 *      Author: Matthew Barondeau
 */

#include "msp.h"
#include "adc.h"

extern int ADC_total;
extern int ADC_Samples;

//Enables SysTick Timer
void SysTick_Init(int period){
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;    //Enable SysTick
    SysTick->LOAD = period;                         //Set Period
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;      //Enable Interrupt
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;       //Start SysTick
}

//Disables SysTick Timer
void SysTick_Stop(void){
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

//SysTick Handler
//No explicit acknowledgement required
void SysTick_Handler(void){
    ADC_total += ADC_Ch6_In();
    ADC_Samples += 1;
}
