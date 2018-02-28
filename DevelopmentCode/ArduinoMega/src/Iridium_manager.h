#include <Arduino.h>
#include "IridiumSBD.h"
#include "parameters.h"
#include "GPS_controller.h"
#include "board_manager.h"
#include "SDManager.h"

// information can be found at: http://arduiniana.org/libraries/iridiumsbd/
// The maximum size of a transmitted packet (including header and checksum) is 340 bytes.
// The maximum size of a received packet is 270 bytes.

#ifndef IRIDIUM_MANAGER
#define IRIDIUM_MANAGER

class IridiumManager{
public:
    IridiumManager(HardwareSerial *, GPSController *, BoardManager *, SDManager *);

    void send_receive_iridium_vital_information(void);

private:
    HardwareSerial * serial_port;

    GPSController * gps_controller;

    BoardManager * board_manager;

    SDManager * sd_manager;

    IridiumSBD iridium_sbd;

    char buffer_received[IRIDIUM_RECEIVED_PACKET_SIZE];
    char buffer_transmit[IRIDIUM_TRANSMIT_PACKET_SIZE];

    int buffer_received_position;
    int buffer_transmit_position;

    /*
      Clean the received buffer and reset pointer
    */
    void clean_reset_buffer_received(void);

    /*
      Clean the transmitted buffer and reset pointer
    */
    void clean_reset_buffer_transmit(void);

    /*
      Append the buffer_transmit_packet with the battery value
    */
    void set_battery_message(void);

    /*
      Append the buffer_transmit_packet with the current filename
    */
    void set_filename_message(void);

    /*
      Append the buffer_transmit_packet with a (if possible, valid)
      GPRMC message.
    */
    void set_GPRMC_message(void);

};

#endif


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Iridium
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// send and receive Iridium vital information and commands
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// NOTE: there is much improvement room for this function

// note: as of now, this is inefficient with Iridium space, as it uses
// plaintext way of noting information; but it makes it easier to debug vital
// data
void send_receive_Iridium_vital_information(void){
  wdt_reset();

  // wake up the Iridium ------------------------------------------------------
  isbd.begin();
  wdt_reset();

  // get the Iridium feedback string ready ------------------------------------
  set_battery_value_Iridium_message();
  wdt_reset();

  set_file_number_Iridium_message();
  wdt_reset();

  obtain_GPRMC_Iridium_message();
  set_GPRMC_Iridium_message();
  wdt_reset();

  #if SERIAL_PRINT
    Serial.println((uint8_t)Iridium_msg);
  #endif

  // try to send the Iridium feedback string ----------------------------------
  // note: retries the operation for up to 300 seconds by default; put watchdog reset in
  // ISBDCallback.
  ird_feedback = isbd.sendReceiveSBDBinary((uint8_t *)Iridium_msg, size_t(1 + GPS_rx_buffer_position),
                                           Ird_rx, Ird_rx_position);  // NOTE: strange that Ird_rx_position; should it be a pointer?
                                                                      // check all occurences

  wdt_reset();

  #if SERIAL_PRINT
    if (ird_feedback != 0){
      Serial.print("D;Problem transmitting: ");
      Serial.println(ird_feedback);
    }
    else{
      Serial.println("D;Transmitted well");
    }
  #endif

  // read Iridium command if available, as many times as incoming messages ------
  do{

      // NOTE: this will display the content, but double check not interacting with it because of the isbd call
      #if SERIAL_PRINT
        Serial.print("Inbound buffer size is ");
        Serial.println(Ird_rx_position);
        for (int i=0; i<Ird_rx_position; ++i){
          Serial.write(Ird_rx[i]);
          Serial.print("(");
          Serial.print(Ird_rx[i]);
          Serial.print(") ");
        }
        Serial.print("Messages left: ");
        Serial.println(isbd.getWaitingMessageCount());
      #endif

    wdt_reset();

    // check if command back ---------------------------------------------------
    // NOTE: need a better command back control system
    if (Ird_rx_position > 0){
      #if SERIAL_PRINT
        Serial.print(F("Ird_rx_position > 0: "));
        Serial.println(Ird_rx_position);
      #endif

      // check if should update number of sleep cycles --------------------------
      // this kind of message should be of the form: SLP(12) [set the number of sleep cycles
      // to 12, where (12) is one byte in binary value]
      if (char(Ird_rx[0]) == 'S' && char(Ird_rx[1]) == 'L' && char(Ird_rx[2]) == 'P'){
         uint8_t value_sleep = uint8_t(Ird_rx[3]);

        #if SERIAL_PRINT
          Serial.print(F("Will sleep for:"));
          Serial.println(value_sleep);
        #endif

        // update the EEPROM
        EEPROM.write(address_total_sleeps, value_sleep);
      }

      // check if should send some commands to RPi -----------------------------
      // NOTE: could add functionalities, ie commands from Iridium, here
      // such as: send a whole file, send an analysis, send another analysis etc
    }

    // if more messages to get, use more sendReceive
    if (isbd.getWaitingMessageCount() > 0){

      #if SERIAL_PRINT
        Serial.println(F("D;Receive one more message"));
      #endif

      Ird_rx_position = sizeof(Ird_rx);
      ird_feedback = isbd.sendReceiveSBDBinary(NULL, 0, Ird_rx, Ird_rx_position);

    }
    // otherwise, get out of the reading loop
    else{

      #if SERIAL_PRINT
        Serial.println(F("D;No more messages"));
      #endif

      break;

    }

  }while(true);

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Put the battery value reading in the Iridium message
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void set_battery_value_Iridium_message(void){
  // measure battery
  string_battery = String(analogRead(PIN_MSR_BATTERY), DEC);

  #if SERIAL_PRINT
    Serial.println(F("D;Battery reading"));
    Serial.print("D;");
    Serial.print(string_battery[0]);
    Serial.print(string_battery[1]);
    Serial.println(string_battery[2]);
  #endif

  Iridium_msg[0] = string_battery[0];
  Iridium_msg[1] = string_battery[1];
  Iridium_msg[2] = string_battery[2];
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Put the file number in the Iridium message
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void set_file_number_Iridium_message(void){
  // get file number

  #if SERIAL_PRINT
    Serial.println(F("D;File number"));
    Serial.print("D;");
    for (int i = 1; i < nbrOfZeros + 1; i++) {
      Serial.print(currentFileName[i]);
    }
    Serial.println();
  #endif

  Iridium_msg[3] = currentFileName[1];
  Iridium_msg[4] = currentFileName[2];
  Iridium_msg[5] = currentFileName[3];
  Iridium_msg[6] = currentFileName[4];
  Iridium_msg[7] = currentFileName[5];
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Set the GPS GPRMC string in the Iridium message
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void set_GPRMC_Iridium_message(void){
  #if SERIAL_PRINT
    Serial.println(F("GPRMC message to use:"));
    for (int i = 0; i < GPS_rx_buffer_position; i++) {
      Serial.print(GPS_rx_buffer[i]);
    }
    Serial.println();
  #endif

  // add the GPS information into the message string
  for (int i = 0; i < GPS_rx_buffer_position - 7; i++) {
    Iridium_msg[8 + i] = GPS_rx_buffer[i + 7];
  }

  #if SERIAL_PRINT
    Serial.print(F("Iridum_msg length:"));
    Serial.println(GPS_rx_buffer_position + 1);
    for (int i = 0; i < 1 + GPS_rx_buffer_position; i++) { // note: maybe missing last GPRMC character?
      Serial.print(Iridium_msg[i]);
    }
    Serial.println();
  #endif
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Callback to avoid watchdog reset
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
bool ISBDCallback(void){
  wdt_reset();
  return(true);
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Set the GPS GPRMC string in the Iridium message
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void set_GPRMC_Iridium_message(void){
  #if SERIAL_PRINT
    Serial.println(F("GPRMC message to use:"));
    for (int i = 0; i < GPS_rx_buffer_position; i++) {
      Serial.print(GPS_rx_buffer[i]);
    }
    Serial.println();
  #endif

  // add the GPS information into the message string
  for (int i = 0; i < GPS_rx_buffer_position - 7; i++) {
    Iridium_msg[8 + i] = GPS_rx_buffer[i + 7];
  }

  #if SERIAL_PRINT
    Serial.print(F("Iridum_msg length:"));
    Serial.println(GPS_rx_buffer_position + 1);
    for (int i = 0; i < 1 + GPS_rx_buffer_position; i++) { // note: maybe missing last GPRMC character?
      Serial.print(Iridium_msg[i]);
    }
    Serial.println();
  #endif
}
