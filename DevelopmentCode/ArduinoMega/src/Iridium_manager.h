#include <Arduino.h>
#include "IridiumSBD.h"
#include "parameters.h"
#include "GPS_controller.h"
#include "board_manager.h"
#include "SDManager.h"

// information can be found at: http://arduiniana.org/libraries/iridiumsbd/
// The maximum size of a transmitted packet (including header and checksum) is 340 bytes.
// The maximum size of a received packet is 270 bytes.

#ifndef IRIDIUM_MANAGER
#define IRIDIUM_MANAGER

class IridiumManager{
public:
    IridiumManager(HardwareSerial *, GPSController *, BoardManager *, SDManager *);

    void start(void);

    void send_receive_iridium_vital_information(void);

private:
    HardwareSerial * serial_port;

    GPSController * gps_controller;

    BoardManager * board_manager;

    SDManager * sd_manager;

    IridiumSBD iridium_sbd;

    unsigned char buffer_received[IRIDIUM_RECEIVED_PACKET_SIZE];
    unsigned char buffer_transmit[IRIDIUM_TRANSMIT_PACKET_SIZE];

    size_t buffer_received_position;
    size_t buffer_transmit_position;

    /*
      Clean the received buffer and reset pointer
    */
    void clean_reset_buffer_received(void);

    /*
      Clean the transmitted buffer and reset pointer
    */
    void clean_reset_buffer_transmit(void);

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