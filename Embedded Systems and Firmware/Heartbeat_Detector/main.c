#include "msp.h"
#include "Drivers/Clock.h"
#include "Drivers/Edge_Interrupts_Port1.h"
#include "Drivers/LaunchPad.h"
#include "Drivers/UART0.h"
#include "Drivers/SysTick.h"
#include "Drivers/CortexM.h"
#include "Drivers/PWM.h"

#define DEBUG

#define Cycles_Per_Second 48000000
#define Cycles_Per_Systick 16777216
#define Seconds_Per_Minute 60


volatile int number_SysTick_Interrupts = 0;
volatile int SysTick_Started = 0;
volatile int Edge_Triggered_Capture = 0;
volatile int Experimental_HeartBeat = 0;

void main(void){
	Clock_Init48MHz();
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
    LaunchPad_Init();
    Edge_Trigger_Port1_Init(0x56);

    // Initialize SysTick then turn it off
    SysTick_Init(0xFFFFFF);
    SysTick_Stop();

    // Initialize Timer32 to interrupt once per second
    //Timer32Init(Cycles_Per_Second);

    PWM_Init1(6000, 3000);

    // Initialize UART0
    UART0_Init();

    while(1){
        WaitForInterrupt();
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
            unsigned int heart_rate = Seconds_Per_Minute * 1000 / interval;

            #ifdef DEBUG
            UART0_OutString("Heart Rate: ");
            UART0_OutUDec(heart_rate);
            UART0_OutString("\n\r");
            #endif
        }

    }

}

// 32-bit timer interrupt handler
void T32_INT1_IRQHandler(void){

    // Acknowledge
    TIMER32_1->INTCLR |= BIT0;              // Clear Timer32 interrupt flag

    #ifdef DEBUG
    UART0_OutString("Experimental Heart Beat: ");
    UART0_OutUDec(Experimental_HeartBeat);
    UART0_OutString("\n\r");
    #endif

    Experimental_HeartBeat = 0;
}


