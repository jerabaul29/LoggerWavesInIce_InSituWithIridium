#include "RaspberryManager.h"

RaspberryManager::RaspberryManager(BoardManager * board_manager,
                SDManager * sd_manager, IridiumManager * iridium_manager,
                HardwareSerial * serial_port) :
    board_manager(board_manager),
    sd_manager(sd_manager),
    iridium_manager(iridium_manager),
    serial_port(serial_port)
    {
        // not much to do
    }

void RaspberryManager::start(){
    board_manager->turn_raspberry_on();
    wdt_reset();

    this->wait_some_time_ms(10000UL);
    wdt_reset();

    serial_port->begin(115200);
    delay(50);
    wdt_reset();

    if (this->ask_acknowledge_raspberry('B', 'R')){
        wdt_reset();
    }
    else{
        // reboot
        while (true) {
            //nothing
        }
    }
}

void RaspberryManager::wait_some_time_ms(unsigned long time_wait){
    unsigned long time_start = millis();

    while (millis() - time_start < time_wait){
        delay(10);
        wdt_reset();
    }
}

bool RaspberryManager::ask_acknowledge_raspberry(char to_rpi, char expected_answer){

    this->flush_buffer_in();
    wdt_reset();

    unsigned long time_start_try_acknowledge = millis();

    // try to get acknowledgement
    while (millis() - time_start_try_acknowledge < TIMEOUT_ACKNOWLEDGEMENT_RPI_MS){
        serial_port->print(to_rpi);
        delay(DELAY_ANSWER_RPI);  // let the time to answer.

        if (serial_port->available() > 0){
            char answer = serial_port->read();
            
            if (answer == expected_answer){
                return(true);
            }
        }
    }
    
    // if not acknowledged, timeout error
    return(false);
}

void RaspberryManager::flush_buffer_in(void){
    while (serial_port->available() > 0){
        serial_port->read();
    }
}

bool RaspberryManager::acknowledge_to_raspberry(char from_raspberry, char answer_to_raspberry){
    unsigned long time_start = millis();

    while (millis() - time_start < TIMEOUT_ACKNOWLEDGEMENT_RPI_MS){
        if (serial_port->available() >0){
            char char_in = serial_port->read();
            if (char_in == from_raspberry){
                serial_port->print(answer_to_raspberry);
                return(true);
            }
        }
    }

    return(false);
}

bool RaspberryManager::receive_between_acknowledgement_messages(const char * acknowledgement_message, char answer){

}
