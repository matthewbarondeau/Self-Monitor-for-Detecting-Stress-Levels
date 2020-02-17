// Self_Monitor.c
// Author: Matthew Barondeau
// Created 11/24/2019
// Updated 11/24/2019

#include "msp.h"
#include "Drivers/Clock.h"
#include "Drivers/LaunchPad.h"
#include "Drivers/CortexM.h"
#include "Drivers/AP.h"
#include "Drivers/UART0.h"
#include "GSR.h"
#include "ROS.h"
#include "Accelerometer.h"

#define DEBUG0  // UART0 Debugging

uint8_t Accelerometer = 0;
uint8_t GSR = 0;
uint8_t ROS = 0;

void readAccelerometer(){
    #ifdef DEBUG0
    UART0_OutString("Sent updated heartbeat: ");
    UART0_OutUDec(Accelerometer);
    UART0_OutString("\n\r");
    #endif
}

void readGSR(){
    #ifdef DEBUG0
    UART0_OutString("Sent updated GSR: ");
    UART0_OutUDec(GSR);
    UART0_OutString("\n\r");
    #endif
}

void readROS(){
    #ifdef DEBUG0
    UART0_OutString("Sent updated ROS: ");
    UART0_OutUDec(ROS);
    UART0_OutString("\n\r");
    #endif
}

void main(void){
	Clock_Init48MHz();
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
    LaunchPad_Init();

    #ifdef DEBUG0
    UART0_Init();
    #endif

    // Initialize Sensors
    ACC_Init();
    GSR_Init();
    ROS_Init();

    // Initialize Bluetooth
    int r = AP_Init();
    AP_GetStatus();
    AP_GetVersion();
    AP_AddService(0x00FF);

    // Add Notify Characteristics for Heart Rate from GMR
    AP_AddNotifyCharacteristic(0xFFF1, 1, &Accelerometer, "Accelerometer", &readAccelerometer);

    // Add Notify Characteristic for GSR
    AP_AddNotifyCharacteristic(0xFFF2, 1, &GSR, "GSR", &readGSR);

    // Add Notify Characteristic for ROS
    AP_AddNotifyCharacteristic(0xFFF3, 1, &ROS, "ROS", &readROS);

    // Start Broadcasting Bluetooth
    AP_RegisterService();
    AP_StartAdvertisement();
    AP_GetStatus();

    uint32_t time = 0;

    while(1){
        AP_BackgroundProcess();
        time++;

        if(time > 2000000){
            time = 0;

            // Read new value of Heartbeat
            // Send to Mobile Application
            Accelerometer = ACC_Read_Data();
            if(AP_GetNotifyCCCD(0)){
                AP_SendNotification(0);
            }

            // Read new value of GSR
            // Send to Mobile Application
            GSR = GSR_Read_Data();
            if(AP_GetNotifyCCCD(1)){
                AP_SendNotification(1);
            }

            // Read new value of ROS
            // Send to Mobile Application
            ROS = ROS_Read_Data();
            if(AP_GetNotifyCCCD(2)){
                AP_SendNotification(2);
            }
        }
    }
}
