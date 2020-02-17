/*
 * ROS.h
 *
 *  Created on: Nov 24, 2019
 *      Author: Matthew Barondeau
 */

#ifndef ROS_H_
#define ROS_H_
#include <stdint.h>

void ROS_Init(void);

int8_t ROS_Read_Data(void);

int8_t ROS_Read_Heartrate(void);

int8_t ROS_Read_SpO2(void);

#endif /* ROS_H_ */
