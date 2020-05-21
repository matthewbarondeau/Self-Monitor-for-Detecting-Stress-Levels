
#include <stdint.h>
#include "msp.h"
#include "Drivers/AP.h"
#include "Drivers/adc.h"
#include "Drivers/UART0.h"
#include "Drivers/Clock.h"
#include "Drivers/CortexM.h"

// Global ADC value
uint16_t ADC;

void ADC6(void){
    UART0_OutString("ADC: ");
    UART0_OutUDec(ADC);
    UART0_OutString("\n\r");
}

int main(void){
    volatile int r;
    int time=0;
    DisableInterrupts();

    // Initialize Clock to 48 MHz
    Clock_Init48MHz();

    // Initialize UART0 for debug
    UART0_Init();

    // Initialize ADC
    ADC0_Ch6_Init();

    EnableInterrupts();

    // Initialize Bluetooth
    r = AP_Init();
    AP_GetStatus();  // optional
    AP_GetVersion(); // optional
    AP_AddService(0xFFF0);

    // Add ADC notify characteristic
    AP_AddNotifyCharacteristic(
            0xFFF1,         // Characteristic number
            2,              // Number of bytes the variable is
            &ADC,           // Variable data that the bluetooth sends
            "ADC0 Ch 6",    // Characteristic Name in application
            &ADC6           // Function called send notification
    );

    //Start broadcasting Bluetooth
    AP_RegisterService();
    AP_StartAdvertisement();
    AP_GetStatus(); // optional

    while(1){
        // Time variable used to not overload the bluetooth module
        time++;

        // handle incoming SNP frames
        AP_BackgroundProcess();

        // Send updated ADC value
        if(time >2000000){
            // Average 10 ADC samples
            ADC = ADC_Ch6_Samples(10);
            time = 0;
            if(AP_GetNotifyCCCD(0)){
                AP_SendNotification(0);
            }
        }
    }
}
