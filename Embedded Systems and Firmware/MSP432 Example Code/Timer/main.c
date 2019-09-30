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

//Initialize 32-bit timer
void Timer32Init(int period){
    // Timer32 set up in periodic mode, 32-bit, no pre-scale
    TIMER32_1->CONTROL = TIMER32_CONTROL_SIZE | TIMER32_CONTROL_MODE;

    // Load Period
    TIMER32_1->LOAD= period;

    // Enable the Timer32 interrupt in NVIC
    NVIC->ISER[0] = 1 << ((T32_INT1_IRQn) & 31);

    // Start Timer32 with interrupt enabled
    TIMER32_1->CONTROL |= TIMER32_CONTROL_ENABLE | TIMER32_CONTROL_IE;

    // Enable sleep on exit from ISR
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;
}

//Disables 32-bit timer
void Timer32Stop(void){
    TIMER32_1->CONTROL &= ~(TIMER32_CONTROL_ENABLE | TIMER32_CONTROL_IE);
}

//Enables SysTick Timer
void SysTickInit(int period){
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;    //Enable SysTick
    SysTick->LOAD = period;                         //Set Period
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;      //Enable Interrupt
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;       //Start SysTick
}

//Disables SysTick Timer
void SysTickStop(void){
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

int main(void){
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;
    led1Init();
    led2Init();
    Timer32Init(4800000);   //1 Second period
    SysTickInit(2400000);   //0.5 Second period
    __enable_irq();         //Enable Interrupts
    __sleep();              //Low Power Mode
    __no_operation();
}

//32-bit timer interrupt handler
void T32_INT1_IRQHandler(void){
    TIMER32_1->INTCLR |= BIT0;              // Clear Timer32 interrupt flag
    P1->OUT ^= BIT0;
}

//SysTick Handler
//No explicit acknowledgement required
void SysTick_Handler(void){
    P2->OUT ^= 0x7;
}
