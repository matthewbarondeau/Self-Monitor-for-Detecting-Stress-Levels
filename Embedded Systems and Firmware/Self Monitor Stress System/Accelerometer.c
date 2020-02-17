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
#include "driverlib/MSP432P4xx/driverlib.h"
/* Slave Address for I2C Slave */
#define SLAVE_ADDRESS       0x1D
#define NUM_OF_REC_BYTES    32

/* Variables */
const uint8_t TXData[] = {0x41,0x42};

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

void ACC_Init(void){

}

uint8_t ACC_Read_Data(void){
    return 0;
}
