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

  *
  * The parameters for the power controlling mController
  * Should make sure all the parameters are good, in particular number of cycles
  * before uploading.
  *

*/

#ifndef PARAMETERS
#define PARAMETERS

// TODO: implement the debugging in the classes
#define DEBUG false  // if DEBUG is true, Serial will be activated and used for logging information strings
#define SHOW_LED false  // if SHOW_LED is true, the LED will be used for helping debugging

#define PIN_MSR_BAT A0      // measure of battery
#define PIN_MSR_SOL A1      // measure of solar panel on anode

#define ALPHA 0.41          // R_2 / (R_1 + R_2) in voltage divider

#define PIN_MFT_SOL 2      // command mosfet solar panel / battery
#define PIN_FBK_MGA 3      // feedback from Mega
#define PIN_MFT_MGA 4      // command mosfet giving to the Mega
#define PIN_CMD_LED 13     // command of intern LED

#define BAT_FULL_V 3.6  // threshold for full battery
#define BAT_EMPTY_V 2.8     // threshold for empty battery
#define MIN_MARGIN_PANEL 0.5  // minimum over voltage panel vs battery for it to be worth connecting

// number of cycles for which the microcontroller taking care of power sleeps
// each sleep cycle is 8s
// for production: 5 cycles is 40 seconds
// this means that every 40s, the mC will wake up, take care of solar power,
// take care of the Mega.
// #define CYCLES_DEEP_SLEEP 5
#define CYCLES_DEEP_SLEEP 2  // test value

// number of loop () cycles before trying to wake up the Mega after is has been set asleep
// or refused to wake up.
// the duration of a loop() cycle is mostly determined by CYCLES_DEEP_SLEEP
// ie if for production deep sleep is 40s, and CYCLES_BEFORE_MEGA_WAKEUP is 30,
// then the Mega will get the possibility to be waken up every 40s * 30  = 1200s = 20 minutes
// and the Mega can decide to be always awake (never stop asking current), or to perform measurements
// every 20 minutes (sleep one cycle), or every 40 minutes (sleep 2 cycles), etc.
// #define CYCLES_BEFORE_MEGA_WAKEUP 30
#define CYCLES_BEFORE_MEGA_WAKEUP 3 // test value

#endif
