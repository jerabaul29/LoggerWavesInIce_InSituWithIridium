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

    this->clean_incoming_buffer();
}

GPSController::catch_log_message(){
  // while any chars incoming, grab them
  while (serial_port_gps->available() > 0){
    // if reach the end of a message, post it
    int len = this->catch_message()
    if(len > 0){
      sd_manager->post_on_SD_card(GPS_rx_buffer, len);
    }
  }
}


int GPSController::catch_message(void){
  while (serial_port_gps->available()>0){

    char c_GPS = GPS.read();

    if (c_GPS=='\n'){
      int swp = GPS_rx_buffer_position
      GPS_rx_buffer_position = 0;
      return(swp);
    }
    else{
      // if enough space on the buffer, log
      if (GPS_rx_buffer_position < SIZE_GPS_BUFFER){
        // just save on the buffer if enough space
        GPS_rx_buffer[GPS_rx_buffer_position] = c_GPS;
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

int GPSController::load_gprmc_message(){
  this->clean_incoming_buffer();

  current_n_read_GPS = 0;

  // limit the maximum number of attempts
  while (current_n_read_GPS < MAX_NUMBER_ATTEMPTS_GPRMC){

    // catch a complete message
    do{
      int crrt_length_message = this->catch_message();
    }while(0 == crrt_length_message)

    // check if a GPRMC
    if ((GPS_rx_buffer[0] == '$') && (GPS_rx_buffer[1] == 'G') && (GPS_rx_buffer[2] == 'P') &&
        (GPS_rx_buffer[3] == 'R') && (GPS_rx_buffer[4] == 'M') && (GPS_rx_buffer[5] == 'C')) {

          // we have received one more GPRMC
          current_n_read_GPS += 1;

          // if valid, this is what we want
          if (GPS_rx_buffer[18] == 'A'){
            return(crrt_length_message);
          }
    }
  }

  // if no valid message, at least return the last one
  return(crrt_length_message);
}

void clean_incoming_buffer(void){
  // flush current GPS buffer
  while (serial_port_gps->available() > 0) {
    serial_port_gps->read();
  }

  // then flush until the end of a message has arrived
  while (true){
    if (serial_port_gps->available() > 0){
      char crrt_char = serial_port_gps->read();
      if (crrt_char == '\n'){
        break;
      }
    }
  }
}
