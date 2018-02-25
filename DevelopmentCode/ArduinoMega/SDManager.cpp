// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// SD card list files
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print("\t");
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// read from EEPROM the name of the next file on which to write, update the EEPROM
// and open the new current file
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void UpdateCurrentFile(){

  // note: could be problem at initialization in the setup loop, no file opened yet
  // but the library is handling the exception right
  // close current file
  delay(5);
  dataFile.close();
  delay(5);

  // read the current file number in EEPROM
  long value_before_fileIndex = EEPROMReadlong(address_numberReset);

  // update it by increasing by 1. This is the new file number to write on
  long new_value_fileIndex = value_before_fileIndex + 1;
  EEPROMWritelong(address_numberReset,new_value_fileIndex);

  // generate the string to put as the file numbering
  String str_index = String(new_value_fileIndex);
  int str_length = str_index.length();

  // put the characters of the name at the right place
  for (int ind_rank = 0; ind_rank < str_length; ind_rank++){
    int i_rank = nbrOfZeros + ind_rank - str_length + 1;
    currentFileName[i_rank] = str_index[ind_rank];
  }

  #if SERIAL_PRINT
    Serial.print("D;str_rank: ");
    Serial.println(str_index);
    Serial.print("D;File name: ");
    Serial.println(currentFileName);
  #endif

  delay(5);
  // open the file. only one file can be open at a time,
  dataFile = SD.open(currentFileName, FILE_WRITE);
  delay(5);
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// log a string on the SD card
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void postSD(String dataStringPost){

  // note: delays are here to avoid conflicts on using the SD card
  // when using it at high frequency

  #if SERIAL_PRINT
    unsigned long startLog = millis();
    Serial.println();
    Serial.println("D;Start post...");
    Serial.print("D;Time log beginning: ");
    Serial.println(startLog);
  #endif

  // if the file is available, write to it:
  if (dataFile) {
    dataStringPost += '\n';
    dataFile.println(dataStringPost);

    delay(5);

    #if SERIAL_PRINT
      Serial.println("D;post success, content:");
      Serial.print(dataStringPost);
    #endif
  }
  // else the file isn't open, pop up an error:
  // and reboot
  else {
    #if SERIAL_PRINT
    Serial.println("D;post failure");
    #endif
    while(1){
      // do nothing watchdog fires
    }
  }

  #if SERIAL_PRINT
    unsigned long endLog = millis();
    Serial.print("D;Time log end: ");
    Serial.println(endLog);
    Serial.print("D;Time log delay: ");
    Serial.println(endLog-startLog);
  #endif

}
