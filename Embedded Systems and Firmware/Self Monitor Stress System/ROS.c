/*
 * ROS.c
 *
 *  Created on: Nov 24, 2019
 *      Author: Matthew Barondeau
 */

#include "msp.h"
#include <stdint.h>

// MAX30102 Includes
#include "SpO2.h"
#include "Heartrate.h"
#include "MAX30102.h"

#include "driverlib/MSP432P4xx/driverlib.h"

// Allocate variables
int32_t SpO2;
int8_t SpO2_valid;
int32_t heart_rate;
int8_t heart_rate_valid;

/* Slave Address for I2C Slave */
#define SLAVE_ADDRESS       0x57

/* I2C Master Configuration Parameter */
const eUSCI_I2C_MasterConfig MAX30102Config =
{
        EUSCI_B_I2C_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        3000000,                                // SMCLK = 3MHz
        EUSCI_B_I2C_SET_DATA_RATE_400KBPS,      // Desired I2C Clock of 100khz
        0,                                      // No byte counter threshold
        EUSCI_B_I2C_NO_AUTO_STOP                // No Autostop
};

void ROS_Init(){
    /* Select Port 1 for I2C - Set Pin 6, 7 to input Primary Module Function,
     *   (UCB0SIMO/UCB0SDA, UCB0SOMI/UCB0SCL).
     */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6,
            GPIO_PIN4 + GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Initializing I2C Master to SMCLK at 400khz with no autostop */
    MAP_I2C_initMaster(EUSCI_B1_BASE, &MAX30102Config);

    MAP_I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_MODE);

    /* Specify slave address */
    MAP_I2C_setSlaveAddress(EUSCI_B1_BASE, SLAVE_ADDRESS);

    /* Enable I2C Module to start operations */
    MAP_I2C_enableModule(EUSCI_B1_BASE);

    MAP_I2C_disableInterrupt(EUSCI_B1_BASE, 0xFFFF);

    MAX30102_Init();
}


int32_t ROS_Read_IR(){
    int32_t irValue = MAX30102_getIR();
    MAX30102_nextSample();
    return irValue;
}

uint8_t ROS_Read_SpO2(){
    if(SpO2_valid){
        return SpO2;
    } else{
        return 0;
    }
}

uint8_t ROS_Read_Heartrate(){
    return heart_rate;
}

void ROS_Calcuulate(){
    // collect 100 samples of IR and Red LED values
    int bufferLength = 100;
    uint32_t irBuffer[100] = {0};
    uint32_t redBuffer[100] = {0};

    for(int i = 0; i < bufferLength; i++){

        // Check for new sensor data
        while(MAX30102_available() == 0){
            MAX30102_check();
        }

        // store new sensor data
        redBuffer[i] = MAX30102_getRed();
        irBuffer[i] = MAX30102_getIR();

        // update pointer to next sample
        MAX30102_nextSample();

    }

    // Call SpO2 algorithm
    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &SpO2, &SpO2_valid, &heart_rate, &heart_rate_valid);

}
