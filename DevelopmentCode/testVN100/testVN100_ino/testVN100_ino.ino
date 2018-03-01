void setup(){
  Serial.begin(57600);
  Serial.println(F("Booted"));
  
  Serial1.begin(57600);
}

void loop(){
  if (Serial1.available() > 0){
    char crrt_in = Serial1.read();
    Serial.println(crrt_in);
  }
}
