// GMR.h
// Author: Matthew Barondeau
// Created 11/24/2019
// Updated 11/24/2019

#ifndef GMR_H_
#define GMR_H_

// Initializes GMR Sensor
// Inputs: None
// Output: None
void GMR_Init(void);

// Reads Heartbeat value
// Inputs: None
// Output: Heartbeat
uint8_t GMR_Read_Data();

#endif /* GMR_H_ */
