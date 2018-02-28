#include "Arduino.h"
#include <EEPROM.h>

#ifndef EEPROM_INTERACTION
#define EEPROM_INTERACTION

/*
 * Write 4 bytes long to EEPROM splitted on 4 bytes.
 * Uses specifed address to specifed address + 3 included
 */
void EEPROMWritelong(int address, long value);

/*
 * Read 4 bytes of memory from the EEPROM, at (address) up and included to (address + 3)
 * and assemble into a long variable.
 */
long EEPROMReadlong(int address);

#endif
