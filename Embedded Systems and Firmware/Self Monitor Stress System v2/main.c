#include "msp.h"
#include "GSR.h"
#include "ACC.h"
#include "ROS.h"
#include "Drivers/UART0.h"
#include "Drivers/Switches.h"

/**
 * main.c
 */

void main(void){
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	// Initialize UART
	UART0_Init();
	UART0_OutString("Hello\r\n");
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
	    uint8_t avg_heart_rate = ROS_read_avg_heart_rate();
	    uint16_t gsr = GSR_Read_Data();
	    int16_t x = ACC_get_x();
	    int16_t y = ACC_get_y();
	    int16_t z = ACC_get_z();
	    int switch1 = Switches_SW1_Toggle();
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
	}
}
