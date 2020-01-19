/*
 * Accelerometer.c
 *
 *  Created on: Jan 19, 2020
 *      Author: Matthew Barondeau
 */

#define I2CSLAVENUM 1
#define false 1
#define true 0

#include "Accelerometer.h"
#include "Drivers/I2C_Port1.h"

int16_t x;
int16_t y;
int16_t z;

uint8_t Accelerometer_Init(void){
    // Initialize I2C interface
    I2C_Port1_Init();

    // Check Connection
    uint8_t deviceid = readRegister8(MMA8451_REG_WHOAMI);
    if(deviceid != 0x1A){
        return false;
    }

    // Reset Device
    writeRegister8(MMA8451_REG_CTRL_REG2, 0x40);

    // Wait for device to come back up
    while (readRegister8(MMA8451_REG_CTRL_REG2) & 0x40);

    // Enable 4G range?
    writeRegister8(MMA8451_REG_XYZ_DATA_CFG, MMA8451_RANGE_4_G);

    // Set high resolution
    writeRegister8(MMA8451_REG_CTRL_REG2, 0x02);

    // DRDY on INT1?
    writeRegister8(MMA8451_REG_CTRL_REG4, 0x01);
    writeRegister8(MMA8451_REG_CTRL_REG5, 0x01);

    // Turn on orientation config
    writeRegister8(MMA8451_REG_PL_CFG, 0x40);

    // Max rate low noise mode
    writeRegister8(MMA8451_REG_CTRL_REG1, 0x01 | 0x04);

    return true;
}

void writeRegister8(uint8_t reg, uint8_t value){
    I2C_Send2(I2CSLAVENUM, reg, value);
}

uint8_t readRegister8(uint8_t reg){
    I2C_Send1(I2CSLAVENUM, reg);
    return I2C_Recv(I2CSLAVENUM);
}

void Accelerometer_Read(void){
    I2C_Send1(I2CSLAVENUM, MMA8451_REG_OUT_X_MSB);
    x = I2C_Recv2(I2CSLAVENUM) >> 2;
    y = I2C_Recv2(I2CSLAVENUM) >> 2;
    z = I2C_Recv2(I2CSLAVENUM) >> 2;
}

uint8_t Accelerometer_Orientation(void){
    return readRegister8(MMA8451_REG_PL_STATUS) & 0x7;
}

void Accelerometer_Set_Range(mma8451_range_t range){
    uint8_t reg1 = readRegister8(MMA8451_REG_CTRL_REG1);
    writeRegister8(MMA8451_REG_CTRL_REG1, 0x00);
    writeRegister8(MMA8451_REG_XYZ_DATA_CFG, range & 0x3);
    writeRegister8(MMA8451_REG_CTRL_REG1, reg1 | 0x01);
}

mma8451_range_t Accelerometer_getRange(void){
    return(mma8451_range_t)(readRegister8(MMA8451_REG_XYZ_DATA_CFG) & 0x3);
}

void Accelerometer_Set_Data_Rate(mma8451_dataRate_t dataRate){
    uint8_t ctl1 = readRegister8(MMA8451_REG_CTRL_REG1);
    writeRegister8(MMA8451_REG_CTRL_REG1, 0x00);
    ctl1 &= ~(MMA8451_DATARATE_MASK << 3);
    ctl1 |= (dataRate << 3);
    writeRegister8(MMA8451_REG_CTRL_REG1, ctl1 | 0x01);
}

mma8451_dataRate_t Accelerometer_Get_Data_Rate(void){
    return (mma8451_dataRate_t) ((readRegister8(MMA8451_REG_CTRL_REG1) >> 3) & MMA8451_DATARATE_MASK);

}
