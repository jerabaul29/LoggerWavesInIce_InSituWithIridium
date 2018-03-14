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
