# Self Monitor for Detecting Stress Levels

## Overview
A device for detecting and monitoring stress levels. This device consists of a TI MSP432P401R reading physiological indicators and communicating that information to an iOS application. The iOS application has a kNN model integrated in for classifying the stress and allowing for on device retraining.

This was developed as a senior design project at the University of Texas at Austin and supervised by Dr. Emily Porter. Our team was broken up into three subteams, Hardware, Embedded System, and Application. The Hardware team consisted of Harini Cherupalla and Neha Shah. The Embedded System team consisted of Matthew Barondeau and Sneha Pendharkar. The Application team consisted of Matt Davis and Angelos Koulouras.

### Hardware
Our hardware team developed three versions of PCBs, designed a power subsystem, and created a 3D printed enclosure that fits convienetly on the wrist. The PCB consisted of a MSP432P401R chip, CC2650 bluetooth module, power supply, and three sensors. The three sensors were the Grove GSR sensor which measures galvanic skin response, the MAX30102 Reflective Optical Sensor which measures oxygen saturation, and MMA8451 Accelerometer which tracks movement. The power supply we chose was a 1200mAh LiPo battery which outputs 3.7V. We place two of these batteries in series for a Voltage input of 7.4V. The 7.4V input is provided to an LM2937-3.3 which is a 3.3V LDO regulator from Texas Instruments.

#### Sensor Images

#### Power Subsystem

#### PCB Version 1

### Embedded System
Our embedded system team developed all the driver interfaces for the sensors to the MSP432P401R and performed integration testing with the application. The MAX30101 ROS and MMA8451 Accelerometer both communicated using an I2C interface while the Grove GSR sensor sent an analog voltage reading to the MSP432.

#### Bluetooth Characteristics

#### CSV Formatting

### Application

#### iOS Application Images

#### kNN Model information
