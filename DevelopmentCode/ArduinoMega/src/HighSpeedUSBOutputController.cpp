#include <HighSpeedUSBOutputController.h>
#include <DebugMacros.h>
#include <avr/wdt.h>

HighSpeedUSBOutputController::HighSpeedUSBOutputController(HardwareSerial * serial_port):
    serial_port(serial_port),
    number_allowed_remaining_to_send(NBR_MAX_ALLOWED_SEND),
    need_to_wait(false)
    {
        // make sure no trash from RPi: flush input buffer
        while (this->serial_port->available()){
            this->serial_port->read();
        }
    }

void HighSpeedUSBOutputController::write_to_port(char crrt_char){
    // check that the RPi is not asking for a break
    while (this->wait_if_needed()){
        // nothing
    }

    // transmit
    this->serial_port->write(crrt_char);
    this->number_allowed_remaining_to_send--;

    // if have sent a whole block, need to flush
    if(this->number_allowed_remaining_to_send == 0){
        this->number_allowed_remaining_to_send = NBR_MAX_ALLOWED_SEND;

        // do not overflow the output buffer!
        while (this->serial_port->availableForWrite() < NBR_MAX_ALLOWED_SEND){
            // wait
        }
        

        // wait for RPi to ask for more, to be sure do not overfow it
        this->need_to_wait = true;
    }

    wdt_reset();
}

bool HighSpeedUSBOutputController::wait_if_needed(void){
    // return(false);  // never wait

    // if need to wait
    if (this->need_to_wait){

        // if received from the RPi...
        while (this->serial_port->available()){
            char crrt_char = this->serial_port->read();

            PDEBVAR(crrt_char)

            // if Ok to transmit start transmitting again now
            if (crrt_char == 'O'){
                this->need_to_wait = false;

                // flush: maybe asked again and again for new data while flushing
                while (this->serial_port->available() > 0){
                    this->serial_port->read();
                }

                return(false);
            }
        }

        // otherwise need to wait more
        return(true);
    }

    // if does not need to wait, ready to transmit at once, but check if RPi is fed up first
    else{
        // if received from the RPi...
        while (this->serial_port->available()){
            char crrt_char = this->serial_port->read();

            // if full need to start waiting now
            if (crrt_char == 'F'){
                this->need_to_wait = true;
                return(true);
            }
        }

        // if done transmitting a block, should wait for acknowledgement from RPi
        if (this->number_allowed_remaining_to_send == 0){
            this->need_to_wait = true;
            return(true);
        }

        // otherwise do not need to wait: continue transmitting the block
        return(false);
    }
}