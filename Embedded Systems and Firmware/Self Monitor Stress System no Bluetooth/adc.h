/*
 * adc.h
 *
 *  Created on: Jan 29, 2020
 *      Author: Matthew Barondeau
 */

#ifndef DRIVERS_ADC_H_
#define DRIVERS_ADC_H_

// Initializes ADC Channel 6
void ADC0_Ch6_Init(void);

// Reads 1 ADC sample value from Analog Channel 6
uint32_t ADC_Ch6_In(void);

// Software Averaging for ADC Samples
// Calls ADC_Ch6_In numSamples times
uint32_t ADC_Ch6_Samples(int numSamples);


#endif /* DRIVERS_ADC_H_ */
