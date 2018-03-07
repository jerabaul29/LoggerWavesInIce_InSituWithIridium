#include "SDManager.h"

SDManager::SDManager(void) : is_started(false)
{
}

void SDManager::start_sd(void)
{

  PDEBMSG("start SD")

  // intialise the filename
  this->current_file_name[0] = 'F';
  for (int i = 0; i < NBR_ZEROS_FILENAME; i++)
  {
    this->current_file_name[1 + i] = '0';
  }
  current_file_name[NBR_ZEROS_FILENAME + 1] = '\0';

  PDEBVAR(current_file_name)

  while (!SD.begin(PIN_SELECT_SD))
  {
  }

  is_started = true;

  SDManager::update_current_file();

  PDEBVAR(this->current_file_name)
}

// TODO: break this in several methods: update the name, and close / open the datafile
void SDManager::update_current_file(char file_prefix = 'F')
{
  if (!is_started)
  {
    this->start_sd();
  }

  // note: could be problem at initialization in the setup loop, no file opened yet
  // but the library is handling the exception right
  // close current file
  delay(5);
  dataFile.close();
  delay(5);

  // read the current file number in EEPROM
  long value_before_fileIndex = EEPROMReadlong(address_number_file);
  long new_value_fileIndex;
  
  // update it by increasing by 1 if a 'F' prefix. This is the new file number to write on
  if (file_prefix == 'F'){
    new_value_fileIndex = value_before_fileIndex + 1L;
    filenumber = new_value_fileIndex;
    EEPROMWritelong(address_number_file, new_value_fileIndex);
  }
  // otherwise, not a new case, just some additional data
  else{
    new_value_fileIndex = value_before_fileIndex;
    filenumber = new_value_fileIndex;
  }

  // generate the string to put as the file numbering
  String str_index = String(new_value_fileIndex);
  int str_length = str_index.length();

  // put the prefix in the filename
  current_file_name[0] = file_prefix;

  // put the characters of the name at the right place
  for (int ind_rank = 0; ind_rank < min(NBR_ZEROS_FILENAME, str_length); ind_rank++)
  {
    current_file_name[NBR_ZEROS_FILENAME - ind_rank] = str_index[str_length - 1 - ind_rank];
  }

  delay(5);
  // open the file. only one file can be open at a time,
  dataFile = SD.open(current_file_name, FILE_WRITE);
  delay(5);
}

void SDManager::check_SD_available()
{
  if (!dataFile)
  {
    while (1)
    {
      // do nothing: watchdog will fire
    }
  }
}

void SDManager::close_datafile(void)
{
  dataFile.close();
}

void SDManager::post_timestamp(void)
{
  this->check_SD_available();

  dataFile.print("M,");
  dataFile.println(String(millis()));
  dataFile.println();

  // delay(5);
}

void SDManager::post_on_SD_card(String dataStringPost, bool should_timestamp=true)
{

  this->check_SD_available();

  dataFile.println(dataStringPost);

  if (should_timestamp){
    this->post_timestamp();
  }

  // delay(5);
}

void SDManager::post_on_SD_card(char array_to_post[], int end_position)
{

  this->check_SD_available();

  dataFile.write(array_to_post, end_position);
  dataFile.print('\n');

  this->post_timestamp();

  // delay(5);
}

void SDManager::post_on_SD_card(byte * byte_array, int end_position){
  this->check_SD_available();

  dataFile.write(byte_array, end_position);
  dataFile.write('\n');

  this->post_timestamp();

  // delay(5); // TODO: take away all the hard coding in delays
}

const char *SDManager::get_filename(void) const
{
  return (current_file_name);
}

/*
NOTE: a piece of code that could be included for debugging

#if SERIAL_PRINT
  // show the content of all files
  root = SD.open("/");
  printDirectory(root, 0);
  Serial.println("D;done!");
#endif
 */
