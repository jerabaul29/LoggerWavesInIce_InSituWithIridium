#include <Arduino.h>
#include "parameters.h"
#include "DebugMacros.h"
#include "SDManager.h"
#include "IridiumManager.h"
#include "BoardManager.h"

#ifndef RASPBERRY_MANAGER
#define RASPBERRY_MANAGER

class RaspberryManager{
  public:
    RaspberryManager(BoardManager *, SDManager *, IridiumManager *, HardwareSerial *);

    void start(void);

    void file_content_to_raspberry(void);

    void receive_processed_data(void);

    void transmit_by_Iridium(void);

    void shutdown(void);

  private:

    BoardManager board_manager;

    SDManager sd_manager;

    IridiumManager iridium_manager;

    HardwareSerial serial_port;

    void flush_buffer_in(void);

    void wait_some_time_ms(unsigned long time_wait);

    bool ask_acknowledge_raspberry(char to_rpi, char expected_answer);

    bool acknowledge_to_raspberry(char from_raspberry, char answer_to_raspberry);
};

#endif //RASPBERRY_MANAGER

/*
#if DEBUG
  SERIAL_DEBUG.println(F("start RPi"));
#endif


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


*/