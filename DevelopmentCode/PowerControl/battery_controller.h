#include "sleep_and_watchdog.h"

#ifndef BATTERY_CONTROLLER
#define BATTERY_CONTROLLER

#define MAX_TIME_BATTERY_UP_TO_DATE_MS 500UL

class BatteryController{
public:
  BatteryController(const int analog_pin_battery, SleepWatchdog const * const sleep_watchdog_instance);

  bool is_up_to_date(void) const;

  void update(void);

  float battery_voltage(void);

private:
  SleepWatchdog const * const sleep_watchdog_instance

  const int analog_pin_battery;

  float battery_tension_V;

  unsigned long time_last_update;

};

#endif
