#include <Arduino.h>
#include "IridiumSBD.h"
#include "parameters.h"
#include "GPSManager.h"
#include "BoardManager.h"
#include "SDManager.h"
#include "DebugMacros.h"

// information can be found at: http://arduiniana.org/libraries/iridiumsbd/
// The maximum size of a transmitted packet (including header and checksum) is 340 bytes.
// The maximum size of a received packet is 270 bytes.

#ifndef IRIDIUM_MANAGER
#define IRIDIUM_MANAGER

class IridiumManager{
public:
    IridiumManager(HardwareSerial *, GPSManager *, BoardManager *, SDManager *);

    void start(void);

    void send_receive_iridium_vital_information(void);

    // TODO: make the following public to allow filling with the Iridium message;
    // this is not too good a solution
    unsigned char buffer_received[IRIDIUM_RECEIVED_PACKET_SIZE];
    unsigned char buffer_transmit[IRIDIUM_TRANSMIT_PACKET_SIZE];
    size_t buffer_received_position;
    size_t buffer_transmit_position;

    /*
      send the content of the transmit buffer. Note that, at the
      lowest level, always send and receive a message (so this is
      why called send_receive, and maybe some information in receive).
    */
    void send_receive(void);

    /*
      Clean the received buffer and reset pointer
    */
    void clean_reset_buffer_received(void);

    /*
      Clean the transmitted buffer and reset pointer
    */
    void clean_reset_buffer_transmit(void);

private:
    HardwareSerial * serial_port;

    GPSManager * gps_controller;

    BoardManager * board_manager;

    SDManager * sd_manager;

    IridiumSBD iridium_sbd;

    /*
      Clean both buffers
    */
    void clean_reset_buffers(void);

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

// TODO: add a function to fill transmit buffer with data and send it.
