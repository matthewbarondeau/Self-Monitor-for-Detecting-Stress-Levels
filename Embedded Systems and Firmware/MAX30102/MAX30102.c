/*
 * MAX30102.c
 *
 *  Created on: Feb 14, 2020
 *      Author: Matthew Barondeau
 */


/*
 * MAX30102.c
 *
 *  Created on: Jan 29, 2020
 *      Author: Matthew Barondeau
 */

#include <i2cMAX30102.h>
#include <stdint.h>
#include <stdbool.h>
#include "MAX30102.h"
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

/*
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
*/
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

static const uint8_t MAX_30105_EXPECTEDPARTID = 0x15;

static uint8_t activeLEDs = 0;


void softReset(void){
    bitMask(MAX30105_MODECONFIG, MAX30105_RESET_MASK, MAX30105_RESET);

    // Poll for bit to clear, reset is then complete
    // Timeout after 100ms
    unsigned long startTime = 0;
    while (startTime < 100)
    {
      uint8_t response = readRegister8(MAX30105_MODECONFIG);
      if ((response & MAX30105_RESET) == 0) break; //We're done!
      __delay_cycles(48000); //Let's not over burden the I2C bus
      startTime++;
    }
}

void setLEDMode(uint8_t ledMode){
    bitMask(MAX30105_MODECONFIG, MAX30105_MODE_MASK, ledMode);
}

void setADCRange(int adcRange){
    bitMask(MAX30105_PARTICLECONFIG, MAX30105_ADCRANGE_MASK, adcRange);
}

void setSampleRate(uint8_t sampleRate){
    bitMask(MAX30105_PARTICLECONFIG, MAX30105_SAMPLERATE_MASK, sampleRate);
}

void setPulseWidth(int pulseWidth){
    bitMask(MAX30105_PARTICLECONFIG, MAX30105_PULSEWIDTH_MASK, pulseWidth);
}

void setPulseAmplitudeRed(uint8_t amplitude){
    writeRegister8(MAX30105_LED1_PULSEAMP, amplitude);
}

void setPulseAmplitudeIR(uint8_t amplitude){
    writeRegister8(MAX30105_LED2_PULSEAMP, amplitude);
}

void setPulseAmplitudeGreen(uint8_t amplitude){
    writeRegister8(MAX30105_LED3_PULSEAMP, amplitude);
}

void setPulseAmplitudeProximity(uint8_t amplitude){
    writeRegister8(MAX30105_LED_PROX_AMP, amplitude);
}


void enableSlot(uint8_t slotNumber, uint8_t device){
    switch (slotNumber) {
      case (1):
        bitMask(MAX30105_MULTILEDCONFIG1, MAX30105_SLOT1_MASK, device);
        break;
      case (2):
        bitMask(MAX30105_MULTILEDCONFIG1, MAX30105_SLOT2_MASK, device << 4);
        break;
      case (3):
        bitMask(MAX30105_MULTILEDCONFIG2, MAX30105_SLOT3_MASK, device);
        break;
      case (4):
        bitMask(MAX30105_MULTILEDCONFIG2, MAX30105_SLOT4_MASK, device << 4);
        break;
      default:
        //Shouldn't be here!
        break;
    }
}

void disableSlots(void){
    writeRegister8(MAX30105_MULTILEDCONFIG1, 0);
    writeRegister8(MAX30105_MULTILEDCONFIG2, 0);
}

/*
 * FIFO Configurations
 */

void setFIFOAverage(uint8_t sampleAverage){
    bitMask(MAX30105_FIFOCONFIG, MAX30105_SAMPLEAVG_MASK, sampleAverage);
}

void clearFIFO(void){
    writeRegister8(MAX30105_FIFOWRITEPTR, 0);
    writeRegister8(MAX30105_FIFOOVERFLOW, 0);
    writeRegister8(MAX30105_FIFOREADPTR, 0);
}

void enableFIFORollover(void){
    bitMask(MAX30105_FIFOCONFIG, MAX30105_ROLLOVER_MASK, MAX30105_ROLLOVER_ENABLE);
}

void disableFIFORollover(void){
    bitMask(MAX30105_FIFOCONFIG, MAX30105_ROLLOVER_MASK, MAX30105_ROLLOVER_DISABLE);
}

uint8_t getWritePointer(void){
    return (readRegister8(MAX30105_FIFOWRITEPTR));
}

uint8_t getReadPointer(void){
    return (readRegister8(MAX30105_FIFOREADPTR));
}

/*
 * Setup Routines
 */

void MAX30102_Setup(uint8_t powerLevel, uint8_t sampleAverage, uint8_t ledMode,
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

/*
 * Data Collection
 */

uint8_t MAX30102_available(void){
    int8_t numberOfSamples = sense.head - sense.tail;
}
