/*
 * Accelerometer.c
 *
 *  Created on: Jan 19, 2020
 *      Author: Matthew Barondeau
 *  Changed on: Feb 29, 2020
 */

#define I2CSLAVENUM 1

#include "Accelerometer.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "driverlib/MSP432P4xx/driverlib.h"

int16_t x;
int16_t y;
int16_t z;
float xACC;
float yACC;
float zACC;

const eUSCI_I2C_MasterConfig ACCConfig =
{
    EUSCI_B_I2C_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
    3000000,                                // SMCLK = 3MHz
    EUSCI_B_I2C_SET_DATA_RATE_400KBPS,      // Desired I2C Clock of 100khz
    0,                                      // No byte counter threshold
    EUSCI_B_I2C_NO_AUTO_STOP                // No Autostop
};

// ACC_init
// Configure MMA8451 on I2C EUSCI_B1
void ACC_init(void){
    // Configure I2C module
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6,
             GPIO_PIN4 + GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);

    // Pass configuration to I2C module
    MAP_I2C_initMaster(EUSCI_B1_BASE, &ACCConfig);

    // Set as transmit mode
    MAP_I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_MODE);

    // Set I2C Address
    MAP_I2C_setSlaveAddress(EUSCI_B1_BASE, MMA8451_DEFAULT_ADDRESS);

    // Enable I2C module
    MAP_I2C_enableModule(EUSCI_B1_BASE);

    // Polling not interrupt
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
}

// ACC_calc
// Calculates acceleration in the given direction
// TODO: should be reconfigured to return int16
float ACC_calc(int16_t value){
    float value_g = (float) value / 2048;
    return 9.80665 * value_g;
}

// ACC_get_x
// returns x acceleration value
float ACC_get_x(void){
    return xACC;
}

// ACC_get_y
// returns y acceleration value
float ACC_get_y(void){
    return yACC;
}

// ACC_get_z
// returns z acceleration value
float ACC_get_z(void){
    return zACC;
}

// ACC_read_data
// Reads 6 bytes from MMA8451 using repeated start I2C
// Calculates acceleration using 6 bytes
void ACC_read_data(void){
    // Send out start and register
    I2C_masterSendMultiByteStart(EUSCI_B1_BASE, MMA8451_REG_OUT_X_MSB);

    // Wait for data to be sent
    while(!(EUSCI_B1->IFG & EUSCI_B_IFG_TXIFG0));

    // repeated start
    I2C_masterReceiveStart(EUSCI_B1_BASE);

    x = I2C_getByte();
    x = x << 8;
    x |= I2C_getByte();
    x = x >> 2;

    y = I2C_getByte();
    y = y << 8;
    y |= I2C_getByte();
    y = y >> 2;

    z = I2C_getByte();
    z = z << 8;

    // Send Stop
    I2C_masterReceiveMultiByteStop(EUSCI_B1_BASE);

    z |= I2C_getByte();
    z = z >> 2;

    // Delay 1ms
    __delay_cycles(3000);

    int q = I2C_slaveGetData(EUSCI_B1_BASE); // TODO: Test removal

    xACC = ACC_calc(x);
    yACC = ACC_calc(y);
    zACC = ACC_calc(z);

    __delay_cycles(3000); // TODO: Test removal
}

