
//------------See AP.c for details of hardware connections to CC2650--------------------
//------------See LaunchPad.c for details of switches and LEDs--------------------------

#include <stdint.h>
#include "Drivers/Clock.h"
#include "Drivers/CortexM.h"
#include "Drivers/AP.h"
#include "Drivers/UART0.h"
#include "Drivers/LaunchPad.h"
#include "msp.h"
#include "adc.h"

uint8_t LEDs;

// ********OutValue**********
// Debugging dump of a data value to virtual serial port to PC
// data shown as 1 to 8 hexadecimal characters
// Inputs:  response (number returned by last AP call)
// Outputs: none
void OutValue(char *label,uint32_t value){ 
  UART0_OutString(label);
  UART0_OutUHex(value);
}

void readLED(void){
    OutValue("\n\rRead LED=", LEDs);
}

void writeLED(void){
    LaunchPad_Output(LEDs);
    OutValue("\n\rWrote LED= ", LEDs);
}

int main(void){
    volatile int r;
    int time=0;
    DisableInterrupts();
    Clock_Init48MHz();
    UART0_Init();
    LaunchPad_Init();  // input from switches, output to LEDs on LaunchPad
    ADC0_Ch6_Init();
    EnableInterrupts();
    UART0_OutString("\n\rApplication Processor - MSP432-CC2650\n\r");
    // Initialize Bluetooth
    r = AP_Init();
    AP_GetStatus();  // optional
    AP_GetVersion(); // optional
    AP_AddService(0xFFF0);

    // Example Characteristic
    LEDs = P2->OUT & 0x7;
    AP_AddCharacteristic(
        0xFFF4,             // Unique ID Number
        1,                  // Number of bytes to send
        &LEDs,              // Pointer to the user data
        0x03,               // Permission: 0=none, 1=Read, 2=Write, 3=Read+Write
        0x0A,               // Properties: 2=Read, 8=Write, 0xA=Read+Write
        "LEDs",             // Null-terminated string up to 20 bytes
        &readLED,      // Read function called
        &writeLED      // Write function called
    );

    //Start broadcasting Bluetooth
    AP_RegisterService();
    AP_StartAdvertisement();
    AP_GetStatus(); // optional


    while(1){
        time++;
        AP_BackgroundProcess();  // handle incoming SNP frames
        int data = ADC_Ch6_Samples(128);
        UART0_OutString("ADC: ");
        UART0_OutUDec(data);
        UART0_OutString("\n\r");
    }
}
