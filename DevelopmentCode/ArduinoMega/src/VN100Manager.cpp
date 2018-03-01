#include "VN100Manager.h"

VN100Manager::VN100Manager(HardwareSerial * serial_port, SDManager * sd_manager) :
    serial_port(serial_port),
    sd_manager(sd_manager)
    {}

void VN100Manager::start(void){
    #if DEBUG
  SERIAL_DEBUG.println(F("VN100 start"));
#endif

// set the fixed bytes corresponding to header
    for(int i = 0; i < SIZE_VN100_HEADER; i++){
    buffer_rx[i] = header[i];
    }

    serial_port->begin(57600);
    delay(DELAY_START_SERIAL);

    #if DEBUG
  SERIAL_DEBUG.println(F("VN100 opened serial"));
#endif

    this->reset_buffer_header();
    this->flush_serial_buffer();
}

void VN100Manager::flush_serial_buffer(void){
    #if DEBUG
  SERIAL_DEBUG.println(F("VN100 flush"));
#endif

    // flush all
    while (serial_port->available() > 0){
        /*
        #if DEBUG
        char crrt_char = serial_port->read();
        Serial.println(crrt_char);
        #else
        serial_port->read();
        #endif
        */
        serial_port->read();
    }

    // now flush until received new message
    while(!this->catch_message()){
        // do!
    }

    this->reset_buffer_header();
}

void VN100Manager::reset_buffer_header(void){
    buffer_rx_position = SIZE_VN100_HEADER;
    number_received_header_bytes = 0;
}

bool VN100Manager::catch_message(void){

    while (serial_port->available() > 0){
        // read
        byte byte_in = serial_port->read();

        // add it to buffer
        buffer_rx[buffer_rx_position] = byte_in;
        buffer_rx_position += 1;
        // buffer_rx_position = buffer_rx_position % SIZE_BUFFER_VN100;  // avoid overflow

        /*
        #if DEBUG
            SERIAL_DEBUG.print(F("B | P: "));
            SERIAL_DEBUG.print(byte_in);
            SERIAL_DEBUG.print(F(" "));
            SERIAL_DEBUG.println(buffer_rx_position);
        #endif
        */

        // check for header
        if (header[number_received_header_bytes] == byte_in){
        // if (byte0 == byte_in){
            number_received_header_bytes += 1;

            /*
            #if DEBUG
            SERIAL_DEBUG.print(F("R"));
            SERIAL_DEBUG.print(number_received_header_bytes);
            SERIAL_DEBUG.print(F(" "));
            SERIAL_DEBUG.print(buffer_rx_position);
            #endif
            */

        }
        else{
            number_received_header_bytes = 0;
        }

        // if received all header bytes, received a full message
        if (number_received_header_bytes == 6){
            /*
            #if DEBUG
            SERIAL_DEBUG.print(F("R"));
            SERIAL_DEBUG.println(buffer_rx_position);
            #endif
            */
            number_received_header_bytes = 0;
            return(true);
        }
    }

    // if come all the way here, not received a full message
    return(false);
}

void VN100Manager::perform_logging(void){
    // try to catch a message
    bool catched_message = this->catch_message();

    if (catched_message){
        // log
        this->post_to_SD_card();

        // reset
        this->reset_buffer_header();
    }
}

void VN100Manager::post_to_SD_card(void){
    sd_manager->post_on_SD_card(String("B,\n"), false);
    sd_manager->post_on_SD_card(buffer_rx, buffer_rx_position - SIZE_VN100_HEADER);
}