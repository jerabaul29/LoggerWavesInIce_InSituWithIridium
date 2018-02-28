#include "Iridium_manager.h"
#include "parameters.h"

IridiumManager::IridiumManager(HardwareSerial *serial_port,
                               GPSController *gps_controller,
                               BoardManager *board_manager,
                               SDManager *sd_manager)
    : serial_port(serial_port), gps_controller(gps_controller),
      board_manager(board_manager), sd_manager(sd_manager),
      iridium_sbd(IridiumSBD{SERIAL_IRIDIUM, PIN_IRD_SLEEP})
{

    IridiumManager::clean_reset_buffer_received();
    IridiumManager::clean_reset_buffer_transmit();

    serial_port->begin(19200);
    iridium_sbd.setPowerProfile(IRIDIUM_HIGH_POWER_SUPPLY);

    iridium_sbd.begin();
    iridium_sbd.sleep(); // go to sleep: will not be needed in quite some time.
}

void IridiumManager::clean_reset_buffer_received(void)
{
    for (int i = 0; i < IRIDIUM_RECEIVED_PACKET_SIZE; i++)
    {
        buffer_received[i] = '0';
    }

    buffer_received_position = 0;
}

void IridiumManager::clean_reset_buffer_transmit(void)
{
    for (int i = 0; i < IRIDIUM_TRANSMIT_PACKET_SIZE; i++)
    {
        buffer_transmit[i] = '0';
    }

    buffer_transmit_position = 0;
}

void IridiumManager::clean_reset_buffers(void)
{
    IridiumManager::clean_reset_buffer_received();
    IridiumManager::clean_reset_buffer_transmit();
}

void IridiumManager::send_receive_iridium_vital_information(void)
{
    iridium_sbd.begin(); // wake up

    // start with clean buffers
    IridiumManager::clean_reset_buffers();

    // TODO: check that ok length for buffer
    // add the information
    IridiumManager::set_battery_message();
    IridiumManager::set_filename_message();
    IridiumManager::set_GPRMC_message();

    // try to send the Iridium feedback string ----------------------------------
    // note: retries the operation for up to 300 seconds by default; put watchdog
    // reset in
    // ISBDCallback.
    int ird_feedback;
    ird_feedback = iridium_sbd.sendReceiveSBDBinary(
        (uint8_t *)buffer_transmit, buffer_received_position,
        (uint8_t *)buffer_received, buffer_received_position);

    // TODO: take care of command received through Iridium to change the state of the logger
}

// TODO: on all set methods, put condition to check no buffer overflow

void IridiumManager::set_battery_message(void){
    // get the message
    float battery_level_V = board_manager->measure_battery_level();
    String battery_level_string = String{battery_level_V};

    // TODO: can optimize a bit the transmission: transmit something more efficient and reduce Iridium message size
    // add it to the buffer_transmit and update buffer_transmit_position
    for (int i=0; i < 2 + NBR_SIGNIFICANT_DIGITS_BATT_LEVEL; i++){  // because need the first digit and dot
        buffer_transmit[buffer_transmit_position] = battery_level_string[i];
        buffer_transmit_position += 1;
    }
}

void IridiumManager::set_filename_message(void){
    // get the filename
    const char * filename = sd_manager->get_filename();

    // TODO: can optimize a bit the transmission: transmit something more efficient and reduce Iridium message size
    for (int i=0; i < NBR_ZEROS_FILENAME + 1; i++){
        buffer_transmit[buffer_transmit_position] = filename[i];
        buffer_transmit_position += 1;
    }
}

void IridiumManager::set_GPRMC_message(void){
    // get the GPRMC message
    int GPRMC_length = gps_controller->load_gprmc_message();
    const char * GPRMC_message = gps_controller->get_rx_buffer();

    for (int i=0; i < GPRMC_length; i++){
        buffer_transmit[buffer_transmit_position] = GPRMC_message[i];
        buffer_transmit_position += 1;
    }
}


// TODO: add watchdog resets

/*
  Callback to avoid watchdog reset.
*/
bool ISBDCallback(void)
{
    wdt_reset();
    return (true);
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


A piece of code that can be useful to implement the change of state of the logger
depending on commands received from Iridium

do
{

// NOTE: this will display the content, but double check not interacting with it
// because of the isbd call
#if SERIAL_PRINT
    Serial.print("Inbound buffer size is ");
    Serial.println(Ird_rx_position);
    for (int i = 0; i < Ird_rx_position; ++i)
    {
        Serial.write(Ird_rx[i]);
        Serial.print("(");
        Serial.print(Ird_rx[i]);
        Serial.print(") ");
    }
    Serial.print("Messages left: ");
    Serial.println(isbd.getWaitingMessageCount());
#endif

    wdt_reset();

    // check if command back ---------------------------------------------------
    // NOTE: need a better command back control system
    if (Ird_rx_position > 0)
    {
#if SERIAL_PRINT
        Serial.print(F("Ird_rx_position > 0: "));
        Serial.println(Ird_rx_position);
#endif

        // check if should update number of sleep cycles
        // --------------------------
        // this kind of message should be of the form: SLP(12) [set the number of
        // sleep cycles
        // to 12, where (12) is one byte in binary value]
        if (char(Ird_rx[0]) == 'S' && char(Ird_rx[1]) == 'L' &&
            char(Ird_rx[2]) == 'P')
        {
            uint8_t value_sleep = uint8_t(Ird_rx[3]);

#if SERIAL_PRINT
            Serial.print(F("Will sleep for:"));
            Serial.println(value_sleep);
#endif

            // update the EEPROM
            EEPROM.write(address_total_sleeps, value_sleep);
        }

        // check if should send some commands to RPi -----------------------------
        // NOTE: could add functionalities, ie commands from Iridium, here
        // such as: send a whole file, send an analysis, send another analysis etc
    }

    // if more messages to get, use more sendReceive
    if (isbd.getWaitingMessageCount() > 0)
    {

#if SERIAL_PRINT
        Serial.println(F("D;Receive one more message"));
#endif

        Ird_rx_position = sizeof(Ird_rx);
        ird_feedback =
            isbd.sendReceiveSBDBinary(NULL, 0, Ird_rx, Ird_rx_position);
    }
    // otherwise, get out of the reading loop
    else
    {

#if SERIAL_PRINT
        Serial.println(F("D;No more messages"));
#endif

        break;
    }

} while (true);



To help look at error messages

    #if SERIAL_PRINT
    if (ird_feedback != 0){
        Serial.print("D;Problem transmitting: ");
        Serial.println(ird_feedback);
    }
    else{
        Serial.println("D;Transmitted well");
    }
    #endif
    
*/
