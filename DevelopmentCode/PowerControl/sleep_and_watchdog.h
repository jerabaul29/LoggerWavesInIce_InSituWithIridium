/*

 *
 * A class to help use the watchdog for both deep sleep and watchdog.
 *
 * Careful: remember that once this class has been called, then the watchdog
 * is running: need to call wdt_reset at least each 8 seconds.
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

//clear bit macro
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

//set bit macro
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <avr/wdt.h>            // library for default watchdog functions
#include <avr/interrupt.h>      // library for interrupts handling
#include <avr/sleep.h>          // library for sleep
#include <avr/power.h>          // library for power control

#ifndef SLEEP_AND_WATCHDOG
#define SLEEP_AND_WATCHDOG

#define DURATION_SLEEP_WDT_MS 8000UL  // duration of one watchdog sleep in ms

class SleepWatchdog{
public:
  SleepWatchdog(void);

  /*
   *
   * Make sleep for ncycles using the lowest power level.
   *
   */
  void sleep(int ncycles);

  /*
   *
   * Configure the watchodg for later use for sleep and watchdog keeping.
   *
   */
  void configure_wdt(void);

  /*
   *
   * Return the true elapsed time, taking into account how many times
   * have been slept.
   *
   */
  unsigned long millis(void) const;

private:
  unsigned long int number_of_cycles_slept;
};

#endif
