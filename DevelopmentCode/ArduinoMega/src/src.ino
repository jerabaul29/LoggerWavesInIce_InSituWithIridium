// TODO: Avoid #define for constant
// TODO: function macros use () around
// TODO: rationalize debug messages
// TODO: macro define a debugprint and debugprintln function
// TODO: implement RPi interface
// TODO: got through all delays and clean / remove them
// TODO: add information about board status (battery) in SD log files
// go through watchdogs

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

// TODO: could put the VN100, GPS, SD and Iridium on a MOSFET (physical change to board)

// NOTE: cut the capacitor that prevents reboot when RPi opens serial

#include "parameters.h"
#include "GPSManager.h"
#include "EEPROM_interaction.h"
#include "SDManager.h"
#include "BoardManager.h"
#include "IridiumManager.h"
#include "VN100Manager.h"
#include "RaspberryManager.h"

// board manager
BoardManager board_manager{};

// SD
SDManager sd_manager{};

// VN100
VN100Manager vn100_manager{&SERIAL_VN100, &sd_manager};

// GPS
GPSManager gps_controller{&SERIAL_GPS, &sd_manager};

// Iridium
IridiumManager iridium_manager{&SERIAL_IRIDIUM, &gps_controller, &board_manager, &sd_manager};

// RPi
RaspberryManager raspberry_manager{&board_manager, &sd_manager, &iridium_manager, &SERIAL_RASPBERRY};

// TODO: class for the VN100
// TODO: class for interaction with RaspberryPi
// TODO: not urgent: class for the LSM9DS0
// TODO: fix all names: name cpp file and header should be the same as class they contain

void setup(){

  // manage the board: decide if should wake up
   board_manager.start();
   wdt_reset();

   print_debug_status();

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

  #if DEBUG && DEBUG_SLOW
    SERIAL_DEBUG.println(F("calling loop"));
  #endif

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
      iridium_manager.send_receive_iridium_vital_information();

      // go through Raspberry Pi interaction
      perform_raspberry_interaction();

      // ask to be put off
      board_manager.ask_to_be_off();

      // put to deep sleep: TODO: implement in board_manager
      board_manager.sleep_or_reboot();
      break;
  }
}

// TODO: put in raspberry_manager class
void perform_raspberry_interaction(void){
  raspberry_manager.start();
  raspberry_manager.send_filename();
  raspberry_manager.file_content_to_raspberry();
  raspberry_manager.receive_processed_data();
  raspberry_manager.transmit_through_iridium();
}