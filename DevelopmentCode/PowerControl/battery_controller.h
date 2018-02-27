#include "sleep_and_watchdog.h"

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#ifndef BATTERY_CONTROLLER
#define BATTERY_CONTROLLER

#define MAX_TIME_BATTERY_UP_TO_DATE_MS 500UL

class BatteryController{
public:
  BatteryController(const int analog_pin_battery, SleepWatchdog const * const sleep_watchdog_instance);

  /*
   *
   * Is the cached battery measurement up to date?
   * note that this will NOT update the battery measurement if not up to date
   *
   */
  bool is_up_to_date(void) const;

  /*
   *
   * Update the battery measurement.
   *
   */
  void update(void);

  /*
   *
   * If necessary, update the battery measurement first.
   * Then return a valid battery measurement in volts
   *
   */
  float battery_voltage(void);

private:
  SleepWatchdog const * const sleep_watchdog_instance;

  const int analog_pin_battery;

  float battery_tension_V;

  unsigned long time_last_update;
};

#endif
