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

#include "client_device.h"

CDV::CDV(int pin_feedback, int pin_control, int cycles_sleep, BatteryController * const battery_controller_instance) :
  pin_feedback(pin_feedback),
  pin_control(pin_control),
  cycles_sleep(cycles_sleep),
  battery_controller_instance(battery_controller_instance),
  device_awake(false),  // off by default
  cycles_counter(0)  // ready to wake up at once
{
}

void CDV::check_number_cycles(void){
  if (cycles_counter > cycles_sleep){
    cycles_counter = cycles_sleep;
  }
  else if (cycles_counter < 0){
    cycles_counter = cycles_sleep;
  }
}

void CDV::update_status(void){
  CDV::check_number_cycles();

  if (device_awake){
    CDV::update_awake();
  }
  else{
    CDV::update_asleep();
  }
}

void CDV::update_awake(void){
  if (digitalRead(pin_feedback)){
    // nothing to do!
  }
  else{
    CDV::switch_off();
  }
}

void CDV::update_asleep(void){

  // if time to wake up...
  if (0 == cycles_counter){  // ------------------------------------------------

    // if enough battery
    if (battery_controller_instance->battery_voltage() > BAT_EMPTY_V){ // --------------
      // power
      CDV::switch_on();

      // give the time to boot
      // NOTE: if slow bootloader, this may not be enough?
      // NOTE: if extend the delay, be careful: the watchdog will fire after 8
      // seconds
      delay(1000);

      wdt_reset();

      // if wants to be awake
      if (digitalRead(pin_feedback)){
        // already on
      }
      // if does not want to be awake
      else{
        // switch off
        CDV::switch_off();
      }
    }

    // if not enough battery
    else{  // --------------------------------------------------
      CDV::switch_off();
    }
  }

  // if not time to wake up, sleep
  else{  // --------------------------------------------------------------------
    cycles_counter--;
  }
}

void CDV::switch_on(void){
  device_awake = true;

  pinMode(pin_control, OUTPUT);
  digitalWrite(pin_control, HIGH);
}

void CDV::switch_off(void){
  device_awake = false;

  pinMode(pin_control, INPUT);

  cycles_counter = cycles_sleep - 1;
}
