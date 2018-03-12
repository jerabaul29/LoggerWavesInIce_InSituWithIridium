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

  while (!SD.begin(PIN_SELECT_SD))
  {
  }

  is_started = true;

  SDManager::update_current_file();
}

// TODO: break this in several methods: update the name, and close / open the datafile
void SDManager::update_current_file(char file_prefix = 'F'){
  PDEBMSG("call to update_current_file")

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
    EEPROMWritelong(address_number_file, new_value_fileIndex);
  }
  // otherwise, not a new case, just some additional data
  else{
    new_value_fileIndex = value_before_fileIndex;
  }

  this->set_filename(file_prefix, new_value_fileIndex);

  delay(5);
  // open the file. only one file can be open at a time,
  dataFile = SD.open(current_file_name, FILE_WRITE);
  delay(5);

  PDEBVAR(this->current_file_name)
}

void SDManager::set_filename(char file_prefix, long number_value){
  PDEBMSG("call to set_filename")

  // generate the string to put as the file numbering
  String str_index = String(number_value);
  int str_length = str_index.length();

  // put the prefix in the filename
  current_file_name[0] = file_prefix;

  // put the characters of the name at the right place
  for (int ind_rank = 0; ind_rank < min(NBR_ZEROS_FILENAME, str_length); ind_rank++)
  {
    current_file_name[NBR_ZEROS_FILENAME - ind_rank] = str_index[str_length - 1 - ind_rank];
  }
}

void SDManager::open_current_filenumber(char file_prefix){
  // close currently opened file
  delay(5);
  dataFile.close();
  delay(5);

  // the current filenumber is the one saved on EEPROM
  long value_fileIndex = EEPROMReadlong(address_number_file);

  this->set_filename(file_prefix, value_fileIndex);

  delay(5);
  dataFile = SD.open(current_file_name);
  delay(5);
}

bool SDManager::more_to_read(void){
  // can open file
  if (dataFile) {
    // more to read
    if (dataFile.available()) {
      return(true);
    }
    // nothing more to read
    else{
      return(false);
    }
  }
  // cannot open file
  else{
    return(false);
  }
}

char SDManager::read_char(void){
  return(dataFile.read());
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

void SDManager::write_char(char crrt_char){
  dataFile.print(crrt_char);
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
