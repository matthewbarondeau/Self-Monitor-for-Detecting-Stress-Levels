/*
 * Switches.h
 *
 *  Created on: Apr 27, 2020
 *      Author: Matthew Barondeau
 */

#ifndef SWITCHES_H_
#define SWITCHES_H_

// Initialize Built in Switches on P1.1 and P1.4
void Switches_Init(void);

// Read P1.1 and P1.4
// Masks read values with mask
// Returns result
// For just switch P1.1, mask = 0x2
// For just switch P1.4, mask = 0x10
// For both switches, mas = 0x12
uint8_t Switches_Read(uint8_t mask);

// Reads P1.1
// Shifts P1.1 to LSB
// Converts Negative Logic Switch to Positive Logic
// Returns 1 if pressed, 0 if not pressed
uint8_t Switches_SW1_Pressed(void);
#endif /* SWITCHES_H_ */
