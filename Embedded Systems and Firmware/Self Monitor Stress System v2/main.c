#include "msp.h"
#include "Drivers/Clock.h"
#include "Drivers/AP.h"
#include "GSR.h"
#include "ACC.h"


/**
 * main.c
 */

void readBLE(){
    // Does nothing
}

void main(void){
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

    //Clock_Init48MHz();

    // Initialize GSR
    GSR_Init();

    // Initialize ACC
     ACC_init();


/*

	// Initialize CC2650
	int r = AP_Init();
	AP_GetStatus();
	AP_GetVersion();
	AP_AddService(0x00FF);

	// Add characteristics
	uint16_t GSR = 0;
	AP_AddNotifyCharacteristic(0xFFFC, 2, &GSR, "GSR", &readBLE);

	int16_t x = 0;
	AP_AddNotifyCharacteristic(0xFFFD, 2, &x, "x", &readBLE);

	int16_t y = 0;
	AP_AddNotifyCharacteristic(0xFFFE, 2, &y, "y", &readBLE);

	int16_t z = 0;
	AP_AddNotifyCharacteristic(0xFFFF, 2, &z, "z", &readBLE);



	// Finish Bluetooth Init
	AP_RegisterService();
	AP_StartAdvertisement();
	AP_GetStatus();

*/
	int time = 0;
	while(1){
	    // Bluetooth Background stuff
	    //AP_BackgroundProcess();

	    if(time == 2000){
	        ACC_read_data();
	    }
/*
	    if(time == 4000){
	        x = ACC_get_x();
	        if(AP_GetNotifyCCCD(1)){
	            AP_SendNotification(1);
	        }
	    }

	    if(time == 6000){
	        y = ACC_get_y();
	        if(AP_GetNotifyCCCD(2)){
	            AP_SendNotification(2);
	        }
	    }

	    if(time == 8000){
	        z = ACC_get_z();
	        if(AP_GetNotifyCCCD(3)){
	            AP_SendNotification(3);
	        }
	    }

	    if(time > 10000){
	        // Read GSR
	        GSR = GSR_Read_Data();
	        // Send update to app
	        if(AP_GetNotifyCCCD(0)){
	            AP_SendNotification(0);
	        }
	        time = 0;
	    }
*/
	    time ++;

	}
}
