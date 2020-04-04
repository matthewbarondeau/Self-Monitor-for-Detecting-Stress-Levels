/*
 * Accelerometer.h
 *
 *  Created on: Jan 19, 2020
 *      Author: Matthew Barondeau
 *  Changed on: Feb 29, 2020
 */

#ifndef ACC_H_
#define ACC_H_

#include <stdint.h>

#define MMA8451_DEFAULT_ADDRESS                 (0x1C)

#define MMA8451_REG_OUT_X_MSB     0x01
#define MMA8451_REG_SYSMOD        0x0B
#define MMA8451_REG_WHOAMI        0x0D
#define MMA8451_REG_XYZ_DATA_CFG  0x0E
#define MMA8451_REG_PL_STATUS     0x10
#define MMA8451_REG_PL_CFG        0x11
#define MMA8451_REG_CTRL_REG1     0x2A
#define MMA8451_REG_CTRL_REG2     0x2B
#define MMA8451_REG_CTRL_REG4     0x2D
#define MMA8451_REG_CTRL_REG5     0x2E
#define F_SETUP 0x09

#define MMA8451_PL_PUF            0
#define MMA8451_PL_PUB            1
#define MMA8451_PL_PDF            2
#define MMA8451_PL_PDB            3
#define MMA8451_PL_LRF            4
#define MMA8451_PL_LRB            5
#define MMA8451_PL_LLF            6
#define MMA8451_PL_LLB            7

typedef enum
{
  MMA8451_RANGE_8_G           = 0b10,   // +/- 8g
  MMA8451_RANGE_4_G           = 0b01,   // +/- 4g
  MMA8451_RANGE_2_G           = 0b00    // +/- 2g (default value)
} mma8451_range_t;


/* Used with register 0x2A (MMA8451_REG_CTRL_REG1) to set bandwidth */
typedef enum
{
  MMA8451_DATARATE_800_HZ     = 0b000, //  800Hz
  MMA8451_DATARATE_400_HZ     = 0b001, //  400Hz
  MMA8451_DATARATE_200_HZ     = 0b010, //  200Hz
  MMA8451_DATARATE_100_HZ     = 0b011, //  100Hz
  MMA8451_DATARATE_50_HZ      = 0b100, //   50Hz
  MMA8451_DATARATE_12_5_HZ    = 0b101, // 12.5Hz
  MMA8451_DATARATE_6_25HZ     = 0b110, // 6.25Hz
  MMA8451_DATARATE_1_56_HZ    = 0b111, // 1.56Hz

  MMA8451_DATARATE_MASK       = 0b111
} mma8451_dataRate_t;

// ACC_init
// Configure MMA8451 on I2C EUSCI_B1
void ACC_init(void);

// ACC_read_data
// Reads 6 bytes from MMA8451 using repeated start I2C
// Calculates acceleration using 6 bytes
void ACC_read_data(void);

// ACC_get_x
// returns x acceleration value
int16_t ACC_get_x(void);

// ACC_get_y
// returns y acceleration value
int16_t ACC_get_y(void);

// ACC_get_z
// returns z acceleration value
int16_t ACC_get_z(void);

#endif /* ACC_H_ */
