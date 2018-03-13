#include "RaspberryManager.h"

// TODO: clean communications with Raspberry Pi, use standardized functions
// TODO: improve / clean communications

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
    PDEBMSG("call RaspberryManager::start")

    #if DEBUG
        delay(100);
    #endif

    board_manager->turn_raspberry_on();
    wdt_reset();

    // let the time to boot; should be unnecessary if no issue with serial port
    // beginning without the RPi connected.

    // this->wait_some_time_ms(10000);  // TODO: take away or reduce! shoulnd not be necessary

    // start serial while maybe no computer listening; should not be a problem.
    serial_port->begin(115200);
    delay(50);
    wdt_reset();

    // TODO: change this: ask the RPi for acknowlegement in a maximum time duration
    unsigned long time_start = millis();
    bool wait_for_answer = true;

    while (millis() - time_start < TIMEOUT_WAKEUP_RPI){
        wdt_reset();

        if (this->ask_acknowledge_raspberry('B', 'R')){
            wait_for_answer = false;
            break;
        }
    }

    if (wait_for_answer){
        // reboot
        while(true){
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

    PDEBMSG("call RaspberryManager::ask_acknowledge_raspberry")
    PDEBVAR(to_rpi)
    PDEBVAR(expected_answer)

    this->flush_buffer_in();
    wdt_reset();

    unsigned long time_start_try_acknowledge = millis();

    // try to get acknowledgement
    while (millis() - time_start_try_acknowledge < TIMEOUT_ACKNOWLEDGEMENT_RPI_MS){

        wdt_reset();

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

    PDEBMSG("call RaspberryManager::acknowledge_to_raspberry")
    PDEBVAR(from_raspberry)
    PDEBVAR(answer_to_raspberry)

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

// TODO: find a way to transmit chunks of data, otherwise too slow!! works now but slow
void RaspberryManager::file_content_to_raspberry(void){

    // send the start message
    serial_port->print(F("START_TRANSMIT_FILE"));

    // open the data file to send
    sd_manager->open_current_filenumber('F');

    // send the whole content
    // TODO: improve here...
    while(sd_manager->more_to_read()){
        char crrt_char = sd_manager->read_char();  // TODO: check if no problem with type here
        serial_port->write(crrt_char);
        delayMicroseconds(1);  // TODO: 5 works, reduced to make data transfer faster
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

// TODO: put in separate functions
void RaspberryManager::receive_processed_data(void){
    PDEBMSG("call RaspberryManager::receive_processed_data")

    StreamAnalyzer stream_analyzer1{"PROCESSING_OK"};

    // wait for the message from Raspberry Pi about processing OK
    PDEBMSG("wait for PROCESSING_OK")
    wdt_reset();

    unsigned long time_start = millis();

    while(true){
        // if not timed out
        if (millis() - time_start < TIMEOUT_PROCESSING_RPI){
            if (serial_port->available() > 0){
                char crrt_char = serial_port->read();
                if (stream_analyzer1.load_char(crrt_char)){
                    break;
                }
            }

            wdt_reset();
        }
        // if timed out
        else{
            PDEBMSG("erro: timeout, no PROCESSING_OK")
            // reboot
            while (true){
                // nothing
            }
        }
    }

    wdt_reset();

    PDEBMSG("fine: receive and save from RPi")

    // open the file where to save the data
    sd_manager->update_current_file('I');

    // ask the RPi to transmit
    serial_port->write('T');
    // receive the data: save it to file

    // NOTE: this approach may be slow (write character per character to SD card)
    // should be ok for short messages, potentially dangerous for large messages.
        
    time_start =  millis();

    PDEBMSG("wait for PROCESSED_END")
    StreamAnalyzer stream_analyzer2{"PROCESSED_END"};

    // ad hoc, to have only the content of the buffer in the file; this is the
    // length of PROCESSED_END
    char latest_chars[13];
    int left_to_receive = 13 - 1;
    int position_in_buffer = 0;

    wdt_reset();

    // TODO: improve here... this is a hacky ring buffer adapted to this
    // specific message length (13 hard coded)
    while (millis() - time_start < TIMEOUT_RECEIVED_TO_TRANSMIT_FROM_PI){
        if (serial_port->available() > 0){
            char crrt_char;
            crrt_char = serial_port->read();

            left_to_receive -= 1;
            latest_chars[position_in_buffer] = crrt_char;
            position_in_buffer = (position_in_buffer + 1) % 13;

            if (stream_analyzer2.load_char(crrt_char)){
                break;
            }

            if (left_to_receive < 0){
                sd_manager->write_char(latest_chars[position_in_buffer]);
            }

            wdt_reset();
        }
    }

    // let some time for writing on SD card
    delay(100);
}

void RaspberryManager::send_filename(void){
    // name? -> name!

    wdt_reset();
    this->ask_acknowledge_raspberry('N', 'N');

    for (int ind = 0; ind < NBR_ZEROS_FILENAME + 1; ind++){
        char crrt_char = sd_manager->get_filename()[ind];
        serial_port->print(crrt_char);
    }

    // done? -> done!
    this->ask_acknowledge_raspberry('N', 'N');
}

void RaspberryManager::shutdown(void){
    // ask the RPi to shutdown
    serial_port->write('S');

    // give some time for shutdown
    this->wait_some_time_ms(10000);

    board_manager->turn_raspberry_off();
}

void RaspberryManager::transmit_through_iridium(char filename_prefix = 'I'){
    PDEBMSG("call RaspberryManager::transmit_through_iridium")

    wdt_reset();

    sd_manager->open_current_filenumber(filename_prefix);

    // TODO: here, allow to transmit only one message of 340 bytes maximum. Change here,
    // or bettern call the transmit_

    iridium_manager->clean_reset_buffer_transmit();

    while (iridium_manager->buffer_transmit_position < IRIDIUM_TRANSMIT_PACKET_SIZE){
        // if more to read, add to buffer
        if (sd_manager->more_to_read()){
            char crrt_char;
            crrt_char = sd_manager->read_char();
            iridium_manager->buffer_transmit[iridium_manager->buffer_transmit_position] = crrt_char;
            iridium_manager->buffer_transmit_position += 1;
        }
        // if end of file, ok
        else{
            PDEBMSG("put all content in transmit buffer")
            PDEBVAR(iridium_manager->buffer_transmit_position)

            break;
        }

        wdt_reset();
    }

    iridium_manager->send_receive();
}