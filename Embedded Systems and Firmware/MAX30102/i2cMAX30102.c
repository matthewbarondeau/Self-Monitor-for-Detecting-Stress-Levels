/*
 * i2c.c
 *
 *  Created on: Feb 14, 2020
 *      Author: Matthew Barondeau
 */
#include "driverlib.h"

#include <stdint.h>

static int RXData;


void writeRegister8(uint8_t reg, uint8_t value){
    while(MAP_I2C_masterIsStartSent(EUSCI_B0_BASE));
    MAP_I2C_masterSendMultiByteStart(EUSCI_B0_BASE, reg);  // Start + 1Byte
    MAP_I2C_masterSendMultiByteNext(EUSCI_B0_BASE, value);
    //I2C_masterSendMultiByteNext(EUSCI_B0_BASE, value); // Poll for TXINT,Send 1Byte
    while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG0));
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP;
    while (I2C_masterIsStopSent(EUSCI_B0_BASE));

}

uint8_t readRegister8(uint8_t reg){

    //while (I2C_masterIsStopSent(EUSCI_B0_BASE) == EUSCI_B_I2C_SENDING_STOP);

    /* Send out EEPROM Mock Read Cmd (2 databytes) */
    MAP_I2C_masterSendMultiByteStart(EUSCI_B0_BASE, reg);  // Start + 1Byte


    //MAP_I2C_masterSendMultiByteFinish(EUSCI_B0_BASE, reg); // Poll for TXINT,Send 1Byte
    /*---------------------------------------------*/
    /* Now we need to initiate the read */
    /* Wait until 2nd Byte has been output to shift register */
    while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG0));

    // Send the restart condition, read one byte, send the stop condition right away
    EUSCI_B0->CTLW0 &= ~(EUSCI_B_CTLW0_TR);
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;

    while(I2C_masterIsStartSent(EUSCI_B0_BASE));

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP;

    while(!(EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG0));
    RXData = EUSCI_B0->RXBUF;

    return RXData;
}

void bitMask(uint8_t reg, uint8_t mask, uint8_t thing){
    uint8_t value = readRegister8(reg);
    value = value & mask;
    writeRegister8(reg, value | thing);
}
