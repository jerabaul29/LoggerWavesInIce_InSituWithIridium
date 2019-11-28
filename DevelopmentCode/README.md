# DevelopmentCode

Development code for all the modules of the logger, including processing on the Pi.

Each folder contains one component.

The Arduino stuff was developped in Visual Studio Code with the PlatformIO IDE; this may explain for some specificities in the folder structures.

- **ArduinoMega**: the code on the Arduino Mega on the center PCB; effectively, the logger and interface to all other components.

- **PowerControl**: the power controller, ie controlling the solar panel, battery, and wakeup planning.

- **RPI_side**: the stuff on the RPI. An image of the RPi image ready for use is available here: https://folk.uio.no/jeanra/Research/Research/img_0208_Rpi_2Bv11.img .

- The rest is tests, help, etc.

# Old ------------

## To build a logger:

- upload the power controller and put it on the PCB.

Note: check the values in the parameters.h. In particular for production (no debugs or similar, and update the solar panel each 40s, possibility for main
instrument wakeup each 20 minutes):

- DEBUG should be false
- SHOW_LED should be false
- CYCLES\_DEEP\_SLEEP should be 5
- CYCLES\_BEFORE\_MEGA\_WAKEUP should be 30

