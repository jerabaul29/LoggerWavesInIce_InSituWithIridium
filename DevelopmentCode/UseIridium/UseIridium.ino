/*
   Tests about IdidiumSBD library with the RockBlock7

   Pins

   - RockBlock7        to    Arduino Mega
   - Rockblock7 OnOff  to    49
   - Rockblock7 GND    to    Mega GND
   - Rockblock7 5vIn   to    Mega 5V
   - Rockblock7 TXD    to    Mega TX2
   - Rockblock7 RXD    to    Mega RX2

   This means that the D in RXD and TXD means 'Device', i.e. RX of the device (Mega)
  instead of RX of the host (Rockblock7)

   TODO

   - send vital information by Iridium: battery, GPS, file number
   - receive update information from Iridium
   - send additional processed results

*/




// Iridium module: to add to Mega code %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#include "IridiumSBD.h"
#include<stdlib.h>

#define PIN_IRD_SLEEP 49
#define SERIAL_IRIDIUM Serial2

#define PIN_MSR_BATTERY A0
String string_battery = "";

IridiumSBD isbd(SERIAL_IRIDIUM, PIN_IRD_SLEEP);
int signalQuality = 255;

char GPS_rx_buffer[128];
int GPS_rx_buffer_position = 0;

int current_n_read_GPS = 0;

char Iridium_msg[100];
int Iridium_msg_position = 0;

uint8_t Ird_rx[270];
size_t Ird_rx_position = sizeof(Ird_rx);

int ird_feedback = -1;


/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
// Defines that are already in the Mega code %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#define SERIAL_PRINT true

#include <Adafruit_GPS.h>
// note: we do not use software serial but it seems to be necessary for initializing
// Adafruit_GPS library
#include <SoftwareSerial.h>
#include <avr/wdt.h>

// GPS
// config GPS chip and instance creation
#define SERIAL_GPS Serial1
Adafruit_GPS GPS(&SERIAL_GPS);
// GPS string
String dataStringGPS = "";

char currentFileName[] = "F12345";
// number of zeros after the letter in the name convention
int nbrOfZeros = 5;
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void setup() {
  delay(5000);

  // Start the serial port for debugging
#if SERIAL_PRINT
  Serial.begin(115200);
  Serial.println("-- Starting Serial");
#endif

  // setup GPS
  delay(250);

  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);

  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  // GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);

  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  GPS.sendCommand(PMTK_API_SET_FIX_CTL_1HZ);
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  // GPS.sendCommand(PGCMD_ANTENNA);

  delay(1000);

  // Ask for firmware version
  // SERIAL_GPS.println(PMTK_Q_RELEASE);

#if SERIAL_PRINT
  Serial.println("D;GPS initialized");
#endif

  // Iridium setup %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#if SERIAL_PRINT
  Serial.println("D;Starting Iridium SBD object");
#endif

  SERIAL_IRIDIUM.begin(19200);

#if SERIAL_PRINT
  Serial.println("D;Serial to Iridium SBD");
  isbd.attachConsole(Serial);
  isbd.attachDiags(Serial);
#endif

#if SERIAL_PRINT
  Serial.println("D;Iridium power profile");
#endif

  // set power profile
  // 0 when high power supply
  // 1 when low power supply
  isbd.setPowerProfile(0);

#if SERIAL_PRINT
  Serial.println("D;Iridium begin method");
#endif

  // Wake up and prepare to communicate
  isbd.begin();

  // sleep and wake up example:
  /*
    isbd.sleep();
    delay(5000);
    isbd.begin();
  */



  wdt_enable(WDTO_8S);
  wdt_reset();
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void loop() {

  while (1){

    wdt_reset();
    
    if (Serial.available() > 0){
      char c_read;
      c_read = Serial.read();
      if (c_read == 'T'){
        break;
      }
    }
  }

  wdt_reset();

  // get the Iridium feedback string ready
  set_battery_value_Iridium_message();

  set_file_number_Iridium_message();

  obtain_GPRMC_Iridium_message();
  set_GPRMC_Iridium_message();

  // try to send the Iridium feedback string
  // note: retries the operation for up to 300 seconds by default; put watchdog reset in
  // ISBDCallback.
  ird_feedback = isbd.sendReceiveSBDBinary((uint8_t *)Iridium_msg, size_t(1 + GPS_rx_buffer_position),
                                           Ird_rx, Ird_rx_position);

  #if SERIAL_PRINT
    Serial.println((uint8_t)Iridium_msg);
  #endif

  #if SERIAL_PRINT
    if (ird_feedback != 0){
      Serial.print("D;Problem transmitting: ");
      Serial.println(ird_feedback);
    }
    else{
      Serial.println("D;Transmitted well");
    }
  #endif

  wdt_reset();

  //////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////

  // read Iridium command if available

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

  // check if command back
  if (Ird_rx_position > 0){
    #if SERIAL_PRINT
      Serial.println(F("Ird_rx_position > 0"));
    #endif

    // check if should update number of sleep cycles
    // this kind of message should be of the form: SLP12 [set the number of sleep cycles
    // to 12]
    if (char(Ird_rx[0]) == 'S' && char(Ird_rx[1]) == 'L' && char(Ird_rx[2]) == 'P'){
       int value_sleep = uint8_t(Ird_rx[3]);

      #if SERIAL_PRINT
        Serial.print(F("Will sleep for:"));
        Serial.println(value_sleep);
      #endif
    }

    // last hex message: 534c5006

    // check if should send some commands to RPi
  }

  //////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////
}

/*
 * GOT ANSWER IN CONSOLE:
 * 
D;Transmitted well
Inbound buffer size is 5
T(84) e(101) s(115) t(116) 1(49) Messages left: 0
 *
 */

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
// Obtain a GPS GPRMC string to be used in the Iridium message
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void obtain_GPRMC_Iridium_message(void){
  
  // flush GPS buffer
  while (SERIAL_GPS.available() > 0) {
    SERIAL_GPS.read();
  }

  // variable for holding GPS chars
  char c_GPS = '0';

  // try to read at most 10 GPS strings; if no valid GPRMC strings
  // 10 times in a row, just use the last one
  while (current_n_read_GPS < 10) {

    wdt_reset();

    // if this is the beginning of a new GPS message, log it,
    // check if GPRMC and check if valid
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
        if (SERIAL_GPS.available() > 0) {

          c_GPS = GPS.read();

          if (c_GPS == '\n') {
            end_GPS_msg = true;
          }

          else {
            GPS_rx_buffer[GPS_rx_buffer_position] = c_GPS;
            GPS_rx_buffer_position++;
          }
        }
      }

      #if SERIAL_PRINT
        Serial.print(F("D;Read:"));
        for (int i = 0; i < GPS_rx_buffer_position; i++) {
          Serial.print(GPS_rx_buffer[i]);
        }
        Serial.println();
      #endif

      // check if the logged message was GPRMC
      if ((GPS_rx_buffer[0] == '$') && (GPS_rx_buffer[1] == 'G') && (GPS_rx_buffer[2] == 'P') &&
          (GPS_rx_buffer[3] == 'R') && (GPS_rx_buffer[4] == 'M') && (GPS_rx_buffer[5] == 'C')) {

      #if SERIAL_PRINT
          Serial.println("D;GPRMC");
      #endif

        current_n_read_GPS++;
      }

      // check if valid fix; if it is the case, finish here
      if (GPS_rx_buffer[18] == 'A') {
      #if SERIAL_PRINT
          Serial.println("D;Valid");
      #endif

        break;
      }

    }
    // if GPS logging started at the middle of a string,
    // wait for the end of next message
    else if (SERIAL_GPS.available() > 0) {
            c_GPS = GPS.read();
    }

  }
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
    for (int i = 0; i < 1 + GPS_rx_buffer_position; i++) {
      Serial.print(Iridium_msg[i]);
    }
    Serial.println();
  #endif
}

bool ISBDCallback(void){
  wdt_reset();
  return(true);
}

