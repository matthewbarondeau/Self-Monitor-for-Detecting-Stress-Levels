/*
 * GSR.c
 *
 *  Created on: Nov 24, 2019
 *      Author: Matthew Barondeau
 */

#include "msp.h"
#include "Drivers/adc.h"
#include <stdint.h>

void GSR_Init(){
    ADC0_Ch6_Init();
}

int8_t GSR_Read_Data(){
    int gsr_average;
    gsr_average = ADC_Ch6_Samples(10);
    return gsr_average;
}




