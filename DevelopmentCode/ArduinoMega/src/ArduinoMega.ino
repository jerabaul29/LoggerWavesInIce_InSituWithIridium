// TODO: Arduino board manager (sleeps, wake up, feedback)
// TODO: prepare VN100 IMU
// TODO: Iridium manager
// TODO: RaspberryPi manager
// TODO: to reduce energy consumption, make the board sleep if nothing to log
// TODO: take a look at the watchdogs
// TODO: check the ISBD callback routine wrt watchdog

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// PRELIMINARY
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// include libraries
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM9DS0.h>
#include "IridiumSBD.h"
#include <stdlib.h>

#include "parameters.h"
#include "GPS_controller.h"
#include "EEPROM_interaction.h"
#include "SDManager.h"

// SD --------------------------------------------------------------------------
SDManager sd_manager{};

// GPS -------------------------------------------------------------------------
GPSController gps_controller{&SERIAL_GPS, &sd_manager};

// TODO: class for the VN100
// TODO: class for interaction with Iridium
// TODO: class for interaction with RaspberryPi
// TODO: not urgent: class for the LSM9DS0

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
// Battery
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// SETUP LOOP
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void setup(){

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
  // SETUP LOGGING
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

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


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// logging loop
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void logging_loop(void){

  // add a small delay to avoid conflicts when reading several
  // ports at possibly 'high' frequency
  delayMicroseconds(10);

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
