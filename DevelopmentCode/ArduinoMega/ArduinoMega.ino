/*
 * A logger using Arduino Mega, Adafruit GPS, Adafruit SD, Adafruit LSM9DS0, RockBlock7
 * and connecting to RaspberryPi for processing
 * 
 * PINS:
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
 */

 /*
  * TODO:
  * clean
  * watchodog
  * RaspberryPi
  * Iridium
  */

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// include the libraries
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#include <SPI.h>
#include <SD.h>
#include <Adafruit_GPS.h>
// note: we do not use software serial
// but it seems to be necessary for initializing
// Adafruit_GPS library
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM9DS0.h>
#include <avr/wdt.h>

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// SD card
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// slave select on Arduino Mega
// this is for the SPI driven SD card
const int chipSelect = 53;
// name of the file on which writting in the SD card library type
File dataFile;

// for tests
File root;

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// GPS def
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// config GPS chip and instance creation
#define SERIAL_GPS Serial1
Adafruit_GPS GPS(&SERIAL_GPS);
// GPS string
String dataStringGPS = "";

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// filename gestion
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// filename, will be modified based on EEPROM data for saving on several files
char currentFileName[] = "F00000";
int nbrOfZeros = 5; // number of zeros after the letter in the name convention
// time in milliseconds after which write to a new file
//                      900 s is 900 000 milliseconds is 15 minutes
//#define time_WNF 900000
#define time_WNF 60000
//#define time_WNF 10000
// timer for writting new file
unsigned long timer_WNF = 1;

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// EEPROM gestion
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// address in which stores if the EEPROM has been used to generate file numbers before
int address_init = 0;
// address in which stores the current file numbering
int address_numberReset = 1;

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// IMU gestion
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Connection of LSM9DS0 as I2C.

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
//unsigned long interval_sampling_micro = 100000L; // 10 HZ
unsigned long interval_sampling_micro = 1000000L; // 1 HZ
unsigned long time_previous;
unsigned long time_current;

// IMU string
String dataStringIMU = "";

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Some parameters
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#define SERIAL_PRINT true

void setup(){
  // let the time to open computer serial if needed %%%%%%%%%%%%%%%%%%%%%%%%%%%%
  delay(5000);

  // enable watchdog, if 8 seconds hang %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  wdt_enable(WDTO_8S);
  wdt_reset();

  // open serial %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  #if SERIAL_PRINT
    // Open serial communications and wait for port to open:
    Serial.begin(115200);
    while (!Serial) {
      ; // wait for serial port to connect.
    }
    Serial.println();
    Serial.println();
    Serial.println("Booting");
    // check if everyhting is normal with Mega speed
    int time1 = millis();
    Serial.println();
    Serial.print("Time1: ");
    Serial.print(time1);
    time1 = millis();
    Serial.println();
    Serial.print("Time2: ");
    Serial.print(time1);
    Serial.println();
  #endif
  
  wdt_reset();

  // setup SD card %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    #if SERIAL_PRINT
      Serial.println("Card failed, or not present");
    #endif
    // blink to indicate failure
    blinkLED();
  }
  #if SERIAL_PRINT
    Serial.println("card initialized.");
  #endif

  #if SERIAL_PRINT
    root = SD.open("/");
    printDirectory(root, 0);
    Serial.println("done!");
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
    Serial.println("GPS initialized");
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
    Serial.println(F("IMU initialisation")); Serial.println("");
  #endif

  /* Initialise the sensor */
  if (!lsm.begin())
  {
    #if SERIAL_PRINT
      Serial.print(F("No LSM9DS0 detected"));
    #endif
    while (1);
  }
  #if SERIAL_PRINT
    Serial.println(F("found LSM9DS0"));
  #endif

  #if SERIAL_PRINT
    Serial.println(F("configure sensor"));
  #endif
  configureSensor();

  wdt_reset();
  
}

void loop(){

  wdt_reset();

  // add a small delay to avoid conflicts when reading several
  // ports at possibly high frequency
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
  time_current = micros();
  if (time_current - time_previous >= interval_sampling_micro) {

    time_previous += interval_sampling_micro;

    // Get a new sensor event
    lsm.getEvent(&accel, &mag, &gyro, &temp);

    #if SERIAL_PRINT
      Serial.print("**********************\n");
      // print out accelleration data
      Serial.print("Accel X: "); Serial.print(accel.acceleration.x); Serial.print(" ");
      Serial.print("  \tY: "); Serial.print(accel.acceleration.y);       Serial.print(" ");
      Serial.print("  \tZ: "); Serial.print(accel.acceleration.z);     Serial.println("  \tm/s^2");

      // print out magnetometer data
      Serial.print("Magn. X: "); Serial.print(mag.magnetic.x); Serial.print(" ");
      Serial.print("  \tY: "); Serial.print(mag.magnetic.y);       Serial.print(" ");
      Serial.print("  \tZ: "); Serial.print(mag.magnetic.z);     Serial.println("  \tgauss");
  
      // print out gyroscopic data
      Serial.print("Gyro  X: "); Serial.print(gyro.gyro.x); Serial.print(" ");
      Serial.print("  \tY: "); Serial.print(gyro.gyro.y);       Serial.print(" ");
      Serial.print("  \tZ: "); Serial.print(gyro.gyro.z);     Serial.println("  \tdps");

      // print out temperature data
      Serial.print("Temp: "); Serial.print(temp.temperature); Serial.println(" *C");

      Serial.println("**********************\n");
    #endif

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

  // RPI stuff %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  // IRD stuff %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  // SLEEPING DECISION stuff %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  
}

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
      Serial.print('\t');
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
  Serial.println("Start post...");
  Serial.print("Time log beginning: ");
  Serial.println(startLog);
  #endif

  // decide if time to write to a new file
  if (millis() - timer_WNF > time_WNF){
    // reset timer
    timer_WNF = millis();
    #if SERIAL_PRINT
    Serial.println("Update file name because timer");
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
    Serial.println("post success, content:");
    Serial.print(dataStringPost);
    #endif
  }
  // if the file isn't open, pop up an error:
  // and reboot
  else {
    #if SERIAL_PRINT
    Serial.println("post failure");
    #endif
    while(1){
      // do nothing watchdog fires
    }
  }

  #if SERIAL_PRINT
  unsigned long endLog = millis();
  Serial.print("Time log end: ");
  Serial.println(endLog);
  Serial.print("Time log delay: ");
  Serial.println(endLog-startLog);
  #endif

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// reads from EEPROM the name of the next file on which to write and updates the EEPROM
// open the new current file
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
  Serial.print("str_rank: ");
  Serial.println(str_index);
  Serial.print("File name: ");
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
// read 4 bytes of memory from the EEPROM (address up to address + 4)
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
void generateDataStringIMU(){
  dataStringIMU = "";
  dataStringIMU += "IMU;";
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

