// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// SD card filename
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// filename, will be modified based on EEPROM data for saving on several files
char currentFileName[] = "F00000";
// number of zeros after the letter in the name convention
int nbrOfZeros = 5;

// time tracking variable for writting new file
unsigned long time_tracking_WNF = 1;

// EEPROM gestion for filename on SD card ---------------------------------------
// address in which stores the current file numbering; this will extend from
// this address to 4 more bytes (use long int)
int address_numberReset = 1; // ie EEPROM bytes 1, 2, 3, and 4 are used
