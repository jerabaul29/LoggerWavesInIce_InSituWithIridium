#define DEBUG true

#define PIN_MSR_BAT A0      // measure of battery
#define PIN_MSR_SOL A1      // measure of solar panel on anode

#define alpha 0.41          // R_2 / (R_1 + R_2) in voltage divider

#define PIN_MFT_SOL 2      // command mosfet solar panel / battery
#define PIN_FBK_MGA 3      // feedback from Mega
#define PIN_MFT_MGA 4      // command mosfet giving to the Mega
#define PIN_CMD_LED 13      // command of intern LED

#define BAT_THRESHOLD 748  // threshold for full battery 3.65V: 1024 * 3.65 / 5.0 approx 748
#define EPS_VOLT 20 // epsilon voltage; 20 is 20 * 5 / 1024.0 approx 0.1V
float meas_battery = 0.0;
float meas_solar_panel_anode = 0.0;
float solar_panel_voltage = 0.0;

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

}

void loop() {
  if (DEBUG){
    Serial.println(F("Start new cycle..."));
  }

  // --------------------------------------------------------------
  // All measurements
  // --------------------------------------------------------------
  meas_battery = float(analogRead(PIN_MSR_BAT)) * 5.0 / 1024.0;
  meas_solar_panel_anode = read_solar_panel_anode();
  solar_panel_voltage = meas_battery - meas_solar_panel_anode;

  if (DEBUG){
    Serial.print(F("meas_battery: "));
    Serial.println(meas_battery);
    Serial.print(F("meas_solar_panel_anode: "));
    Serial.println(meas_solar_panel_anode);
    Serial.print(F("solar_panel_voltage: "));
    Serial.println(solar_panel_voltage);
    
  }

  delay(1000);

}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Solar panel stuff
////////////////////////////////////////////////////////////////////////////////////////////////////
float read_solar_panel_anode(){
  // return the tension accross the solar panel
  // Should call take_care_battery_solar after this function to not let the solar panel
  // simply disconnected.
  
  // open the MOSFET commanding solar panel to be sure that +5V on +
  pinMode(PIN_MFT_SOL, OUTPUT);
  digitalWrite(PIN_MFT_SOL, LOW);

  delay(1000);
  
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
