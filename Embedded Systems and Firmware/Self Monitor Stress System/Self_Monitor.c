// Self_Monitor.c
// Author: Matthew Barondeau
// Created 11/24/2019
// Updated 03/01/2020

#include "msp.h"
#include "Drivers/CortexM.h"
#include "Drivers/AP.h"
#include "GSR.h"
#include "ROS.h"
#include "ACC.h"

volatile int16_t x = 0;
volatile int16_t y = 0;
volatile int16_t z = 0;

void readheart_rate(){

}

void readIR(){

}

void readACC(){

}

void readY(){

}

void readZ(){

}

void readGSR(){

}

void main(void){

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

    // Initialize Sensors
    ACC_init();
    GSR_Init();
    ROS_init();

    int time = 0;

    int r = AP_Init();
    AP_GetStatus();
    AP_GetVersion();
    AP_AddService(0x00FF);

    // Add Notify Characteristic for GSR
    uint16_t GSR = 0;
    AP_AddNotifyCharacteristic(0xFFFc, 2, &GSR, "GSR", &readGSR);

    // Add Notify Characteristic for ACC
    uint32_t ACC_data = 0;
    AP_AddNotifyCharacteristic(0xFFF8, 4, &ACC_data, "ACC", &readACC);

    int16_t z = 0;
    // Add Notify Characteristics for Heart Rate
    AP_AddNotifyCharacteristic(0xFFE8, 2, &z, "z", &readZ);

    // Add Notify Characteristic for SpO2
    uint32_t ir = 0;
    AP_AddNotifyCharacteristic(0xFFEC, 4, &ir, "ir", &readIR);

    // Start Broadcasting Bluetooth
    AP_RegisterService();
    AP_StartAdvertisement();
    AP_GetStatus();

    while(1){
        AP_BackgroundProcess();

        if(time == 2500){
            GSR = GSR_Read_Data();
            if(AP_GetNotifyCCCD(0)){
                AP_SendNotification(0);
            }
        }

        if(time == 5000){
            ACC_read_data();
            x = ACC_get_x();
            y = ACC_get_y();

            ACC_data = x << 16;
            ACC_data |= 0x0000FFFF & y;
            if(AP_GetNotifyCCCD(1)){
                AP_SendNotification(1);
            }
        }

        if(time == 7500){
            ir = ROS_read_ir();
            if(AP_GetNotifyCCCD(2)){
                AP_SendNotification(2);
            }
        }

        if(time == 10000){
            time = 0;
            z = ACC_get_z();
            if(AP_GetNotifyCCCD(3)){
                AP_SendNotification(3);
            }
        }

        time ++;

    }
}
