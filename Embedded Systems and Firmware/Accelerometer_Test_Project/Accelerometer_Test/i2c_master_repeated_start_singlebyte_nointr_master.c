/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/******************************************************************************
 *  MSP432P4 I2C - EUSCI_B0 I2C Master TX/RX bytes to MSP432P4 Slave-Repeated Start
 *
 *  For use with the i2c_master_rw_repeated_start_singlebyte_slave project
 *
 *  Description: This demo connects two MSP432's via the I2C bus. This is the
 *  MASTER CODE.The master transmits 2B to the slave, then receives a byte. 
 *  The master continuously transmits two bytes to the slave followed by a
 *  repeated start and a single-byte read from the slave.
 *  This is a common operation for bytes from I2C slave devices such as
 *  external EEPROMs.
 *  The transaction for the I2C that is written looks as follows:
 *  ________________________________________________________
 *  |  Start    | WData | WData | Start    | RData  |       |
 *  | 0x48 Addr | 0x042 | 0x041 |0x48 Addr |  0x52  | Stop  |
 *  |    W      |       |       |    R     |        |       |
 *  |___________|_______|_______|__________|________|_______|
 *
 *  ACLK = n/a, MCLK = HSMCLK = SMCLK = BRCLK = default DCO = ~3.0MHz
 *
 *                                /|\  /|\
 *                MSP432P411      10k  10k      MSP432P411
 *                   slave         |    |         master
 *             -----------------   |    |   -----------------
 *            |     P1.6/UCB0SDA|<-|----+->|P1.6/UCB0SDA     |
 *            |                 |  |       |                 |
 *            |                 |  |       |                 |
 *            |     P1.7/UCB0SCL|<-+------>|P1.7/UCB0SCL     |
 *            |                 |          |             P1.0|-- Error
 *            |                 |          |             P2.1|-- Activity
 *
 *****************************************************************************/
/* DriverLib Defines */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Defines */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MMA8451_REG_OUT_X_MSB     0x01
#define MMA8451_REG_SYSMOD        0x0B
#define MMA8451_REG_WHOAMI        0x0D
#define MMA8451_REG_XYZ_DATA_CFG  0x0E
#define MMA8451_REG_PL_STATUS     0x10
#define MMA8451_REG_PL_CFG        0x11
#define MMA8451_REG_CTRL_REG1     0x2A
#define MMA8451_REG_CTRL_REG2     0x2B
#define MMA8451_REG_CTRL_REG4     0x2D
#define MMA8451_REG_CTRL_REG5     0x2E



#define MMA8451_PL_PUF            0
#define MMA8451_PL_PUB            1
#define MMA8451_PL_PDF            2
#define MMA8451_PL_PDB            3
#define MMA8451_PL_LRF            4
#define MMA8451_PL_LRB            5
#define MMA8451_PL_LLF            6
#define MMA8451_PL_LLB            7

typedef enum
{
  MMA8451_RANGE_8_G           = 0b10,   // +/- 8g
  MMA8451_RANGE_4_G           = 0b01,   // +/- 4g
  MMA8451_RANGE_2_G           = 0b00    // +/- 2g (default value)
} mma8451_range_t;


/* Used with register 0x2A (MMA8451_REG_CTRL_REG1) to set bandwidth */
typedef enum
{
  MMA8451_DATARATE_800_HZ     = 0b000, //  800Hz
  MMA8451_DATARATE_400_HZ     = 0b001, //  400Hz
  MMA8451_DATARATE_200_HZ     = 0b010, //  200Hz
  MMA8451_DATARATE_100_HZ     = 0b011, //  100Hz
  MMA8451_DATARATE_50_HZ      = 0b100, //   50Hz
  MMA8451_DATARATE_12_5_HZ    = 0b101, // 12.5Hz
  MMA8451_DATARATE_6_25HZ     = 0b110, // 6.25Hz
  MMA8451_DATARATE_1_56_HZ    = 0b111, // 1.56Hz

  MMA8451_DATARATE_MASK       = 0b111
} mma8451_dataRate_t;


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



void MMA8451_Read(void){

}

int main(void)
{
    /* Disabling the Watchdog  */
    MAP_WDT_A_holdTimer();

    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
                   GPIO_PIN6 + GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);

    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);

    RXData=0;

    /* Initializing I2C Master to SMCLK at 100khz with no autostop */
    MAP_I2C_initMaster(EUSCI_B0_BASE, &i2cConfig);

    /* Specify slave address */
    MAP_I2C_setSlaveAddress(EUSCI_B0_BASE, SLAVE_ADDRESS);

    /* Enable I2C Module to start operations */
    MAP_I2C_enableModule(EUSCI_B0_BASE);

    // Check Connection
    uint8_t deviceid = readRegister8(MMA8451_REG_WHOAMI);
    if(deviceid != 0x1A){
        return false;
    }

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

    MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN1);

    writeRegister8(MMA8451_REG_WHOAMI, 0x01);

    deviceid = readRegister8(MMA8451_REG_WHOAMI);

    x = 0;
    y = 0;
    z = 0;



    while(1){
        __delay_cycles(300000);

        // read and print out x, y, and z values

        // Arduino code
        /*
          Wire.beginTransmission(_i2caddr);
          i2cwrite(MMA8451_REG_OUT_X_MSB);
          Wire.endTransmission(false); // MMA8451 + friends uses repeated start!!

          Wire.requestFrom(_i2caddr, 6);
          x = Wire.read(); x <<= 8; x |= Wire.read(); x >>= 2;
          y = Wire.read(); y <<= 8; y |= Wire.read(); y >>= 2;
          z = Wire.read(); z <<= 8; z |= Wire.read(); z >>= 2;


          uint8_t range = getRange();
          uint16_t divider = 1;
          if (range == MMA8451_RANGE_8_G) divider = 1024;
          if (range == MMA8451_RANGE_4_G) divider = 2048;
          if (range == MMA8451_RANGE_2_G) divider = 4096;

          x_g = (float)x / divider;
          y_g = (float)y / divider;
          z_g = (float)z / divider;
         */
    }

}

