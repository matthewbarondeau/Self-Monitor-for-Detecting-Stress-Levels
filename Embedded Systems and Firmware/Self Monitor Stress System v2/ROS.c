/*
 * ROS.c
 *
 *  Created on: Nov 24, 2019
 *      Author: Matthew Barondeau
 *  Changed on: Feb 29, 2020
 */

#include "msp.h"
#include <stdint.h>
#include "driverlib/MSP432P4xx/driverlib.h"
#include "Heartrate.h"
#include "Drivers/MAX30102.h"
#include "Drivers/Timer32.h"

#define I2C_MODULE EUSCI_B1_BASE

// Allocate variables
const int8_t RATE_SIZE = 4;
uint8_t rates[4];
uint8_t rateSpot = 0;
long lastBeat = 0;
float beatsPerMinute;
int beatAvg;


/* I2C Master Configuration Parameter */
const eUSCI_I2C_MasterConfig MAX30102Config =
{
        EUSCI_B_I2C_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        3000000,                                // SMCLK = 3MHz
        EUSCI_B_I2C_SET_DATA_RATE_400KBPS,      // Desired I2C Clock of 100khz
        0,                                      // No byte counter threshold
        EUSCI_B_I2C_NO_AUTO_STOP                // No Autostop
};

// ROS_init
// Initialize I2C port for ROS communication
void ROS_init(){
    /* Select Port 1 for I2C - Set Pin 6, 7 to input Primary Module Function,
     *   (UCB0SIMO/UCB0SDA, UCB0SOMI/UCB0SCL).
     */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6,
            GPIO_PIN4 + GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Initializing I2C Master to SMCLK at 400khz with no autostop */
    MAP_I2C_initMaster(EUSCI_B1_BASE, &MAX30102Config);

    MAP_I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_MODE);

    /* Specify slave address */
    MAP_I2C_setSlaveAddress(EUSCI_B1_BASE, MAX30102_ADDRESS);

    /* Enable I2C Module to start operations */
    MAP_I2C_enableModule(EUSCI_B1_BASE);

    MAP_I2C_disableInterrupt(EUSCI_B1_BASE, 0xFFFF);

    // Check Connection
    uint8_t deviceid = I2C_readRegister(1, 0xFF);
    if(deviceid != 0x15){
        while(1){};
    }

    //setPulseAmplitudeRed(0x1F);
    //setPulseAmplitudeIR(0x1F);

    MAX30102_init();
}

// ROS_read_ir
// returns the raw infrared values from MAX30102
int32_t ROS_read_ir(){
    static int32_t ir;
    // Determines if there is already a sample
    // If there is not a sample, check device for new sample
    if(!MAX30102_available()){
        MAX30102_check_device();
    }

    // Get ir value from record data structure
    ir = MAX30102_get_ir();

    // Update record fifo pointer
    MAX30102_next_sample();

    return ir;
}

// ROS_read_heart_rate
// return heart_rate calculated value
// return 0 if heart rate is not valid
uint8_t ROS_read_heart_rate(){
    long irValue = ROS_read_ir();
    if(checkForBeat(irValue) == true){
      long delta = getOS_MsTime() - lastBeat;
      lastBeat = getOS_MsTime();
      beatsPerMinute = 60 / (delta / 1000.0);
      if(beatsPerMinute < 255 && beatsPerMinute > 20){
        rates[rateSpot++] = (uint8_t)beatsPerMinute;
        rateSpot %= RATE_SIZE;
        beatAvg = 0;
        for(int8_t x = 0; x < RATE_SIZE; x++){
          beatAvg += rates[x];
        }
        beatAvg /= RATE_SIZE;
      }
    }
    return beatsPerMinute;
}

uint8_t ROS_read_avg_heart_rate(){
    return beatAvg;
}
