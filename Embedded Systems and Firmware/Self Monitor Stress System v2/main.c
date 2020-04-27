#include "msp.h"
#include "Drivers/Clock.h"
#include "Drivers/AP.h"
#include "GSR.h"
#include "ACC.h"
#include "Drivers/UART0.h"
#include "ROS.h"
#include "Drivers/Timer32.h"
#include "Drivers/Switches.h"

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
	uint64_t lastprint = 0;

	// Initialize UART
	UART0_Init();

    // Initialize GSR
    GSR_Init();

    // Initialize ACC
    ACC_init();

    // Initialize ROS
    ROS_init();

    // Initialize Switches
    Switches_Init();

	while(1){
	    ACC_read_data();
	    uint8_t heart_rate = ROS_read_heart_rate();
	    uint8_t avg_heart_rate = ROS_read_avg_heart_rate();
	    uint16_t gsr = GSR_Read_Data();
	    int16_t x = ACC_get_x();
	    int16_t y = ACC_get_y();
	    int16_t z = ACC_get_z();
	    int switch1 = Switches_Read(0x2);
	    //if(getOS_MsTime() - lastprint > 100){
	        //UART0_OutUDec(heart_rate);
	        //UART0_OutChar(',');
	        UART0_OutUDec(avg_heart_rate);
	        UART0_OutChar(',');
	        UART0_OutDec(x);
	        UART0_OutChar(',');
	        UART0_OutDec(y);
	        UART0_OutChar(',');
	        UART0_OutDec(z);
	        UART0_OutChar(',');
	        UART0_OutDec(gsr);
	        UART0_OutChar(',');
	        UART0_OutDec(switch1);
	        UART0_OutString("\n\r");
	    //    lastprint = getOS_MsTime();
	    //}


	}
}
