// TODO: attach a serial port for debugging
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
#include "VN100Manager.h"

// board manager
BoardManager board_manager{};

// SD
SDManager sd_manager{};

// VN100
VN100Manager vn100_manager{&SERIAL_VN100, &sd_manager};

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

  // make Iridium ready
  iridium_manager.start();
  wdt_reset();

  // make GPS ready
  gps_controller.start();
  wdt_reset();

  // make VN100 ready
  vn100_manager.start();
  wdt_reset();

  // raspberry Pi

  // start logging!
  board_manager.start_logging(DURATION_LOGGING_MS);
}

void loop(){

  wdt_reset();

  // decide which step in the process at
  int board_status = board_manager.check_status();

  switch(board_status){
    case BOARD_LOGGING:
      vn100_manager.perform_logging();
      gps_controller.perform_logging();
      break;
    case BOARD_DONE_LOGGING:
      // close SD card
      sd_manager.close_datafile();
      // go through Iridium vital messages
      // go through Raspberry Pi interaction
      // go through send data by Iridium
      // ask to be put off
      board_manager.ask_to_be_off();
      // put to deep sleep: TODO: implement in board_manager
      break;
  }

  // act in consequence.
}