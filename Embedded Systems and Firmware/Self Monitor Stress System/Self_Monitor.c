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

volatile uint16_t x = 0;
volatile uint16_t y = 0;
volatile uint16_t z = 0;

void readheart_rate(){

}

void readspo2(){

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

    // Initialize BLE
    // BLE_init();

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

    uint8_t heart_rate = 0;
    // Add Notify Characteristics for Heart Rate
    AP_AddNotifyCharacteristic(0xFFE8, 1, &heart_rate, "heart_rate", &readheart_rate);

    // Add Notify Characteristic for SpO2
    uint8_t spo2 = 0;
    AP_AddNotifyCharacteristic(0xFFEC, 1, &spo2, "spo2", &readspo2);

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
            x = (int16_t) ACC_get_x();
            y = (int16_t) ACC_get_y();

            ACC_data = x << 16;
            ACC_data |= 0x0000FFFF & y;
            if(AP_GetNotifyCCCD(1)){
                AP_SendNotification(1);
            }
        }

        if(time == 7500){
            ROS_calculate();
            heart_rate = ROS_read_heart_rate();
            if(AP_GetNotifyCCCD(2)){
                AP_SendNotification(2);
            }
        }

        if(time == 10000){
            time = 0;
            spo2 = ROS_read_spo2();
            if(AP_GetNotifyCCCD(3)){
                AP_SendNotification(3);
            }
        }

        time ++;

    }
}
