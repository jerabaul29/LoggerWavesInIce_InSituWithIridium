#include <Arduino.h>
#include "SDManager.h"

#ifndef VN100_MANAGER
#define VN100_MANAGER

// TODO: test if works; possible that does not work because of the char vs byte issue

// for the following configuration of the VN100:
/*
Baud rate      : 57600
Async          : NoOutput
Binary output 1: 
    Async mode    : Serial Port 1
    Rate dividor  : 80
    Common group  : None
    IMU group     : UncompensatedMag
                    UncompensatedAccel
                    UncompensatedGyro
                    Temp
                    Press
    Attitude group: Yaw Pitch Roll
                    DCM
                    MagNed
                    AccelNed
    IMU filtering : 80
*/
// the header is:
// fa 14 3e 00 3a 00
// bytes:
// 0  1  2  3  4  5
#define SIZE_VN100_HEADER 6
#define byte0 0xfa
#define byte1 0x14
#define byte2 0x3e
#define byte3 0x00
#define byte4 0x3a
#define byte5 0x00

class VN100Manager{
    public:
        VN100Manager(HardwareSerial *, SDManager *);

        void start(void); // i

        /*
          Perform logging:
            - check if incoming data
            - if incoming data, record it and check for header
            - if time to log, save on SD card and restaure clean working buffer and header detection
        */
        void perform_logging(void); // i

    private:
        HardwareSerial * serial_port;
        SDManager * sd_manager;

        // some buffering: max length of a message is in theory 123. Take some margin, use 256
        // This is redundant with the incoming buffer of the Mega serial port, but should be ok as long as
        // enough space in RAM
        byte buffer_rx[SIZE_BUFFER_VN100];
        int buffer_rx_position;

        // the header of VN100 data
        const byte header[SIZE_VN100_HEADER] = {byte0, byte1, byte2, byte3, byte4, byte5};
        // how many header bytes have been received
        int number_received_header_bytes;

        /*
          Catch all incoming chars in the RX buffer from the VN100, and
          add them to the buffer.
          In addition, check for the arrival of a header.
          If received a full message, return true. Return false otherwise
        */
        bool catch_message(void);  // i

        /*
          Clear the buffer and header detection
        */
        void reset_buffer_header(void);  // i

        /*
          Post the content of the buffer to the SD card
        */
        void post_to_SD_card(void); // i

        /*
          flush the input serial buffer until the start of a VN message
        */
        void flush_serial_buffer(void); // i

};

#endif

/*
// check at each loop if new information from the IMU
  while (SERIAL_IMU.available()>0){

    byte byte_IMU = SERIAL_IMU.read();
    binaryIMUBuffer[position_buffer] = byte_IMU;
    position_buffer = (position_buffer + 1) % lengthBufferIMU;

    if (checkHeaderIMU(byte_IMU)){
      #if SERIAL_PRINT
      Serial.println();
      Serial.println("Reached a new IMU header end");
      #endif

      postSD_binary(binaryIMUBuffer, lengthIMUBinarDataFrame);

      initIMUBuffer();
    }

  }

  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// check if received a full IMU header
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
bool checkHeaderIMU(byte byte_IMU){

  if (state_header == 255){
    if (byte_IMU == byte0){
      state_header = 0;
      #if SERIAL_PRINT
      Serial.println();
      Serial.print("Received byte0 ");
      Serial.print(byte_IMU, HEX);
      #endif
    }

    else{
      #if SERIAL_PRINT
      Serial.print(byte_IMU, HEX);
      Serial.print(" ");
      #endif

      return false;

    }
  }

  else if (state_header == 0){
    if (byte_IMU == byte1){
      state_header = 1;
      #if SERIAL_PRINT
      Serial.println();
      Serial.print("Received byte1 ");
      Serial.print(byte_IMU, HEX);
      #endif
    }
    else{
      state_header = 255;
      #if SERIAL_PRINT
      Serial.println();
      Serial.print("Received wrong byte!");
      #endif
    }
  }

  else if (state_header == 1){
    if (byte_IMU == byte2){
      state_header = 2;
      #if SERIAL_PRINT
      Serial.println();
      Serial.print("Received byte2 ");
      Serial.print(byte_IMU, HEX);
      #endif
    }
    else{
      state_header = 255;
      #if SERIAL_PRINT
      Serial.println();
      Serial.print("Received wrong byte!");
      #endif
    }
  }

  else if (state_header == 2){
    if (byte_IMU == byte3){
      state_header = 3;
      #if SERIAL_PRINT
      Serial.println();
      Serial.print("Received byte3 ");
      Serial.print(byte_IMU, HEX);
      #endif
    }
    else{
      state_header = 255;
      #if SERIAL_PRINT
      Serial.println();
      Serial.print("Received wrong byte!");
      #endif
    }
  }

  else if (state_header == 3){
    if (byte_IMU == byte4){
      state_header = 4;
      #if SERIAL_PRINT
      Serial.println();
      Serial.print("Received byte4 ");
      Serial.print(byte_IMU, HEX);
      #endif
    }
    else{
      state_header = 255;
      #if SERIAL_PRINT
      Serial.println();
      Serial.print("Received wrong byte!");
      #endif
    }
  }

  else if (state_header == 4){
    if (byte_IMU == byte5){

      state_header = 255;

      #if SERIAL_PRINT
      Serial.println();
      Serial.print("Received byte5 ");
      Serial.print(byte_IMU, HEX);
      Serial.println();
      #endif

      return true;

    }
    else{
      state_header = 255;
      #if SERIAL_PRINT
      Serial.println();
      Serial.print("Received wrong byte!");
      #endif
    }
  }

  return false;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// initialize the IMU binary buffer to make it ready to a new string to log
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void initIMUBuffer(){
  binaryIMUBuffer[0] = byte0;
  binaryIMUBuffer[1] = byte1;
  binaryIMUBuffer[2] = byte2;
  binaryIMUBuffer[3] = byte3;
  binaryIMUBuffer[4] = byte4;
  binaryIMUBuffer[5] = byte5;
  position_buffer = 6;

  #if SERIAL_PRINT
  Serial.println("Initialized binary buffer IMU");
  #endif
}
*/