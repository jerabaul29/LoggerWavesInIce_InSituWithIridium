#include <SPI.h>
#include <SD.h>
#include "EEPROM_interaction.h"
#include "parameters.h"
#include <stdlib.h>
#include <Arduino.h>

#ifndef SD_MANAGER
#define SD_MANAGER

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
         */
        void update_current_file(void);

        /*
         * Post a string on the SD card.
         */
        void post_on_SD_card(String message_to_post);

        /*
         Post array_to_post until end_position (not included) on the SD card.
         */
         void post_on_SD_card(char array_to_post[], int end_position);

         // int return_filenumber(void) const;

        /*
          give the array as read only; we know how long it is since it is in parameters
        */
        const char * get_filename(void) const;

    private:
        /*
         Post the current millis timestamp.
         */
        void post_timestamp(void);

        /*
         Check if the SD card is available, if not let the watchdog reboot.
         */
        void check_SD_available(void);

        File dataFile;
        char current_file_name[NBR_ZEROS_FILENAME + 1];  // initialize from chars: no need for end string \0
        const long address_number_file = ADDRESS_LONG_FILENBR;
        bool is_started = false;
        long filenumber;
        
};

#endif

// TODO: add methods to close / read the current dataFile
// TODO: add methods that make sure opened or closed and keep track of status