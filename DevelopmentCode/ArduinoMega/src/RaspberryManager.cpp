#include "RaspberryManager.h"

RaspberryManager::RaspberryManager(BoardManager * board_manager,
                SDManager * sd_manager, IridiumManager * iridium_manager,
                HardwareSerial * serial_port):
    board_manager(board_manager),
    sd_manager(sd_manager),
    iridium_manager(iridium_manager),
    serial_port(serial_port)
    {
        // not much to do
    }

RaspberryManager::start(){
    board_manager->turn_raspberry_on();

    this->wait_some_time_ms(10000UL);

    serial_port->begin(115200);
    delay(50);

    this->flush_buffer_in();

    this->ask_acknowledge_raspberry('B', 'R');
}

RaspberryManager::ask_acknowledge_raspberry(char to_rpi, char expected_answer){
    // TODO:
}