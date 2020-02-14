/*
 * i2c.h
 *
 *  Created on: Feb 14, 2020
 *      Author: Matthew Barondeau
 */

#ifndef I2CMAX30102_H_
#define I2CMAX30102_H_

#include <stdint.h>

void writeRegister8(uint8_t reg, uint8_t value);

uint8_t readRegister8(uint8_t reg);

void bitMask(uint8_t reg, uint8_t mask, uint8_t thing);

#endif /* I2CMAX30102_H_ */
