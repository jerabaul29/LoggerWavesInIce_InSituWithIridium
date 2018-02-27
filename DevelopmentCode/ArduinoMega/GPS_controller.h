#include <Adafruit_GPS.h>
// note: we do not use software serial but it seems to be necessary for initializing
// Adafruit_GPS library
#include <SoftwareSerial.h>
#include "SDManager.h"

#include <avr/wdt.h>

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#ifndef GPS_CONTROLLER
#define GPS_CONTROLLER

class GPSController{
public:
  GPSController(Serial * serial_port_gps, SDManager * sd_manager){};

  /*
  Setup the GPS for making it ready to log
  */
  void setup_GPS();

  /*
  call catch_message, if a full message is received then post it on the SD card
  */
  void catch_log_message(void);


private:
  /*
  Catch incoming NMEA string, as long as some incoming chars in the GPS buffer
  and the end of the message is not received.
  Return the length if a full NMEA string is captured, 0 otherwise if no full incoming NMEA string.
  */
  int catch_message(void);

  Serial * serial_port_gps;

  // a buffer for incoming GPS information
  char GPS_rx_buffer[SIZE_GPS_BUFFER];

  // current position in the buffer
  int GPS_rx_buffer_position;

  // the GPS object from adafruit
  Adafruit_GPS GPS;

  // how many strings have been trying to read for Iridium
  int current_n_read_GPS;

  SDManager * sd_manager;

};

#endif
