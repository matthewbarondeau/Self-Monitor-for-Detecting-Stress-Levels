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
    return 0;
}
