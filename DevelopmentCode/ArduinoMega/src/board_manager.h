#include <Arduino.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include "EEPROM_interaction.h"
#include "parameters.h"

#ifndef BOARD_MANAGER
#define BOARD_MANAGER

class BoardManager{
public:
  /*
    Constructor. Gives a well initialized board:
    - put raspberry and led off
    - decide if should wake up
  */
  BoardManager(void);

  /*
    The function to run to initialize the object.
  */
  void start(void);

  /*
  Return a boolean, true if still should log, false if time to stop logging
  */
  bool should_log(void);

  /*
  Power off the raspberry pi
  */
  void turn_raspberry_off(void);

  /*
  Power on the raspberry pi
  */
  void turn_raspberry_on(void);

  /*
  Check if the battery level is enough to work,
  */
  bool enough_battery(void);

  /*
  Decide if should wake up. Update the contents of the EEPROM
  */
  bool should_wakeup(void);

  /*
  Compute the level of the battery
  */

  float measure_battery_level(void);

  /*
    Ask to be on
  */
  void ask_to_be_on(void);

  /*
    Ask to be off
  */
  void ask_to_be_off(void);

  /*
   Make sleep for a long time
  */
  // TODO: add some sleeping possibilities similar to the power control in the Arduino Uno
  


private:

  void setup_watchdog(void);

  /*
    Check if the sleep status is Ok; if not, correct it
  */
  void check_sleep_status(void);

  /*
    The battery level in volts
  */
  float battery_level_V;

  // how many 'wake up cycles' by the power controller currently left before really wake up
  // the value is stored in EEPROM
  uint8_t number_sleeps_left;

  // how many 'wake up cycles' should sleep between two real wake ups
  // defined in EEPROM so that possible to update by Iridium
  // total_number_sleeps_before_wakeup value 0 means that never sleep
  // total_number_sleeps_before_wakeup value 1 means that measures and then sleeps
  // once (ie sleep typically 20 minutes)
  uint8_t total_number_sleeps_before_wakeup;

  unsigned long time_start_logging_ms;  // when the logging actually started
};

#endif
