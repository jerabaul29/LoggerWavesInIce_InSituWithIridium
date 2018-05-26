#include <Arduino.h>

#ifndef HIGH_SPEED_USB_OUT_CONTROLLER
#define HIGH_SPEED_USB_OUT_CONTROLLER

#define NBR_MAX_ALLOWED_SEND (SERIAL_TX_BUFFER_SIZE / 4)

class HighSpeedUSBOutputController
{
public:

    /*
      constructor needs only the serial
    */
    HighSpeedUSBOutputController(HardwareSerial *);

    /*
      write to the serial port (if ok)
    */
    void write_to_port(char);
   
private:
    HardwareSerial * serial_port;

    unsigned int number_allowed_remaining_to_send;

    bool need_to_wait;

    bool wait_if_needed(void);
};

#endif //HIGH_SPEED_USB_OUT_CONTROLLER
