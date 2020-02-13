#include "msp.h"
#include "spo2.h"
#include "heartRate.h"
#include "i2c.h"
/**
 * main.c
 */

#define SLAVE_ADDRESS 0x57;

/* I2C Master Configuration Parameter */
// Baud rate selectable (100KBPS, 400KBPS)
const eUSCI_I2C_MasterConfig i2cConfig =
{
        EUSCI_B_I2C_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        3000000,                                // SMCLK = 3MHz (default)
        EUSCI_B_I2C_SET_DATA_RATE_100KBPS,      // Desired I2C Clock of 100khz
        0,                                      // No byte counter threshold
        EUSCI_B_I2C_NO_AUTO_STOP                // No Autostop
};

void main(void){
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	I2C_initMaster(EUSCI_B0_BASE, &i2cConfig);
	I2C_setSlaveAddress(EUSCI_B0_BASE, 0x57);
	I2C_enableModule(EUSCI_B0_BASE);

	MAX30102_Init();



}
