/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
// Already present in Mega code

#include <SPI.h>
#include <SD.h>
#include <EEPROM.h>
#include <avr/wdt.h>

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
// SD card filename
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// filename, will be modified based on EEPROM data for saving on several files
char currentFileName[] = "F00113";

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Some parameters for serial
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Parameters about serial connections on Serial (USB port) ---------------------

// for debugging: print strings about actions on serial
#define SERIAL_PRINT false
// for connection with the Raspberry Pi
#define SERIAL_RPI true

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

void setup(){

  /////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////
  // Already present in Mega code

  #if SERIAL_PRINT || SERIAL_RPI
  // #if SERIAL_PRINT
    // let the time to open computer serial if needed
    delay(5000);
    // Open serial communications and wait for port to open:
    Serial.begin(115200);
  #endif

  // enable watchdog, if 8 seconds hang %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  wdt_enable(WDTO_8S);
  wdt_reset();

  // setup SD card %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    #if SERIAL_PRINT
      Serial.println("D;Card failed, or not present");
    #endif
  }
  #if SERIAL_PRINT
    Serial.println("D;card initialized.");
  #endif
  
  wdt_reset();

  /////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////
  
}

void loop(){

  // power up the RaspberryPi -----------------------------------
  #if SERIAL_PRINT
    Serial.println(F("D;Boot RPi"));
  #endif
  
  // TODO

  // give the time to boot --------------------------------------
  #if SERIAL_PRINT
    Serial.println(F("D;Let RPi boot"));
  #endif
  
  delay(5000);
  wdt_reset();
  delay(5000);
  wdt_reset();
  delay(5000);
  wdt_reset();
  
  // check if the RPi has booted --------------------------------
  // put watchdog reset or not?
  Serial.print('B');
  
  while(true){

    if (Serial.available() > 0){
      char answer = Serial.read();

      if (answer == 'R'){
        break;
      }
    }
  }

  wdt_reset();

  // send commmand updates to the RPi --------------------------
  // TODO

  // send the file name to the RPi -----------------------------
  Serial.print('N');
  Serial.print(currentFileName[0]);
  Serial.print(currentFileName[1]);
  Serial.print(currentFileName[2]);
  Serial.print(currentFileName[3]);
  Serial.print(currentFileName[4]);
  Serial.print(currentFileName[5]);

  delay(500);

  wdt_reset();

  // send the file content to the RPi ---------------------------
  File dataFile = SD.open(currentFileName);

  // if the file is available, read it to the Arduino:
  Serial.print('N');

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
      Serial.println("error opening FILE_TO_DUMP");
    #endif
  }

  #if SERIAL_PRINT
    Serial.println();
    Serial.println(F("D;All data transmitted"));
  #endif

  Serial.print("FINISHED");

  // get back the data to send by Iridium, or shut off if RPi is done --------------
  while(true){
    wdt_reset();

    if (Serial.available() > 0){
      char command_Mega = Serial.read();

      // RPi wants to send some Iridium data
      if (command_Mega == 'I'){
        #if SERIAL_PRINT
            Serial.println(F("D;Pi to Iridium"));
        #endif
        
        // TODO
      }

      // RPi is finished
      if (command_Mega == 'F'){
        #if SERIAL_PRINT
            Serial.println(F("D;Shut down RPi"));
        #endif

        // shut down current
        // TODO

        break;
      }
    }
  }

  #if SERIAL_PRINT
    Serial.println(F("D; Done with RPi"));
  #endif

  ////////////////////// just for this test function ////////////////////////
  while(true){
    wdt_reset();
  }
}

