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
        3000000,                                // SMCLK = 3MHz
        EUSCI_B_I2C_SET_DATA_RATE_400KBPS,      // Desired I2C Clock of 100khz
        0,                                      // No byte counter threshold
        EUSCI_B_I2C_NO_AUTO_STOP                // No Autostop
};

void ACC_Init(void){
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6,
             GPIO_PIN4 + GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);

    MAP_I2C_initMaster(EUSCI_B1_BASE, &i2cConfig);

    MAP_I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_MODE);

    MAP_I2C_setSlaveAddress(EUSCI_B1_BASE, SLAVE_ADDRESS);

    MAP_I2C_enableModule(EUSCI_B1_BASE);

    MAP_I2C_disableInterrupt(EUSCI_B1_BASE, 0xFFFF);

    // Check Connection
    uint8_t deviceid = readRegister8(MMA8451_REG_WHOAMI);
    if(deviceid != 0x1A){
        while(1);
    }

    writeRegister8(MMA8451_REG_CTRL_REG2, 0x40); // reset

    __delay_cycles(1000);

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

    x = 0;
    y = 0;
    z = 0;

    ACC_Read_Data();
    ACC_Read_Data();
    ACC_Read_Data();
    ACC_Read_Data();
}

float ACC_Acceleration_X(int16_t x){
    float x_g = (float) x / 2048;
    return 9.80665 * x_g;
}

float ACC_Acceleration_Y(int16_t y){
    float y_g = (float) y / 2048;
    return 9.80665 * y_g;
}

float ACC_Acceleration_Z(int16_t z){
    float z_g = (float) z / 2048;
    return 9.80665 * z_g;
}

uint8_t ACC_Read_Data(void){
    x = 0;
    y = 0;
    z = 0;

    I2C_masterSendMultiByteStart(EUSCI_B1_BASE, MMA8451_REG_OUT_X_MSB);
    while(!(EUSCI_B1->IFG & EUSCI_B_IFG_TXIFG0));

    // repeated start
    I2C_masterReceiveStart(EUSCI_B1_BASE);

    while(!(EUSCI_B1->IFG & EUSCI_B_IFG_RXIFG0));
    x = I2C_slaveGetData(EUSCI_B1_BASE);
    x = x << 8;
    while(!(EUSCI_B1->IFG & EUSCI_B_IFG_RXIFG0));
    x |= I2C_slaveGetData(EUSCI_B1_BASE);
    x = x >> 2;

    while(!(EUSCI_B1->IFG & EUSCI_B_IFG_RXIFG0));
    y = I2C_slaveGetData(EUSCI_B1_BASE);
    y = y << 8;
    while(!(EUSCI_B1->IFG & EUSCI_B_IFG_RXIFG0));
    y |= I2C_slaveGetData(EUSCI_B1_BASE);
    y = y >> 2;

    while(!(EUSCI_B1->IFG & EUSCI_B_IFG_RXIFG0));
    z = I2C_slaveGetData(EUSCI_B1_BASE);
    z = z << 8;

    I2C_masterReceiveMultiByteStop(EUSCI_B1_BASE);
    while(!(EUSCI_B1->IFG & EUSCI_B_IFG_RXIFG0));
    z |= I2C_slaveGetData(EUSCI_B1_BASE);
    z = z >> 2;
    __delay_cycles(10000);  // need this here for correct values for some reason
    int q = I2C_slaveGetData(EUSCI_B1_BASE);
    //I2C_masterReceiveMultiByteStop(EUSCI_B1_BASE);

    float xACC = ACC_Acceleration_X(x);
    float yACC = ACC_Acceleration_Y(y);
    float zACC = ACC_Acceleration_Z(z);
    xACC += 0;
    yACC += 0;
    zACC += 0;
    __delay_cycles(10000);
}

