// TODO: interaction with Raspberry Pi
// TODO: Arduino board manager (sleeps, wake up, feedback)
// TODO: prepare VN100 IMU
// TODO: Iridium manager
// TODO: RaspberryPi manager
// TODO: to reduce energy consumption, make the board sleep if nothing to log
// TODO: take a look at the watchdogs
// TODO: check the ISBD callback routine wrt watchdog
// TODO: could use struct(char *, str_length) instead of plain char *. I do not want to use strings
// as dynamically allocated, ok on computers, bad on mC.

#include "parameters.h"
#include "GPS_controller.h"
#include "EEPROM_interaction.h"
#include "SDManager.h"
#include "board_manager.h"
#include "Iridium_manager.h"

// board manager
BoardManager board_manager{};

// SD
SDManager sd_manager{};

// GPS
GPSController gps_controller{&SERIAL_GPS, &sd_manager};

// Iridium
IridiumManager iridium_manager{&SERIAL_IRIDIUM, &gps_controller, &board_manager, &sd_manager};


// TODO: class for the VN100
// TODO: class for interaction with RaspberryPi
// TODO: not urgent: class for the LSM9DS0
// TODO: fix all names: name cpp file and header should be the same as class they contain

void setup(){

  // manage the board: decide if should wake up
 board_manager.start();
 wdt_reset();

  // make SD card ready
  sd_manager.start_sd();
  wdt_reset();

  // make GPS ready
  gps_controller.start();
  wdt_reset();

  // make Iridium ready
  iridium_manager.start();
  wdt_reset();

  // VN100

  // raspberry Pi
}

void loop(){

  wdt_reset();

  // decide which step in the process at: board manager?

  // act in consequence.
}