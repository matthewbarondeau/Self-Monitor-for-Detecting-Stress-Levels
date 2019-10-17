/*
 * adc.c
 *
 *  Created on: Oct 16, 2019
 *      Author: Matthew Barondeau
 */


#include <stdio.h>
#include <stdint.h>
#include "msp.h"

//Initializes P4.7 to Analog Channel 6  Input
void ADC0_Ch6_Init(void){
    ADC14->CTL0 &= ~0x2;                // ADC14ENC = 0 allows programming
    while(ADC14->CTL0 & 0x00010000);    // Wait for device to be initialized
    ADC14->CTL0 = 0x04203310;           // Single sample
    ADC14->CTL1 = 0x00000030;
    ADC14->MCTL[0] = 0x00000086;
    ADC14->IER0 = 0;                    // Disable Interrupts
    ADC14->IER1 = 0;

    P4->SEL1 |= 0x80;                   // Analog Mode on A6, P4.7
    P4->SEL1 |= 0x80;

    ADC14->CTL0 |= 0x2;                 // Enable ADC
}

uint32_t ADC_Ch6_In(void){
    while(ADC14->CTL0 & 0x00010000);    // Wait for device to be not busy
    ADC14->CTL0 |= 0x00000001;          // Start single conversion
    while((ADC14->IFGR0 & 0x1) == 0);   // Wait for ADC sample
    return ADC14->MEM[0];
}

// Software averaging
uint32_t ADC_Ch6_Samples(int numSamples){
    uint32_t samples = 0;

    for(int i = 0; i< numSamples; i++){
        samples += ADC_Ch6_In();
    }

    return(samples / numSamples);
}
