/*
 * ROS.c
 *
 *  Created on: Nov 24, 2019
 *      Author: Matthew Barondeau
 */

#include "msp.h"
#include <stdint.h>

// MAX30102 Includes
#include "SpO2.h"
#include "Heartrate.h"
#include "MAX30102.h"

void ROS_Init(){
    MAX30102_Init();
}

int8_t ROS_Read_Data(){
    return 0;
}

int8_t ROS_Read_Heartrate(){
    return 0;
}

int8_t ROS_Read_SpO2(){
    // collect 100 samples of IR and Red LED values
    int bufferLength = 100;
    uint32_t irBuffer[100] = {0};
    uint32_t redBuffer[100] = {0};

    for(int i = 0; i < bufferLength; i++){

        // Check for new sensor data
        while(MAX30102_available() == 0){
            MAX30102_check();
        }

        // store new sensor data
        redBuffer[i] = MAX30102_getRed();
        irBuffer[i] = MAX30102_getIR();

        // update pointer to next sample
        MAX30102_nextSample();

    }

    // Allocate variables
    int32_t SpO2;
    int8_t SpO2_valid;
    int32_t heart_rate;
    int8_t heart_rate_valid;

    // Call SpO2 algorithm
    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &SpO2, &SpO2_valid, &heart_rate, &heart_rate_valid);


    return 0;
}
