# DevelopmentCode

Development code for all the modules of the logger, including processing on the Pi.

Each folder contains one component.

## To build a logger:

- upload the power controller and put it on the PCB.

Note: check the values in the parameters.h. In particular for production (no debugs or similar, and update the solar panel each 40s, possibility for main
instrument wakeup each 20 minutes):

- DEBUG should be false
- SHOW_LED should be false
- CYCLES\_DEEP\_SLEEP should be 5
- CYCLES\_BEFORE\_MEGA\_WAKEUP should be 30

