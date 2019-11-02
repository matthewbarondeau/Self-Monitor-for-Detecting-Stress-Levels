/*
 * Edge_Interrupts_Port1.c
 *
 *  Created on: Oct 19, 2019
 *      Author: Matthew Barondeau
 */

#include "msp.h"
#include <stdint.h>
#include "SysTick.h"


extern int SysTick_Started;
extern int Edge_Triggered_Capture;
extern int Experimental_HeartBeat;

void Edge_Trigger_Port1_Init(uint8_t bitmask){
    // Configured Port 1 as GPIO
    P1->SEL0 &= ~bitmask;
    P1->SEL1 &= ~bitmask;

    // Make Port 1 pins inputs
    P1->DIR  &=  bitmask;

    // Configure Rising Edge Interrupt
    P1->IES  &=  bitmask;       // Rising Edge Events
    P1->IFG  &= ~bitmask;       // Helps with debouncing
    P1->IE   |=  bitmask;       // Arms Interrupts

    // Configure Falling Edge Interrupt
    // P1->IES &=  bitmask;
    // P1->IFG &= ~bitmask;
    // P1->IE  |=  bitmask;

    // Optional
    // Do if using P1.1 or P1.4
    P1->REN  |=  bitmask;       // Resistor Enable. Enables PUR.
    P1->OUT  |=  bitmask;

    NVIC->IP[8] = (NVIC->IP[8] & 0x00FFFFFF) | 0x40000000;
    NVIC->ISER[1] = 0x00000008;
}

void PORT1_IRQHandler(void){
    // Returns value of highest priority interrupt and clears flag
    // For example, P1.1 will return a 0x4 and clear flag one
    // Return value = 2 * (n + 1) where n is the highest priority
    uint8_t status = P1->IV;

    // Reads P1.1
    if(status == 0x04){
      P2->OUT ^= 0x01;                 // toggle Red RGB LED
      status = P1->IV;
    }

    // Reads P1.2
    // Add back later. Using Buttons for testing purposes
    /*
    if(status == 0x05){
        status = P1->IV;
        if(SysTick_Started == 1){
            SysTick_Stop();
            Edge_Triggered_Capture = 1;
        } else {
            SysTick_Restart();
        }
    }
    */

    if(status == 0xA){
        #ifdef DEBUG
        // Toggle blue RGB LED
        P2->OUT ^= 0x04;
        #endif

        // Update every time we detect heartbeat
        // Experimental_HeartBeat++;

        // Calculate difference between edge interrupts
        if(SysTick_Started == 1){
            SysTick_Stop();
            Edge_Triggered_Capture = 1;
        } else {
            SysTick_Restart();
            SysTick_Started = 1;
        }
    }
}
