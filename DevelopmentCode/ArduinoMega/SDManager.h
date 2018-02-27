#include <SPI.h>
#include <SD.h>
#include "EEPROM_interaction.cpp"
#include "parameters.h"

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
         void post_on_SD_card(char [] array_to_post, int end_position);

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
        char current_file_name[] = "F00000";
        const int nbr_of_numbers_file_name = 5;
        const long address_number_file = 1;
        bool is_started = false;
};

#endif
