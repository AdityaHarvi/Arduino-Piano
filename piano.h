#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_ILI9341.h>
#include <TouchScreen.h>
#include <Adafruit_GFX.h>

/*
    Class to deal with file names
*/
class fileNames {
    public:
	    // Variable to determine which song the user is current on
        uint8_t songSelect = 1;

        /*
            Determines how many songs have been saved
        */
        uint8_t songMax() {
            // Will return the appropriate amount of songs depending
            // on which files exist on th SD card
            if (SD.exists(user5))
                return 8;
            if (SD.exists(user4))
                return 7;
            if (SD.exists(user3))
                return 6;
            if (SD.exists(user2))
                return 5;
            if (SD.exists(user1))
                return 4;
            return 3;
        }

        /*
            Will return the name of the file to be used for saving
            depending on which files already exists
        */
        char* nextSaveFile() {
            // Will return the next save file in numerical order,
            // depending on which files current exist
            if (!SD.exists(user1))
                return user1;
            if (!SD.exists(user2))
                return user2;
            if (!SD.exists(user3))
                return user3;
            if (!SD.exists(user4))
                return user4;
            return user5;
        }

        /*
            Will return the either the display name or file name
            of a selected song for the user files
        */
        char* getSaveFile(uint8_t selection, bool file) {
            // Returns either the file name or the song name corresponding
            // to the value of selection and as long as the requested file
            // currently exists
            if (selection == 1 && SD.exists(user1))
                return (file)? user1 : userName1;
            if (selection == 2 && SD.exists(user2))
                return (file)? user2 : userName2;
            if (selection == 3 && SD.exists(user3))
                return (file)? user3 : userName3;
            if (selection == 4 && SD.exists(user4))
                return (file)? user4 : userName4;
            if (selection == 5 && SD.exists(user5))
                return (file)? user5 : userName5;
            return "N/A";
        }

        /*
            Will return the either the display name or file name
            of a selected song for the PreRecorded songs
        */
        char* getPreFile(uint8_t selection, bool file) {
            // Will return either the file name or song name for a PreRecorded
            // song corresponding to the value of selection
            if (selection == 1)
                return (file)? preRecord1 : preRecordName1;
            if (selection == 2)
                return (file)? preRecord2 : preRecordName2;
            if (selection == 3)
                return (file)? preRecord3 : preRecordName3;
            return "N/A";
        }

    private:
        /*
            Names of the user files that the arduino will display
        */
        char* userName1 = "Custom 1";
        char* userName2 = "Custom 2";
        char* userName3 = "Custom 3";
        char* userName4 = "Custom 4";
        char* userName5 = "Custom 5";

        /*
            Names of the PreRecorded files that the arduino will
            display
        */
        char* preRecordName1 = "MarryHadALittleLamb";
        char* preRecordName2 = "HotCrossBuns";
        char* preRecordName3 = "StarWars";

        /*
            File names for the user tunes
        */
        char* user1 = "Cus1.txt";
        char* user2 = "Cus2.txt";
        char* user3 = "Cus3.txt";
        char* user4 = "Cus4.txt";
        char* user5 = "Cus5.txt";

        /*
            File names for the PreRecorded tunes
        */
        char* preRecord1 = "Pre1.txt";
        char* preRecord2 = "Pre2.txt";
        char* preRecord3 = "Pre3.txt";
};

/*
    Structure to deal with storage
*/
struct storageManagement {
    // The current size of the key array
    uint16_t size = 50;
    // The current index of the key array
    uint16_t index = 0;
    // Will be true if no key value are being pressed
    bool pause = false;
    // Determines when a pause should be stopped
    bool maxDelay = false;
    // Will be true if the user wishes to start
    // recording a tune
    bool start = false;

    // The size of the delay array
    uint16_t delaySize = 25;
    // The index of the delay array
    uint16_t delayIndex = 0;

    // The size of the upload key array
    uint16_t uploadSize;
    // The size of the upload delay array
    uint16_t uploadDTSize;
};

/*
    Global Variables which are used throughout the final.cpp file.
*/
struct GlobalVar {
    bool menuToggle = false;
    bool pauseButton = false;
    bool lockFlag = false;
    bool playFlag = false;
    bool saving = false;
    bool beginFlag = false;
    int computerScore, userScore;
};

