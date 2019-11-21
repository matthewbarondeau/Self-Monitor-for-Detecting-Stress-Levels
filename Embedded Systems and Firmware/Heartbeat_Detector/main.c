#include "msp.h"
#include "Drivers/Clock.h"
#include "Drivers/Edge_Interrupts_Port1.h"
#include "Drivers/LaunchPad.h"
#include "Drivers/UART0.h"
#include "Drivers/SysTick.h"
#include "Drivers/CortexM.h"
#include "Drivers/AP.h"
#include "Drivers/Timer32.h"

#define DEBUG
#define BLUETOOTH

#define Cycles_Per_Second 48000000
#define Cycles_Per_Systick 16777216
#define Seconds_Per_Minute 60


volatile int number_SysTick_Interrupts = 0;
volatile int SysTick_Started = 0;
volatile int Edge_Triggered_Capture = 0;
volatile int Experimental_HeartBeat = 0;
volatile int Timer_Interval = 0;
int Heartbeat;

uint8_t LEDs;

void readHeartbeat(void){
    // Print out Heartbeat information to UART0
    #ifdef DEBUG
    UART0_OutString("Bluetooth called\n\r");
    UART0_OutString("Heartrate: ");
    UART0_OutUDec(Experimental_HeartBeat);
    UART0_OutString("\n\r");
    #endif

    // Copy volatile into to Variable that bluetooth tracks
    Heartbeat = Experimental_HeartBeat;
}


void main(void){
	Clock_Init48MHz();
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
    LaunchPad_Init();
    Edge_Trigger_Port1_Init(0x56);

    // Initialize UART0
    UART0_Init();

    // Initialize SysTick then turn it off
    SysTick_Init(0xFFFFFF);
    SysTick_Stop();

    #ifdef BLUETOOTH

    // Initialize CC2650 Bluetooth
    EnableInterrupts();
    int r = AP_Init();
    AP_GetStatus();
    AP_GetVersion();
    AP_AddService(0xFFF0);

    // Add Heartbeat read characteristic
    /*
    AP_AddCharacteristic(
        0xFFF1,                     // Unique ID Number
        4,                          // Number of bytes to send
        &Heartbeat,                 // Pointer to the user data
        0x01,                       // Permission: 0=none, 1=Read, 2=Write, 3=Read+Write
        0x02,                       // Properties: 2=Read, 8=Write, 0xA=Read+Write
        "Heartbeat",                // Null-terminated string up to 20 bytes
        &readHeartbeat,             // Read function called
        0                           // Write function called
    );
    */

    // Finish configuring Bluetooth and start advertising
    AP_AddNotifyCharacteristic(0xFFF1, 1, &Heartbeat, "Heartbeat", &readHeartbeat);

    AP_RegisterService();
    AP_StartAdvertisement();
    AP_GetStatus(); // optional

    int time = 0;
    #endif

    while(1){
        AP_BackgroundProcess();
        time++;

        if(time > 2000000){
            time = 0;
            Heartbeat = Experimental_HeartBeat;
            if(AP_GetNotifyCCCD(0)){
                AP_SendNotification(0);
            }

        }

        if(Edge_Triggered_Capture){
            // Acknowledge Completion of Heartbeat
            Edge_Triggered_Capture = 0;

            #ifdef DEBUG
            UART0_OutString("SysTick Interrupts: ");
            UART0_OutUDec(number_SysTick_Interrupts);
            UART0_OutString("\n\rSysTick_VAL: ");
            UART0_OutUDec(SysTick->VAL);
            UART0_OutString("\n\r");
            #endif

            // Calculate elapsed cycles
            unsigned int time_elapsed = Cycles_Per_Systick * number_SysTick_Interrupts;
            time_elapsed += 0xFFFFFF - SysTick->VAL;

            // Reset SysTick
            number_SysTick_Interrupts = 0;
            SysTick_Started = 0;

            // Convert elapsed cycles into seconds
            // Calculates seconds with fixed point resolution 0.01
            int seconds = time_elapsed / Cycles_Per_Second;
            time_elapsed -= Cycles_Per_Second * seconds;
            time_elapsed *= 10000;
            int seconds_fraction = time_elapsed / Cycles_Per_Second;

            #ifdef DEBUG
            // Print out data to UART
            UART0_OutUDec(seconds);
            UART0_OutChar('.');
            if(seconds_fraction<10){
                UART0_OutUDec(0);
            }
            if(seconds_fraction<100){
                UART0_OutUDec(0);
            }
            UART0_OutUDec(seconds_fraction);
            UART0_OutString("\n\r");
            #endif

            // Calculate Instantaneous Heart Rate
            unsigned int interval = seconds * 10000 + seconds_fraction;
            unsigned int heart_rate = Seconds_Per_Minute * 10000 / interval;

            #ifdef DEBUG
            UART0_OutString("Heart Rate: ");
            UART0_OutUDec(heart_rate);
            UART0_OutString("\n\r");
            #endif

            Experimental_HeartBeat = heart_rate;
        }

    }

}
