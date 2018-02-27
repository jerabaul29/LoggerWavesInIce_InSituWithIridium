#include "SDManager.h"

SDManager::SDManager(){};

SDManager::update_current_file(void){
  // note: could be problem at initialization in the setup loop, no file opened yet
  // but the library is handling the exception right
  // close current file
  delay(5);
  dataFile.close();
  delay(5);

  // read the current file number in EEPROM
  long value_before_fileIndex = EEPROMReadlong(address_number_file);

  // update it by increasing by 1. This is the new file number to write on
  long new_value_fileIndex = value_before_fileIndex + 1L;
  EEPROMWritelong(address_number_file, new_value_fileIndex);

  // generate the string to put as the file numbering
  String str_index = String(new_value_fileIndex);
  int str_length = str_index.length();

  // put the characters of the name at the right place
  for (int ind_rank = 0; ind_rank < max(nbr_of_numbers_file_name, str_length); ind_rank++){
    currentFileName[nbr_of_numbers_file_name - ind_rank] = str_index[str_length - 1 - ind_rank];
  }

  delay(5);
  // open the file. only one file can be open at a time,
  dataFile = SD.open(currentFileName, FILE_WRITE);
  delay(5);
}

void SDManager::check_SD_available(){
  if (!dataFile){
    while(1){
      // do nothing: watchdog will fire
    }
  }
}

void SDManager::post_timestamp(void){
  this->check_SD_available();

  dataFile.print("M");
  dataFile.println(String(millis()));

  delay(5);
}

void SDManager::post_on_SD_card(String dataStringPost){

  this->check_SD_available();

    dataFile.println(dataStringPost);

    this->post_timestamp();

    delay(5);
}

void SDManager::post_on_SD_card(char [] array_to_post, int end_position){

  this->check_SD_available();

    dataFile.write(array_to_post, end_position);
    dataFile.print("\n");

    this->post_timestamp();

    delay(5);
}
