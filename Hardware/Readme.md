# Hardware

## Overview
This directory contains all the hardware design files for our Senior Design project. There were three main interations in this project. They were designed in EasyEDA due to their library already containing all components we needed.

## Subdirectories

### SMSSv0
This directory contains the original board design for our project. We did not order this board, but used it as a starting point for the v1. The board contains a grove GSR sensor, MAX30102 breakout board, and a filter chain for a Giant Magneto Resistor(GMR). We later removed the GMR due do reliability issues.

### SMSSv1
This directory contains the first version of the board we ordered and began assembling. This version contains a GSR and MAX30102 sensor just as v0, but now has a MMA8451 accelerometer as well.

In addition to the sensors, we layout the MSP432P401R chip with accompanying oscillators and capacitors. In this version, data is transmitted from the MSP432 to the user through bluetooth. This is accomplished through the CC2650MODA bluetooth module. This module is labeled as U14.

Finally, there is an LM2937-3.3V regulator that receives power from the USB header. The USB header is labeled as X4 and the regulator as U7. This design was tested and it does receive power properly, but our team was not able to finish soldering the bluetooth module or perform in depth testing.

### SMSSvHeaders
This directory contains the final versions of our hardware. During our final semester, the undergraduate labs were closed, so we moved to a form of PCB that was extremely easy to solder. This verion, called SMSSvHeaders, uses the boosterpack approach for the MSP432P401R Launchpad. There are 2 20 pin headers that connect to the underside of the launchpad. This headers ensure a solid connection to the sensors. The power, bluetooth, and manual layout of the GSR sensor have been removed in this version.

Inside this folder, you can find two versions: one with the headers added to the board and all other components remaining, and one with just the headers and relevant sensors. 
