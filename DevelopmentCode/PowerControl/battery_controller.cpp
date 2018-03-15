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

#include "battery_controller.h"

BatteryController::BatteryController(const int analog_pin_battery, SleepWatchdog const * const sleep_watchdog_instance) :
  analog_pin_battery(analog_pin_battery),
  sleep_watchdog_instance(sleep_watchdog_instance),
  time_last_update(2147483647)
{}

bool BatteryController::is_up_to_date(void) const{
  #if DEBUG
    Serial.println(F("call BatteryController::is_up_to_date"));
    delay(10);
  #endif

  #if DEBUG
    Serial.println(F("call BatteryController::is_up_to_date"));
    delay(10);
  #endif

  PDEBVAR(sleep_watchdog_instance->millis_wdt())
  PDEBVAR(time_last_update)
  PDEBVAR(MAX_TIME_BATTERY_UP_TO_DATE_MS)
  
  if (sleep_watchdog_instance->millis_wdt() - time_last_update > MAX_TIME_BATTERY_UP_TO_DATE_MS){
    return(false);
  }
  else{
    return(true);
  }
};

void BatteryController::update(void){
  #if DEBUG
    Serial.println(F("call BatteryController::update"));
    delay(10);
  #endif
  
  time_last_update = sleep_watchdog_instance->millis_wdt();

  battery_tension_V = 5.0 / 1024.0 * float(analogRead(analog_pin_battery));
}

float BatteryController::battery_voltage(void){
  #if DEBUG
    Serial.println(F("call BatteryController::battery_voltage"));
    delay(10);
  #endif
  
  if (!BatteryController::is_up_to_date()){
    BatteryController::update();
  }

  return(battery_tension_V);
}
