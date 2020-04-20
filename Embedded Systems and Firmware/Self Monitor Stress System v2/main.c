#include "msp.h"
#include "Drivers/Clock.h"
#include "Drivers/AP.h"
#include "GSR.h"
#include "ACC.h"
#include "Drivers/UART0.h"
#include "ROS.h"
#include "Drivers/Timer32.h"

/**
 * main.c
 */

void readBLE(){
    // Does nothing
}

void main(void){
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

    //Clock_Init48MHz();

	// Initialize OS_Time
	Timer32Init(3000);

	// Initialize UART
	UART0_Init();

    // Initialize GSR
    GSR_Init();

    // Initialize ACC
    ACC_init();

    // Initialize ROS
    ROS_init();

	while(1){
	    ACC_read_data();
	    UART0_OutUDec(ROS_read_heart_rate());
	    UART0_OutChar(',');
	    UART0_OutUDec(ROS_read_avg_heart_rate());
	    UART0_OutChar(',');
	    UART0_OutDec(ACC_get_x());
	    UART0_OutChar(',');
	    UART0_OutDec(ACC_get_y());
	    UART0_OutChar(',');
	    UART0_OutDec(ACC_get_z());
	    UART0_OutChar(',');
	    UART0_OutDec(GSR_Read_Data());
	    UART0_OutString("\n\r");
	}
}
