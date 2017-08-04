/*
 * 
 * A simple sketch to initialize the EEPROM of the Arduino Mega to be used as logger
 * 
 */

#include <EEPROM.h>

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// EEPROM
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// EEPROM gestion for filename on SD card ---------------------------------------
// address in which stores the current file numbering; this will extend from
// this address to 4 more bytes (use long int)
int address_numberReset = 1; // ie EEPROM bytes 1, 2, 3, and 4 are used

// EEPROM gestion for sleeping --------------------------------------------------
// address for where number of sleep cycles left stored
int address_sleeps_left = 5;
// address for what is the total number of sleep cycles
int address_total_sleeps = 6;

void setup(){
  EEPROMWritelong(address_numberReset, 0);
  EEPROM.write(address_sleeps_left, 0);
  EEPROM.write(address_total_sleeps, 2);
  
}

void loop(){
  // nothing
}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Write a 4 byte (32bit) long to the eeprom splitted on 4 bytes
// at the specified address up to address + 3
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void EEPROMWritelong(int address, long value)
{
  //Decomposition from a long to 4 bytes by using bitshift.
  //One = Most significant -> Four = Least significant byte
  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  byte two = ((value >> 16) & 0xFF);
  byte one = ((value >> 24) & 0xFF);

  //Write the 4 bytes into the eeprom memory.
  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  EEPROM.write(address + 2, two);
  EEPROM.write(address + 3, one);
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// read 4 bytes of memory from the EEPROM (address up to address + 3)
// assemble the 4 bytes to form a long variable
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
long EEPROMReadlong(long address)
{
  //Read the 4 bytes from the eeprom memory.
  long four = EEPROM.read(address);
  long three = EEPROM.read(address + 1);
  long two = EEPROM.read(address + 2);
  long one = EEPROM.read(address + 3);

  //Return the recomposed long by using bitshift.
  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}
