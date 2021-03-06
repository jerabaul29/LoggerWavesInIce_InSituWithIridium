#include <SPI.h>
#include <SD.h>
#include "EEPROM_interaction.h"
#include "parameters.h"
#include <stdlib.h>
#include <Arduino.h>
#include "DebugMacros.h"

#ifndef SD_MANAGER
#define SD_MANAGER

// TODO: the interface used with the RaspberryPi is not very good; modify

class SDManager{
    public:
        SDManager(void);

        /*
         Start the SD card
         */
        void start_sd(void);

        /*
         * Update the current file, based on the values stored in EEPROM memory, update the EEPROM,
         * and open the new current file.
         * If the file_prefix is 'F', then the number of the file will be increased
         * If the file prefix is other than F, consider it is just additional data,
         * no file number increase.
         */
        void update_current_file(char file_prefix = 'F');

        /*
         * Post a string on the SD card.
         */
        void post_on_SD_card(String message_to_post, bool should_timestamp=true);

        /*
         Post array_to_post until end_position (not included) on the SD card.
         */
         void post_on_SD_card(char array_to_post[], int end_position);

        /*
          Post binary data
        */
        void post_on_SD_card(byte * byte_array, int end_position);

        // int return_filenumber(void) const;

        /*
          give the array as read only; we know how long it is since it is in parameters
        */
        const char * get_filename(void) const;

        /*
          close the currently active datafile
        */
        void close_datafile(void);

        /*
          open the current file number prefixed with file_prefix
        */
        void open_current_filenumber(char file_prefix);

        /*
          true if more to read, false otherwise
        */
        bool more_to_read(void);

        /*
          read one byte from the file
        */
        char read_char(void);

        /*
          write a char to the currently opened file
        */
        void write_char(char);

    private:
        /*
         Post the current millis timestamp.
         */
        void post_timestamp(void);

        /*
          set the filename.
        */
        void set_filename(char file_prefix, long number_value);

        /*
         Check if the SD card is available, if not let the watchdog reboot.
         */
        void check_SD_available(void);

        File dataFile;
        char current_file_name[NBR_ZEROS_FILENAME + 2];
        // char current_file_name[NBR_ZEROS_FILENAME + 2] = "F00000";
        // char current_file_name[] = "F00000";
        const long address_number_file = ADDRESS_LONG_FILENBR;
        bool is_started = false;
        long filenumber;
        
};

#endif

// TODO: add methods to close / read the current dataFile
// TODO: add methods that make sure opened or closed and keep track of status