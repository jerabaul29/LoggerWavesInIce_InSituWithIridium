/*
 * Code for power control
 * 
 * TODO:
 * 
 * put the pololu on a MOSFET from the battery side (to disconnect it when not needed, ie when not using
 * the Mega). Then need to be sure voltage on capacitors do not get too low: re connect pololu when logging with
 * Mega, and connect at least 8 seconds each 15 minutes [note: check values].
 * 
 */

#include <avr/wdt.h>            // library for default watchdog functions
#include <avr/interrupt.h>      // library for interrupts handling
#include <avr/sleep.h>          // library for sleep
#include <avr/power.h>          // library for power control

// how many times remain to sleep before wake up
// volatile to be modified in interrupt function
volatile int nbr_remaining;

#define DEBUG false
#define SHOW_LED true

#define PIN_MSR_BAT A0      // measure of battery
#define PIN_MSR_SOL A1      // measure of solar panel on anode

#define alpha 0.41          // R_2 / (R_1 + R_2) in voltage divider

#define PIN_MFT_SOL 2      // command mosfet solar panel / battery
#define PIN_FBK_MGA 3      // feedback from Mega
#define PIN_MFT_MGA 4      // command mosfet giving to the Mega
#define PIN_CMD_LED 13      // command of intern LED

#define BAT_THRESHOLD_V 3.6  // threshold for full battery
#define BAT_EMPTY_V 2.85     // threshold for empty battery
#define BAT_THRESHOLD_C 758  // threshold for full battery 3.7V: 1024 * 3.7 / 5.0 approx 748
#define EPS_VOLT 20 // epsilon voltage; 20 is 20 * 5 / 1024.0 approx 0.1V
#define MIN_MARGIN_PANEL 0.5  // minimum over voltage panel vs battery for it to be worth connecting

#define CYCLES_BEFORE_MEGA_WAKEUP 10 // number of loop () cycles before waking up the Mega
                                     // if deep sleep 80s, 4 loop() is a bit over 5 minutes

#define CYCLES_DEEP_SLEEP 10 // for production
// #define CYCLES_DEEP_SLEEP 1  // for tests

float meas_battery = 0.0;
float meas_solar_panel_anode = 0.0;
float solar_panel_voltage = 0.0;

bool connect_battery_panel = false;

int command_from_mega = 0;

int remaining_before_mega_wakeup = CYCLES_BEFORE_MEGA_WAKEUP;
bool mega_awake = false;

//clear bit macro
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

//set bit macro
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

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

void loop() {
  wdt_reset();

  #if SHOW_LED
    controlled_blink(50, 20);
    delay(2000);
  #endif

  wdt_reset();

  #if DEBUG
    Serial.println();
    Serial.println(F("Start new cycle..."));
    delay(50);
  #endif

  // --------------------------------------------------------------
  // All measurements
  // --------------------------------------------------------------
  meas_battery = float(analogRead(PIN_MSR_BAT)) * 5.0 / 1024.0;
  meas_solar_panel_anode = read_solar_panel_anode();
  solar_panel_voltage = meas_battery - meas_solar_panel_anode;
  command_from_mega = digitalRead(PIN_FBK_MGA);


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
  connect_battery_panel = should_connect_array();
  if (connect_battery_panel){
    #if DEBUG
      Serial.println(F("Connect battery panel now"));
    #endif
    #if SHOW_LED
      controlled_blink(25, 20);
    #endif
    pinMode(PIN_MFT_SOL, OUTPUT);
    digitalWrite(PIN_MFT_SOL, HIGH);
    delay(50);
  }
  else{
    #if DEBUG
      Serial.println(F("Disconnect battery panel now"));
    #endif
    #if SHOW_LED
      controlled_blink(500, 2);
    #endif
    pinMode(PIN_MFT_SOL, INPUT);
    delay(50);
  }

  #if SHOW_LED
    delay(1000);
  #endif

  wdt_reset();

  // --------------------------------------------------------------
  // Decide what to do with the Arduino Mega
  // --------------------------------------------------------------
  if (mega_awake){
    if (command_from_mega){
      #if DEBUG
        Serial.println(F("Mega is asking for more current; keep awake"));
        delay(50);
      #endif
      #if SHOW_LED
        controlled_blink(25, 20);
      #endif
    }
    else{
      #if DEBUG
        Serial.println(F("Mega is not asking for more current; shut down"));
        delay(50);
      #endif
      #if SHOW_LED
        controlled_blink(500, 2);
      #endif

    pinMode(PIN_MFT_MGA, INPUT);

    mega_awake = false;
    remaining_before_mega_wakeup = CYCLES_BEFORE_MEGA_WAKEUP;
    }
  }

  else{
    if (remaining_before_mega_wakeup <= 0){
      #if DEBUG
        Serial.println(F("Time to wake up the mega"));
        delay(50);
      #endif
      #if SHOW_LED
        controlled_blink(50, 10);
      #endif

      if (BAT_EMPTY_V < meas_battery){
        #if DEBUG
          Serial.println(F("Enough battery"));
          delay(50);
        #endif
      
        pinMode(PIN_MFT_MGA, OUTPUT);
        digitalWrite(PIN_MFT_MGA, HIGH);

        // see if the Mega actually wanted to be waken up
        // give the Mega the time to boot
        delay(2000);

        // measure again the level of the Mega
        command_from_mega = digitalRead(PIN_FBK_MGA);

        if (command_from_mega){
          Serial.println(F("The mega wants to be up"));
          mega_awake = true;
        }

        else{
          Serial.println(F("The mega wants to be down"));
          mega_awake = false;
          pinMode(PIN_MFT_MGA, INPUT);
        }

      }
      else{
        #if DEBUG
          Serial.println(F("Not enough battery"));
          Serial.println(F("Skip this wakeup"));
          delay(50);
        #endif
      }

      remaining_before_mega_wakeup = CYCLES_BEFORE_MEGA_WAKEUP;
    }
    
    else{
      #if DEBUG
        Serial.println(F("Mega should sleep more"));
        delay(50);
      #endif

      remaining_before_mega_wakeup--;
    }
  }

  wdt_reset();

  // --------------------------------------------------------------
  // Sleep
  // --------------------------------------------------------------
  sleep(CYCLES_DEEP_SLEEP);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Solar panel stuff
////////////////////////////////////////////////////////////////////////////////////////////////////
bool should_connect_array(void){
  // return true if should connect the battery and the solar panel, return false if not

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

float read_solar_panel_anode(){
  // return the tension accross the solar panel
  // Should call take_care_battery_solar after this function to not let the solar panel
  // simply disconnected.

  // open the MOSFET commanding solar panel to be sure that +5V on +
  pinMode(PIN_MFT_SOL, INPUT);

  delay(200);

  // measure on - of solar panel
  int value_measurement_solar_panel_anode;
  value_measurement_solar_panel_anode = analogRead(PIN_MSR_SOL);

  // convert ADC to V
  float value_measurement_solar_panel_anode_f;
  value_measurement_solar_panel_anode_f = 5.0 / 1024.0 * float(value_measurement_solar_panel_anode);

  // compute effect voltage divider
  float value_solar_panel_anode_f;
  value_solar_panel_anode_f = (value_measurement_solar_panel_anode_f - 5.0 * alpha) / (1.0 - alpha);

  return(value_solar_panel_anode_f);
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
// Sleeping stuff
////////////////////////////////////////////////////////////////////////////////////////////////////

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

// Put the Arduino to deep sleep. Only an interrupt can wake it up.
void sleep(int ncycles)
{
  nbr_remaining = ncycles; // defines how many cycles should sleep

  // let all pins float instead
  #if SHOW_LED
    pinMode(PIN_CMD_LED, INPUT);
  #endif

  // Set sleep to full power down.  Only external interrupts or
  // the watchdog timer can wake the CPU!
  power_all_disable();

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  // Turn off the ADC while asleep.
  // power_adc_disable();
  cbi(ADCSRA,ADEN);

  sleep_enable();

  while (nbr_remaining > 0){ // while some cycles left, sleep!

    // Enable sleep and enter sleep mode.
    sleep_mode();

    // CPU is now asleep and program execution completely halts!
    // Once awake, execution will resume at this point if the
    // watchdog is configured for resume rather than restart

    // When awake, disable sleep mode
    sleep_disable();

  }

  // put everything on again
  // power_all_enable();
  sbi(ADCSRA,ADEN);
  power_all_enable();

  #if SHOW_LED
    pinMode(PIN_CMD_LED, OUTPUT);
    digitalWrite(PIN_CMD_LED, LOW);
  #endif
}
