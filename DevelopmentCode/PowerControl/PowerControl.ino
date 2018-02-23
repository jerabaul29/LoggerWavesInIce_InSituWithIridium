/*
 * Code for power control
 *
 * This code takes care of the power management, and should be uploaded on the barebone ATMega328P
 *
 * Its functions are to:
 *
 * - Take care of the battery and solar panel. Each time it wakes up, check if the battery and solar panel should be connected
 *
 * - Take care of the power to the Arduino Mega. Each time it wakes up, check what should be the next state of the Arduino Mega.
 *       The logics is the following:
 *           - if the Mega is already awake, do as the Mega asks: give it current if asks for current. If the Mega is awake,
 *               it is the Mega responsibility to check the battery level and to not deplete the battery
 *           - if the Mega is not awake, chek how many more cycles it should sleep. If time to wake up, check if enough battery,
 *                and if enough battery, give current to the Mega. If give current to the Mega, check if the Mega actually wants to
 *                be awake by pulling the feedback pin after enough time for booting. This way, the Mega can participate in decidiing
 *                how often it should get awake.
 *
 * Regarding the number of sleep cycles, there are two different pre-processor values:
 *     - CYCLES_DEEP_SLEEP is how many 8 seconds watchdog deep sleep cycles this ATMega328P sleeps between taking action. i.e., how
 *         often the battery vs solar pannel will be checked, the Mega will be checked and if needed its sleep counter decreased.
 *         Typically, this could be 5 cycles, i.e. this ATMega328P waking up each 40 seconds.
 *     - CYCLES_BEFORE_MEGA_WAKEUP is the number of deep sleep cycles before the Mega is given the chance to wake up again, when it
 *          has just gone to sleep. Note that the Mega can always decide to sleep more if he wants, i.e. to really wake up only
 *          each 0 (never stop asking current, always on), 1, 2, 3, 4, or N CYCLES_BEFORE_MEGA_WAKEUP.
 *          Typically, this could be 30 cycles: 30 * 40 = 1200s, i.e. 20 minutes, i.e. the Mega will be given the change to wake
 *          up if it wants every 20 minutes.
 *
 * NOTE / TODO:s
 *
 * - Possible optimization: require hardware update
 * put the pololu on a MOSFET from the battery side (to disconnect it when not needed, ie when not using
 * the Mega). Then need to be sure voltage on capacitors do not get too low: re connect pololu when logging with
 * Mega, and connect at least 8 seconds each 15 minutes [note: check values].
 *
 * - Possible code refactory:
 * put all the Mega logics in separate functions; this may be a bit painful because of the number of global
 * variables used.
 *
 */

 /*
  *
  * CHECKLIST BEFORE UPLOADING
  * CHECK THAT ALL THOSE PREPROCESSOR INSTRUCTIONS ARE SET TO RIGHT VALUE!!
  *
  * IN PARTICULAR:
  *   - DEBUG
  *   - SHOW_LED
  *   - CYCLES_BEFORE_MEGA_WAKEUP
  *   - CYCLES_DEEP_SLEEP
  *
  */

#include <avr/wdt.h>            // library for default watchdog functions
#include <avr/interrupt.h>      // library for interrupts handling
#include <avr/sleep.h>          // library for sleep
#include <avr/power.h>          // library for power control
#include "client_device.h"

// how many times remain to sleep before wake up
// volatile to be modified in interrupt function
volatile int nbr_remaining;

#define DEBUG false  // if DEBUG is true, Serial will be activated and used for logging information strings
#define SHOW_LED false  // if SHOW_LED is true, the LED will be used for helping debugging

#define PIN_MSR_BAT A0      // measure of battery
#define PIN_MSR_SOL A1      // measure of solar panel on anode

#define ALPHA 0.41          // R_2 / (R_1 + R_2) in voltage divider

#define PIN_MFT_SOL 2      // command mosfet solar panel / battery
#define PIN_FBK_MGA 3      // feedback from Mega
#define PIN_MFT_MGA 4      // command mosfet giving to the Mega
#define PIN_CMD_LED 13     // command of intern LED

#define BAT_THRESHOLD_V 3.6  // threshold for full battery
#define BAT_EMPTY_V 2.8     // threshold for empty battery
#define MIN_MARGIN_PANEL 0.5  // minimum over voltage panel vs battery for it to be worth connecting

// number of cycles for which the microcontroller taking care of power sleeps
// each sleep cycle is 8s
// for production: 5 cycles is 40 seconds
// this means that every 40s, the mC will wake up, take care of solar power,
// take care of the Mega.
#define CYCLES_DEEP_SLEEP 5
// #define CYCLES_DEEP_SLEEP 2  // test value

// number of loop () cycles before trying to wake up the Mega after is has been set asleep
// or refused to wake up.
// the duration of a loop() cycle is mostly determined by CYCLES_DEEP_SLEEP
// ie if for production deep sleep is 40s, and CYCLES_BEFORE_MEGA_WAKEUP is 30,
// then the Mega will get the possibility to be waken up every 40s * 30  = 1200s = 20 minutes
// and the Mega can decide to be always awake (never stop asking current), or to perform measurements
// every 20 minutes (sleep one cycle), or every 40 minutes (sleep 2 cycles), etc.
#define CYCLES_BEFORE_MEGA_WAKEUP 30
// #define CYCLES_BEFORE_MEGA_WAKEUP 3 // test value

float meas_battery = 0.0;
float meas_solar_panel_anode = 0.0;
float solar_panel_voltage = 0.0;

bool connect_battery_panel = false;

CDV mega_device(PIN_FBK_MGA, PIN_MFT_MGA, CYCLES_BEFORE_MEGA_WAKEUP, &meas_battery);

//clear bit macro
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

//set bit macro
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void setup() {
  // --------------------------------------------------------------
  // Open serial port for debugging
  // --------------------------------------------------------------
  #if DEBUG
    Serial.begin(9600);
    Serial.println(F("Booting..."));
    delay(50);
  #endif

  // --------------------------------------------------------------
  // Configure watchdog
  // --------------------------------------------------------------

  #if DEBUG
    Serial.println(F("Configure watchdog..."));
    delay(50);
  #endif

  configure_wdt();
  wdt_reset();

  // --------------------------------------------------------------
  // Prepare pins
  // --------------------------------------------------------------

  #if DEBUG
    Serial.println(F("Set pins..."));
    delay(50);
  #endif

  // set digital pins (analog pins do not need to be set)
  pinMode(PIN_FBK_MGA, INPUT);
  #if SHOW_LED
    pinMode(PIN_CMD_LED, OUTPUT);
  #endif
  pinMode(PIN_MFT_SOL, INPUT);
  pinMode(PIN_MFT_MGA, INPUT);

  #if SHOW_LED
    digitalWrite(PIN_CMD_LED, LOW);
  #endif

}
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void loop() {

  // --------------------------------------------------------------
  // Start a new cycle: let the time to wake up
  // --------------------------------------------------------------
  wdt_reset();

  delay(5);

  #if SHOW_LED
    controlled_blink(50, 20);
    delay(2000);
    wdt_reset();
  #endif

  #if DEBUG
    Serial.println();
    Serial.println(F("Start new cycle..."));
    delay(50);
  #endif

  // --------------------------------------------------------------
  // All measurements of tensions
  // --------------------------------------------------------------
  meas_battery = float(analogRead(PIN_MSR_BAT)) * 5.0 / 1024.0;
  meas_solar_panel_anode = read_solar_panel_anode();  // the call to take_care_battery_solar comes in next block (decide what to do)
  solar_panel_voltage = meas_battery - meas_solar_panel_anode;

  #if DEBUG
    Serial.print(F("meas_battery: "));
    Serial.println(meas_battery);
    delay(50);
    Serial.print(F("meas_solar_panel_anode: "));
    Serial.println(meas_solar_panel_anode);
    delay(50);
    Serial.print(F("solar_panel_voltage: "));
    Serial.println(solar_panel_voltage);
    delay(50);
  #endif

  wdt_reset();

  // --------------------------------------------------------------
  // Decide what should do with the solar panel
  // --------------------------------------------------------------
  connect_battery_panel = should_connect_array(meas_battery, solar_panel_voltage);

  take_care_battery_solar(connect_battery_panel);

  wdt_reset();

  // --------------------------------------------------------------
  // Decide what to do with the Arduino Mega
  // --------------------------------------------------------------

  /*
  check_number_reminding_mega_sleep_cycles();
  wdt_reset();

  update_mega_switching();
  wdt_reset();
  */

  mega_device.update_status();

  // --------------------------------------------------------------
  // Sleep
  // --------------------------------------------------------------
  sleep(CYCLES_DEEP_SLEEP);
}
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

////////////////////////////////////////////////////////////////////////////////////////////////////
// Arduino Mega stuff
////////////////////////////////////////////////////////////////////////////////////////////////////

/*
// immplements the Arduino Mega logics: when to give it power

// check that no problem with the number of remaining sleeps
void check_number_reminding_mega_sleep_cycles(void){
  if (remaining_before_mega_wakeup > CYCLES_BEFORE_MEGA_WAKEUP){
    #if DEBUG
      Serial.println(F("Error; reset number of cycles before Mega up"));
    #endif

    remaining_before_mega_wakeup = CYCLES_BEFORE_MEGA_WAKEUP;
  }

  else if(remaining_before_mega_wakeup < 0){
    #if DEBUG
      Serial.println(F("Error; negative number of cycles before Mega up"));
    #endif

    remaining_before_mega_wakeup = 0;
  }
}

void update_awake_mega(void){
  command_from_mega = digitalRead(PIN_FBK_MGA);
  // if the Mega is asking more current, just keep it awake
  if (command_from_mega){
    #if DEBUG
      Serial.println(F("Mega is asking for more current; keep awake"));
      delay(50);
    #endif
    #if SHOW_LED
      controlled_blink(25, 20);
    #endif
  }
  // if not asking for more current, time to shut it down
  else{
    #if DEBUG
      Serial.println(F("Mega is not asking for more current; shut down"));
      delay(50);
    #endif
    #if SHOW_LED
      controlled_blink(500, 2);
    #endif

  // disconnect the Mega
  pinMode(PIN_MFT_MGA, INPUT);
  mega_awake = false;
  remaining_before_mega_wakeup = CYCLES_BEFORE_MEGA_WAKEUP;
  }
}

void update_sleeping_mega(void){
  // if time to wake up
  if (0 == remaining_before_mega_wakeup){
    finished_sleep_mega();
  }

  // else, it is not yet time to wake up the Mega
  else{
    more_sleep_mega();
  }
}

void enough_battery_for_mega_wakeup(void){
  #if DEBUG
    Serial.println(F("Enough battery"));
    delay(50);
  #endif

  // power the Mega
  pinMode(PIN_MFT_MGA, OUTPUT);
  digitalWrite(PIN_MFT_MGA, HIGH);

  // see if the Mega actually wanted to be waken up
  // this is because we could decide to change the sampling frequency
  // give the Mega the time to boot
  wdt_reset();

  delay(4000);

  wdt_reset();

  // measure again the level of the Mega
  command_from_mega = digitalRead(PIN_FBK_MGA);

  // if the Mega wants to be up, it is now really awake
  if (command_from_mega){
    #if DEBUG
      Serial.println(F("The mega wants to be up"));
    #endif
    mega_awake = true;
  }

  // if the Mega does not want to be up, put it down again
  else{
    #if DEBUG
      Serial.println(F("The mega wants to be down"));
    #endif

    pinMode(PIN_MFT_MGA, INPUT);
    mega_awake = false;
  }
}

void not_enough_battery_for_mega_wakeup(void){
  #if DEBUG
    Serial.println(F("Not enough battery"));
    Serial.println(F("Skip this wakeup"));
    delay(50);
  #endif

  // mega sleeps and gets no current
  pinMode(PIN_MFT_MGA, INPUT);
  mega_awake = false;
}

void finished_sleep_mega(void){
    #if DEBUG
      Serial.println(F("Time to wake up the mega"));
      delay(50);
    #endif
    #if SHOW_LED
      controlled_blink(50, 10);
    #endif

    // if battery is enough to power the Mega, check if it wants to wake up
    if (BAT_EMPTY_V < meas_battery){
      enough_battery_for_mega_wakeup();
    }

    // if battery is not enough to power the Mega; try to save the logger, do not
    // wake up the Mega
    else{
      not_enough_battery_for_mega_wakeup();
    }

    remaining_before_mega_wakeup = CYCLES_BEFORE_MEGA_WAKEUP;
}

void more_sleep_mega(void){
  #if DEBUG
    Serial.println(F("Mega should sleep more"));
    delay(50);
  #endif

  remaining_before_mega_wakeup--;
}

void update_mega_switching(void){
  // what to do if the Mega already is awake: check if asks for more current
  if (mega_awake){
    update_awake_mega();
  }

  // otherwise, if the Mega is not awake, see if time to wake it up
  else{
    update_sleeping_mega();
  }
}
*/


////////////////////////////////////////////////////////////////////////////////////////////////////
// Solar panel stuff
////////////////////////////////////////////////////////////////////////////////////////////////////

// implements the solar panel logics: when to couple it

// return true if should connect the battery and the solar panel, return false if not
bool should_connect_array(float meas_battery, float solar_panel_voltage){

  if (meas_battery > BAT_THRESHOLD_V){
    #if DEBUG
      Serial.println(F("Full battery"));
      delay(50);
    #endif
    return(false);
  }

  else if (solar_panel_voltage < meas_battery + MIN_MARGIN_PANEL){
    #if DEBUG
      Serial.println(F("Not full battery but too low solar panel output"));
      delay(50);
    #endif
    return(false);
  }

  else{
    #if DEBUG
      Serial.println(F("Not full battery and good solar panel output"));
      delay(50);
    #endif
    return(true);
  }

}

// return the tension accross the solar panel. For this, need to disconnect solar panel.
// Should call take_care_battery_solar after this function to not let the solar panel
// simply disconnected.
float read_solar_panel_anode(){

  // disconnect array from battery, otherwise measure battery voltage
  pinMode(PIN_MFT_SOL, INPUT);

  // a bit of time for tensions to settle
  delay(10);

  // measure on - of solar panel
  int value_measurement_solar_panel_anode;
  value_measurement_solar_panel_anode = analogRead(PIN_MSR_SOL);

  // convert ADC to V
  float value_measurement_solar_panel_anode_V;
  value_measurement_solar_panel_anode_V = 5.0 / 1024.0 * float(value_measurement_solar_panel_anode);

  // compute effect voltage divider
  float value_solar_panel_anode_V;
  value_solar_panel_anode_V = (value_measurement_solar_panel_anode_V - 5.0 * ALPHA) / (1.0 - ALPHA);

  return(value_solar_panel_anode_V);
}

// take care of connecting or not the solar pannel
void take_care_battery_solar(bool connect_battery_panel){

  if (connect_battery_panel){
    #if DEBUG
      Serial.println(F("Connect battery panel now"));
    #endif
    #if SHOW_LED
      controlled_blink(25, 20);
    #endif
    pinMode(PIN_MFT_SOL, OUTPUT);
    digitalWrite(PIN_MFT_SOL, HIGH);
  }

  else{
    #if DEBUG
      Serial.println(F("Disconnect battery panel now"));
    #endif
    #if SHOW_LED
      controlled_blink(500, 2);
    #endif
    pinMode(PIN_MFT_SOL, INPUT);
  }

  #if SHOW_LED
    delay(1000);
  #endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Blinking stuff
////////////////////////////////////////////////////////////////////////////////////////////////////

// simply some optional blinking possibilities; used mostly for debuggin.

// blink in a controlled way: ms_timing is how fast blinks (same duration light and
// no light), nbr_blinks is how many flashes
void controlled_blink(int ms_timing, int nbr_blinks){
  for (int blink_nbr=0; blink_nbr<nbr_blinks; blink_nbr++){
    digitalWrite(PIN_CMD_LED, HIGH);
    delay(ms_timing);
    digitalWrite(PIN_CMD_LED, LOW);
    delay(ms_timing);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Sleeping stuff
////////////////////////////////////////////////////////////////////////////////////////////////////

// functions used for controlling sleep
// main idea:
// - get it possible to make the Arduino sleep for a number of 8s sequences
// - get out of sleep through interrupt
// - when the watchdog fires, the ISR checks if it is a legit firing (was sleeping, resume)
// or if it is a bug (bug, reboot)

// interrupt raised by the watchdog firing
// when the watchdog fires, this function will be executed
// remember that interrupts are disabled in ISR functions
// now we must check if the board is sleeping or if this is a genuine
// interrupt (hanging)
ISR(WDT_vect)
{
    // Check if we are in sleep mode or it is a genuine WDR.
    if(nbr_remaining > 0)
    {
        // not hang out, just waiting
        // decrease the number of remaining avail loops
        nbr_remaining = nbr_remaining - 1;
        wdt_reset();
    }
    else
    {
        // must be rebooted
        // configure
        MCUSR = 0;                          // reset flags

                                            // Put timer in reset-only mode:
        WDTCSR |= 0b00011000;               // Enter config mode.
        WDTCSR =  0b00001000 | 0b000000;    // clr WDIE (interrupt enable...7th from left)
                                            // set WDE (reset enable...4th from left), and set delay interval
                                            // reset system in 16 ms...
                                            // unless wdt_disable() in loop() is reached first

        // reboot
        while(1);
    }
}

// function to configure the watchdog: let it sleep 8 seconds before firing
// when firing, configure it for resuming program execution by default
// hangs will correspond to watchdog fired when nbr_remaining <= 0 and will
// be determined in the ISR function
void configure_wdt(void)
{

  cli();                           // disable interrupts for changing the registers

  MCUSR = 0;                       // reset status register flags

                                   // Put timer in interrupt-only mode:
  WDTCSR |= 0b00011000;            // Set WDCE (5th from left) and WDE (4th from left) to enter config mode,
                                   // using bitwise OR assignment (leaves other bits unchanged).
  WDTCSR =  0b01000000 | 0b100001; // set WDIE: interrupt enabled
                                   // clr WDE: reset disabled
                                   // and set delay interval (right side of bar) to 8 seconds

  sei();                           // re-enable interrupts
}

// Put the Arduino to deep sleep for a number of cycles. Only an interrupt can wake it up.
void sleep(int ncycles)
{
  nbr_remaining = ncycles; // defines how many cycles should sleep

  // let all non needed pins float to reduce power use
  #if SHOW_LED
    pinMode(PIN_CMD_LED, INPUT);
  #endif

  // Set sleep to full power down.  Only external interrupts or
  // the watchdog timer can wake the CPU!
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  // Turn off all modules and the ADC while asleep.
  power_all_disable();
  // power_adc_disable();
  cbi(ADCSRA,ADEN);

  while (nbr_remaining > 0){ // while some cycles left, sleep!

    // Enable sleep
    sleep_enable();

    // enter sleep
    sleep_mode();

    // CPU is now asleep and program execution completely halts!
    // when the watchdog fires, the ISR will be called.
    // Once awake after ISR, execution will resume at this point if the
    // watchdog is configured for resume rather than restart

    // When awake, disable sleep mode
    sleep_disable();

  }

  // put everything on again
  sbi(ADCSRA,ADEN);
  power_all_enable();

  // put non needed pins out of float
  #if SHOW_LED
    pinMode(PIN_CMD_LED, OUTPUT);
    digitalWrite(PIN_CMD_LED, LOW);
  #endif
}
