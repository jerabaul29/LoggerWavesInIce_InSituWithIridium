#include <SPI.h>
#include <SD.h>
#include "EEPROM_interaction.cpp"

#ifndef SD_MANAGER
#define SD_MANAGER

class SDManager{
    public:
        SDManager(void);

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
        int nbr_of_numbers_file_name = 5;
        long address_number_file = 1;
};

#endif
