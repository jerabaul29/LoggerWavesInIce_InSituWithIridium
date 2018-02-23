/*
 *
 * A class for client devices, i.e. devices that:
 *
 * - are switched on and off by the master device
 * - should be let the chance to be switched on at regular intervals
 * - give feedback about wether they want to continue to be powered up or no
 *
 * This also includes some checks, such as:
 * - checking if the battery is enough when performing a new start up
 * - allowing only a maximum number of cycles during which the device is awake in a row
 *
 */

#ifndef CLIENT_DEVICE
#define CLIENT_DEVICE
#define CLIENT_DEVICE_VERSION 1.1

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

class CDV{

public:
  CDV(int pin_feedback, int pin_control, int cycles_sleep);

  CDV(int pin_feedback, int pin_control, int cycles_sleep, float (* get_voltage_volts)(void));

  void set_cycles_sleep(int cycles_sleep);

  void update_status(void);

  void print_status(HardwareSerial *port);

private:

  //////////////////////////////////////////////////////////////////////////////
  // VARIABLES
  //////////////////////////////////////////////////////////////////////////////

  // pins definition -----------------------------------------------------------
  int pin_feedback;  // feedback from the device
  int pin_control;  // control of the device

  // status of the device ------------------------------------------------------
  bool device_awake;

  // control of number of sleeps -----------------------------------------------
  int cycles_sleep;  // how many cycles should sleep before wake up
  int max_cycles_awake;  // max number of cycles can be awake
  int cycles_counter;  // counter for either cycles sleep left or cycles awake elapsed

  // options for the class -----------------------------------------------------
  bool perform_voltage_check;  // if the voltage should be checked at wake up
  bool perform_max_cycles_check;  // if should check for max number of cycles awake

  // check battery levels ------------------------------------------------------
  float (* get_voltage_volts)(void);
  float voltage_volts;
  bool enough_volts;

  //////////////////////////////////////////////////////////////////////////////
  // METHODS
  //////////////////////////////////////////////////////////////////////////////

  void switch_on(void);

  void switch_off(void);

  bool check_voltage(void);

  void switch_if_on(void);

  void switch_if_off(void);

};

#endif
