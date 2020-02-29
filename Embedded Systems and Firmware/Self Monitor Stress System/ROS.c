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
#include "SpO2.h"
#include "Heartrate.h"
#include "MAX30102.h"

// Allocate variables
int32_t SpO2;
int8_t SpO2_valid;
int32_t heart_rate;
int8_t heart_rate_valid;
int32_t ir;

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

    // Initializing I2C Master to SMCLK at 400khz with no autostop
    MAP_I2C_initMaster(EUSCI_B1_BASE, &MAX30102Config);

    // Set I2C mode to transmit
    MAP_I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_MODE);

    // Specify slave address
    MAP_I2C_setSlaveAddress(EUSCI_B1_BASE, MAX30102_ADDRESS);

    // Enable I2C Module to start operations
    MAP_I2C_enableModule(EUSCI_B1_BASE);

    // Polling, not interrupt
    MAP_I2C_disableInterrupt(EUSCI_B1_BASE, 0xFFFF);

    // Call initialization function for MAX30102 device
    MAX30102_Init();
}

// ROS_read_ir
// returns the raw infrared values from MAX30102
int32_t ROS_read_ir(){
    // Determines if there is already a sample
    // If there is not a sample, check device for new sample
    if(!MAX30102_available()){
        MAX30102_checkDevice();
    }

    // Get ir value from record data structure
    ir = MAX30102_getFIFOIR();

    // Update record fifo pointer
    MAX30102_nextSample();

    return ir;
}

// ROS_read_spo2
// return spo2 calculated value
// return 0 if spo2 is not valid
uint8_t ROS_read_spo2(){
    if(SpO2_valid){
        return SpO2;
    } else{
        return 0;
    }
}

// ROS_read_heart_rate
// return heart_rate calculated value
// return 0 if heart rate is not valid
uint8_t ROS_read_heart_rate(){
    if(heart_rate_valid){
        return heart_rate;
    } else{
        return 0;
    }
}

// ROS_calculate
// collects 100 samples from MAX30102
// performs heart rate and spo2 calculation
// sets heart_rate valid and spo2_valid
void ROS_calculate(){
    // collect 100 samples of IR and Red LED values
    int bufferLength = 100;
    uint32_t irBuffer[100] = {0};
    uint32_t redBuffer[100] = {0};

    for(int i = 0; i < bufferLength; i++){

        // Check for new sensor data
        while(MAX30102_available() == 0){
            MAX30102_checkDevice();
        }

        // store new sensor data
        redBuffer[i] = MAX30102_getFIFORed();
        irBuffer[i] = MAX30102_getFIFOIR();

        // update pointer to next sample
        MAX30102_nextSample();

    }

    // Call SpO2 algorithm
    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer,
                                           &SpO2, &SpO2_valid, &heart_rate,
                                           &heart_rate_valid);

}
