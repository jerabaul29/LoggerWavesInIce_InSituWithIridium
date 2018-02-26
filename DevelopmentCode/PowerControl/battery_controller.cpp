#include <battery_controller.h>

BatteryController::BatteryController(const int analog_pin_battery, SleepWatchdog const * const sleep_watchdog_instance) :
  analog_pin_battery(analog_pin_battery),
  sleep_watchdog_instance(sleep_watchdog_instance),
  time_last_update(0)
{}

bool BatteryController::is_up_to_date(void) const{
  if (*sleep_watchdog_instance.millis() - time_last_update > MAX_TIME_BATTERY_UP_TO_DATE_MS){
    return(false);
  }
  else{
    return(true);
  }
};

void BatteryController::update(void){
  time_last_update = sleep_watchdog_instance->millis();
  
}
