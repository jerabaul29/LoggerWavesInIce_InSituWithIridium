#include "SDManager.h"

SDManager::SDManager(void) : is_started(false)
{
}

void SDManager::start_sd(void)
{

#if DEBUG
  SERIAL_DEBUG.println(F("start SD"));
#endif

  // intialise the filename
  this->current_file_name[0] = 'F';
  for (int i = 0; i < NBR_ZEROS_FILENAME; i++)
  {
    this->current_file_name[1 + i] = '0';
  }
  current_file_name[NBR_ZEROS_FILENAME + 1] = '\0';

#if DEBUG
  SERIAL_DEBUG.print(F("Filename initialized:"));
  for (int i = 0; i < NBR_ZEROS_FILENAME + 2; i++)
  {
    SERIAL_DEBUG.print(this->current_file_name[i]);
  }
  SERIAL_DEBUG.println();
#endif

  while (!SD.begin(PIN_SELECT_SD))
  {
  }

  is_started = true;

  SDManager::update_current_file();

#if DEBUG
  SERIAL_DEBUG.print(F("Filename set:"));
  for (int i = 0; i < NBR_ZEROS_FILENAME + 2; i++)
  {
    SERIAL_DEBUG.print(this->current_file_name[i]);
  }
  SERIAL_DEBUG.println();
#endif
}

// TODO: break this in several methods: update the name, and close / open the datafile
void SDManager::update_current_file(void)
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

  // update it by increasing by 1. This is the new file number to write on
  long new_value_fileIndex = value_before_fileIndex + 1L;
  filenumber = new_value_fileIndex;
  EEPROMWritelong(address_number_file, new_value_fileIndex);

  // generate the string to put as the file numbering
  String str_index = String(new_value_fileIndex);
  int str_length = str_index.length();

  #if DEBUG
  SERIAL_DEBUG.print(F("string length: "));
SERIAL_DEBUG.print(str_length);
#endif

  // put the characters of the name at the right place
  for (int ind_rank = 0; ind_rank < min(NBR_ZEROS_FILENAME, str_length); ind_rank++)
  {
    current_file_name[NBR_ZEROS_FILENAME - ind_rank] = str_index[str_length - 1 - ind_rank];
    #if DEBUG
  SERIAL_DEBUG.print(F("set: "));
SERIAL_DEBUG.print(str_index[str_length - 1 - ind_rank]);
SERIAL_DEBUG.print(F(" at rank: "));
SERIAL_DEBUG.print(NBR_ZEROS_FILENAME - ind_rank);
  SERIAL_DEBUG.println();
#endif
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

  dataFile.print("M");
  dataFile.println(String(millis()));

  delay(5);
}

void SDManager::post_on_SD_card(String dataStringPost)
{

  this->check_SD_available();

  dataFile.println(dataStringPost);

  this->post_timestamp();

  delay(5);
}

void SDManager::post_on_SD_card(char array_to_post[], int end_position)
{

  this->check_SD_available();

  dataFile.write(array_to_post, end_position);
  dataFile.print("\n");

  this->post_timestamp();

  delay(5);
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
