
/* DriverLib Defines */
#include "driverlib/MSP432P4xx/driverlib.h"
#include "Timer32.h"

/* Standard Defines */
#include <stdint.h>

#include <stdbool.h>
#include <string.h>

#include "MAX30102.h"
#include "UART0.h"
#include "Clock.h"

/* Slave Address for I2C Slave */
#define SLAVE_ADDRESS       0x57

/* I2C Master Configuration Parameter */
const eUSCI_I2C_MasterConfig i2cConfig =
{
        EUSCI_B_I2C_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        3000000,                                // SMCLK = 3MHz
        EUSCI_B_I2C_SET_DATA_RATE_400KBPS,      // Desired I2C Clock of 100khz
        0,                                      // No byte counter threshold
        EUSCI_B_I2C_NO_AUTO_STOP                // No Autostop
};


int main(void)
{
    volatile uint32_t ii;
    //Clock_Init48MHz();
    /* Disabling the Watchdog  */
    MAP_WDT_A_holdTimer();

    /* Select Port 1 for I2C - Set Pin 6, 7 to input Primary Module Function,
     *   (UCB0SIMO/UCB0SDA, UCB0SOMI/UCB0SCL).
     */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6,
            GPIO_PIN4 + GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Initializing I2C Master to SMCLK at 400khz with no autostop */
    MAP_I2C_initMaster(EUSCI_B1_BASE, &i2cConfig);

    MAP_I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_MODE);

    /* Specify slave address */
    MAP_I2C_setSlaveAddress(EUSCI_B1_BASE, SLAVE_ADDRESS);

    /* Enable I2C Module to start operations */
    MAP_I2C_enableModule(EUSCI_B1_BASE);

    MAP_I2C_disableInterrupt(EUSCI_B1_BASE, 0xFFFF);

    // Keeps 10ms Time incriments
    // Timer32Init(30000);

    UART0_Init();

    // Check Connection
    uint8_t deviceid = readRegister8(0xFF);
    if(deviceid != 0x15){
        while(1){};
    }

    MAX30102_Init();

    setPulseAmplitudeRed(0x0A);
    deviceid = readRegister8(0xFF);

    long irValue;
    while(1){
        irValue = MAX30102_getIR();
        MAX30102_nextSample();
        UART0_OutString("IrValue: ");
        UART0_OutUDec(irValue);
        UART0_OutString("\n\r");
    }

}

    
