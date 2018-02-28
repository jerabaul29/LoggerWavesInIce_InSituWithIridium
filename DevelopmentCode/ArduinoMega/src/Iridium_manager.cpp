#include "Iridium_manager.h"

IridiumManager::IridiumManager(HardwareSerial * serial_port, GPSController * gps_controller, BoardManager * board_manager, SDManager * sd_manager) :
    serial_port(serial_port),
    gps_controller(gps_controller),
    board_manager(board_manager),
    sd_manager(sd_manager),
    iridium_sbd{IridiumSBD(SERIAL_IRIDIUM, PIN_IRD_SLEEP)}
    {

        IridiumManager::clean_reset_buffer_received();
        IridiumManager::clean_reset_buffer_transmit();

        serial_port.begin(19200);
        iridium_sbd.setPowerProfile(IRIDIUM_HIGH_POWER_SUPPLY);

        iridium_sbd.begin();
        iridium_sbd.sleep();  // go to sleep: will not be needed in quite some time.

    }

void IridiumManager::clean_reset_buffer_received(void){
    for (int i = 0; i < IRIDIUM_RECEIVED_PACKET_SIZE; i++){
            buffer_received[i] = '0';
        }

    buffer_received_position = 0;

}

void IridiumManager::clean_reset_buffer_transmit(void){
    for (int i = 0; i < IRIDIUM_TRANSMIT_PACKET_SIZE; i++){
            buffer_transmit[i] = '0';
        }

    buffer_transmit_position = 0;
}

void IridiumManager::send_receive_iridium_vital_information(void){
    
}

/*
Possibly useful pieces of code:

To help debuggin Iridium

#if SERIAL_PRINT
    Serial.println("D;Serial to Iridium SBD");
    isbd.attachConsole(Serial);
    isbd.attachDiags(Serial);
  #endif

  #if SERIAL_PRINT
    Serial.println("D;Iridium power profile");
  #endif

  */