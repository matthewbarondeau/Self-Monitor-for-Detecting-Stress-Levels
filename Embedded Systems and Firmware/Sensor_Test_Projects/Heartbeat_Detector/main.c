// main.c
// Author: Matthew Barondeau
// Modified 5/21/2020

#include "msp.h"
#include "Drivers/Clock.h"
#include "Drivers/Edge_Interrupts_Port1.h"
#include "Drivers/LaunchPad.h"
#include "Drivers/UART0.h"
#include "Drivers/SysTick.h"
#include "Drivers/CortexM.h"
#include "Drivers/AP.h"

// Comment out to remove UART0 Printouts
#define DEBUG

#define Cycles_Per_Second 48000000
#define Cycles_Per_Systick 16777216
#define Seconds_Per_Minute 60


volatile int number_SysTick_Interrupts = 0;
volatile int SysTick_Started = 0;
volatile int Edge_Triggered_Capture = 0;
volatile int Experimental_HeartBeat = 0;
volatile int Timer_Interval = 0;
int Heartbeat;

void readHeartbeat(void){
    // Print out Heartbeat information to UART0
    UART0_OutString("Heartrate: ");
    UART0_OutUDec(Experimental_HeartBeat);
    UART0_OutString("\n\r");

}

// Calculates the time difference between two button presses
// Uses that time difference to implement heartbeat
// Sends heartbeat via bluetooth

void main(void){
    // Initialize Clock to 48Mhz
	Clock_Init48MHz();

	// Disable Watchdog Timer
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    // Initialize LEDs and Switches
    LaunchPad_Init();

    // Enable Edge Triggered Interrupts
    Edge_Trigger_Port1_Init(0x56);

    #ifdef DEBUG
    // Initialize UART0
    UART0_Init();
    #endif

    // Initialize SysTick then turn it off
    SysTick_Init(0xFFFFFF);
    SysTick_Stop();

    // Initialize CC2650 Bluetooth
    EnableInterrupts();
    int r = AP_Init();
    AP_GetStatus();
    AP_GetVersion();
    AP_AddService(0x00FF);

    // Finish configuring Bluetooth and start advertising
    AP_AddNotifyCharacteristic(0xFFF1, 1, &Heartbeat, "Heartbeat", &readHeartbeat);

    AP_RegisterService();
    AP_StartAdvertisement();
    AP_GetStatus();

    int time = 0;

    while(1){

        // Handle background SNP frames
        AP_BackgroundProcess();

        // Used to not overload bluetooth
        time++;

        // Send Experimental HeartBeat
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
