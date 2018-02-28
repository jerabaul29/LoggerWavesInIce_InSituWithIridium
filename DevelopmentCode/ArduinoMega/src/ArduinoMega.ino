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

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// PRELIMINARY
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// include libraries
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM9DS0.h>
#include <stdlib.h>

#include "parameters.h"
#include "GPS_controller.h"
#include "EEPROM_interaction.h"
#include "SDManager.h"
#include "board_manager.h"
#include "Iridium_manager.h"

// global definitions; do not initialize, just define to make them global

// board manager
BoardManager board_manager{};

// SD
SDManager sd_manager{};

// GPS
GPSController gps_controller{&SERIAL_GPS, &sd_manager};

// Iridium
IridiumManager iridium_manager{&SERIAL_IRIDIUM, &gps_controller, &board_manager, &sd_manager};


// TODO: class for the VN100
// TODO: class for interaction with Iridium
// TODO: class for interaction with RaspberryPi
// TODO: not urgent: class for the LSM9DS0
// TODO: fix all names: name cpp file and header should be the same as class they contain

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// SETUP LOOP
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

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
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// MAIN LOOP
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void loop(){

  wdt_reset();

  
  /*
  // when done logging: close logging file
  dataFile.close();

  wdt_reset();

  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // VITAL INFORMATION IRIDIUM
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  // send some diagnostic strings by Iridum, including detailed GPS position.
  // done here as doing it at this point one most likely has a GPS fix and full
  // Iridium capacitors
  #if USE_IRIDIUM
    send_receive_Iridium_vital_information();
  #endif

  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // RASPBERRY PI INTERACTION
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  #if SERIAL_RPI
    raspberry_pi_interaction();
  #endif

  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // EVERYTHING IS DONE: REBOOT
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  // if it is time to sleep, this will be caught by the decide_if_wakeup function
  wdt_enable(WDTO_15MS);
  while(1){
  }
  */
}