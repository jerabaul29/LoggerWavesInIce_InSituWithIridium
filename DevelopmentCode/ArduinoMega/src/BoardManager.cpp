// TODO: check number of sleeps: can be a problem of 1 too much / little

#include "BoardManager.h"

BoardManager::BoardManager(void)
{
    pinMode(PIN_FBK_MGA, OUTPUT);
    pinMode(PIN_MFT_RPI, INPUT);
    // digitalWrite(PIN_MFT_RPI, HIGH);
}

void BoardManager::start(void){
    wdt_enable(WDTO_8S);

    board_status = BOARD_STARTING;

    #if DEBUG
        SERIAL_DEBUG.begin(SERIAL_DEBUG_BAUDRATE);
        delay(DELAY_START_SERIAL);
    #endif

    PDEBMSG("use debugging")

    // disable the LED pin to save current
    pinMode(PIN_MGA_LED, INPUT);

    // set the current high when just started
    // this way if boots too slow, at least get a chance to not be
    // switched off at once.
    BoardManager::ask_to_be_on();

    BoardManager::turn_raspberry_off();

    // decide if should be awake
    if (BoardManager::should_wakeup())
    {
        PDEBMSG("wakeup board")

        BoardManager::ask_to_be_on();
    }
    else
    {
        PDEBMSG("switch off board")

        this->ask_to_be_off();
        this->deep_sleep();
    }
}

void BoardManager::turn_raspberry_off(void){
    PDEBMSG("switch off raspberry")
    digitalWrite(PIN_MFT_RPI, HIGH);
}

void BoardManager::turn_raspberry_on(void){
    PDEBMSG("switch on raspberry")
    pinMode(PIN_MFT_RPI, OUTPUT);
    digitalWrite(PIN_MFT_RPI, LOW);
}

bool BoardManager::should_wakeup(void)
{
    // read the EEPROM values
    total_number_sleeps_before_wakeup = EEPROM.read(ADDRESS_TOTAL_NBR_SLEEPS);
    number_sleeps_left = EEPROM.read(ADDRESS_SLEEPS_LEFT);

    PDEBVAR(total_number_sleeps_before_wakeup)
    PDEBVAR(number_sleeps_left)

    BoardManager::check_sleep_status();

    // either should sleep
    if (number_sleeps_left > 0)
    {
        EEPROM.write(ADDRESS_SLEEPS_LEFT, uint8_t(number_sleeps_left - 1));
        return (false);
    }
    // or should wake up
    else
    {
        // TODO: add battery check
        EEPROM.write(ADDRESS_SLEEPS_LEFT, total_number_sleeps_before_wakeup);
        number_sleeps_left = total_number_sleeps_before_wakeup; // important for the sleep or reboot function
        return (true);
    }
}

void BoardManager::ask_to_be_off(void)
{
    digitalWrite(PIN_FBK_MGA, LOW);
}

void BoardManager::ask_to_be_on(void)
{
    digitalWrite(PIN_FBK_MGA, HIGH);
}

void BoardManager::check_sleep_status(void)
{
    // if too high value: set back to max number of sleep cycles and sleep
    if (number_sleeps_left > total_number_sleeps_before_wakeup)
    {
        EEPROM.write(ADDRESS_SLEEPS_LEFT, total_number_sleeps_before_wakeup);

        number_sleeps_left = total_number_sleeps_before_wakeup;
    }

    if (number_sleeps_left > MAX_NBR_SLEEPS_BEFORE_WAKEUP)
    {
        EEPROM.write(ADDRESS_TOTAL_NBR_SLEEPS, MAX_NBR_SLEEPS_BEFORE_WAKEUP);
        EEPROM.write(ADDRESS_SLEEPS_LEFT, MAX_NBR_SLEEPS_BEFORE_WAKEUP);

        total_number_sleeps_before_wakeup = MAX_NBR_SLEEPS_BEFORE_WAKEUP;
        number_sleeps_left = MAX_NBR_SLEEPS_BEFORE_WAKEUP;
    }

    // negative value should not arise because from uint8
}

float BoardManager::measure_battery_level(void){

    // an attempted fix to possible battery value read problem.
    delay(5);
    analogRead(PIN_MSR_BATTERY);
    delay(5);
    analogRead(PIN_MSR_BATTERY);
    delay(5);
    
    battery_level_V =  float(analogRead(PIN_MSR_BATTERY)) * 5.0 / 1024.0;
    PDEBVAR(battery_level_V)
    return(battery_level_V);
}

bool BoardManager::enough_battery(void){
    BoardManager::measure_battery_level();

    if (battery_level_V < BAT_EMPTY_V){
        return(false);
    }
    else{
        return(true);
    }
}

void BoardManager::sleep_or_reboot(void){
    PDEBMSG("call BoardManager::sleep_or_reboot")

    if (number_sleeps_left == 0){
        this->reboot();
    }
    else{
        this->deep_sleep();
    }

}

void BoardManager::reboot(void){
    PDEBMSG("call BoardManager::reboot")
    wdt_enable(WDTO_15MS);
    while (true){
        // nothing
    }
}

void BoardManager::deep_sleep(void){
    PDEBMSG("call BoardManager::deep_sleep")
    delay(100);  // a bit of time to let time to finish

    wdt_disable();
    power_adc_disable();
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_mode();
}

void BoardManager::start_logging(unsigned long duration_ms){
    PDEBMSG("call BoardManager::start_logging")

    this->duration_ms = duration_ms;
    time_start_logging_ms = millis();
    board_status = BOARD_LOGGING;
}

int BoardManager::check_status(void){

    switch(board_status){
        case BOARD_STARTING:
            PDEBMSG("Board starting")
            return(BOARD_STARTING);

        case BOARD_LOGGING:
            if (millis() - time_start_logging_ms < duration_ms){
                return(BOARD_LOGGING);
            }
            else{
                PDEBMSG("Board just done logging")
                board_status = BOARD_DONE_LOGGING;
                return(BOARD_DONE_LOGGING);
            }
        case BOARD_DONE_LOGGING:
            PDEBMSG("Board already done logging")
            return(BOARD_DONE_LOGGING);
    }

    PDEBMSG("Board status error")
    return(BOARD_STATUS_ERROR);
}

// TODO: add sleep function

/*

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// make Arduino Mega sleep
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void make_sleep(void){
  #if SERIAL_PRINT
  Serial.println(F("D;Make Mega sleep"));
  delay(5);
  #endif

  power_all_disable();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  power_adc_disable();
  sleep_enable();
  sleep_mode();
}

*/
