# Self Monitor for Detecting Stress Levels

## Overview
A device for detecting and monitoring stress levels. This device consists of a TI MSP432P401R reading physiological indicators and communicating that information to an iOS application. The iOS application has a kNN model integrated in for classifying the stress and allowing for on device retraining.

This was developed as a senior design project at the University of Texas at Austin and supervised by Dr. Emily Porter. Our team was broken up into three subteams, Hardware, Embedded System, and Application. The Hardware team consisted of Harini Cherupalla and Neha Shah. The Embedded System team consisted of Matthew Barondeau and Sneha Pendharkar. The Application team consisted of Matt Davis and Angelos Koulouras.

Shown Below is the system block diagram of this project.

<p align="center">
  <img src="/Images/SystemBlockDiagram.png"
</p>

### Hardware
Our hardware team developed three versions of PCBs, designed a power subsystem, and created a 3D printed enclosure that fits convienetly on the wrist. The PCB consisted of a MSP432P401R chip, CC2650 bluetooth module, power supply, and three sensors. The three sensors were the Grove GSR sensor which measures galvanic skin response, the MAX30102 Reflective Optical Sensor which measures oxygen saturation, and MMA8451 Accelerometer which tracks movement. The power supply we chose was a 1200mAh LiPo battery which outputs 3.7V. We place two of these batteries in series for a Voltage input of 7.4V. The 7.4V input is provided to an LM2937-3.3 which is a 3.3V LDO regulator from Texas Instruments. More information about these subsystems can be found in the Hardware Directory

#### Sensors
This project made use of three existing hardware sensors. These sensors are the MAX30102 Reflective Optical Sensor, Grove Galvanic Skin Response sensor, and MMA8451 Accelerometer. The MAX30102 is placed at the wrist where it measures reflected infrared light corresponding to oxygen saturation. The Grove GSR sensor works by applying a voltage across a voltage divider. The divider consists of a tunable reference resistor nominally set to 160kΩ and two electrodes connected to the user’s fingertips. The resistance across the fingers will change as the user becomes more or less stressed, which in turn will modify the voltage output of the divider. The accelerometer is placed at the wrist and communicates the status of its triple axis accelerometer to the microcontroller. These three sensors are shown below in the order mentioned.

<p align="center">
  <img src="/Images/Sensors.png"
</p>

#### Power Subsystem
As part of our system design, we developed and tested a power subsystem for the MSP432P401R PCB layout. This power subsystem consisted of an LM2937-3.3 LDO regulator from texas instruments and a LiPo 1200mAh battery. The LiPo battery nominally outputs 3.7V, so we place two of these batteries in series for our design. The 7.4V input perfectly generates the 3.3V output as shown in an oscilloscope capture below.

<p align="center">
  <img src="/Images/PowerSupply.png"
</p>

#### Printed Circuit Boards
To develop our system, our team designed a Printed Circuit Board and placed all of our system components on the board. We began placing the components using a reflow oven and were able to flash the MSP432 chip but unable to test the bluetooth chip before the lab closed for the semester. The versions of our PCBs can be found in the Hardware section of the design, but the surface mount version is shown below.

<p align="center">
  <img src="/Images/PCB.png"
</p>

#### Packaging
In order to place all components in a convenient position and ensure minimal intrusivness to the user, our team design a package for our system. This package consists of glove with the GSR sensor sewn into the lining. A 3D printed enclosure was designed for our PCB and placed at the wrist. The glove and PCB placement are shown below.

<p align="center">
  <img src="/Images/Glove.png"
</p>

### Embedded System
Our embedded system team developed all the driver interfaces for the sensors to the MSP432P401R and performed integration testing with the application. The MAX30101 ROS and MMA8451 Accelerometer both communicated using an I2C interface while the Grove GSR sensor sent an analog voltage reading to the MSP432.

<p align="center">
  <img src="/Images/EmbeddedDiagram.png"
</p>

### Application
The mobile application and machine learning model were developed in a different repository and linked as a submodule under PowerPorterIOS. It may also be found here: https://github.com/mattqd97/PowerPorterIOS
