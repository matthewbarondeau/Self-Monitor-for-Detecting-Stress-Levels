/*
 * MAX30102.c
 *
 *  Created on: Jan 29, 2020
 *      Author: Matthew Barondeau
 */

#include <stdint.h>
#include <stdbool.h>

#include "i2c.h"
#include "rom_map.h"

// Status Registers
static const uint8_t MAX30105_INTSTAT1 =        0x00;
static const uint8_t MAX30105_INTSTAT2 =        0x01;
static const uint8_t MAX30105_INTENABLE1 =      0x02;
static const uint8_t MAX30105_INTENABLE2 =      0x03;

// FIFO Registers
static const uint8_t MAX30105_FIFOWRITEPTR =    0x04;
static const uint8_t MAX30105_FIFOOVERFLOW =    0x05;
static const uint8_t MAX30105_FIFOREADPTR =     0x06;
static const uint8_t MAX30105_FIFODATA =        0x07;

// Configuration Registers
static const uint8_t MAX30105_FIFOCONFIG =      0x08;
static const uint8_t MAX30105_MODECONFIG =      0x09;
static const uint8_t MAX30105_PARTICLECONFIG =  0x0A;    // Note, sometimes listed as "SPO2" config in datasheet (pg. 11)
static const uint8_t MAX30105_LED1_PULSEAMP =   0x0C;
static const uint8_t MAX30105_LED2_PULSEAMP =   0x0D;
static const uint8_t MAX30105_LED3_PULSEAMP =   0x0E;
static const uint8_t MAX30105_LED_PROX_AMP =    0x10;
static const uint8_t MAX30105_MULTILEDCONFIG1 = 0x11;
static const uint8_t MAX30105_MULTILEDCONFIG2 = 0x12;

// Die Temperature Registers
static const uint8_t MAX30105_DIETEMPINT =      0x1F;
static const uint8_t MAX30105_DIETEMPFRAC =     0x20;
static const uint8_t MAX30105_DIETEMPCONFIG =   0x21;

// Proximity Function Registers
static const uint8_t MAX30105_PROXINTTHRESH =   0x30;

// Part ID Registers
static const uint8_t MAX30105_REVISIONID =      0xFE;
static const uint8_t MAX30105_PARTID =          0xFF;    // Should always be 0x15. Identical to MAX30102.

// MAX30105 Commands
// Interrupt configuration (pg 13, 14)
static const uint8_t MAX30105_INT_A_FULL_MASK =     (uint8_t)~0b10000000;
static const uint8_t MAX30105_INT_A_FULL_ENABLE =   0x80;
static const uint8_t MAX30105_INT_A_FULL_DISABLE =  0x00;

static const uint8_t MAX30105_INT_DATA_RDY_MASK = (uint8_t)~0b01000000;
static const uint8_t MAX30105_INT_DATA_RDY_ENABLE = 0x40;
static const uint8_t MAX30105_INT_DATA_RDY_DISABLE = 0x00;

static const uint8_t MAX30105_INT_ALC_OVF_MASK = (uint8_t)~0b00100000;
static const uint8_t MAX30105_INT_ALC_OVF_ENABLE =  0x20;
static const uint8_t MAX30105_INT_ALC_OVF_DISABLE = 0x00;

static const uint8_t MAX30105_INT_PROX_INT_MASK = (uint8_t)~0b00010000;
static const uint8_t MAX30105_INT_PROX_INT_ENABLE = 0x10;
static const uint8_t MAX30105_INT_PROX_INT_DISABLE = 0x00;

static const uint8_t MAX30105_INT_DIE_TEMP_RDY_MASK = (uint8_t)~0b00000010;
static const uint8_t MAX30105_INT_DIE_TEMP_RDY_ENABLE = 0x02;
static const uint8_t MAX30105_INT_DIE_TEMP_RDY_DISABLE = 0x00;

static const uint8_t MAX30105_SAMPLEAVG_MASK =  (uint8_t)~0b11100000;
static const uint8_t MAX30105_SAMPLEAVG_1 =     0x00;
static const uint8_t MAX30105_SAMPLEAVG_2 =     0x20;
static const uint8_t MAX30105_SAMPLEAVG_4 =     0x40;
static const uint8_t MAX30105_SAMPLEAVG_8 =     0x60;
static const uint8_t MAX30105_SAMPLEAVG_16 =    0x80;
static const uint8_t MAX30105_SAMPLEAVG_32 =    0xA0;

static const uint8_t MAX30105_ROLLOVER_MASK =   0xEF;
static const uint8_t MAX30105_ROLLOVER_ENABLE = 0x10;
static const uint8_t MAX30105_ROLLOVER_DISABLE = 0x00;

static const uint8_t MAX30105_A_FULL_MASK =     0xF0;

// Mode configuration commands (page 19)
static const uint8_t MAX30105_SHUTDOWN_MASK =   0x7F;
static const uint8_t MAX30105_SHUTDOWN =        0x80;
static const uint8_t MAX30105_WAKEUP =          0x00;

static const uint8_t MAX30105_RESET_MASK =      0xBF;
static const uint8_t MAX30105_RESET =           0x40;

static const uint8_t MAX30105_MODE_MASK =       0xF8;
static const uint8_t MAX30105_MODE_REDONLY =    0x02;
static const uint8_t MAX30105_MODE_REDIRONLY =  0x03;
static const uint8_t MAX30105_MODE_MULTILED =   0x07;

// Particle sensing configuration commands (pgs 19-20)
static const uint8_t MAX30105_ADCRANGE_MASK =   0x9F;
static const uint8_t MAX30105_ADCRANGE_2048 =   0x00;
static const uint8_t MAX30105_ADCRANGE_4096 =   0x20;
static const uint8_t MAX30105_ADCRANGE_8192 =   0x40;
static const uint8_t MAX30105_ADCRANGE_16384 =  0x60;

static const uint8_t MAX30105_SAMPLERATE_MASK = 0xE3;
static const uint8_t MAX30105_SAMPLERATE_50 =   0x00;
static const uint8_t MAX30105_SAMPLERATE_100 =  0x04;
static const uint8_t MAX30105_SAMPLERATE_200 =  0x08;
static const uint8_t MAX30105_SAMPLERATE_400 =  0x0C;
static const uint8_t MAX30105_SAMPLERATE_800 =  0x10;
static const uint8_t MAX30105_SAMPLERATE_1000 = 0x14;
static const uint8_t MAX30105_SAMPLERATE_1600 = 0x18;
static const uint8_t MAX30105_SAMPLERATE_3200 = 0x1C;

static const uint8_t MAX30105_PULSEWIDTH_MASK = 0xFC;
static const uint8_t MAX30105_PULSEWIDTH_69 =   0x00;
static const uint8_t MAX30105_PULSEWIDTH_118 =  0x01;
static const uint8_t MAX30105_PULSEWIDTH_215 =  0x02;
static const uint8_t MAX30105_PULSEWIDTH_411 =  0x03;

//Multi-LED Mode configuration (pg 22)
static const uint8_t MAX30105_SLOT1_MASK =      0xF8;
static const uint8_t MAX30105_SLOT2_MASK =      0x8F;
static const uint8_t MAX30105_SLOT3_MASK =      0xF8;
static const uint8_t MAX30105_SLOT4_MASK =      0x8F;

static const uint8_t SLOT_NONE =                0x00;
static const uint8_t SLOT_RED_LED =             0x01;
static const uint8_t SLOT_IR_LED =              0x02;
static const uint8_t SLOT_GREEN_LED =           0x03;
static const uint8_t SLOT_NONE_PILOT =          0x04;
static const uint8_t SLOT_RED_PILOT =           0x05;
static const uint8_t SLOT_IR_PILOT =            0x06;
static const uint8_t SLOT_GREEN_PILOT =         0x07;

static const uint8_t MAX_30105_EXPECTEDPARTID = 0x15;

static int RXData;

void writeRegister8(uint8_t reg, uint8_t value){
    //while(MAP_I2C_masterIsStartSent(EUSCI_B0_BASE));
    MAP_I2C_masterSendMultiByteStart(EUSCI_B0_BASE, reg);  // Start + 1Byte
    MAP_I2C_masterSendMultiByteNext(EUSCI_B0_BASE, value); // Poll for TXINT,Send 1Byte

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP;
    while (MAP_I2C_masterIsStopSent(EUSCI_B0_BASE));

}

uint8_t readRegister8(uint8_t reg){

    /* Making sure the last transaction has been completely sent out */
    while (MAP_I2C_masterIsStopSent(EUSCI_B0_BASE));

    /* Send out EEPROM Mock Read Cmd (2 databytes) */
    MAP_I2C_masterSendMultiByteStart(EUSCI_B0_BASE, reg);  // Start + 1Byte
    //MAP_I2C_masterSendMultiByteNext(EUSCI_B0_BASE, TXData[0]); // Poll for TXINT,Send 1Byte
    /*---------------------------------------------*/
    /* Now we need to initiate the read */
    /* Wait until 2nd Byte has been output to shift register */
    while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG0));

    // Send the restart condition, read one byte, send the stop condition right away
    EUSCI_B0->CTLW0 &= ~(EUSCI_B_CTLW0_TR);
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;
    while(MAP_I2C_masterIsStartSent(EUSCI_B0_BASE));
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP;
    while(!(EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG0));
    RXData = EUSCI_B0->RXBUF;

    return RXData;
}

void bitMask(uint8_t reg, uint8_t mask, uint8_t thing){

}


void MAX30102_Soft_Reset(void){

}

void MAX30102_Sampling(uint8_t sampleAverage){

}

void MAX30102_FIFO_Interrupt(uint8_t interruptOn){

}

void MAX30102_LED_Mode(uint8_t ledMode){

}

void MAX30102_ADC_Resolution(int adcRange){

}

void MAX30102_Sampling_Rate(uint8_t sampleRate){

}

void MAX30102_Pulse_Width(int pulseWidth){

}

void MAX30102_LED_Brightness(uint8_t ledBrightness){

}

void MAX30102_LED_Slots(uint8_t ledMode){

}

void MAX30102_Clear_FIFO(void){

}

void MAX30102_Setup(uint8_t ledBrightness, uint8_t sampleAverage, uint8_t ledMode,
                    uint8_t sampleRate, int pulseWidth, int adcRange){

    // Reset all previous configuration data
    MAX30102_Soft_Reset();

    // Set FIFO Averaging
    MAX30102_Sampling(sampleAverage);

    // Set Interrupt Condition
    // Optional
    MAX30102_FIFO_Interrupt(2);

    // Set LED Mode
    MAX30102_LED_Mode(ledMode);

    // Set ADC Resolution
    MAX30102_ADC_Resolution(adcRange);

    // Set Sampling Rate
    MAX30102_Sampling_Rate(sampleRate);

    // Set Pulse Width
    MAX30102_Pulse_Width(pulseWidth);

    // Set Brightness
    MAX30102_LED_Brightness(ledBrightness);

    // Enable Slots
    MAX30102_LED_Slots(ledMode);

    // Clear FIFO
    MAX30102_Clear_FIFO();
}

int MAX30102_Init(void){
    // Setup I2C at 400kHz
    // call setup with configuration options

    // LED Brightness
    // Between 0 and 255
    // 0 the LED is off, 255 it draws 50mA
    uint8_t ledBrightness = 60;

    // sampleAveraging
    // Options of 1, 2, 4, 8, 16, and 32
    uint8_t sampleAverage = 16;

    // LED Mode
    // Options: 1 = Red, 2 = Red + IR, 3 = Red + IR + Green
    uint8_t ledMode = 2;

    // Sample Rate
    // Options of 50, 100, 200, 400, 800, 1000, 1600, 3200
    uint8_t sampleRate = 100;

    // Pulse width
    // Options of 69, 118, 215, and 411
    // TODO look further into this
    int pulseWidth = 411;

    // ADC Range
    // Options of 2048, 4096, 8192, 16384
    int adcRange = 16382;

    MAX30102_Setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);

    return 0;
}
