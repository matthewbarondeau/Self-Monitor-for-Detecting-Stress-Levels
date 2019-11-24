// GMR.c
// Author: Matthew Barondeau
// Created 11/24/2019
// Updated 11/24/2019

#include "msp.h"
#include "Drivers/Edge_Interrupts_Port1.h"
#include "Drivers/SysTick.h"
#include <stdint.h>

#define CYCLES_PER_SYSTICK      16777216
#define CYCLES_PER_SECOND       48000000
#define SECONDS_PER_MINUTE      60
#define RESOLUTION  10000

volatile int SysTick_Started;
volatile int Edge_Triggered_Capture;
volatile int number_SysTick_Interrupts;

uint8_t heartbeat;

// Initializes GMR Sensor
// Inputs: None
// Output: None
void GMR_Init(){
    Edge_Trigger_Port1_Init(0x56);
    SysTick_Init(0xFFFFFF);
    SysTick_Stop();
    // TODO Setup 16 bit timer and pass GMR_Update_Heartbeat
}

// Reads Heartbeat value
// Inputs: None
// Output: Heartbeat
uint8_t GMR_Read_Data(){
    return heartbeat;
}

// Calculates Heartbeat based on Edge Triggered Interrupts
// Called periodically by Timer
// TODO Call it periodically
// Inputs: None
// Output: None
void GMR_Update_Heartbeat(){
    if(Edge_Triggered_Capture){
        // Clear Flag
        Edge_Triggered_Capture = 0;

        // Calculate time elapsed since last interrupt
        uint32_t time_elapsed = CYCLES_PER_SYSTICK * number_SysTick_Interrupts;
        time_elapsed += 0xFFFFFF - SysTick->VAL;

        // Resets SysTick flags
        number_SysTick_Interrupts = 0;
        SysTick_Started = 0;

        // Converts cycles to seconds
        uint32_t seconds = time_elapsed / CYCLES_PER_SECOND;
        time_elapsed -= CYCLES_PER_SECOND * seconds;
        time_elapsed *= RESOLUTION;
        uint32_t seconds_fraction = time_elapsed / CYCLES_PER_SECOND;

        // Calculates heart rate
        uint32_t interval = seconds * RESOLUTION + seconds_fraction;
        uint32_t heart_rate = SECONDS_PER_MINUTE * RESOLUTION / interval;

        // Updates heartrate variable
        heartbeat = heart_rate;
    }
}
