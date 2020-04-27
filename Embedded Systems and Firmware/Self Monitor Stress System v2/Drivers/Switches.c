/*
 * Switches.c
 *
 *  Created on: Apr 27, 2020
 *      Author: Matthew Barondeau
 */

#include "msp.h"
#include <stdint.h>

// Initialize Built in Switches on P1.1 and P1.4
void Switches_Init(void){
    P1->SEL0 &= ~0x12;
    P1->SEL1 &- ~0x12;
    P1->DIR  &= ~0x2;
    P1->REN  |= 0x12;
    P1->OUT  |= 0x12;
}

// Read P1.1 and P1.4
// Masks read values with mask
// Returns result
uint8_t Switches_Read(uint8_t mask){
    unsigned char switches = P1->IN & 0x2 | P1->IN & 0x10;
    switches &= mask;
    return switches;
}

// Reads P1.1
// Shifts P1.1 to LSB
// Converts Negative Logic Switch to Positive Logic
// Returns 1 if pressed, 0 if not pressed
uint8_t Switches_SW1_Pressed(void){
    uint8_t SW1 = Switches_Read(0x2);
    SW1 = SW1 >> 1;
    SW1 = ~SW1;
    SW1 &= 0x1;
    return SW1;
}
