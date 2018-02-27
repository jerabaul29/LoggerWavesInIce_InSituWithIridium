

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// PRELIMINARY
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// include libraries
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#include <SPI.h>
#include <Adafruit_GPS.h>
// note: we do not use software serial but it seems to be necessary for initializing
// Adafruit_GPS library
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM9DS0.h>
#include <avr/wdt.h>
#include <avr/sleep.h>          // library for sleep
#include <avr/power.h>          // library for power control
#include "IridiumSBD.h"
#include<stdlib.h>

#include "parameters.h"
#include "GPS_controller.h"
#include "EEPROM_interaction.h"
#include "SDManager.h"

// SD --------------------------------------------------------------------------
SDManager sd_manager{};
GPSController gps_controller;


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// EEPROM
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// EEPROM gestion for sleeping --------------------------------------------------
// address for where number of sleep cycles left stored
int address_sleeps_left = 5;
// address for what is the total number of sleep cycles
int address_total_sleeps = 6;

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// IMU
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Connection of LSM9DS0 as I2C -------------------------------------------------

// Assign a unique base ID for the sensor
Adafruit_LSM9DS0 lsm = Adafruit_LSM9DS0(1000);  // Use I2C, ID #1000

// event object
sensors_event_t accel, mag, gyro, temp;

// configure the sensor: set range
void configureSensor(void)
{
  // 1.) Set the accelerometer range
  lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_2G);
  //lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_4G);
  //lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_6G);
  //lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_8G);
  //lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_16G);

  // 2.) Set the magnetometer sensitivity
  lsm.setupMag(lsm.LSM9DS0_MAGGAIN_2GAUSS);
  //lsm.setupMag(lsm.LSM9DS0_MAGGAIN_4GAUSS);
  //lsm.setupMag(lsm.LSM9DS0_MAGGAIN_8GAUSS);
  //lsm.setupMag(lsm.LSM9DS0_MAGGAIN_12GAUSS);

  // 3.) Setup the gyroscope
  lsm.setupGyro(lsm.LSM9DS0_GYROSCALE_245DPS);
  //lsm.setupGyro(lsm.LSM9DS0_GYROSCALE_500DPS);
  //lsm.setupGyro(lsm.LSM9DS0_GYROSCALE_2000DPS);
}

// how often should sample
//unsigned long interval_sampling_micro = 10000L; // 100 HZ
// unsigned long interval_sampling_micro = 20000L; // 50 HZ
//unsigned long interval_sampling_micro = 50000L; // 20 HZ
// unsigned long interval_sampling_micro = 100000L; // 10 HZ
unsigned long interval_sampling_micro = 200000L; // 5 HZ
// unsigned long interval_sampling_micro = 1000000L; // 1 HZ

// variables for timing when should measure again
unsigned long time_previous_IMU;

// IMU string
String dataStringIMU = "";

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Timing parameters for the logger behavior
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// how often should wake up -----------------------------------------------------
// how many 'wake up cycles' by the power controller currently left before really wake up
// this should be stored in EEPROM, see EEPROM section: address_sleeps_left
uint8_t number_sleeps_left;

// how many 'wake up cycles' should sleep between two real wake ups
// defined in EEPROM so that possible to update by Iridium
// total_number_sleeps_before_wakeup value 0 means that never sleep
// total_number_sleeps_before_wakeup value 1 means that measures and then sleeps
// once (ie sleep typically 20 minutes)
uint8_t total_number_sleeps_before_wakeup;

// how long should log ----------------------------------------------------------
// for production: 25 * 60 * 1000 = 1500000 ms
// #define DURATION_LOGGING_MS 1500000
#define DURATION_LOGGING_MS 20000  // for tests
unsigned long time_start_logging_ms = 1;

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Battery
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#define PIN_MSR_BATTERY A0
#define BAT_EMPTY_V 2.8     // threshold for empty battery

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Iridium
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#define USE_IRIDIUM false

#define PIN_IRD_SLEEP 49
#define SERIAL_IRIDIUM Serial2

String string_battery = "";

IridiumSBD isbd(SERIAL_IRIDIUM, PIN_IRD_SLEEP);
int signalQuality = 255;

char GPS_rx_buffer[128];
int GPS_rx_buffer_position = 0;

int current_n_read_GPS = 0;

char Iridium_msg[340];
int Iridium_msg_position = 0;

uint8_t Ird_rx[270];
size_t Ird_rx_position = sizeof(Ird_rx);
//size_t Ird_rx_position = 0;

int ird_feedback = -1;

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// SETUP LOOP
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void setup(){

  // No power to Raspberry Pi %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  pinMode(PIN_MFT_RPI, OUTPUT);
  digitalWrite(PIN_MFT_RPI, HIGH);

  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // OPEN SERIAL IF NECESSARY
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  #if SERIAL_PRINT || SERIAL_RPI
    Serial.begin(115200);
    while (!Serial) {
      ; // wait for serial port to connect.
    }
  #endif

  #if SERIAL_PRINT
    Serial.println();
    Serial.println();
    Serial.println("D;Booting");
  #endif

  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // SEE IF ENOUGH BATTERY FOR WORKING
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  // necessary to check battery here, because maybe in the case where no sleep at
  // all: then need to check that there is enough battery

  check_battery_level();

  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // SEE IF SHOULD WAKE UP OR JUST SLEEP MORE
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  decide_if_wakeup();

  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // SETUP LOGGING
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  wdt_enable(WDTO_8S);
  wdt_reset();

  // take care of LED PIN_MGA_LED
  pinMode(PIN_MGA_LED, OUTPUT);
  digitalWrite(PIN_MGA_LED, LOW);

  // make SD card ready
  sd_manager.start_sd();
  sd_manager.update_current_file();
  wdt_reset();

  // make GPS ready
  GPSController gps_controller{&SERIAL_GPS, &sd_manager};
  GPS_controller.setup_GPS();
  wdt_reset();

  setup_logging();




  time_tracking_WNF = millis();
  time_start_logging_ms = millis();

  wdt_reset();

  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // SETUP IRIDIUM
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  #if USE_IRIDIUM
    setup_iridium();
  #endif

  wdt_reset();

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// MAIN LOOP
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void loop(){

  wdt_reset();

  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // LOGGING
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  // log for all the required time
  while (millis() - time_start_logging_ms < DURATION_LOGGING_MS){
    wdt_reset();
    logging_loop();
  }

  // when done logging: close logging file
  dataFile.close();

  wdt_reset();

  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // VITAL INFORMATION IRIDIUM
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  // send some diagnostic strings by Iridum, including detailed GPS position.
  // done here as doing it at this point one most likely has a GPS fix and full
  // Iridium capacitors
  #if USE_IRIDIUM
    send_receive_Iridium_vital_information();
  #endif

  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // RASPBERRY PI INTERACTION
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  #if SERIAL_RPI
    raspberry_pi_interaction();
  #endif

  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // EVERYTHING IS DONE: REBOOT
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  // if it is time to sleep, this will be caught by the decide_if_wakeup function
  wdt_enable(WDTO_15MS);
  while(1){
  }
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// FUNCTIONS / OTHER
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// check the battery level
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void check_battery_level(void){
  float battery_level =  float(analogRead(PIN_MSR_BATTERY)) * 5.0 / 1024.0;

  if (battery_level < BAT_EMPTY_V){
    #if SERIAL_PRINT
      Serial.println("D;Low battery!");
    #endif

    // should stop now to try to save the logger
    pinMode(PIN_FBK_MGA, OUTPUT);
    digitalWrite(PIN_FBK_MGA, LOW);

    while(1){
      make_sleep();
    }
  }

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// LED PIN_MGA_LED blink
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// make led 13 on the board blink continuously to report fault
void blinkLED(){
  while (1){
    digitalWrite(PIN_MGA_LED, HIGH);   // turn the LED on
    delay(100);              // wait for a second
    digitalWrite(PIN_MGA_LED, LOW);    // turn the LED
    delay(100);              // wait for a second
  }
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// logging loop
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void logging_loop(void){

  // add a small delay to avoid conflicts when reading several
  // ports at possibly 'high' frequency
  delayMicroseconds(10);

  // GPS stuff %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  // check at each loop if new information from the GPS
  while (SERIAL_GPS.available()>0){

    char c_GPS = GPS.read();

    if (c_GPS=='\n'){

      // reached the end of a string: time to log

      // add millis informations
      dataStringGPS += "\nM,";
      dataStringGPS += String(millis());

      // SD post
      postSD(dataStringGPS);
      #if SERIAL_PRINT
        Serial.println(dataStringGPS);
      #endif

      // re initialize the logging string
      dataStringGPS = "";

    }
    else{
      dataStringGPS += c_GPS;
    }
  }

  wdt_reset();

  // IMU stuff %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  // check if time to do new measurement
  if (micros() - time_previous_IMU >= interval_sampling_micro) {

    time_previous_IMU += interval_sampling_micro;

    // Get a new sensor event
    lsm.getEvent(&accel, &mag, &gyro, &temp);

    /*
    #if SERIAL_PRINT
      Serial.print("D;**********************\n");
      // print out accelleration data
      Serial.print("D;Accel X: "); Serial.print(accel.acceleration.x); Serial.print(" ");
      Serial.print("D;  \tY: "); Serial.print(accel.acceleration.y);       Serial.print(" ");
      Serial.print("D;  \tZ: "); Serial.print(accel.acceleration.z);     Serial.println("  \tm/s^2");

      // print out magnetometer data
      Serial.print("D;Magn. X: "); Serial.print(mag.magnetic.x); Serial.print(" ");
      Serial.print("D;  \tY: "); Serial.print(mag.magnetic.y);       Serial.print(" ");
      Serial.print("D;  \tZ: "); Serial.print(mag.magnetic.z);     Serial.println("  \tgauss");

      // print out gyroscopic data
      Serial.print("D;Gyro  X: "); Serial.print(gyro.gyro.x); Serial.print(" ");
      Serial.print("D;  \tY: "); Serial.print(gyro.gyro.y);       Serial.print(" ");
      Serial.print("D;  \tZ: "); Serial.print(gyro.gyro.z);     Serial.println("  \tdps");

      // print out temperature data
      Serial.print("D;Temp: "); Serial.print(temp.temperature); Serial.println(" *C");

      Serial.println("D;**********************\n");
    #endif
    */

    // generate the string to post on SD card
    generateDataStringIMU();

    // post data
    // SD post
    postSD(dataStringIMU);
    #if SERIAL_PRINT
      Serial.println(dataStringIMU);
    #endif
  }

  wdt_reset();

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// setup if should log
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void setup_logging(void){


  // Initialize IMU %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  #if SERIAL_PRINT
    Serial.println(F("D;IMU initialisation")); Serial.println("");
  #endif

  /* Initialise the sensor */
  if (!lsm.begin())
  {
    #if SERIAL_PRINT
      Serial.print(F("D;No LSM9DS0 detected"));
    #endif
    while (1);
  }
  #if SERIAL_PRINT
    Serial.println(F("D;found LSM9DS0"));
  #endif

  #if SERIAL_PRINT
    Serial.println(F("D;configure sensor"));
  #endif
  configureSensor();

  wdt_reset();
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// decide if should really wake up or just sleep more
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// look at the values contained in EEPROM indicating how many times need to sleep
// use them to decide if should sleep or not
// if number_sleeps_left is 0 when this function called, time to wake up.
void decide_if_wakeup(void){

  // pull up FBK_MEGA
  #if SERIAL_PRINT
    Serial.println(F("D;Put PIN_FBK_MGA HIGH"));
    delay(5);
  #endif

  pinMode(PIN_FBK_MGA, OUTPUT);
  digitalWrite(PIN_FBK_MGA, HIGH);

  // read from EEPROM total number of sleeps
  total_number_sleeps_before_wakeup = EEPROM.read(address_total_sleeps);

  #if SERIAL_PRINT
    Serial.print(F("D;total number sleeps: "));
    Serial.println(total_number_sleeps_before_wakeup);
    delay(5);
  #endif

  // value of the EEPROM: number of more cycles to sleep
  number_sleeps_left = EEPROM.read(address_sleeps_left);

  #if SERIAL_PRINT
    Serial.print(F("D;number_sleeps_left: "));
    Serial.println(number_sleeps_left);
    delay(5);
  #endif

  // if too high value: set back to max number of sleep cycles and sleep
  if (number_sleeps_left > total_number_sleeps_before_wakeup){
    #if SERIAL_PRINT
      Serial.println(F("D;Too high number_sleeps_left: reset"));
      delay(5);
    #endif

    EEPROM.write(address_sleeps_left, total_number_sleeps_before_wakeup);

    digitalWrite(PIN_FBK_MGA, LOW);

    while(1){
      make_sleep();
    }
  }

  // if number_sleeps_left > 0: must sleep more; decrease by one and sleep
  else if (number_sleeps_left > 0){

    #if SERIAL_PRINT
      Serial.println(F("D;Sleep more"));
      delay(5);
    #endif

    EEPROM.write(address_sleeps_left, uint8_t(number_sleeps_left - 1));

    digitalWrite(PIN_FBK_MGA, LOW);

    while(1){
      make_sleep();
    }
  }

  // else, time to wake up
  else {
    #if SERIAL_PRINT
      Serial.println(F("D;Time to wake up!"));
      delay(5);
    #endif

    // write how many sleeping cycles left
    EEPROM.write(address_sleeps_left, total_number_sleeps_before_wakeup);
  }

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// make Arduino Mega sleep
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void make_sleep(void){
  #if SERIAL_PRINT
    Serial.println(F("D;Make Mega sleep"));
    delay(5);
  #endif

  power_all_disable();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  power_adc_disable();
  sleep_enable();
  sleep_mode();
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// setup iridium
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void setup_iridium(void){

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

  // go to sleep
  isbd.sleep();

}

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
// Interaction with Raspberry Pi
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void raspberry_pi_interaction(void){

  wdt_reset();

  // power up the RaspberryPi -----------------------------------
  #if SERIAL_PRINT
    Serial.println(F("D;Boot RPi"));
  #endif

  digitalWrite(PIN_MFT_RPI, LOW);

  wdt_reset();

  // give the time to boot --------------------------------------
  #if SERIAL_PRINT
    Serial.println(F("D;Let RPi boot"));
  #endif

  Serial.flush();

  wdt_reset();

  for (int nbr_seconds=0; nbr_seconds<15; nbr_seconds++){
    delay(1000);
    wdt_reset();
  }

  // flush the buffer -------------------------------------------
  while (Serial.available() > 0){
    Serial.read();
  }

  wdt_reset();

  // check if the RPi has booted --------------------------------

  while(true){

    Serial.print('B');
    delay(100);

    if (Serial.available() > 0){
      char answer = Serial.read();

      if (answer == 'R'){
        break;
      }
    }
  }

  wdt_reset();

  // send commmand updates to the RPi --------------------------
  // TODO: send a list of commands to execute to the RPi. Those commands can
  //       be updated from Iridium

  // send the file name to the RPi -----------------------------
  Serial.print('N');
  for (int i=0; i<6; i++){
    Serial.print(currentFileName[i]);
  }

  Serial.print('N');

  delay(50);

  wdt_reset();

  // send the file content to the RPi ---------------------------
  File dataFile = SD.open(currentFileName);

  // if the file is available, send it all
  if (dataFile) {
    while (dataFile.available()) {
      Serial.write(dataFile.read());
      delayMicroseconds(5);
      wdt_reset();
    }
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    #if SERIAL_PRINT
      Serial.println("D;error opening FILE_TO_DUMP");
    #endif

    while(1){
      // reboot
    }
  }

  #if SERIAL_PRINT
    Serial.println();
    Serial.println(F("D;All data transmitted"));
  #endif

  Serial.print("FINISHED");

  wdt_reset();

  // get back the data to send by Iridium, or shut off if RPi is done ----------
  // TODO
  while(true){
    wdt_reset();

    if (Serial.available() > 0){

      char command_Mega = Serial.read();

      // check if RPi wants to send some Iridium data
      if (command_Mega == 'I'){
        #if SERIAL_PRINT
            Serial.println(F("D;Pi to Iridium"));
        #endif

        // get the number of bytes to transmit
        int number_of_bytes = Serial.parseInt();

        // store the whole message in the transmitt buffer
        int byte_number = 0;
        while (byte_number < number_of_bytes){
          if (Serial.available() > 0){
            Iridium_msg[byte_number] = Serial.read();
            byte_number++;
          }
        }
        Iridium_msg_position = number_of_bytes - 1;

        // TODO: add a delimiter in the communication from the RPi to make sure
        // TODO: that right number of bytes even if first byte evaluates as a number?
        // TODO: other possibility: ask the RPi to always give a chunk of max number (340? check value)
        // TODO: bytes; this is anyway the minimum size of a message

        // try to send the Iridium feedback string ----------------------------------
        // note: retries the operation for up to 300 seconds by default; put watchdog reset in
        // ISBDCallback.
        ird_feedback = isbd.sendReceiveSBDBinary((uint8_t *)Iridium_msg, size_t(Iridium_msg_position),
                                           Ird_rx, Ird_rx_position);  // NOTE: strange that Ird_rx_position; check if should not be
                                                                      // a pointer

        // NOTE: MAYBE SHOULD CONSIDER PUT CHECK FOR COMMANDS FROM IRIDIUM HERE

        // confirm to Raspberry Pi that well transmitted
        Serial.print("R");

      }

      // check RPi is finished
      if (command_Mega == 'F'){
        #if SERIAL_PRINT
            Serial.println(F("D;Shut down RPi"));
        #endif

        // give the time to shut down gracefully
        delay(5000);
        wdt_reset();
        delay(5000);
        wdt_reset();

        digitalWrite(PIN_MFT_RPI, HIGH);

        break;
      }
    }
  }

  #if SERIAL_PRINT
    Serial.println(F("D; Done with RPi"));
  #endif

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// generate the dataStringIMU to be posted on SD card
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void generateDataStringIMU(void){
  dataStringIMU = "";
  dataStringIMU += "I;";
  dataStringIMU += String(accel.acceleration.x);
  dataStringIMU += ";";
  dataStringIMU += String(accel.acceleration.y);
  dataStringIMU += ";";
  dataStringIMU += String(accel.acceleration.z);
  dataStringIMU += ";";
  dataStringIMU += String(gyro.gyro.x);
  dataStringIMU += ";";
  dataStringIMU += String(gyro.gyro.y);
  dataStringIMU += ";";
  dataStringIMU += String(gyro.gyro.z);
  dataStringIMU += ";";
  dataStringIMU += String(mag.magnetic.x);
  dataStringIMU += ";";
  dataStringIMU += String(mag.magnetic.y);
  dataStringIMU += ";";
  dataStringIMU += String(mag.magnetic.z);
  dataStringIMU += ";";
  dataStringIMU += "\nM,";
  dataStringIMU += String(millis());
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
