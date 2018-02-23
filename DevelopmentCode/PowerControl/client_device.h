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
 * TODO - allowing only a maximum number of cycles during which the device is awake in a row
 *   before considering it a hang
 *
 * by Jean RABAULT <jean.rblt@gmail.com>

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

 *
 */

 #if ARDUINO >= 100
   #include "Arduino.h"
 #else
   #include "WProgram.h"
 #endif

#ifndef CLIENT_DEVICE
#define CLIENT_DEVICE
#define CLIENT_DEVICE_VERSION 1.1

#ifndef THRESHOLD_VOLTAGE
#define THRESHOLD_VOLTAGE 2.8
#endif

class CDV{

public:

  CDV(int pin_feedback, int pin_control, int cycles_sleep, const float * const battery_voltage);

  /*
    update the status of the client, and adapts the control (power or no power)
    in consequence.
  */
  void update_status(void);

private:

  const int pin_feedback;  // feedback from the device
  const int pin_control;  // control of the device

  bool device_awake;

  int cycles_sleep;  // how many cycles should sleep before wake up
  int cycles_counter;  // counter for either cycles sleep left or cycles awake elapsed

  const float * const battery_voltage;

  void switch_on(void);

  void switch_off(void);

  void update_awake(void);

  void update_asleep(void);

  /*
    check that the number of sleep cycles remaining is valid (between 0 and cycles_sleep)
    if not, set it to cycles_sleep
  */
  void check_number_cycles(void);

};

#endif
