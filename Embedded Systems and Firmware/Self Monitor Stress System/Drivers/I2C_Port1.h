/*
 * I2C_Port1.h
 *
 *  Created on: Nov 24, 2019
 *      Author: Matthew Barondeau
 */

#ifndef DRIVERS_I2C_PORT1_H_
#define DRIVERS_I2C_PORT1_H_

// UCB0SCL connected to P1.7 and to pin 4 of HMC6352 compass or pin 3 of TMP102 thermometer
// UCB0SDA connected to P1.6 and to pin 3 of HMC6352 compass or pin 2 of TMP102 thermometer
// SCL and SDA lines pulled to +3.3 V with 10 k resistors (part of breakout module)
// ADD0 pin of TMP102 thermometer connected to GND

void I2C_Port1_Init(void);

// receives one byte from specified slave
// Note for HMC6352 compass only:
// Used with 'r' and 'g' commands
// Note for TMP102 thermometer only:
// Used to read the top byte of the contents of the pointer register
//  This will work but is probably not what you want to do.
uint8_t I2C_Recv(int8_t slave);

// receives two bytes from specified slave
// Note for HMC6352 compass only:
// Used with 'A' commands
// Note for TMP102 thermometer only:
// Used to read the contents of the pointer register
uint16_t I2C_Recv2(int8_t slave);

// sends one byte to specified slave
// Note for HMC6352 compass only:
// Used with 'S', 'W', 'O', 'C', 'E', 'L', and 'A' commands
//  For 'A' commands, I2C_Recv2() should also be called
// Note for TMP102 thermometer only:
// Used to change the pointer register
// Returns 0 if successful, nonzero if error
uint16_t I2C_Send1(int8_t slave, uint8_t data1);

// sends two bytes to specified slave
// Note for HMC6352 compass only:
// Used with 'r' and 'g' commands
//  For 'r' and 'g' commands, I2C_Recv() should also be called
// Note for TMP102 thermometer only:
// Used to change the top byte of the contents of the pointer register
//  This will work but is probably not what you want to do.
// Returns 0 if successful, nonzero if error
uint16_t I2C_Send2(int8_t slave, uint8_t data1, uint8_t data2);

// sends three bytes to specified slave
// Note for HMC6352 compass only:
// Used with 'w' and 'G' commands
// Note for TMP102 thermometer only:
// Used to change the contents of the pointer register
// Returns 0 if successful, nonzero if error
uint16_t I2C_Send3(int8_t slave, uint8_t data1, uint8_t data2, uint8_t data3);

#endif /* DRIVERS_I2C_PORT1_H_ */
