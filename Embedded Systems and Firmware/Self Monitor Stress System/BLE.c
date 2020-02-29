/*
 * BLE.c
 *
 *  Created on: Feb 29, 2020
 *      Author: Matthew Barondeau
 */

#include "Drivers/AP.h"
#include <stdint.h>

extern uint8_t spo2;
extern uint8_t heart_rate;
extern uint16_t x;
extern uint16_t y;
extern uint16_t z;
extern uint16_t GSR;

void readheart_rate(){

}

void readspo2(){

}

void readX(){

}

void readY(){

}

void readZ(){

}

void readGSR(){

}

void BLE_init(){
    // Initialize Bluetooth
    int r = AP_Init();
    AP_GetStatus();
    AP_GetVersion();
    AP_AddService(0x00FF);

    // Add Notify Characteristics for Heart Rate
    AP_AddNotifyCharacteristic(0xFFF9, 1, &heart_rate, "heart_rate", &readheart_rate);

    // Add Notify Characteristic for SpO2
    AP_AddNotifyCharacteristic(0xFFFA, 1, &spo2, "spo2", &readspo2);

    // Add Notify Characteristic for X
    AP_AddNotifyCharacteristic(0xFFFB, 2, &x, "X", &readX);

    // Add Notify Characteristic for Y
    AP_AddNotifyCharacteristic(0xFFFC, 2, &y, "Y", &readY);

    // Add Notify Characteristic for Z
    AP_AddNotifyCharacteristic(0xFFFD, 2, &z, "Z", &readZ);

    // Add Notify Characteristic for GSR
    AP_AddNotifyCharacteristic(0xFFFE, 2, &GSR, "GSR", &readGSR);

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
