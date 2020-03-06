/*
 * MAX30102.h
 *
 *  Created on: Feb 14, 2020
 *      Author: Matthew Barondeau
 */

#ifndef MAX30102_H_
#define MAX30102_H_
// FIFO Registers
#define MAX30105_FIFOWRITEPTR   0x04
#define MAX30105_FIFOOVERFLOW   0x05
#define MAX30105_FIFOREADPTR    0x06
#define MAX30105_FIFODATA       0x07

// Configuration Registers
#define MAX30105_FIFOCONFIG         0x08
#define MAX30105_MODECONFIG         0x09
#define MAX30105_PARTICLECONFIG     0x0A
#define MAX30105_LED1_PULSEAMP      0x0C
#define MAX30105_LED2_PULSEAMP      0x0D
#define MAX30105_LED3_PULSEAMP      0x0E
#define MAX30105_LED_PROX_AMP       0x10
#define MAX30105_MULTILEDCONFIG1    0x11
#define MAX30105_MULTILEDCONFIG2    0x12

// MAX30105 Commands
#define MAX30105_SAMPLEAVG_MASK 0xE0
#define MAX30105_SAMPLEAVG_1    0x00
#define MAX30105_SAMPLEAVG_2    0x20
#define MAX30105_SAMPLEAVG_4    0x40
#define MAX30105_SAMPLEAVG_8    0x60
#define MAX30105_SAMPLEAVG_16   0x80
#define MAX30105_SAMPLEAVG_32   0xA0

#define MAX30105_ROLLOVER_MASK      0xEF
#define MAX30105_ROLLOVER_ENABLE    0x10
#define MAX30105_ROLLOVER_DISABLE   0x00

#define MAX30105_RESET_MASK         0xBF
#define MAX30105_RESET              0x40

#define MAX30105_MODE_MASK          0xF8
#define MAX30105_MODE_REDONLY       0x02
#define MAX30105_MODE_REDIRONLY     0x03
#define MAX30105_MODE_MULTILED      0x07

// Particle sensing configuration commands (pgs 19-20)
#define MAX30105_ADCRANGE_MASK      0x9F
#define MAX30105_ADCRANGE_2048      0x00
#define MAX30105_ADCRANGE_4096      0x20
#define MAX30105_ADCRANGE_8192      0x40
#define MAX30105_ADCRANGE_16384     0x60

#define MAX30105_SAMPLERATE_MASK    0xE3
#define MAX30105_SAMPLERATE_50      0x00
#define MAX30105_SAMPLERATE_100     0x04
#define MAX30105_SAMPLERATE_200     0x08
#define MAX30105_SAMPLERATE_400     0x0C
#define MAX30105_SAMPLERATE_800     0x10
#define MAX30105_SAMPLERATE_1000    0x14
#define MAX30105_SAMPLERATE_1600    0x18
#define MAX30105_SAMPLERATE_3200    0x1C

#define MAX30105_PULSEWIDTH_MASK    0xFC
#define MAX30105_PULSEWIDTH_69      0x00
#define MAX30105_PULSEWIDTH_118     0x01
#define MAX30105_PULSEWIDTH_215     0x02
#define MAX30105_PULSEWIDTH_411     0x03

//Multi-LED Mode configuration (pg 22)
#define MAX30105_SLOT1_MASK     0xF8
#define MAX30105_SLOT2_MASK     0x8F
#define MAX30105_SLOT3_MASK     0xF8
#define MAX30105_SLOT4_MASK     0x8F

#define SLOT_RED_LED    0x01
#define SLOT_IR_LED     0x02
#define SLOT_GREEN_LED  0x03

void setPulseAmplitudeRed(uint8_t amplitude);

int MAX30102_init(void);

uint8_t MAX30102_available(void);

uint32_t MAX30102_get_ir(void);

uint32_t MAX30102_get_red(void);

void MAX30102_next_sample(void);

uint16_t MAX30102_check_device(void);

#define STORAGE_SIZE 32
typedef struct Record
{
  uint32_t red[STORAGE_SIZE];
  uint32_t IR[STORAGE_SIZE];
  uint32_t green[STORAGE_SIZE];
  uint8_t head;
  uint8_t tail;
} sense_struct; //This is our circular buffer of readings from the sensor

sense_struct sense;

#endif /* MAX30102_H_ */
