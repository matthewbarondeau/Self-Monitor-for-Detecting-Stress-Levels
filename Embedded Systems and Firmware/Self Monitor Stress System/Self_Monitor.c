// Self_Monitor.c
// Author: Matthew Barondeau
// Created 11/24/2019
// Updated 11/24/2019

#include "msp.h"
#include "Drivers/CortexM.h"
#include "Drivers/AP.h"
#include "GSR.h"
#include "ROS.h"
#include "Accelerometer.h"
#include "BLE.h"

volatile uint8_t spo2 = 0;
volatile uint8_t heart_rate = 0;
volatile uint16_t x = 0;
volatile uint16_t y = 0;
volatile uint16_t z = 0;
volatile uint16_t GSR = 0;

void main(void){

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

    // Initialize Sensors
    ACC_init();
    GSR_Init();
    ROS_init();

    uint32_t time = 0;

    while(1){
        AP_BackgroundProcess();
        time++;

        if(time > 1000){
            time = 0;

            // Read new accelerometer values
            ACC_read_data();
            x = (uint16_t) ACC_get_x();
            y = (uint16_t) ACC_get_y();
            z = (uint16_t) ACC_get_z();

            // Read new GSR value
            GSR = GSR_Read_Data();

            // Read new ROS values
            ROS_calculate();
            spo2 = ROS_read_spo2();
            heart_rate = ROS_read_heart_rate();

            BLE_update_app();
        }
    }
}
