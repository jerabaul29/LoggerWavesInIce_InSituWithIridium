#include "VN100Manager.h"

VN100Manager::VN100Manager(HardwareSerial * serial_port, SDManager * sd_manager) :
    serial_port(serial_port),
    sd_manager(sd_manager)
    {}

void VN100Manager::start(void){
    #if DEBUG
  SERIAL_DEBUG.println(F("start VN100"));
#endif

    serial_port->begin(57600);
    delay(DELAY_START_SERIAL);

    this->reset_buffer_header();
    this->flush_serial_buffer();
}

void VN100Manager::flush_serial_buffer(void){
    // flush all
    while (serial_port->available() > 0){
        serial_port->read();
    }

    // now flush until received new message
    while(!this->catch_message()){
        // do!
    }

    this->reset_buffer_header();
}

void VN100Manager::reset_buffer_header(void){
    buffer_rx_position = 0;
    number_received_header_bytes = 0;
}

bool VN100Manager::catch_message(void){
    while (serial_port->available() > 0){
        // read
        byte byte_in = serial_port->read();

        // add it to buffer
        buffer_rx[buffer_rx_position] = byte_in;
        buffer_rx_position += 1;
        buffer_rx_position = buffer_rx_position % SIZE_BUFFER_VN100;  // avoid overflow

        // check for header
        if (header[number_received_header_bytes] == byte_in){
            number_received_header_bytes += 1;
        }
        else{
            number_received_header_bytes = 0;
        }

        // if received all header bytes, received a full message
        if (number_received_header_bytes == 6){
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
    sd_manager->post_on_SD_card(buffer_rx, buffer_rx_position);
}