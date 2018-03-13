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

/*
  crucial for good functionning:
    - check DEBUG parameter: should be false for use with RPi, otherwise will not work!!
    - check USE_IRIDIUM: should be true for use of real iridium and not dummy
    - check DURATION_LOGGING_MS
*/

#ifndef PARAMETERS_LOGGER_MEGA
#define PARAMETERS_LOGGER_MEGA

#define DELAY_START_SERIAL 20

#define PIN_MSR_BATTERY A0
#define BAT_EMPTY_V 2.8     // threshold for empty battery

// Debug
#define DEBUG false
#define SERIAL_DEBUG Serial
#define SERIAL_DEBUG_BAUDRATE 115200

// SD
#define PIN_SELECT_SD 53
#define NBR_ZEROS_FILENAME 5

// GPS
#define SERIAL_GPS Serial1
#define SIZE_GPS_BUFFER 128
#define MAX_NUMBER_ATTEMPTS_GPRMC 10  // GPS works at 1Hz

// constraints on max number of sleep cycles between wakeups
#define MAX_NBR_SLEEPS_BEFORE_WAKEUP 5

// Iridium
#define IRIDIUM_TRANSMIT_PACKET_SIZE 340
#define IRIDIUM_RECEIVED_PACKET_SIZE 270
#define USE_IRIDIUM false
#define PIN_IRD_SLEEP 49
#define SERIAL_IRIDIUM Serial2
#define IRIDIUM_LOW_POWER_SUPPLY 1
#define IRIDIUM_HIGH_POWER_SUPPLY 0
#define NBR_SIGNIFICANT_DIGITS_BATT_LEVEL 2

// Raspberry
#define SERIAL_RASPBERRY Serial
#define DELAY_ANSWER_RPI 500
#define TIMEOUT_ACKNOWLEDGEMENT_RPI_MS 4000UL
#define TIMEOUT_PROCESSING_RPI 60000UL
#define TIMEOUT_WAKEUP_RPI 40000UL
#define TIMEOUT_RECEIVED_TO_TRANSMIT_FROM_PI 60000UL

// VN 100
#define SERIAL_VN100 Serial3
#define SIZE_BUFFER_VN100 256

// feedback mega pin: high as long as the Mega should get power
#define PIN_FBK_MGA 48

// how long should log
#define DURATION_LOGGING_MS 1500000UL
// #define DURATION_LOGGING_MS 5000UL  // for tests

// Mosfet power Raspberry Pi
#define PIN_MFT_RPI 46

// LED
#define PIN_MGA_LED 13

// EEPROM
#define ADDRESS_LONG_FILENBR 1  // address for the filenumber; takes 4 bytes
#define ADDRESS_SLEEPS_LEFT 5  // address for where number of sleep cycles left stored
#define ADDRESS_TOTAL_NBR_SLEEPS 6  // address for what is the total number of sleep cycles

#endif
