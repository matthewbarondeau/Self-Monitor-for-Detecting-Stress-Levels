/*
 * Accelerometer.c
 *
 *  Created on: Jan 19, 2020
 *      Author: Matthew Barondeau
 */

#define I2CSLAVENUM 1

#include "Accelerometer.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "Drivers/i2c.h"
#include "Drivers/rom_map.h"
//#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Slave Address for I2C Slave */
#define SLAVE_ADDRESS       0x1D
#define NUM_OF_REC_BYTES    32

/* Variables */
const uint8_t TXData[] = {0x41,0x42};
static uint8_t RXData;

int16_t x;
int16_t y;
int16_t z;

/* I2C Master Configuration Parameter */
// Baud rate selectable (100KBPS, 400KBPS)
const eUSCI_I2C_MasterConfig i2cConfig =
{
        EUSCI_B_I2C_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        3000000,                                // SMCLK = 3MHz (default)
        EUSCI_B_I2C_SET_DATA_RATE_100KBPS,      // Desired I2C Clock of 100khz
        0,                                      // No byte counter threshold
        EUSCI_B_I2C_NO_AUTO_STOP                // No Autostop
};

void writeRegister8(uint8_t reg, uint8_t value){
    //while(MAP_I2C_masterIsStartSent(EUSCI_B0_BASE));
    MAP_I2C_masterSendMultiByteStart(EUSCI_B0_BASE, reg);  // Start + 1Byte
    MAP_I2C_masterSendMultiByteNext(EUSCI_B0_BASE, value); // Poll for TXINT,Send 1Byte

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP;
    while (MAP_I2C_masterIsStopSent(EUSCI_B0_BASE));

}

uint8_t readRegister8(uint8_t reg){

    /* Making sure the last transaction has been completely sent out */
    while (MAP_I2C_masterIsStopSent(EUSCI_B0_BASE));

    /* Send out EEPROM Mock Read Cmd (2 databytes) */
    MAP_I2C_masterSendMultiByteStart(EUSCI_B0_BASE, reg);  // Start + 1Byte
    //MAP_I2C_masterSendMultiByteNext(EUSCI_B0_BASE, TXData[0]); // Poll for TXINT,Send 1Byte
    /*---------------------------------------------*/
    /* Now we need to initiate the read */
    /* Wait until 2nd Byte has been output to shift register */
    while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG0));

    // Send the restart condition, read one byte, send the stop condition right away
    EUSCI_B0->CTLW0 &= ~(EUSCI_B_CTLW0_TR);
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;
    while(MAP_I2C_masterIsStartSent(EUSCI_B0_BASE));
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP;
    while(!(EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG0));
    RXData = EUSCI_B0->RXBUF;

    return RXData;
}


void Accelerometer_Init(){
    /* Initializing I2C Master to SMCLK at 100khz with no autostop */
        MAP_I2C_initMaster(EUSCI_B0_BASE, &i2cConfig);

        /* Specify slave address */
        MAP_I2C_setSlaveAddress(EUSCI_B0_BASE, SLAVE_ADDRESS);

        /* Enable I2C Module to start operations */
        MAP_I2C_enableModule(EUSCI_B0_BASE);

        // Check Connection
        uint8_t deviceid = readRegister8(MMA8451_REG_WHOAMI);
//        if(deviceid != 0x1A){
//            //return false;
//        }

        writeRegister8(MMA8451_REG_CTRL_REG2, 0x40); // reset

        while (readRegister8(MMA8451_REG_CTRL_REG2) & 0x40);

        // enable 4G range
        writeRegister8(MMA8451_REG_XYZ_DATA_CFG, MMA8451_RANGE_4_G);

        // High res
        writeRegister8(MMA8451_REG_CTRL_REG2, 0x02);

        // DRDY on INT1
        writeRegister8(MMA8451_REG_CTRL_REG4, 0x01);
        writeRegister8(MMA8451_REG_CTRL_REG5, 0x01);

        // Turn on orientation config
        writeRegister8(MMA8451_REG_PL_CFG, 0x40);

        // Activate at max rate, low noise mode
        writeRegister8(MMA8451_REG_CTRL_REG1, 0x01 | 0x04);

        //MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN1);

        writeRegister8(MMA8451_REG_WHOAMI, 0x01);

        deviceid = readRegister8(MMA8451_REG_WHOAMI);

        x = 0;
        y = 0;
        z = 0;

}

