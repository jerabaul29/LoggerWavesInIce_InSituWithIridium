#include <client_device.h>

CDV::CDV(int pin_feedback, int pin_control, int cycles_sleep) :
pin_feedback(pin_feedback),
pin_control(pin_control),
cycles_sleep(cycles_sleep)
{
  device_awake = false;

  perform_max_cycles_check = false;
  perform_voltage_check = false;
}

CDV::CDV(int pin_feedback, int pin_control, int cycles_sleep, float (* get_voltage_volts)(void)) :
pin_feedback(pin_feedback),
pin_control(pin_control),
cycles_sleep(cycles_sleep),
get_voltage_volts(get_voltage_volts)
{
  device_awake = false;

  perform_max_cycles_check = false;
  perform_voltage_check = true;
}
