#include "RaspberryManager.h"

RaspberryManager::RaspberryManager(BoardManager * board_manager,
                SDManager * sd_manager, IridiumManager * iridium_manager,
                HardwareSerial * serial_port) :
    board_manager(board_manager),
    sd_manager(sd_manager),
    iridium_manager(iridium_manager),
    serial_port(serial_port),
    message_from_raspberry_processing_ok("PROCESSING_OK"),
    length_message_from_raspberry_processing_ok(message_from_raspberry_processing_ok.length())
    {
        
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

void RaspberryManager::file_content_to_raspberry(void){
    // send the start message
    serial_port->print(F("START_TRANSMIT_FILE"));

    // open the data file to send
    sd_manager->open_current_filenumber('F');

    // send the whole content
    while(sd_manager->more_to_read()){
        char crrt_char = sd_manager->read_char();  // TODO: check if no problem with type here
        serial_port->write(crrt_char);
        delayMicroseconds(5);
        wdt_reset();
    }
    // send the end message
    serial_port->print(F("END_TRANSMIT_FILE"));

    // TODO: put some checks here:
    // check that data well receiced, otherwise send again
}

bool RaspberryManager::detect_message(const String & message){
    int current_nbr_char_received = 0;

    while(true){
        if (serial_port->available()){
            char crrt_char = serial_port->read();

            if (crrt_char == message[current_nbr_char_received]){
                current_nbr_char_received += 1;
            }
            else{
                current_nbr_char_received = 0;
            }

            if (message[current_nbr_char_received == '\0']){
                return(true);
            }
        }
    }
}

void RaspberryManager::receive_processed_data(void){
    // wait for the message from Raspberry Pi about processing OK
    unsigned long time_start = millis();
    while (serial_port->available() < length_message_from_raspberry_processing_ok){
        if (millis() - time_start < TIMEOUT_PROCESSING_RPI){
            wdt_reset();
        }
        else{
            // problem, whould have received acknowledgement
        }
    }

    // check thar it is well what to be expected: raspberry Pi ok
    if (this->detect_message(message_from_raspberry_processing_ok)){
        // open the file where to save the data
        sd_manager->update_current_file('I');

        // ask the RPi to transmit
        serial_port->write('T');

        // receive the data: save it to file
        // NOTE: this approach may be slow (write character per character to SD card)
        // should be ok for short messages, potentially dangerous for large messages.
        while (serial_port->available() > 0){
            char crrt_char;
            crrt_char = serial_port->read();
            sd_manager->write_char(crrt_char);
            wdt_reset();
        }

        // let some time for writing on SD card
        delay(100);
    }

    else{
        while(true){
            // reboot
        }
    }
}

void RaspberryManager::shutdown(void){
    // ask the RPi to shutdown
    serial_port->write('S');

    // give some time for shutdown
    this->wait_some_time_ms(10000);

    board_manager->turn_raspberry_off();
}

void RaspberryManager::transmit_through_iridium(char filename_prefix = 'I'){
    iridium_manager->buffer_transmit_position = 0;

    sd_manager->open_current_filenumber(filename_prefix);

    // TODO: here, allow to transmit only one message of 340 bytes maximum. Change here,
    // or bettern call the transmit_

    while (iridium_manager->buffer_transmit_position < IRIDIUM_TRANSMIT_PACKET_SIZE){
        // if more to read, add to buffer
        if (sd_manager->more_to_read()){
            char crrt_char;
            crrt_char = sd_manager->read_char();
            iridium_manager->buffer_transmit[iridium_manager->buffer_received_position] = crrt_char;
            iridium_manager->buffer_received_position += 1;
        }
        // if end of file, ok
        else{
            break;
        }
    }

    iridium_manager->send_receive();
}