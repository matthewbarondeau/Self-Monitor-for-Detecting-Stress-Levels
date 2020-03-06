/*
 * Accelerometer.c
 *
 *  Created on: Jan 19, 2020
 *      Author: Matthew Barondeau
 *  Changed on: Feb 29, 2020
 */

#include <ACC.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "driverlib/MSP432P4xx/driverlib.h"

#define I2C_MODULE EUSCI_B2_BASE

static int16_t x;
static int16_t y;
static int16_t z;
static int16_t xACC;
static int16_t yACC;
static int16_t zACC;

const eUSCI_I2C_MasterConfig ACCConfig =
{
    EUSCI_B_I2C_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
    3000000,                                // SMCLK = 3MHz
    EUSCI_B_I2C_SET_DATA_RATE_400KBPS,      // Desired I2C Clock of 100khz
    0,                                      // No byte counter threshold
    EUSCI_B_I2C_NO_AUTO_STOP                // No Autostop
};

void ACC_init_spi(void){

}

// ACC_init
// Configure MMA8451 on I2C EUSCI_B1
void ACC_init(void){
    // Configure I2C module
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3,
             GPIO_PIN6 + GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);

    // Pass configuration to I2C module
    MAP_I2C_initMaster(I2C_MODULE, &ACCConfig);

    // Set as transmit mode
    MAP_I2C_setMode(I2C_MODULE, EUSCI_B_I2C_TRANSMIT_MODE);

    // Set I2C Address
    MAP_I2C_setSlaveAddress(I2C_MODULE, MMA8451_DEFAULT_ADDRESS);

    // Enable I2C module
    MAP_I2C_enableModule(I2C_MODULE);

    // Polling not interrupt
    MAP_I2C_disableInterrupt(I2C_MODULE, 0xFFFF);

    // Check Connection
    uint8_t deviceid = I2C_readRegister2(I2C_MODULE, MMA8451_REG_WHOAMI);
    if(deviceid != 0x1A){
        while(1);
    }

    I2C_writeRegister2(I2C_MODULE, MMA8451_REG_CTRL_REG2, 0x40); // reset

    __delay_cycles(48000);

    while (I2C_readRegister2(I2C_MODULE, MMA8451_REG_CTRL_REG2) & 0x40);

    // enable 4G range
    I2C_writeRegister2(I2C_MODULE, MMA8451_REG_XYZ_DATA_CFG, MMA8451_RANGE_4_G);

    // High res
    I2C_writeRegister2(I2C_MODULE, MMA8451_REG_CTRL_REG2, 0x02);

    // DRDY on INT1
    I2C_writeRegister2(I2C_MODULE, MMA8451_REG_CTRL_REG4, 0x01);
    I2C_writeRegister2(I2C_MODULE, MMA8451_REG_CTRL_REG5, 0x01);

    // Turn on orientation config
    I2C_writeRegister2(I2C_MODULE, MMA8451_REG_PL_CFG, 0x40);

    // Activate at max rate, low noise mode
    I2C_writeRegister2(I2C_MODULE, MMA8451_REG_CTRL_REG1, 0x01 | 0x04);

    x = 0;
    y = 0;
    z = 0;
}

// ACC_calc
// Calculates acceleration in the given direction
int16_t ACC_calc(int16_t value){
    int32_t value_g = value * 980665 / 2048;
    //float value_g2 = (float) value / 2048;
    //value_g2 *= 9.80665;
    return value_g / 1000;

//    return 9.80665 * value_g;
}

// ACC_get_x
// returns x acceleration value
// Fixed point precision of 0.01
int16_t ACC_get_x(void){
    return xACC;
}

// ACC_get_y
// returns y acceleration value
int16_t ACC_get_y(void){
    return yACC;
}

// ACC_get_z
// returns z acceleration value
int16_t ACC_get_z(void){
    return zACC;
}

// ACC_read_data
// Reads 6 bytes from MMA8451 using repeated start I2C
// Calculates acceleration using 6 bytes
void ACC_read_data(void){
    // Send out start and register
    I2C_masterSendMultiByteStart(I2C_MODULE, MMA8451_REG_OUT_X_MSB);

    // Wait for data to be sent
    while(!(EUSCI_B2->IFG & EUSCI_B_IFG_TXIFG0));

    // repeated start
    I2C_masterReceiveStart(I2C_MODULE);

    x = I2C_getByte(I2C_MODULE);
    x = x << 8;
    x |= I2C_getByte(I2C_MODULE);
    x = x >> 2;

    y = I2C_getByte(I2C_MODULE);
    y = y << 8;
    y |= I2C_getByte(I2C_MODULE);
    y = y >> 2;

    z = I2C_getByte(I2C_MODULE);
    z = z << 8;

    // Send Stop
    I2C_masterReceiveMultiByteStop(I2C_MODULE);

    z |= I2C_getByte(I2C_MODULE);
    z = z >> 2;

    // Delay 1ms need to test removal
    __delay_cycles(3000);

    int q = I2C_slaveGetData(I2C_MODULE); // TODO: Test removal

    xACC = ACC_calc(x);
    yACC = ACC_calc(y);
    zACC = ACC_calc(z);

    __delay_cycles(3000); // TODO: Test removal
}
