#include "GPS_controller.h"

// TODO:
// fully use the Adafruit GPS class instead of doing some parsing by hand.
// this may request spending some time looking at their library

GPSController::GPSController(Serial * serial_port_gps, SDManager * sd_manager):
  serial_port_gps(serial_port_gps),
  sd_manager(sd_manager),
  GPS_rx_buffer_position(0),
  current_n_read_GPS(0),
{
  Adafruit_GPS GPS(serial_port_gps);
}

GPSController::setup_GPS(){
    delay(250);

    GPS.begin(9600);

    // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
    GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    // uncomment this line to turn on only the "minimum recommended" data
    // GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);

    // Set the update rate
    GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
    GPS.sendCommand(PMTK_API_SET_FIX_CTL_1HZ);

    // Request updates on antenna status, comment out to keep quiet
    // GPS.sendCommand(PGCMD_ANTENNA);

    delay(1000);

    wdt_reset();
}

GPSController::catch_log_message(){
  // while any chars incoming, grab them
  while (serial_port_gps->available() > 0){
    // if reach the end of a message, posrt it
    if(int len = this->catch_message() > 0){
      sd_manager->post_on_SD_card(GPS_rx_buffer, len);
    }
  }
}


int GPSController::catch_message(void){
  while (serial_port_gps->available()>0){

    char c_GPS = GPS.read();

    if (c_GPS=='\n'){
      GPS_rx_buffer_position = 0;
      return(GPS_rx_buffer_position);
    }
    else{
      // just save on the buffer if enough space
      GPS_rx_buffer[GPS_rx_buffer_position] = c_GPS;
      if (GPS_rx_buffer_position < SIZE_GPS_BUFFER){
        GPS_rx_buffer_position += 1;
      }
      // if not enough space, ask to do something anyway
      else{
        GPS_rx_buffer_position = 0;
        return(GPS_rx_buffer_position);
      }
    }
  }

  return(0)

  wdt_reset();
}


  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Obtain a GPS GPRMC string to be used in the Iridium message
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// obtain a GPRMC string for sending through Iridium
// read at most 10 GPRMC strings trying to get a valid signal
// if no valid GPS signal after 10 GPRMC strings, just send the last one
void obtain_GPRMC_Iridium_message(void){

  // flush GPS buffer
  while (serial_port_gps->available() > 0) {
    serial_port_gps->read();
  }

  // variable for holding current GPS chars
  char c_GPS;

  // try to read at most 10 GPS strings; if no valid GPRMC strings
  // 10 times in a row, just use the last one
  current_n_read_GPS = 0;

  while (current_n_read_GPS < 10) {

    wdt_reset();

    // see if some GPS data available
    if (serial_port_gps->available() > 0) {
            c_GPS = GPS.read();
    }

    // if this is the beginning of a new GPS message, log it; otherwise, just
    // try to read next character by restarting the loop
    if (c_GPS == '\n') {

      #if SERIAL_PRINT
        Serial.print(F("D;Attempt reading GPS number: "));
        Serial.println(current_n_read_GPS);
      #endif

      // init buffer position
      GPS_rx_buffer_position = 0;

      bool end_GPS_msg = false;

      // log the whole message
      while (!end_GPS_msg) {
        if (serial_port_gps->available() > 0) {

          c_GPS = GPS.read();

          // newline: end of the GPS message
          if (c_GPS == '\n') {
            end_GPS_msg = true;
          }

          // other than newline: GPS message continues
          else {
            GPS_rx_buffer[GPS_rx_buffer_position] = c_GPS;
            GPS_rx_buffer_position++;
          }
        }
      }

      // now a full GPS message has been logged

      #if SERIAL_PRINT
        Serial.print(F("D;Read:"));
        for (int i = 0; i < GPS_rx_buffer_position; i++) {
          Serial.print(GPS_rx_buffer[i]);
        }
        Serial.println();
      #endif

      // check if the logged message was GPRMC
      // if not a GPRMC, just try to log the next incoming GPS string
      if ((GPS_rx_buffer[0] == '$') && (GPS_rx_buffer[1] == 'G') && (GPS_rx_buffer[2] == 'P') &&
          (GPS_rx_buffer[3] == 'R') && (GPS_rx_buffer[4] == 'M') && (GPS_rx_buffer[5] == 'C')) {

      #if SERIAL_PRINT
          Serial.println("D;GPRMC");
      #endif

      // if valid fix: we have the GPRMC string we want to send, stop here.
      if (GPS_rx_buffer[18] == 'A') {
      #if SERIAL_PRINT
          Serial.println("D;Valid");
      #endif

        break;
      }

      current_n_read_GPS++;

      }
    }
  }
}
