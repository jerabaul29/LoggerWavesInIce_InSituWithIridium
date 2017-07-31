/*
 * Tests about IdidiumSBD library with the RockBlock7
 * 
 * Pins
 * 
 * - RockBlock7        to    Arduino Mega
 * - Rockblock7 OnOff  to    49
 * - Rockblock7 GND    to    Mega GND
 * - Rockblock7 5vIn   to    Mega 5V
 * - Rockblock7 TXD    to    Mega TX2
 * - Rockblock7 RXD    to    Mega RX2
 * 
 * This means that the D in RXD and TXD means 'Device', i.e. RX of the device (Mega)
instead of RX of the host (Rockblock7)
 *
 * TODO
 * 
 * - send vital information by Iridium: battery, GPS, file number
 * - receive update information from Iridium
 * - send additional processed results
 * 
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

unsigned char GPS_rx_buffer[128];
unsigned int GPS_rx_buffer_position = 0;

int current_n_read_GPS = 0;

unsigned char Iridium_msg[100];
unsigned int Iridium_msg_position = 0;


/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
// Defines that are already in the Mega code %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#define SERIAL_PRINT true

#include <Adafruit_GPS.h>
// note: we do not use software serial but it seems to be necessary for initializing
// Adafruit_GPS library
#include <SoftwareSerial.h>

// GPS
// config GPS chip and instance creation
#define SERIAL_GPS Serial1
Adafruit_GPS GPS(&SERIAL_GPS);
// GPS string
String dataStringGPS = "";

char currentFileName[] = "F00000";
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




  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
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

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////




}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void loop() {

  // measure battery
  string_battery = String(analogRead(PIN_MSR_BATTERY), DEC);

  #if SERIAL_PRINT
    Serial.println(F("Battery reading"));
    Serial.print(string_battery[0]);
    Serial.print(string_battery[1]);
    Serial.println(string_battery[2]);
  #endif

  Iridium_msg[0] = string_battery[0]
  Iridium_msg[1] = string_battery[1]
  Iridium_msg[2] = string_battery[2]
  
  /*
  

  // get file number

  // flush GPS buffer
  while (SERIAL_GPS.available()>0){
    SERIAL_GPS.read();
  }

  // try to read at most 10 GPS strings
  while (current_n_read_GPS < 10){

    // wait for data to arrive and log one whole GPS message
    if (SERIAL_GPS.available()>0){

      char c_GPS = GPS.read();

      // if this is the beginning of a new GPS message, log it
      if (c_GPS=='\n'){

        // init buffer position
        GPS_rx_buffer_position = 0;

        bool end_GPS_msg = false;

        // log the whole message
        while (~end_GPS_msg){
          if (SERIAL_GPS.available()>0){

            if (c_GPS == '\n'){
              end_GPS_msg = true;
            }

            else{
              c_GPS = GPS.read();
              GPS_rx_buffer[GPS_rx_buffer_position] = c_GPS;
              GPS_rx_buffer_position++;
            }          
          }
        }
      }            
    }

    // check if the logged message was GPRMC
    if (GPS_rx_buffer[0] == "$" && GPS_rx_buffer[1] == "G" && GPS_rx_buffer[2] == "P" &&
        GPS_rx_buffer[3] == "R" && GPS_rx_buffer[4] == "M" && GPS_rx_buffer[5] == "C"){
        current_n_read_GPS++;
    }

    // check if valid fix; if it is the case, finish here
    if (GPS_rx_buffer[18] == 'A'{
      break;
    }
  }

  // add the GPS information into the message string

  */
  

  delay(20000);

}
