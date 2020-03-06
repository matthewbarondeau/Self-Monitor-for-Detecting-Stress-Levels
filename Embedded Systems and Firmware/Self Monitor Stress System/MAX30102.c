/*
 * MAX30102.c
 *
 *  Created on: Feb 14, 2020
 *      Author: Matthew Barondeau
 */

#include <stdint.h>
#include <stdbool.h>
#include "MAX30102.h"
#include "driverlib/MSP432P4xx/driverlib.h"

#define I2C_MODULE EUSCI_B0_BASE

sense_struct sense;

static uint8_t activeLEDs = 0;


void softReset(void){
    I2C_bitMask(I2C_MODULE, MAX30105_MODECONFIG, MAX30105_RESET_MASK, MAX30105_RESET);

    // Poll for bit to clear, reset is then complete
    // Timeout after 100ms
    unsigned long startTime = 0;
    while (startTime < 100)
    {
      uint8_t response = I2C_readRegister(I2C_MODULE, MAX30105_MODECONFIG);
      if ((response & MAX30105_RESET) == 0) break; //We're done!
      __delay_cycles(3000); //Let's not over burden the I2C bus
      startTime++;
    }
}

void setLEDMode(uint8_t ledMode){
    I2C_bitMask(I2C_MODULE, MAX30105_MODECONFIG, MAX30105_MODE_MASK, ledMode);
}

void setADCRange(int adcRange){
    I2C_bitMask(I2C_MODULE, MAX30105_PARTICLECONFIG, MAX30105_ADCRANGE_MASK, adcRange);
}

void setSampleRate(uint8_t sampleRate){
    I2C_bitMask(I2C_MODULE, MAX30105_PARTICLECONFIG, MAX30105_SAMPLERATE_MASK, sampleRate);
}

void setPulseWidth(int pulseWidth){
    I2C_bitMask(I2C_MODULE, MAX30105_PARTICLECONFIG, MAX30105_PULSEWIDTH_MASK, pulseWidth);
}

void setPulseAmplitudeRed(uint8_t amplitude){
    I2C_writeRegister(I2C_MODULE, MAX30105_LED1_PULSEAMP, amplitude);
}

void setPulseAmplitudeIR(uint8_t amplitude){
    I2C_writeRegister(I2C_MODULE, MAX30105_LED2_PULSEAMP, amplitude);
}

void setPulseAmplitudeGreen(uint8_t amplitude){
    I2C_writeRegister(I2C_MODULE, MAX30105_LED3_PULSEAMP, amplitude);
}

void setPulseAmplitudeProximity(uint8_t amplitude){
    I2C_writeRegister(I2C_MODULE, MAX30105_LED_PROX_AMP, amplitude);
}

void enableSlot(uint8_t slotNumber, uint8_t device){
    switch (slotNumber) {
      case (1):
        I2C_bitMask(I2C_MODULE, MAX30105_MULTILEDCONFIG1, MAX30105_SLOT1_MASK, device);
        break;
      case (2):
        I2C_bitMask(I2C_MODULE, MAX30105_MULTILEDCONFIG1, MAX30105_SLOT2_MASK, device << 4);
        break;
      case (3):
        I2C_bitMask(I2C_MODULE, MAX30105_MULTILEDCONFIG2, MAX30105_SLOT3_MASK, device);
        break;
      case (4):
        I2C_bitMask(I2C_MODULE, MAX30105_MULTILEDCONFIG2, MAX30105_SLOT4_MASK, device << 4);
        break;
      default:
        //Shouldn't be here!
        break;
    }
}

void disableSlots(void){
    I2C_writeRegister(I2C_MODULE, MAX30105_MULTILEDCONFIG1, 0);
    I2C_writeRegister(I2C_MODULE, MAX30105_MULTILEDCONFIG2, 0);
}

void setFIFOAverage(uint8_t sampleAverage){
    I2C_bitMask(I2C_MODULE, MAX30105_FIFOCONFIG, MAX30105_SAMPLEAVG_MASK, sampleAverage);
}

void clearFIFO(void){
    I2C_writeRegister(I2C_MODULE, MAX30105_FIFOWRITEPTR, 0);
    I2C_writeRegister(I2C_MODULE, MAX30105_FIFOOVERFLOW, 0);
    I2C_writeRegister(I2C_MODULE, MAX30105_FIFOREADPTR, 0);
}

void enableFIFORollover(void){
    I2C_bitMask(I2C_MODULE, MAX30105_FIFOCONFIG, MAX30105_ROLLOVER_MASK, MAX30105_ROLLOVER_ENABLE);
}

void disableFIFORollover(void){
    I2C_bitMask(I2C_MODULE, MAX30105_FIFOCONFIG, MAX30105_ROLLOVER_MASK, MAX30105_ROLLOVER_DISABLE);
}

uint8_t getWritePointer(void){
    return (I2C_readRegister(I2C_MODULE, MAX30105_FIFOWRITEPTR));
}

uint8_t getReadPointer(void){
    return (I2C_readRegister(I2C_MODULE, MAX30105_FIFOREADPTR));
}

// MAX30102_setup
// calls all initialization routines for MAX30102
void MAX30102_setup(uint8_t powerLevel, uint8_t sampleAverage, uint8_t ledMode,
                    uint8_t sampleRate, int pulseWidth, int adcRange){

    softReset(); //Reset all configuration, threshold, and data registers to POR values

    //FIFO Configuration
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    //The chip will average multiple samples of same type together if you wish
    if (sampleAverage == 1) setFIFOAverage(MAX30105_SAMPLEAVG_1); //No averaging per FIFO record
    else if (sampleAverage == 2) setFIFOAverage(MAX30105_SAMPLEAVG_2);
    else if (sampleAverage == 4) setFIFOAverage(MAX30105_SAMPLEAVG_4);
    else if (sampleAverage == 8) setFIFOAverage(MAX30105_SAMPLEAVG_8);
    else if (sampleAverage == 16) setFIFOAverage(MAX30105_SAMPLEAVG_16);
    else if (sampleAverage == 32) setFIFOAverage(MAX30105_SAMPLEAVG_32);
    else setFIFOAverage(MAX30105_SAMPLEAVG_4);

    //setFIFOAlmostFull(2); //Set to 30 samples to trigger an 'Almost Full' interrupt
    enableFIFORollover(); //Allow FIFO to wrap/roll over
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    //Mode Configuration
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    if (ledMode == 3) setLEDMode(MAX30105_MODE_MULTILED); //Watch all three LED channels
    else if (ledMode == 2) setLEDMode(MAX30105_MODE_REDIRONLY); //Red and IR
    else setLEDMode(MAX30105_MODE_REDONLY); //Red only
    activeLEDs = ledMode; //Used to control how many bytes to read from FIFO buffer
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    //Particle Sensing Configuration
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    if(adcRange < 4096) setADCRange(MAX30105_ADCRANGE_2048); //7.81pA per LSB
    else if(adcRange < 8192) setADCRange(MAX30105_ADCRANGE_4096); //15.63pA per LSB
    else if(adcRange < 16384) setADCRange(MAX30105_ADCRANGE_8192); //31.25pA per LSB
    else if(adcRange == 16384) setADCRange(MAX30105_ADCRANGE_16384); //62.5pA per LSB
    else setADCRange(MAX30105_ADCRANGE_2048);

    if (sampleRate < 100) setSampleRate(MAX30105_SAMPLERATE_50); //Take 50 samples per second
    else if (sampleRate < 200) setSampleRate(MAX30105_SAMPLERATE_100);
    else if (sampleRate < 400) setSampleRate(MAX30105_SAMPLERATE_200);
    else if (sampleRate < 800) setSampleRate(MAX30105_SAMPLERATE_400);
    else if (sampleRate < 1000) setSampleRate(MAX30105_SAMPLERATE_800);
    else if (sampleRate < 1600) setSampleRate(MAX30105_SAMPLERATE_1000);
    else if (sampleRate < 3200) setSampleRate(MAX30105_SAMPLERATE_1600);
    else if (sampleRate == 3200) setSampleRate(MAX30105_SAMPLERATE_3200);
    else setSampleRate(MAX30105_SAMPLERATE_50);

    //The longer the pulse width the longer range of detection you'll have
    //At 69us and 0.4mA it's about 2 inches
    //At 411us and 0.4mA it's about 6 inches
    if (pulseWidth < 118) setPulseWidth(MAX30105_PULSEWIDTH_69); //Page 26, Gets us 15 bit resolution
    else if (pulseWidth < 215) setPulseWidth(MAX30105_PULSEWIDTH_118); //16 bit resolution
    else if (pulseWidth < 411) setPulseWidth(MAX30105_PULSEWIDTH_215); //17 bit resolution
    else if (pulseWidth == 411) setPulseWidth(MAX30105_PULSEWIDTH_411); //18 bit resolution
    else setPulseWidth(MAX30105_PULSEWIDTH_69);
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    //LED Pulse Amplitude Configuration
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    //Default is 0x1F which gets us 6.4mA
    //powerLevel = 0x02, 0.4mA - Presence detection of ~4 inch
    //powerLevel = 0x1F, 6.4mA - Presence detection of ~8 inch
    //powerLevel = 0x7F, 25.4mA - Presence detection of ~8 inch
    //powerLevel = 0xFF, 50.0mA - Presence detection of ~12 inch

    setPulseAmplitudeRed(powerLevel);
    setPulseAmplitudeIR(powerLevel);
    setPulseAmplitudeGreen(powerLevel);
    setPulseAmplitudeProximity(powerLevel);
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    //Multi-LED Mode Configuration, Enable the reading of the three LEDs
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    enableSlot(1, SLOT_RED_LED);
    if (ledMode > 1) enableSlot(2, SLOT_IR_LED);
    if (ledMode > 2) enableSlot(3, SLOT_GREEN_LED);
    //enableSlot(1, SLOT_RED_PILOT);
    //enableSlot(2, SLOT_IR_PILOT);
    //enableSlot(3, SLOT_GREEN_PILOT);
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    clearFIFO(); //Reset the FIFO before we begin checking the sensor
}

// MAX30102_init
// Calls setup
int MAX30102_init(void){
    // Setup I2C at 400kHz
    // call setup with configuration options

    // LED Brightness
    // Between 0 and 255
    // 0 the LED is off, 255 it draws 50mA
    uint8_t ledBrightness = 0x1F;

    // sampleAveraging
    // Options of 1, 2, 4, 8, 16, and 32
    uint8_t sampleAverage = 4;

    // LED Mode
    // Options: 1 = Red, 2 = Red + IR, 3 = Red + IR + Green
    uint8_t ledMode = 2;

    // Sample Rate
    // Options of 50, 100, 200, 400, 800, 1000, 1600, 3200
    uint16_t sampleRate = 400;

    // Pulse width
    // Options of 69, 118, 215, and 411
    // TODO look further into this
    int pulseWidth = 411;

    // ADC Range
    // Options of 2048, 4096, 8192, 16384
    int adcRange = 4096;

    MAX30102_setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);

    return 0;
}

// MAX30102_available
// determine how many samples are available
uint8_t MAX30102_available(void){
    int8_t numberOfSamples = sense.head - sense.tail;
    if(numberOfSamples < 0){
        numberOfSamples += STORAGE_SIZE;
    }
    return(numberOfSamples);
}

// MAX30102_get_red
// Returns next red led value
uint32_t MAX30102_get_red(void){
    return sense.red[sense.tail];
}

// MAX30102_get_ir
// returns next ir led value
uint32_t MAX30102_get_ir(void){
    return sense.IR[sense.tail];
}

// MAX30102_next_sample
// updates tail pointers for sense
void MAX30102_next_sample(void){
    if(MAX30102_available()){
        sense.tail++;
        sense.tail %= STORAGE_SIZE;
    }
}

// MAX30102_check_device
// gets new values from MAX30102 and stores values in sense
uint16_t MAX30102_check_device(void){

    // Start
    // send address + write mode
    I2C_masterSendMultiByteStart(EUSCI_B1_BASE, MAX30105_FIFODATA);
    while(!(EUSCI_B1->IFG & EUSCI_B_IFG_TXIFG0));

    // repeated start
    I2C_masterReceiveStart(EUSCI_B1_BASE);

    // update fifo pointer
    sense.head++;
    sense.head &= 3;

    uint8_t temp[4];
    uint32_t tempLong;

    temp[3] = 0;
    while(!(EUSCI_B1->IFG & EUSCI_B_IFG_RXIFG0));
    temp[2] = I2C_slaveGetData(EUSCI_B1_BASE);
    while(!(EUSCI_B1->IFG & EUSCI_B_IFG_RXIFG0));
    temp[1] = I2C_slaveGetData(EUSCI_B1_BASE);
    while(!(EUSCI_B1->IFG & EUSCI_B_IFG_RXIFG0));
    temp[0] = I2C_slaveGetData(EUSCI_B1_BASE);

    memcpy(&tempLong, temp, sizeof(tempLong));

    tempLong &= 0x3FFFF;

    sense.red[sense.head] = tempLong;

    if(activeLEDs > 1){
        temp[3] = 0;
        while(!(EUSCI_B1->IFG & EUSCI_B_IFG_RXIFG0));
        temp[2] = I2C_slaveGetData(EUSCI_B1_BASE);
        while(!(EUSCI_B1->IFG & EUSCI_B_IFG_RXIFG0));
        temp[1] = I2C_slaveGetData(EUSCI_B1_BASE);
        I2C_masterReceiveMultiByteStop(EUSCI_B1_BASE);
        __delay_cycles(10000);
        while(!(EUSCI_B1->IFG & EUSCI_B_IFG_RXIFG0));
        temp[0] = I2C_slaveGetData(EUSCI_B1_BASE);

        memcpy(&tempLong, temp, sizeof(tempLong));

        tempLong &= 0x3FFFF;

        sense.IR[sense.head] = tempLong;
    }
    while(EUSCI_B1->IFG & EUSCI_B_IFG_RXIFG0){
        int foo = I2C_slaveGetData(EUSCI_B1_BASE);
    }
    __delay_cycles(3000);
    return 1;
}
