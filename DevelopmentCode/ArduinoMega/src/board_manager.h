#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/power.h>

#ifndef BOARD_MANAGER
#define BOARD_MANAGER

class BoardManager{
public:

  /*
  Return a boolean, true if still should log, false if time to stop logging
  */
  bool should_log(void);

  /*
  Power off the raspberry pi
  */
  void raspberry_off(void);

  /*
  Power on the raspberry pi
  */
  void raspberry_on(void);

  /*
  Check if the battery level is enough to work,
  */
  bool enough_battery(void);

  /*
  Decide if should wake up. Update the contents of the EEPROM
  */
  bool should_wakeup(void);

  /*
  Compute the level of the battery
  */

  float battery_level(void);

  /*
   Make sleep for a long time
  */
  // TODO: change the watchdog between use for reboot and use for sleeping


private:

  void setup_watchdog(void);

  // how many 'wake up cycles' by the power controller currently left before really wake up
  // the value is stored in EEPROM
  uint8_t number_sleeps_left;

  // how many 'wake up cycles' should sleep between two real wake ups
  // defined in EEPROM so that possible to update by Iridium
  // total_number_sleeps_before_wakeup value 0 means that never sleep
  // total_number_sleeps_before_wakeup value 1 means that measures and then sleeps
  // once (ie sleep typically 20 minutes)
  uint8_t total_number_sleeps_before_wakeup;

  unsigned long time_start_logging_ms;  // when the logging actually started
};

#endif



// No power to Raspberry Pi %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
pinMode(PIN_MFT_RPI, OUTPUT);
digitalWrite(PIN_MFT_RPI, HIGH);

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// SEE IF ENOUGH BATTERY FOR WORKING
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// necessary to check battery here, because maybe in the case where no sleep at
// all: then need to check that there is enough battery

check_battery_level();

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// SEE IF SHOULD WAKE UP OR JUST SLEEP MORE
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

decide_if_wakeup();

wdt_enable(WDTO_8S);

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// check the battery level
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void check_battery_level(void){
  float battery_level =  float(analogRead(PIN_MSR_BATTERY)) * 5.0 / 1024.0;

  if (battery_level < BAT_EMPTY_V){
    #if SERIAL_PRINT
    Serial.println("D;Low battery!");
    #endif

    // should stop now to try to save the logger
    pinMode(PIN_FBK_MGA, OUTPUT);
    digitalWrite(PIN_FBK_MGA, LOW);

    while(1){
      make_sleep();
    }
  }

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// LED PIN_MGA_LED blink
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// make led 13 on the board blink continuously to report fault
void blinkLED(){
  while (1){
    digitalWrite(PIN_MGA_LED, HIGH);   // turn the LED on
    delay(100);              // wait for a second
    digitalWrite(PIN_MGA_LED, LOW);    // turn the LED
    delay(100);              // wait for a second
  }
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// decide if should really wake up or just sleep more
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// look at the values contained in EEPROM indicating how many times need to sleep
// use them to decide if should sleep or not
// if number_sleeps_left is 0 when this function called, time to wake up.
void decide_if_wakeup(void){

  // pull up FBK_MEGA
  #if SERIAL_PRINT
  Serial.println(F("D;Put PIN_FBK_MGA HIGH"));
  delay(5);
  #endif

  pinMode(PIN_FBK_MGA, OUTPUT);
  digitalWrite(PIN_FBK_MGA, HIGH);

  // read from EEPROM total number of sleeps
  total_number_sleeps_before_wakeup = EEPROM.read(address_total_sleeps);

  #if SERIAL_PRINT
  Serial.print(F("D;total number sleeps: "));
  Serial.println(total_number_sleeps_before_wakeup);
  delay(5);
  #endif

  // value of the EEPROM: number of more cycles to sleep
  number_sleeps_left = EEPROM.read(address_sleeps_left);

  #if SERIAL_PRINT
  Serial.print(F("D;number_sleeps_left: "));
  Serial.println(number_sleeps_left);
  delay(5);
  #endif

  // if too high value: set back to max number of sleep cycles and sleep
  if (number_sleeps_left > total_number_sleeps_before_wakeup){
    #if SERIAL_PRINT
    Serial.println(F("D;Too high number_sleeps_left: reset"));
    delay(5);
    #endif

    EEPROM.write(address_sleeps_left, total_number_sleeps_before_wakeup);

    digitalWrite(PIN_FBK_MGA, LOW);

    while(1){
      make_sleep();
    }
  }

  // if number_sleeps_left > 0: must sleep more; decrease by one and sleep
  else if (number_sleeps_left > 0){

    #if SERIAL_PRINT
    Serial.println(F("D;Sleep more"));
    delay(5);
    #endif

    EEPROM.write(address_sleeps_left, uint8_t(number_sleeps_left - 1));

    digitalWrite(PIN_FBK_MGA, LOW);

    while(1){
      make_sleep();
    }
  }

  // else, time to wake up
  else {
    #if SERIAL_PRINT
    Serial.println(F("D;Time to wake up!"));
    delay(5);
    #endif

    // write how many sleeping cycles left
    EEPROM.write(address_sleeps_left, total_number_sleeps_before_wakeup);
  }

}

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
