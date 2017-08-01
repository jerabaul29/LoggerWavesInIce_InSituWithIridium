/*
 * A logger using Arduino Mega, Adafruit GPS, Adafruit SD, Adafruit LSM9DS0, RockBlock7
 * and connecting to RaspberryPi for processing
 * 
 * PINS:
 * 
 * -- Misc.
 * A0 to batt + (measure battery)
 * 
 * -- Adafruit GPS:
 * GND, Vin (5V)
 * TX to RX1
 * RX to TX1
 * 
 * -- Adafruit SD:
 * 5V, GND
 * CLK to 52
 * DO to 50
 * DI to 51
 * CS to 53
 * 
 * -- Adafruit LSM9DS0:
 * GND, Vin (5V)
 * SCL to SCL
 * SDA to SDA
 * 
 * -- RockBlock7
 * GND, Vin (5V)
 * Sleep (On Off) to 49 
 * TXD to Mega TX2
 * RXD to Mega RX2
 * 
 * CONVENTIONS:
 * - S,: message about the Start of the file: booting, or new file timer
 * - M,: timestamp in Arduino internal clock reference frame
 * - R,: raw data about battery level
 * - C,: converted data about battery level
 * - B,: binary data
 * - $G: start of GPS data. When parsing, this is turned into a _G and _Gt file. Problematic lines (ie lines for which parsing
do not work) are stored in a _P (and timestamps in a _Pt) file.
 * - I;: start IMU data
 * - D;: debugging data
 * - P:: commands for the Raspberry Pi
 * 
 */

 /*
  * TODO:
  * 
  * receive commands from Iridium
  * RaspberryPi
  * Iridium
  * 
  * Improve the logging by average of oversampling?
  * Use an extended buffer Arduino Mega core
  * 
  * open serial port if Raspberry pi alone only later on?
  * 
  * wake up Iridium modem
  * wake up RPi, send last data file, get back processed data
  * find a (if possible, valid) GPS position string; wait at most N=20 GPS messages to find a valid
  * if necessary, wait a bit more for Iridium to be ready
  * send critical information by Iridium: GPS. battery, current file number, other (?)
  * send data received back from the Pi
  * update all parameters following instructions by Iridium
  * 
  * 
  * RaspberryPi
  * Iridium
  * clean
  * 
  * put logging and post logging in loops
  * 
  */

/*
 * IDEAS:
 * 
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * Wake-up
 * 
 * Power control should give power each 5 minutes
 * 
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * Keep track of configuration
 * 
 * Mega:
 * -- how often to wake up
 * -- battery limit to not wake up at all
 * 
 * RPi:
 * -- what processing to do
 * -- what information to send back by iridium
 * 
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * Iridium:
 * 
 * controls to the logger:
 * -- to the Mega:
 * ------ number of cycles to sleep
 * ------ dump lines number A to B in a SD file F
 * -- to the RPi:
 * ------ which processing to do
 * ------ which processing to send back
 * ------ send back old processing
 * 
 * data from the logger:
 * -- about the logger
 * ------ battery voltage
 * ------ if solar panel is connected
 * ------ GPS position (if can get a fix)
 * ------ current file index used (the one when waking up)
 * 
 * -- about the files
 * ------ dump of file F lines A to B
 * 
 * -- about the RPi
 * ------ send the result of the analysis
 *
 */

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// PRELIMINARY
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// include libraries
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#include <SPI.h>
#include <SD.h>
#include <Adafruit_GPS.h>
// note: we do not use software serial but it seems to be necessary for initializing
// Adafruit_GPS library
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM9DS0.h>
#include <avr/wdt.h>
#include <avr/sleep.h>          // library for sleep
#include <avr/power.h>          // library for power control
#include "IridiumSBD.h"
#include<stdlib.h>

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// SD card
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// slave select on Arduino Mega: this is for the SPI driven SD card use
const int chipSelect = 53;
// name of the file on which writting in the SD card library type
File dataFile;
// for printing information about all files present on SD card
File root;

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// GPS
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// config GPS chip and instance creation
#define SERIAL_GPS Serial1
Adafruit_GPS GPS(&SERIAL_GPS);
// GPS string
String dataStringGPS = "";

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// SD card filename
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// filename, will be modified based on EEPROM data for saving on several files
char currentFileName[] = "F00000";
// number of zeros after the letter in the name convention
int nbrOfZeros = 5;
// time in milliseconds after which write to a new file, 900 s is 900 000 milliseconds is 15 minutes
//#define TIME_WNF 900000
#define TIME_WNF 960000 // put some margin relatively to duration_logging_ms
//#define TIME_WNF 60000
// time tracking variable for writting new file
unsigned long time_tracking_WNF = 1;

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// EEPROM
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// EEPROM gestion for filename on SD card ---------------------------------------
// address in which stores the current file numbering; this will extend from
// this address to 4 more bytes (use long int)
int address_numberReset = 1; // ie EEPROM bytes 1, 2, 3, and 4 are used

// EEPROM gestion for sleeping --------------------------------------------------
// address for where number of sleep cycles left stored
int address_sleeps_left = 5;

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
// Feedback Mega
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// feedback mega pin: high as long as the Mega should get power
#define PIN_FBK_MGA 48

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Timing parameters for the logger behavior
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// how often should wake up -----------------------------------------------------
// how many 'wake up cycles' by the power controlleft before really wake up
// this should be stored in EEPROM, see EEPROM section: address_sleeps_left
int number_sleeps_left;
// how many 'wake up cycles' should sleep between two real wake ups
// this can be either hard coded
#define TOTAL_NUMBER_SLEEPS_BEFORE_WAKEUP 1
// or defined in EEPROM so that possible to update by Iridium
// TODO

// how long should log ----------------------------------------------------------
#define DURATION_LOGGING_MS 930000
unsigned long time_start_logging_ms = 1;

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Some parameters for serial
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Parameters about serial connections on Serial (USB port) ---------------------

// for debugging: print strings about actions on serial
#define SERIAL_PRINT true
// for connection with the Raspberry Pi
#define SERIAL_RPI false

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Iridium
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
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

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// SETUP LOOP
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void setup(){

  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // OPEN SERIAL IF NECESSARY
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // #if SERIAL_PRINT || SERIAL_RPI
  #if SERIAL_PRINT
    // let the time to open computer serial if needed
    delay(5000);
    // Open serial communications and wait for port to open:
    Serial.begin(115200);
  #endif
  
  #if SERIAL_PRINT
    while (!Serial) {
      ; // wait for serial port to connect.
    }
    Serial.println();
    Serial.println();
    Serial.println("D;Booting");
  #endif

  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // SEE IF SHOULD WAKE UP OR JUST SLEEP MORE
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  decide_if_wakeup();

  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // SETUP LOGGING
  // NOTE: watchdog is activated after here
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  setup_logging();

  
  time_tracking_WNF = millis();
  time_start_logging_ms = millis();

  wdt_reset();

  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // SETUP IRIDIUM
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  setup_iridium();

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

  while (millis() - time_start_logging_ms < DURATION_LOGGING_MS){
    wdt_reset();
    logging_loop();
  }

  // close logging file
  dataFile.close();

  wdt_reset();
  
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // POST LOGGING
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  // ------------------------------------------------------------------------------
  // send the vital information Iridium messages ----------------------------------

  send_Iridium_vital_information();

  

  // ------------------------------------------------------------------------------
  // receive the Iridium instructions and parse -----------------------------------

  // start booting RPi ------------------------------------------------------------

  // set Iridium up; record when set up (note: do it earlier? Wait at least 120s --
  // before trying to send messages)

  // wait for RPi to be booted and ready ------------------------------------------

  // send last data file to RPi ---------------------------------------------------
  // this is file currentFileName[]

  // send vital information by Iridium --------------------------------------------
  // get vital information
  // battery level

  // GPS position
  // try to get a fix for at most one minute

  // assemble information

  // try to send for at most five minutes
  

  // wait for answer from Pi being ready to transmit by Iridium -------------------

  // send all RPi data by Iridium -------------------------------------------------

  // try to send each message at most 5 times; if does not work at some point,
  // abort all following attempts

  // receive commands from Iridium ------------------------------------------------

  // try at most 5 times

  // update Mega setup with commands ----------------------------------------------

  // send update orders to RPi ----------------------------------------------------

  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // EVERYTHING IS DONE: SLEEP OR REBOOT
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  if (TOTAL_NUMBER_SLEEPS_BEFORE_WAKEUP == 0){
    #if SERIAL_PRINT
      Serial.println(F("D;Do not sleep; log again at once"));
    #endif

    // reboot
    wdt_enable(WDTO_15MS);     // enable the watchdog, 15 ms
    while(1){
    }
  }

  else{

    #if SERIAL_PRINT
      Serial.println(F("D;Ask no more current and sleep"));
    #endif
    
    pinMode(PIN_FBK_MGA, INPUT);

    wdt_disable();

    while (1){
      make_sleep();
    }
  }
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// FUNCTIONS / OTHER
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// LED 13 blink
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// make led 13 on the board blink continuously to report fault
void blinkLED(){
  while (1){
    digitalWrite(13, HIGH);   // turn the LED on
    delay(100);              // wait for a second
    digitalWrite(13, LOW);    // turn the LED
    delay(100);              // wait for a second
  }
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// SD card list files
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print("\t");
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// log a string on the SD card
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void postSD(String dataStringPost){

  // note: delays are here to avoid conflicts on using the SD card
  // when using it at high frequency

  #if SERIAL_PRINT
    unsigned long startLog = millis();
    Serial.println();
    Serial.println("D;Start post...");
    Serial.print("D;Time log beginning: ");
    Serial.println(startLog);
  #endif

  // decide if time to write to a new file
  if (millis() - time_tracking_WNF > TIME_WNF){
    // update time tracking
    time_tracking_WNF += TIME_WNF;
    #if SERIAL_PRINT
      Serial.println("D;Update file name because timer");
    #endif
    UpdateCurrentFile();
    dataFile.println("S,New file created (timer)\n");
  }

  // if the file is available, write to it:
  if (dataFile) {
    dataStringPost += '\n';
    dataFile.println(dataStringPost);

    delay(5);

    #if SERIAL_PRINT
      Serial.println("D;post success, content:");
      Serial.print(dataStringPost);
    #endif
  }
  // else the file isn't open, pop up an error:
  // and reboot
  else {
    #if SERIAL_PRINT
    Serial.println("D;post failure");
    #endif
    while(1){
      // do nothing watchdog fires
    }
  }

  #if SERIAL_PRINT
    unsigned long endLog = millis();
    Serial.print("D;Time log end: ");
    Serial.println(endLog);
    Serial.print("D;Time log delay: ");
    Serial.println(endLog-startLog);
  #endif

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// read from EEPROM the name of the next file on which to write, update the EEPROM
// and open the new current file
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void UpdateCurrentFile(){

  // note: could be problem at initialization in the setup loop, no file opened yet
  // but the library is handling the exception right
  // close current file
  delay(5);
  dataFile.close();
  delay(5);

  // read the current file number in EEPROM
  long value_before_fileIndex = EEPROMReadlong(address_numberReset);

  // update it by increasing by 1. This is the new file number to write on
  long new_value_fileIndex = value_before_fileIndex + 1;
  EEPROMWritelong(address_numberReset,new_value_fileIndex);

  // generate the string to put as the file numbering
  String str_index = String(new_value_fileIndex);
  int str_length = str_index.length();

  // put the characters of the name at the right place
  for (int ind_rank = 0; ind_rank < str_length; ind_rank++){
    int i_rank = nbrOfZeros + ind_rank - str_length + 1;
    currentFileName[i_rank] = str_index[ind_rank];
  }

  #if SERIAL_PRINT
    Serial.print("D;str_rank: ");
    Serial.println(str_index);
    Serial.print("D;File name: ");
    Serial.println(currentFileName);
  #endif

  delay(5);
  // open the file. only one file can be open at a time,
  dataFile = SD.open(currentFileName, FILE_WRITE);
  delay(5);
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Write a 4 byte (32bit) long to the eeprom splitted on 4 bytes
// at the specified address up to address + 3
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void EEPROMWritelong(int address, long value)
{
  //Decomposition from a long to 4 bytes by using bitshift.
  //One = Most significant -> Four = Least significant byte
  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  byte two = ((value >> 16) & 0xFF);
  byte one = ((value >> 24) & 0xFF);

  //Write the 4 bytes into the eeprom memory.
  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  EEPROM.write(address + 2, two);
  EEPROM.write(address + 3, one);
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// read 4 bytes of memory from the EEPROM (address up to address + 3)
// assemble the 4 bytes to form a long variable
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
long EEPROMReadlong(long address)
{
  //Read the 4 bytes from the eeprom memory.
  long four = EEPROM.read(address);
  long three = EEPROM.read(address + 1);
  long two = EEPROM.read(address + 2);
  long one = EEPROM.read(address + 3);

  //Return the recomposed long by using bitshift.
  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
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

  // enable watchdog, if 8 seconds hang %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  wdt_enable(WDTO_8S);
  wdt_reset();

  // setup SD card %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    #if SERIAL_PRINT
      Serial.println("D;Card failed, or not present");
    #endif
    // blink to indicate failure
    blinkLED();
  }
  #if SERIAL_PRINT
    Serial.println("D;card initialized.");
  #endif

  #if SERIAL_PRINT
    root = SD.open("/");
    printDirectory(root, 0);
    Serial.println("D;done!");
  #endif
  
  wdt_reset();
  
  // setup GPS %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
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

  wdt_reset();

  // update the name of the file to write on %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  // update the EEPROM
  // name of the current file on which writting on the SD card, print that
  // booted from beginning, print battery state
  UpdateCurrentFile();
  dataFile.println("S,Booting from beginning!\n");

  wdt_reset();

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

void decide_if_wakeup(void){

  // pull up FBK_MEGA
  #if SERIAL_PRINT
    Serial.println(F("D;Put PIN_FBK_MGA HIGH"));
    delay(5);
  #endif
  
  pinMode(PIN_FBK_MGA, OUTPUT);
  digitalWrite(PIN_FBK_MGA, HIGH);
  
  // value of the EEPROM: number of more cycles to sleep
  number_sleeps_left = int(EEPROM.read(address_sleeps_left));

  #if SERIAL_PRINT
    Serial.print(F("D;number_sleeps_left: "));
    Serial.println(number_sleeps_left);
    delay(5);
  #endif

  // if too high value: set back to max number of sleep cycles and sleep
  if (number_sleeps_left > TOTAL_NUMBER_SLEEPS_BEFORE_WAKEUP){
    #if SERIAL_PRINT
      Serial.println(F("D;Too high number_sleeps_left: reset"));
      delay(5);
    #endif
  
    EEPROM.write(address_sleeps_left, TOTAL_NUMBER_SLEEPS_BEFORE_WAKEUP);

    pinMode(PIN_FBK_MGA, INPUT);

    while(1){
      make_sleep();
    }
  }

  // if ok value >1: must sleep more; decrease by one and sleep
  else if (number_sleeps_left > 1){
    
    #if SERIAL_PRINT
      Serial.println(F("D;Sleep more"));
      delay(5);
    #endif
    
    EEPROM.write(address_sleeps_left, number_sleeps_left - 1);

    pinMode(PIN_FBK_MGA, INPUT);

    while(1){
      make_sleep();
    }
  }

  // if <2: time to wake up
  else if (number_sleeps_left < 2){
    #if SERIAL_PRINT
      Serial.println(F("D;Time to wake up!"));
      delay(5);
    #endif
    
    EEPROM.write(address_sleeps_left, TOTAL_NUMBER_SLEEPS_BEFORE_WAKEUP);
  }

  else{
    #if SERIAL_PRINT
      Serial.println(F("D;Something wrong"));
      delay(5);
    #endif

    EEPROM.write(address_sleeps_left, TOTAL_NUMBER_SLEEPS_BEFORE_WAKEUP - 1);

    pinMode(PIN_FBK_MGA, INPUT);

    while(1){
      make_sleep();
    }
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
// send Iridium vital information
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void send_Iridium_vital_information(void){
  wdt_reset();

  // wake up the Iridium
  isbd.begin();

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
  if (SERIAL_GPS.available() > 0) {
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


