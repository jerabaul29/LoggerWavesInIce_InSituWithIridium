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

#include "solar_controller.h"

SolarController::SolarController(const int pin_control_panel, const int pin_measure_anode_divider, BatteryController * const battery_controller_instance) :
  pin_control_panel(pin_control_panel),
  pin_measure_anode_divider(pin_measure_anode_divider),
  battery_controller_instance(battery_controller_instance)
{
}

void SolarController::update_panel_status(void){
  float tension_solar_panel_anode_V;
  tension_solar_panel_anode_V = SolarController::read_solar_panel_anode();

  float solar_panel_tension;
  float battery_tension_V = battery_controller_instance->battery_voltage();
  solar_panel_tension = battery_tension_V - tension_solar_panel_anode_V;

  if (SolarController::should_connect_array(battery_tension_V, solar_panel_tension)){
    SolarController::connect_array();
  }

  else{
    SolarController::disconnect_array();
  }
}

void SolarController::connect_array(void){
  pinMode(pin_control_panel, OUTPUT);
  digitalWrite(pin_control_panel, HIGH);
}

void SolarController::disconnect_array(void){
  pinMode(pin_control_panel, INPUT);
}

float SolarController::read_solar_panel_anode(void){
  // disconnect array from battery, otherwise measure battery voltage
  pinMode(pin_control_panel, INPUT);

  // a bit of time for tensions to settle
  delay(5);

  // measure on - of solar panel voltage divider
  int value_measurement_solar_panel;
  value_measurement_solar_panel = analogRead(pin_measure_anode_divider);

  // convert ADC to V
  float value_measurement_solar_panel_V;
  value_measurement_solar_panel_V = 5.0 / 1024.0 * float(value_measurement_solar_panel);

  // invert effect voltage divider to get anode
  float tension_solar_panel_anode_V;
  tension_solar_panel_anode_V = (value_measurement_solar_panel_V - 5.0 * ALPHA) / (1.0 - ALPHA);

  return(tension_solar_panel_anode_V);
}

bool SolarController::should_connect_array(float battery_tension_V, float solar_panel_tension){
  // battery full
  if (battery_tension_V > BAT_FULL_V){
    return(false);
  }

  // too low solar panel
  else if (solar_panel_tension < battery_tension_V + MIN_MARGIN_PANEL){
    return(false);
  }

  // not full battery and high solar panel
  else{
    return(true);
  }
}
