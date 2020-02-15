/*
 * MAX30102.h
 *
 *  Created on: Feb 14, 2020
 *      Author: Matthew Barondeau
 */

#ifndef MAX30102_H_
#define MAX30102_H_


void setPulseAmplitudeRed(uint8_t amplitude);

int MAX30102_Init(void);

uint8_t MAX30102_available(void);

uint32_t MAX30102_getIR(void);

uint32_t MAX30102_getFIFORed(void);

uint32_t MAX30102_getFIFOIR(void);

uint32_t MAX30102_getFIFOGreen(void);

void MAX30102_nextSample(void);

uint16_t MAX30102_check(void);

#define STORAGE_SIZE 4
typedef struct Record
{
  uint32_t red[STORAGE_SIZE];
  uint32_t IR[STORAGE_SIZE];
  uint32_t green[STORAGE_SIZE];
  uint8_t head;
  uint8_t tail;
} sense_struct; //This is our circular buffer of readings from the sensor

sense_struct sense;

#endif /* MAX30102_H_ */
