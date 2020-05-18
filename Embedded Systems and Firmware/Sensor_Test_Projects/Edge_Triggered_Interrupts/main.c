#include "msp.h"
#include "Edge_Interrupts_Port1.h"
#include "CortexM.h"
#include "Clock.h"


/**
 * main.c
 */
void main(void){
    Clock_Init48MHz();
    Edge_Trigger_Port1_Init(0x12);

    // initialize P2.2-P2.0 and make them outputs (P2.2-P2.0 built-in RGB LEDs)
    P2->SEL0 &= ~0x07;
    P2->SEL1 &= ~0x07;                 // configure built-in RGB LEDs as GPIO
    P2->DS |= 0x07;                    // make built-in RGB LEDs high drive strength
    P2->DIR |= 0x07;                   // make built-in RGB LEDs out
    P2->OUT &= ~0x07;                  // RGB = off
    EnableInterrupts();                // clear the I bit

    while(1){
      WaitForInterrupt();
    }
  }
