#include <HighSpeedUSBOutputController.h>
#include <DebugMacros.h>
#include <avr/wdt.h>

HighSpeedUSBOutputController::HighSpeedUSBOutputController(HardwareSerial * serial_port):
    serial_port(serial_port),
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

    // wait for transmission to be ok
    this->serial_port->flush();

    wdt_reset();
}

bool HighSpeedUSBOutputController::wait_if_needed(void){
    // if the RPi has something to say, the state may have changed
    if (this->serial_port->available()){
        char crrt_char = this->serial_port->read();

        // the RPi is full: wait for it to be more ready
        if (crrt_char == 'F'){
            this->need_to_wait = true;
            PDEBMSG("Pi full")
            return(true);
        }

        // the RPi is empty; can start sending again
        else if(crrt_char == 'E')
        {
            this->need_to_wait = false;
            PDEBMSG("Pi empty")
            return(false);
        }

        // non recognized message
        else{
            PDEBMSG("read problematic value")
            PDEBVAR(crrt_char)
        }
    }

    // if not something from RPi, still in the same state
    else{
        return(this->need_to_wait);
    }
}