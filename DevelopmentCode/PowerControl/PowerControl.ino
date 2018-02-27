/*
 * Code for power control
 *
 * This code takes care of the power management, and should be uploaded on the barebone ATMega328P
 *
 * Its functions are to:
 *
 * - Sleep most of the time in lowest power mode, but watchdog wake up at regular intervals
 *
 * - Take care of the battery and solar panel. Each time it wakes up, check if the battery and solar panel should be connected
 *
 * - Take care of the power to the Arduino Mega. Each time it wakes up, check what should be the next state of the Arduino Mega.
 *
 * - The parameters are set in the parameters.h header file
 *
 * TODO:
 *   write a post about the power manager
 *   point to the post
 *
 * TODO:
 * - Possible optimization (require hardware update; overkill and probably over engineering)
 * put the pololu on a MOSFET from the battery side (to disconnect it when not needed, ie when not using
 * the Mega). Then need to be sure voltage on capacitors do not get too low: re connect pololu when logging with
 * Mega, and connect at least 8 seconds each 15 minutes [note: check values].
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

#include "sleep_and_watchdog.h"
#include "client_device.h"
#include "solar_controller.h"

SleepWatchdog sleep_watchdog_instance{};
BatteryController battery_controller_instance{PIN_MSR_BAT, &sleep_watchdog_instance};
CDV mega_device{PIN_FBK_MGA, PIN_MFT_MGA, CYCLES_BEFORE_MEGA_WAKEUP, &battery_controller_instance};
SolarController solar_controller_instance{PIN_MFT_SOL, PIN_MSR_SOL, &battery_controller_instance};

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void setup() {

  // --------------------------------------------------------------
  // Configure watchdog
  // --------------------------------------------------------------

  sleep_watchdog_instance.configure_wdt();
  wdt_reset();

  // --------------------------------------------------------------
  // Prepare pins
  // --------------------------------------------------------------

  // set the mode of the digital pins (analog pins do not need to be set)
  pinMode(PIN_FBK_MGA, INPUT);
  pinMode(PIN_MFT_SOL, INPUT);
  pinMode(PIN_MFT_MGA, INPUT);

  // --------------------------------------------------------------
  // first battery measurement
  // --------------------------------------------------------------
  battery_controller_instance.update();

}
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void loop() {

  // --------------------------------------------------------------
  // Start a new cycle: let the time to wake up
  // --------------------------------------------------------------

  delay(5);

  // --------------------------------------------------------------
  // measure battery
  // --------------------------------------------------------------

  battery_controller_instance.update();

  wdt_reset();

  // --------------------------------------------------------------
  // Decide what should do with the solar panel
  // --------------------------------------------------------------

  solar_controller_instance.update_panel_status();

  wdt_reset();

  // --------------------------------------------------------------
  // Decide what to do with the Arduino Mega
  // --------------------------------------------------------------

  mega_device.update_status();

  wdt_reset();

  // --------------------------------------------------------------
  // Sleep
  // --------------------------------------------------------------
  sleep_watchdog_instance.sleep(CYCLES_DEEP_SLEEP);
}
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
