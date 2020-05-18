/*
 * adc.h
 *
 *  Created on: Oct 16, 2019
 *      Author: Matthew Barondeau
 */

#ifndef ADC_H_
#define ADC_H_

// Initializes ADC Channel 6
void ADC0_Ch6_Init(void);

// Reads 1 ADC sample value from Analog Channel 6
uint32_t ADC_Ch6_In(void);

// Software Averaging for ADC Samples
// Calls ADC_Ch6_In numSamples times
uint32_t ADC_Ch6_Samples(int numSamples);

#endif /* ADC_H_ */
