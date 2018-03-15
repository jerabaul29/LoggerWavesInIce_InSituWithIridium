/*

 Copyright (c) 2015 Jean Rabault jean.rblt@gmail.com

 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in
 the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 of the Software, and to permit persons to whom the Software is furnished to do
 so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 */

#include "sleep_and_watchdog.h"
#include "parameters.h"
#include "DebugMacros.h"

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
