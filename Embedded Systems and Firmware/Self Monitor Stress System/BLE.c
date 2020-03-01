/*
 * BLE.c
 *
 *  Created on: Feb 29, 2020
 *      Author: Matthew Barondeau
 */

#include "Drivers/AP.h"
#include <stdint.h>
#include "ROS.h"

extern uint8_t spo2;
extern uint8_t heart_rate;
extern uint16_t x;
extern uint16_t y;
extern uint16_t z;
extern uint16_t GSR;
extern uint64_t ACC_data;

/*

void BLE_init(){
    // Initialize Bluetooth
    int r = AP_Init();
    AP_GetStatus();
    AP_GetVersion();
    AP_AddService(0x00FF);



    // Add Notify Characteristic for SpO2
    //AP_AddNotifyCharacteristic(0xFFEC, 1, &spo2, "spo2", &readspo2);


    // Add Notify Characteristic for GSR
    AP_AddNotifyCharacteristic(0xFFFc, 2, &GSR, "GSR", &readGSR);

    // Add Notify Characteristic for ACC
    AP_AddNotifyCharacteristic(0xFFF8, 4, &ACC_data, "ACC", &readACC);

    // Add Notify Characteristics for Heart Rate
    AP_AddNotifyCharacteristic(0xFFE8, 1, &heart_rate, "heart_rate", &readheart_rate);

    // Start Broadcasting Bluetooth
    AP_RegisterService();
    AP_StartAdvertisement();
    AP_GetStatus();
}

void BLE_update_app(){
    if(AP_GetNotifyCCCD(0)){
        AP_SendNotification(0);
    }

    if(AP_GetNotifyCCCD(1)){
        AP_SendNotification(1);
    }

    if(AP_GetNotifyCCCD(2)){
        AP_SendNotification(2);
    }

    if(AP_GetNotifyCCCD(3)){
        AP_SendNotification(3);
    }

    if(AP_GetNotifyCCCD(4)){
        AP_SendNotification(4);
    }

    if(AP_GetNotifyCCCD(5)){
        AP_SendNotification(5);
    }
}
*/
