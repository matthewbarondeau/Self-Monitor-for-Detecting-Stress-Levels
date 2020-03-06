#include "msp.h"
#include "Drivers/Clock.h"
#include "Drivers/AP.h"
#include "GSR.h"


/**
 * main.c
 */

void readGSR(){
    // Does nothing
}

void main(void){
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	Clock_Init48MHz();

	// Initialize CC2650
	int r = AP_Init();
	AP_GetStatus();
	AP_GetVersion();
	AP_AddService(0x00FF);

	// Add characteristics
	uint16_t GSR = 0;
	AP_AddNotifyCharacteristic(0xFFFC, 2, &GSR, "GSR", &readGSR);

	// Initialize GSR
	GSR_Init();

	// Finish Bluetooth Init
	AP_RegisterService();
	AP_StartAdvertisement();
	AP_GetStatus();

	int time = 0;
	while(1){
	    // Bluetooth Background stuff
	    AP_BackgroundProcess();
	    if(time > 10000){
	        // Read GSR
	        GSR = GSR_Read_Data();
	        // Send update to app
	        if(AP_GetNotifyCCCD(0)){
	            AP_SendNotification(0);
	        }
	        time = 0;
	    }

	    time ++;

	}
}
