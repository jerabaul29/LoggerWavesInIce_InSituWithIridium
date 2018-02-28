#include "board_manager.h"

BoardManager::BoardManager(void)
{
    wdt_enable(WDTO_8S);

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
        BoardManager::ask_to_be_on();
    }
    else
    {
        BoardManager::ask_to_be_off();
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