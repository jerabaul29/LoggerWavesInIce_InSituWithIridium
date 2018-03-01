#include "BoardManager.h"

BoardManager::BoardManager(void)
{
}

void BoardManager::start(void){
    wdt_enable(WDTO_8S);

    board_status = BOARD_STARTING;

    #if DEBUG
        SERIAL_DEBUG.begin(115200);
        delay(DELAY_START_SERIAL);
        SERIAL_DEBUG.println(F("Start debugging!"));
    #endif

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
        #if DEBUG
        SERIAL_DEBUG.println(F("Ask to be on"));
    #endif

        BoardManager::ask_to_be_on();
    }
    else
    {
        #if DEBUG
        SERIAL_DEBUG.println(F("Ask to be off"));
    #endif
        BoardManager::ask_to_be_off();
        // make it stop here: TODO: make it sleep instead
        while(true){
            wdt_reset();
        }
    }
}

void BoardManager::turn_raspberry_off(void){
    digitalWrite(PIN_MFT_RPI, HIGH);
}

void BoardManager::turn_raspberry_on(void){
    digitalWrite(PIN_MFT_RPI, LOW);
}

bool BoardManager::should_wakeup(void)
{
    // read the EEPROM values
    total_number_sleeps_before_wakeup = EEPROMReadlong(ADDRESS_TOTAL_NBR_SLEEPS);
    number_sleeps_left = EEPROMReadlong(ADDRESS_SLEEPS_LEFT);

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
    }

    // negative value should not arise because from uint8
}

float BoardManager::measure_battery_level(void){
    battery_level_V =  float(analogRead(PIN_MSR_BATTERY)) * 5.0 / 1024.0;
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

void BoardManager::start_logging(unsigned long duration_ms){
    #if DEBUG
  SERIAL_DEBUG.println(F("start logging"));
#endif
    this->duration_ms = duration_ms;
    time_start_logging_ms = millis();
    board_status = BOARD_LOGGING;
}

int BoardManager::check_status(void){
    #if DEBUG && DEBUG_SLOW
  SERIAL_DEBUG.print(F("status: "));
  SERIAL_DEBUG.println(board_status);
#endif

    switch(board_status){
        case BOARD_STARTING:
            return(BOARD_STARTING);

        case BOARD_LOGGING:
            if (millis() - time_start_logging_ms < duration_ms){
                return(BOARD_LOGGING);
            }
            else{
                board_status = BOARD_DONE_LOGGING;
                return(BOARD_DONE_LOGGING);
            }
        case BOARD_DONE_LOGGING:
            return(BOARD_DONE_LOGGING);
    }
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