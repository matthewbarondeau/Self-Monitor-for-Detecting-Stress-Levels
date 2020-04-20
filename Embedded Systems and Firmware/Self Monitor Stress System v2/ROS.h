/*
 * ROS.h
 *
 *  Created on: Nov 24, 2019
 *      Author: Matthew Barondeau
 */

#ifndef ROS_H_
#define ROS_H_
#include <stdint.h>

// ROS_init
// Initialize I2C port for ROS communication
void ROS_init(void);

// ROS_read_ir
// returns the raw infrared values from MAX30102
int8_t ROS_read_ir(void);

// ROS_read_heart_rate
// return heart_rate calculated value
// return 0 if heart rate is not valid
uint8_t ROS_read_heart_rate(void);

uint8_t ROS_read_avg_heart_rate(void);

#endif /* ROS_H_ */
