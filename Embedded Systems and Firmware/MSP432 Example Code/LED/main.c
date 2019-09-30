#include "msp.h"

//Initializes built in switches on P1.1 and P1.4
void switchInit(void){
    P1->SEL0 &= ~0x12;
    P1->SEL1 &- ~0x12;
    P1->DIR  &= ~0x2;
    P1->REN  |= 0x12;
    P1->OUT  |= 0x12;
}

//Initializes LED1 on P1.0
void led1Init(void){
    P1->SEL0 &= ~0x1;
    P1->SEL1 &= ~0x1;
    P1->DIR |= 0x1;
    P1->REN &= ~0x1;
    P1->OUT &= ~0x1;
}

//Initializes LED2 on P2.0-2.2
void led2Init(void){
    P2->SEL0 &= ~0x7;
    P2->SEL1 &= ~0x7;
    P2->DIR |= 0x7;
    P2->REN &= ~0x7;
    P2->OUT &= ~0x7;
}

int main(void) {
    volatile char LED1 = 1;
    volatile char LED2 = 1;
    volatile uint32_t i;
    volatile uint32_t k;

    WDT_A->CTL = WDT_A_CTL_PW |             // Stop WDT
                 WDT_A_CTL_HOLD;

    led1Init();
    led2Init();
    switchInit();

    while (1){
        for (i = 40000; i > 0; i--);        // Delay
		
		//Read Inputs
        int SW1 = P1->IN & 0x2;
        int SW2 = P1->IN & 0x10;
		
		//If SW1 is pressed, LED outputs change color
		//IF SW1 not pressed, LED flashes red LED
        if(!SW1){
            for(k = 0; k < 40000; k++); //Debounce
            LED1 = LED1 << 1;
            if(LED1 > 4){
                LED1 = 1;
            }
            P2->OUT &= ~0x7;
            P2->OUT ^= LED1;
        }else{
            for(k = 0; k < 40000; k++); //Debounce
            P2->OUT &- ~0x7;
            P2->OUT ^= LED1;
        }
		
		//If SW2 is pressed, toggle LED2
        if(!SW2){
            for(k = 0; k < 40000; k++); //Debounce
            if(LED2){
                LED2 = 0;
            } else{
                LED2 = 1;
            }
            P1->OUT &= ~0x1;
            P1->OUT |= LED2;
        } else{
            for(k = 0; k < 40000; k++); //Debounce
            P1->OUT |= LED2;
        }
    }
}

