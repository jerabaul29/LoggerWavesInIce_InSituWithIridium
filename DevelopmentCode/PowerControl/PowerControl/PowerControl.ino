/*
 * ATMega328P power control
 * 
 * Pins:
 * Pin DGTL 2: control MOSFET for Battery / Solar connection
 * Pin DGTL 3: feedback from Mega
 * Pin A0: measure battery +
 * Pin A1: measure solar - through divider of tension
 * 
 * Workflow:
 * - deep sleep most of the time
 * - wake up every 8 seconds
 *     - check voltage battery; if necessary, disconnect solar pannel
 *     - if Mega has current: check if done (LOW), if yes shut
 *       down, set count_sleep and make ready for long period sleep
 *     - if count_sleep is 0, wake up Mega
 *     - if Mega has no current: decrement count_sleep
 */

 /*
  * TODO - NOTES
  * 
  *  - add measurement of solar panel voltage
  *  - add battery / solar panel update each wakening up
  */

#include <avr/wdt.h>            // library for default watchdog functions
#include <avr/interrupt.h>      // library for interrupts handling
#include <avr/sleep.h>          // library for sleep
#include <avr/power.h>          // library for power control

#define DEBUG true

#define PIN_FBK_MGA 3      // feedback from Mega
#define PIN_CMD_LED 13      // command of intern LED
#define PIN_MSR_BAT A0      // measure of battery
#define PIN_MFT_SOL 2      // command mosfet solar panel / battery
#define PIN_MFT_MGA 4      // command mosfet giving to the Mega

#define BAT_THRESHOLD 748  // threshold for full battery 3.65V: 1024 * 3.65 / 5.0 approx 748
#define EPS_VOLT 20 // epsilon voltage; 20 is 20 * 5 / 1024.0 approx 0.1V
int meas_battery = 0;
int meas_solar_panel = 0;

volatile int count_sleep;

//clear bit macro
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

//set bit macro
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

void setup() {

  if (DEBUG){
    Serial.begin(9600);
    Serial.println(F("Booting..."));
  }


  if (DEBUG){
    Serial.println(F("Set pins..."));
  }
  // set digital pins (analog pins do not need to be set)
  pinMode(PIN_FBK_MGA, INPUT);
  pinMode(PIN_CMD_LED, OUTPUT);
  pinMode(PIN_MFT_SOL, OUTPUT);
  pinMode(PIN_MFT_MGA, OUTPUT);

  digitalWrite(PIN_CMD_LED, LOW);
  digitalWrite(PIN_MFT_SOL, LOW);
  digitalWrite(PIN_MFT_MGA, LOW);

  // blink slowly once (NOTE: this must be after PIN_CMD_LED init)
  if (DEBUG){
    controlled_blink(3000, 3);
  }

  if (DEBUG){
    Serial.println(F("Configure watchodg..."));
  }
  // configure watchdog
  configure_wdt();  // now watchdog is running! fire after 8s! do not forget to wdt_reset(); !
  
  // sleep configuration is taken care of in sleep()

  // blink fast many times; if blink for too long, watchdog fires
  if (DEBUG){
    controlled_blink(50, 20);
    wdt_reset();
    delay(5000);
    wdt_reset();
  }
  
  wdt_reset();
}

void loop() {
  // loop is one complete cycle: check battery, let Mega work, sleep
  // ie loop is 'slow'. This is not a problem as the watchdog is awake
  // so if something hangs, just restart.

  if (DEBUG){
    Serial.println(F("Start new cycle..."));
  }
  // blink fast many times; if blink for too long, watchdog fires
  if (DEBUG){
    controlled_blink(50, 20);
    wdt_reset();
    delay(5000);
    wdt_reset();
  }

  // --------------------------------------------------------------
  // take care of check about battery voltage ---------------------
  // --------------------------------------------------------------

  if (DEBUG){
    Serial.println(F("Check battery voltage..."));
  }
  // checks only voltage battery
  take_care_battery_solar_simple();

  // or

  // checks both voltage battery and solar panel, and charge only
  // if panel is higher than battery [in addition to max battery]
  // take_care_battery_solar();

  wdt_reset();

  // --------------------------------------------------------------
  // take care of power to Mega -----------------------------------
  // --------------------------------------------------------------

  if (DEBUG){
    Serial.println(F("Take care of the Mega..."));
  }
  // give power to the Mega
  //digitalWrite();
  
  wdt_reset();

  // --------------------------------------------------------------
  // when taken care of everything: sleep for a delay between measurements
  // --------------------------------------------------------------

  if (DEBUG){
    Serial.println(F("Sleep..."));
  }
  sleep(1);
  wdt_reset();

}


////////////////////////////////////////////////////////////////////////////////////////////////////
// Battery and solar panel stuff
////////////////////////////////////////////////////////////////////////////////////////////////////

void take_care_battery_solar_simple(void){
  /*
   * Take care of battery and solar panel.
   * This is less sophisticated: just measure voltage
   * battey, do not care about effect solar panel
   */

  // connect MOSFET, in case analog input is between
  // MOSFET and solar panel
  pinMode(PIN_MFT_SOL, OUTPUT);
  digitalWrite(PIN_MFT_SOL, HIGH);

  // do a few conversions to be sure no strange transient
  // effect
  for(int i = 0; i < 5; i++){
    meas_battery = analogRead(PIN_MSR_BAT);
  }
  
  if (meas_battery > BAT_THRESHOLD){
    // full battery!
    
    // disconnect MOSFET
    digitalWrite(PIN_MFT_SOL, LOW);
    // even more: let the MOSFET float
    pinMode(PIN_MFT_SOL, INPUT);

    // blink; if blink for too long, watchdog fires
    if (DEBUG){
      controlled_blink(1000, 1);
      wdt_reset();
      delay(5000);
      wdt_reset();
    }
  }
  else{
    // not full battery
    
    // connect MOSFET
    digitalWrite(PIN_MFT_SOL, HIGH);

    // blink; if blink for too long, watchdog fires
    if (DEBUG){
      controlled_blink(1000, 2);
      wdt_reset();
      delay(5000);
      wdt_reset();
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Blinking stuff
////////////////////////////////////////////////////////////////////////////////////////////////////

// blink in a controlled way: ms_timing is how fast blinks, nbr_blinks is how many flashes
void controlled_blink(int ms_timing, int nbr_blinks){
  for (int blink_nbr=0; blink_nbr<nbr_blinks; blink_nbr++){
    digitalWrite(PIN_CMD_LED, HIGH);
    delay(ms_timing);
    digitalWrite(PIN_CMD_LED, LOW);
    delay(ms_timing);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Watchdog low power and sleep stuff
////////////////////////////////////////////////////////////////////////////////////////////////////

ISR(WDT_vect)
{
    // Check if we are in sleep mode or it is a genuine WDR.
    if(count_sleep > 0)
    {
        // not hang out, just waiting
        // decrease the number of remaining avail loops
        count_sleep = count_sleep - 1;
        wdt_reset();
    }
    else
    {
        // must be rebooted
        // configure
        MCUSR = 0;                          // reset flags
       
                                            // Put timer in reset-only mode:
        WDTCR |= 0b00011000;                // Enter config mode.
        WDTCR =  0b00001000 | 0b000000;     // clr WDIE (interrupt enable...7th from left)
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
  WDTCR |= 0b00011000;             // Set WDCE (5th from left) and WDE (4th from left) to enter config mode,
                                   // using bitwise OR assignment (leaves other bits unchanged).
  WDTCR =  0b01000000 | 0b100001;  // set WDIE: interrupt enabled
                                   // clr WDE: reset disabled
                                   // and set delay interval (right side of bar) to 8 seconds

  sei();                           // re-enable interrupts 
}

// Put the Arduino to deep sleep. Only an interrupt can wake it up.
void sleep(int ncycles)
{  
  count_sleep = ncycles; // defines how many cycles should sleep

  /*
   * NOTE: must put the pins here to let float
   * CAREFUL with Pin SOL: do not set it here!
   */

  pinMode(PIN_CMD_LED, INPUT);
  pinMode(PIN_MFT_MGA, INPUT);
  
  // Set sleep to full power down.  Only external interrupts or
  // the watchdog timer can wake the CPU!
  power_all_disable();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
 
  // Turn off the ADC while asleep.
  // power_adc_disable();
  cbi(ADCSRA,ADEN);
 
  while (count_sleep > 0){ // while some cycles left, sleep!

    // Enable sleep and enter sleep mode.
    sleep_mode();

    // CPU is now asleep and program execution completely halts!
    // Once awake, execution will resume at this point if the
    // watchdog is configured for resume rather than restart
 
    // When awake, disable sleep mode
    sleep_disable();
  
    // we have slept one time more
    // count_sleep = count_sleep - 1;
 
  }
 
  // put everything on again
  // power_all_enable();
  sbi(ADCSRA,ADEN);
  power_all_enable();

  /*
   * NOTE: must put the pins here to put them back again
   * CAREFUL with Pin SOL: do not set it here!
   */

  pinMode(PIN_CMD_LED, OUTPUT);
  pinMode(PIN_MFT_MGA, OUTPUT);

  digitalWrite(PIN_CMD_LED, LOW);
  digitalWrite(PIN_MFT_MGA, LOW);

   
}



