
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
        6000000,                                // SMCLK = 3MHz
        EUSCI_B_I2C_SET_DATA_RATE_400KBPS,      // Desired I2C Clock of 100khz
        0,                                      // No byte counter threshold
        EUSCI_B_I2C_NO_AUTO_STOP                // No Autostop
};

void Clock_Init(){
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ,
                GPIO_PIN3 | GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION);
        MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);

        /* Just in case the user wants to use the getACLK, getMCLK, etc. functions,
         * let's set the clock frequency in the code.
         */
        CS_setExternalClockSourceFrequency(32000,48000000);

        /* Starting HFXT in non-bypass mode without a timeout. Before we start
         * we have to change VCORE to 1 to support the 48MHz frequency */
        MAP_PCM_setCoreVoltageLevel(PCM_VCORE1);
        MAP_FlashCtl_setWaitState(FLASH_BANK0, 1);
        MAP_FlashCtl_setWaitState(FLASH_BANK1, 1);
        CS_startHFXT(false);

        /* Initializing MCLK to HFXT (effectively 48MHz) */
        MAP_CS_initClockSignal(CS_MCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_4);
        MAP_CS_initClockSignal(CS_SMCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_4);
}

void ROS_Init(){
    /* Select Port 1 for I2C - Set Pin 6, 7 to input Primary Module Function,
     *   (UCB0SIMO/UCB0SDA, UCB0SOMI/UCB0SCL).
     */
    uint32_t smclk = CS_getSMCLK();
    uint32_t mclk = CS_getMCLK();
    Clock_Init();
    smclk = CS_getSMCLK();
    mclk = CS_getMCLK();

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
    
    // Check Connection
    uint8_t deviceid = readRegister8(0xFF);
    if(deviceid != 0x15){
        while(1){};
    }

    MAX30102_Init();

}

int main(void)
{
    ROS_Init();

    long irValue;
    while(1){
        irValue = MAX30102_getIR();
        MAX30102_nextSample();
        UART0_OutString("IrValue: ");
        UART0_OutUDec(irValue);
        UART0_OutString("\n\r");
    }

}



int32_t ROS_Read_IR(){
    int32_t irValue = MAX30102_getIR();
    MAX30102_nextSample();
}

uint8_t ROS_Read_SpO2(){

}

uint8_t ROS_Read_Heartrate(){

}

uint8_t ROS_calcuulate_SpO2(){

}
