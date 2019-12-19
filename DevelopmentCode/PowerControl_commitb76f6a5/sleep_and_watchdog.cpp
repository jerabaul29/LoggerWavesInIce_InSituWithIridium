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

#include "sleep_and_watchdog.h"

// how many times remain to sleep before wake up
// volatile to be modified in interrupt function
volatile int nbr_remaining = 0;

/*
 *
 * Check if it is a normal watchdog (just sleeping) or if should reboot.
 * If normal watchdog, decrease number of remaining sleeps
 * If should reboot, watchdog reboot
 *
 */
ISR(WDT_vect)
{
    // Check if we are in sleep mode or it is a genuine WDR.
    if(nbr_remaining > 0)
    {
        // not hang out, just waiting
        // decrease the number of remaining avail loops
        nbr_remaining = nbr_remaining - 1;
        wdt_reset();
    }
    else
    {
        // must be rebooted
        // configure
        MCUSR = 0;                          // reset flags

                                            // Put timer in reset-only mode:
        WDTCSR |= 0b00011000;               // Enter config mode.
        WDTCSR =  0b00001000 | 0b000000;    // clr WDIE (interrupt enable...7th from left)
                                            // set WDE (reset enable...4th from left), and set delay interval
                                            // reset system in 16 ms...
                                            // unless wdt_disable() in loop() is reached first

        // reboot
        while(1);
    }
}

void configure_wdt(void)
{

  cli();                           // disable interrupts for changing the registers

  MCUSR = 0;                       // reset status register flags

                                   // Put timer in interrupt-only mode:
  WDTCSR |= 0b00011000;            // Set WDCE (5th from left) and WDE (4th from left) to enter config mode,
                                   // using bitwise OR assignment (leaves other bits unchanged).
  WDTCSR =  0b01000000 | 0b100001; // set WDIE: interrupt enabled
                                   // clr WDE: reset disabled
                                   // and set delay interval (right side of bar) to 8 seconds

  sei();                           // re-enable interrupts
}

void sleep(int ncycles)
{
  nbr_remaining = ncycles; // defines how many cycles should sleep

  // let all non needed pins float to reduce power use
  #if SHOW_LED
    pinMode(PIN_CMD_LED, INPUT);
  #endif

  // Set sleep to full power down.  Only external interrupts or
  // the watchdog timer can wake the CPU!
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  // Turn off all modules and the ADC while asleep.
  power_all_disable();
  // power_adc_disable();
  cbi(ADCSRA,ADEN);

  while (nbr_remaining > 0){ // while some cycles left, sleep!

    // Enable sleep
    sleep_enable();

    // enter sleep
    sleep_mode();

    // CPU is now asleep and program execution completely halts!
    // when the watchdog fires, the ISR will be called.
    // Once awake after ISR, execution will resume at this point if the
    // watchdog is configured for resume rather than restart

    // When awake, disable sleep mode
    sleep_disable();

  }

  // put everything on again
  sbi(ADCSRA,ADEN);
  power_all_enable();

  // put non needed pins out of float
  #if SHOW_LED
    pinMode(PIN_CMD_LED, OUTPUT);
    digitalWrite(PIN_CMD_LED, LOW);
  #endif
}
