#include "Arduino.h"
#include <Adafruit_GPS.h>
// note: we do not use software serial but it seems to be necessary for initializing
// Adafruit_GPS library
#include <SoftwareSerial.h>
#include "SDManager.h"
#include <avr/wdt.h>


#ifndef GPS_CONTROLLER
#define GPS_CONTROLLER

// NOTE: buffering here is redundant with the ring buffer of the serial port, but proably
// necessary as do not want to miss with the ring byffers and take the risk to miss some
// bytes.

class GPSManager{
public:
  GPSManager(HardwareSerial * serial_port_gps, SDManager * sd_manager);

  /*
  Setup the GPS for making it ready to log
  */
  void start(void);

  /*
  call catch_message, if a full message is received then post it on the SD card
  */
  void perform_logging(void);

  /*
   Load a GPRMC message in the GPS buffer. Perform a maximum of MAX_NUMBER_ATTEMPTS_GPRMC
   attempts to get a valid GPRMC message. Return the size of the message in buffer.
   */
  int load_gprmc_message(void);

  /*
   Clen the Arduino RX buffer corresponding to the GPS, until the end of a
   message is reached.
   */
  void clean_incoming_buffer(void);

  /*
    Get the address of the buffer.
  */
  const char * get_rx_buffer(void) const;


private:
  /*
  Catch incoming NMEA string, as long as some incoming chars in the GPS buffer
  and the end of the message is not received.
  Return the length if a full NMEA string is captured, 0 otherwise if no full incoming NMEA string.
  */
  int catch_message(void);

  HardwareSerial * serial_port_gps;

  SDManager * sd_manager;

  // a buffer for incoming GPS information
  char GPS_rx_buffer[SIZE_GPS_BUFFER];

  // current position in the buffer
  int GPS_rx_buffer_position;

  // the GPS object from adafruit
  Adafruit_GPS instance_GPS;

  // how many strings have been trying to read for Iridium
  int current_n_read_GPS;

};

#endif
