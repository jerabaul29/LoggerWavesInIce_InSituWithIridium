/*

  *
  * A class to control a solar panel
  * Determines how much voltage a solar panel can provide, and depending on this
  * and the level of the battery, decide if should couple the solar panel
  * to the battery or not.
  *
  * the logics includes:
  * - check for full battery
  * - check for non productive solar panel
  *

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

#include "parameters.h"

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#ifndef SOLAR_CONTROLLER
#define SOLAR_CONTROLLER

class SolarController{

public:
  SolarController(const int pin_control_panel, const int pin_measure_anode_divider, const float * const battery_tension_V);

  /*
    The function to call to update the status of the panel. Computes how much tension
    the panel can provide, and couple it if relevant. CAREFUL: you must have an up
    to date measurement of the battery (const float * const battery_tension_V) before
    calling this function.
  */
  void update_panel_status(void);

private:

  const int pin_control_panel;  // the pin to the MOSFET that couples the panel on and off to the battery
  const int pin_measure_anode_divider;  // the pin that measures the output of the voltage divider on the anode of the panel
  const float * const battery_tension_V;  // to the tension of the battery

  /*
    the logics to decide if should couple the solar panel or not
  */
  bool should_connect_array(const float battery_tension_V, float solar_panel_tension);

  /*
    read the potential at the anode of the solar panel
  */
  float read_solar_panel_anode(void);

  void connect_array(void);

  void disconnect_array(void);

};

#endif
