/*
  Name: Aditya Harvi
  Student ID: 1532770
  Name: Ralph Milford
  Student ID: 1534131
  CMPUT 275, Winter 2019
  Final Project
*/

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_ILI9341.h>
#include <TouchScreen.h>
#include <Adafruit_GFX.h>
#include "piano.h"


// Defines varaibles requiBLUE for the display/Joystick
#define TFT_DC 9
#define TFT_CS 10
#define SD_CS 6

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

#define DISPLAY_WIDTH  320
#define DISPLAY_HEIGHT 240
#define YEG_SIZE 2048

// thresholds to determine if there was a touch
#define MINP   30

// touch screen pins, obtained from the documentaion
#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM  5  // can be a digital pin
#define XP  4  // can be a digital pin

// Buzzer pins
#define BUZZ 24

#define PreRecordAmount 3

// a multimeter reading says there are 300 ohms of resistance across the plate,
// so initialize with this to get more accurate readings
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// different than SD
Sd2Card card;

// All the global variables/structures.
File Tunes;
GlobalVar Var;
storageManagement currentS;
fileNames Names;
uint8_t * stored = new uint8_t[50];
uint16_t * storedDT = new uint16_t[25];
uint8_t * upload = new uint8_t[1];
uint16_t * uploadDT = new uint16_t[1];

/*
    This is the first function called in Main. Sets initial conditions for
    assignment. Returns nothing, no Inputs.
*/
void setup() {
    init();
    // Begins serial communication
    Serial.begin(9600);
    // Begins tft
    tft.begin();
    // Checks for SD card and prints to serial-mon if there its okay or not.
    Serial.print("Initializing SD card...");
    if (!SD.begin(SD_CS)) {
        Serial.println("failed! Is it inserted properly?");
        while (true) {}
    }
    Serial.println("OK!");
    pinMode(24, OUTPUT);
    // Rotates the screen so its wide
    tft.setRotation(3);
    // Fills everything with black initially
    tft.fillScreen(ILI9341_BLACK);
    // Sets the textsize for the program
    tft.setTextSize(1);
    // Sets wrapping to false
    tft.setTextWrap(false);
    // Sets the text color to white
    tft.setTextColor(ILI9341_WHITE);
    // Allows reading of the SD card
    card.init(SPI_HALF_SPEED, SD_CS);
    tft.fillRect(0, 0, 320, 69, 0xA6A1A1);
    // Draws in the white keys
    tft.drawRect(0, 70, 80, 170, ILI9341_BLUE);
    tft.drawRect(80, 70, 80, 170, ILI9341_BLUE);
    tft.drawRect(160, 70, 80, 170, ILI9341_BLUE);
    tft.drawRect(240, 70, 80, 170, ILI9341_BLUE);
    tft.fillRect(1, 71, 79, 168, ILI9341_WHITE);
    tft.fillRect(81, 71, 79, 168, ILI9341_WHITE);
    tft.fillRect(161, 71, 79, 168, ILI9341_WHITE);
    tft.fillRect(241, 71, 78, 168, ILI9341_WHITE);
    // Draws in the black keys
    tft.drawRect(65, 70, 30, 100, ILI9341_BLUE);
    tft.drawRect(145, 70, 30, 100, ILI9341_BLUE);
    tft.drawRect(225, 70, 30, 100, ILI9341_BLUE);
    tft.fillRect(66, 71, 29, 99, ILI9341_BLACK);
    tft.fillRect(145, 71, 29, 99, ILI9341_BLACK);
    tft.fillRect(225, 71, 29, 99, ILI9341_BLACK);
    // Creats the record/test button
    tft.drawRect(10, 10, 110, 50, 0xBCB6B6);
    tft.setCursor(14, 14);
    tft.print("TEST YOUR ABILITY");
    tft.fillTriangle(55, 50, 55, 30, 75, 40, 0xBCB6B6);
    // Creates the Var.menuToggle button
    tft.drawRect(200, 10, 110, 50, 0xBCB6B6);
    tft.setTextSize(4);
    tft.setCursor(210, 20);
    tft.print("MENU");
    // Creates the play button
    tft.fillTriangle(145, 60, 174, 60, 160, 30, 0xBCB6B6);
    // sets the initial value to 0 for the user score.
    Var.userScore = 0;
}

/*
    This function will increase the size of the dynamic array for
    key inputs by doubling its current size
*/
void reSize() {
    // Doubles the current size variable
    currentS.size = 2*currentS.size;
    // Declares a temporary array of the new size
    uint8_t * temp = new uint8_t[currentS.size];
    // Copies all the data from the old array to the temporary
    // array
    for(uint16_t i = 0; i < currentS.size; i++) {
        temp[i] = stored[i];
    }

    // Creates a pointer to the old allocation of memory
    uint8_t * toDelete = stored;
    // Points the stored variable to the new allocation of memory
    stored = temp;
    // Deletes the old allocation of memory
    delete [] toDelete;
}

/*
    Will add a value to the array for the keys, the values will be
    overwritten until the user wishes to record a tune
*/
void addStored(uint8_t value) {
    // Checks to see if the array size needs to be increased
    if (currentS.size == currentS.index) {
        reSize();
    }

    // Adds the new value at the current index in the array
    stored[currentS.index] = value;
    // If the start variable has been set the index will be
    // incremented
    if (currentS.start) {
        currentS.index++;
    }
}

/*
    Resets both the key input array and the delay array back to
    to their initial sizes
*/
void deleteStored() {
    // Declares new allocation of memory of the original size
    // for the key inputs
    uint8_t * temp = new uint8_t[50];
    // Creates a pointer to the old allocation of memory
    uint8_t * toDelete = stored;
    // Points the key inputs array to the new allocation of memory
    stored = temp;
    // Deletes the old allocation of memory
    delete [] toDelete;

    // Resets the key input array size back to 50
    currentS.size = 50;
    // Resest the key input array index back to 0
    currentS.index = 0;

    // Declares new allocation of memory of the original size
    // for the delays
    uint16_t * temp2 = new uint16_t[25];
    // Creates a pointer to the old allocation of memory
    uint16_t * toDelete2 = storedDT;
    // Points the delay array to the new allocation of memory
    storedDT = temp2;
    // Deletes the old allocation of memory
    delete [] toDelete2;

    // Resets the delay array size back to 25
    currentS.delaySize = 25;
    // Resets the delay array index back to 0
    currentS.delayIndex = 0;
}

/*
    Deletes the requested save and then will re-organize the saves
    to still be numerical order
*/
void overwriteSave(uint8_t selection) {
    // Retrieves the file name
    char* toRemove = Names.getSaveFile(selection, true);
    // Ensures that the file exists before it deletes it
    if (SD.exists(toRemove))
        SD.remove(toRemove);

    // Variable for the following file name
    char* toOverwrite;
    // Will go through each save up to the maximum save file of 5
    for (uint8_t i = (selection + 1); i <= 5; i++) {
        // Get the next save file
        toOverwrite = Names.getSaveFile(i, true);
        // If the next save file does not exist the function will end
        if (!SD.exists(toOverwrite))
            break;
        
	    // Variable for the first line in the file
        String temp = "";
        // Variable for the second line in the file
        String temp1 = "";
    	// Variable for the third line in the file
        String temp2 = "";
	    // Variable for the fourth line in the file
        String temp3 = "";
	    // Variable for reading in a character from a file
        char character;
	    // Opens the next file
        Tunes = SD.open(toOverwrite, FILE_READ);
	    // Reads the first character from the file
        character = Tunes.read();
        // Adds character to the variable for the first line until the
	    // line ends by encountering a new line character
        while (character != '\n') {
            temp += character;
            character = Tunes.read();
        }
	    // Reads the first character for the key array
        character = Tunes.read();
        // Adds character to the variable for the second line until the
	    // line ends by encountering a new line character
        while (character != '\n') {
            temp1 += character;
            character = Tunes.read();
        }
	    // Reads the first character for the size of delay array
        character = Tunes.read();
	    // Adds character to the variable for the third line until the
	    // line ends by encountering a new line character
        while (character != '\n') {
            temp2 += character;
            character = Tunes.read();
        }
	    // Reads the first character for the inputs for the delay array
        character = Tunes.read();
	    // Adds character to the variable for the fourth line unitl the
	    // line ends by encountering a new line character
        while (character != '\n') {
            temp3 += character;
            character = Tunes.read();
        }
	    // Flushes the opened file
        Tunes.flush();
	    // Closes the opened file
        Tunes.close();
	    // Deletes the next file
        SD.remove(toOverwrite);

        // Opens the current file
        Tunes = SD.open(toRemove, FILE_WRITE);
        // Print the first line from the next file
        Tunes.println(temp);
        // Print the second line from the next file
        Tunes.println(temp1);
        // Print the third line from the next file
        Tunes.println(temp2);
        // Print the fourth line from the next file
        Tunes.println(temp3);
        // Flushes the opened file 
        Tunes.flush();
        // Closes the opened file
        Tunes.close();
        // The current file becomes the next file
        toRemove = toOverwrite;
    }
}

/*
    Saves the current key array and delay array to the next
    available file for saving
*/
void saveStored() {
    // If the size of key array does not match the index the
    // array will resized
    if ((currentS.index - 1) != (currentS.size - 1)) {
        // Sets the key array size to its index
        currentS.size = currentS.index;
	    // Creates a new allocation of memory of the correct size
        uint8_t * temp = new uint8_t[currentS.size];

        // Copies the data from the old allocation of memory to the
	    // new allocation of memory
        for(uint16_t i = 0; i < currentS.size; i++) {
            temp[i] = stored[i];
        }

	    // Set a pointer to the old allocation of memory
        uint8_t * toDelete = stored;
	    // Point the stored variable to the new allocation of memory
        stored = temp;
	    // Delete the old allocation of memory
        delete [] toDelete;
    }

    // If the size of delay array does not match the index the
    // array will resized
    if ((currentS.delayIndex - 1) != (currentS.delaySize - 1)) {
        // Sets the delay array size to its index
        currentS.delaySize = currentS.delayIndex;
	    // Creates a new allocation of memory of the correct size
        uint16_t * tempD = new uint16_t[currentS.delaySize];

        // Copies the data from the old allocation of memory to the
	    // new allocation of memory
        for(uint16_t i = 0; i < currentS.delaySize; i++) {
            tempD[i] = storedDT[i];
        }

	    // Set a pointer to the old allocation of memory
        uint16_t * toDeleteD = storedDT;
	    // Point the stored variable to the new allocation of memory
        storedDT = tempD;
	    // Delete the old allocation of memory
        delete [] toDeleteD;
    }

    // Retrieve the name for the next save file
    char* file = Names.nextSaveFile();
    // If the file already exist will delete it
    if (SD.exists(file))
        SD.remove(file);

    // Open the file
    Tunes = SD.open(file, FILE_WRITE);

    // Prints the size of the key array and a new line to the opened file
    Tunes.println(currentS.size);
    // Prints all key array values to the opened file 
    for (uint16_t i = 0; i < currentS.size; i++) {
        Tunes.print(stored[i]);
    }
    // Print a new line character
    Tunes.println();

    // Prints the size of the delay array and a new line to the opened
    // file
    Tunes.println(currentS.delaySize);
    // Prints all delay array values seperated by spaces to the opened
    // file
    for (uint16_t i = 0; i < currentS.delaySize; i++) {
        Tunes.print(storedDT[i]);
        Tunes.print(" ");
    }
    // Prints a new line character
    Tunes.println();

    // Flushes the opened file
    Tunes.flush();
    // Closes the opened file
    Tunes.close();
}

/*
    Reads the current selected song into the upload key array and the 
    upload delay array
*/
void readFile() {
    // Variable for the name of the file
    char* myfile;
    // Retrieves the name of the file to be read by looking at
    // the global variable songSelect
    if (Names.songSelect > 3) {
	    // Will retrieve a user file
        myfile = Names.getSaveFile(Names.songSelect - 3, true);
    } else {
	    // Will retrieve a PreRecorded file
        myfile = Names.getPreFile(Names.songSelect, true);
    }
    // Open the file to be read
    Tunes = SD.open(myfile, FILE_READ);

    // Variable for values from file
    String value;
    // Reads the first character from the file
    char confirm = Tunes.read();
    // Adds the characters from the file to value until a new
    // line is encountered
    while (confirm != '\n') {
        value += confirm;
        confirm = Tunes.read();
    }

    // Sets the upload key array size to the value just read from the file
    currentS.uploadSize = value.toInt();
    // Allocate memory to the new size
    uint8_t * temp = new uint8_t[value.toInt()];
    // Create a pointer to the old allocation
    uint8_t * toDelete = upload;
    // Point upload to the new allocation    
    upload = temp;
    // Delete old allocation
    delete [] toDelete;

    // Read the first key value from the file
    confirm = Tunes.read();
    // Create a counter to be an index
    uint16_t counter = 0;
    // Will loop until the line being read is done
    while (confirm != '\n') {
	    // Resets value
        value = "";
	    // Add read character to value
        value += confirm;
	    // Add value to the upload key array at the current counter
        upload[counter] = value.toInt();
	    // Increment counter
        counter++;
	    // Read in the next character
        confirm = Tunes.read();
    }

    // Resets value
    value = "";
    // Read in the first digit of the upload delay array
    confirm = Tunes.read();
    // Adds characters to value until the line ends
    while (confirm != '\n') {
        value += confirm;
        confirm = Tunes.read();
    }

    // Sets the size of upload delay to value just read from the file
    currentS.uploadDTSize = value.toInt();
    // Create new allocation of memory
    uint16_t * tempD = new uint16_t[value.toInt()];
    // Create a pointer to the old allocation
    uint16_t * toDeleteD = uploadDT;
    // Pointer the upload delay array to the new allocation
    uploadDT = tempD;
    // Delete the old allocation
    delete [] toDeleteD;

    // Reset value
    value = "";
    // Reset the counter
    counter = 0;
    // Read in the first digit for the first upload delay value
    confirm = Tunes.read();
    // Will loop as long numbers are still being read
    while (confirm != '\n' && confirm != ' ') {
	    // Obtain a whole number by looping until a space is read
        while (confirm != ' ') {
            value += confirm;
            confirm = Tunes.read();
        }
	    // Add the read value to the upload delay array
        uploadDT[counter] = value.toInt();
	    // Increment counter
        counter++;
	    // Reset value
        value = "";
	    // Read in the next character
        confirm = Tunes.read();
        // Breaks from the loop if the counter reaches the size
	    // of the upload delay array
        if (counter == currentS.uploadDTSize)
            break;
    }

    // Flushes the opened file
    Tunes.flush();
    // Closes the opened file
    Tunes.close();
}

/*
    Will create a PreRecorded song depending on the selection
    and save it to the provided file name
*/
void createPreRecorded(char* file, uint8_t selection) {
    // Will delete any pre-existing instances of the PreRecorded
    // tunes in order to insure that they are correct
    if (SD.exists(file))
        SD.remove(file);

    // Open the given file
    Tunes = SD.open(file, FILE_WRITE);

    // Records the data for the first PreRecorded song, Marry Had a Little Lamb
    // followed by the second PreRecorded song, Hot Cross Buns.  Finished
    // with the last PreRecording song, Star Wars
    if (selection == 1) {
        Tunes.println("53");
        Tunes.println("03020102030303020202030404030201020303030302020302010");
        Tunes.println("27");
        Tunes.println("50 300 300 300 300 300 300 500 300 300 500 300 300 500 300 300 300 300 300 300 300 300 300 300 300 300 300 ");
    } else if (selection == 2) {
        Tunes.println("31");
        Tunes.println("0302010302010101010202020302010");
        Tunes.println("16");
        Tunes.println("50 300 300 500 300 300 500 300 300 500 300 300 500 300 300 300 ");
    } else if (selection == 3) {
        Tunes.println("37");
        Tunes.println("0202020106050106050404040706050106050");
        Tunes.println("19");
        Tunes.println("50 350 350 350 325 250 375 325 250 550 350 350 350 350 250 375 350 250 300 ");
    }

    // Flushes the opened file
    Tunes.flush();
    // Closes the opened file
    Tunes.close();
}

/*
    Will create the PreRecored file if they don't already exists
*/
void checkFiles() {
    // Will loop for the amount of the PreRecorded files
    for (uint8_t i = 1; i <= PreRecordAmount; i++) {
	    // Obtain the name of the next PreRecord file
        char* file = Names.getPreFile(i, true);
        // As long as the name is not N/A and as long as it does not already exists the song will be
        // created
        if (file != "N/A")
            createPreRecorded(file, i);
    }
}

/*
    The following function group will 'display' the key that is being pressed.
    The key will be highlighted green temporarly.
    Then the tuen is played
    Then the key is returned to its original colour
*/
void showKey1() {
    tft.fillRect(1, 71, 64, 168, ILI9341_GREEN);
    tft.fillRect(63, 171, 17, 68, ILI9341_GREEN);
    tone(BUZZ, 350, 200);
    tft.fillRect(1, 71, 64, 168, ILI9341_WHITE);
    tft.fillRect(63, 171, 17, 68, ILI9341_WHITE);
}
void showKey2() {
    tft.fillRect(96, 71, 49, 168, ILI9341_GREEN);
    tft.fillRect(81, 171, 17, 68, ILI9341_GREEN);
    tft.fillRect(138, 171, 22, 68, ILI9341_GREEN);
    tone(BUZZ, 400, 200);
    tft.fillRect(96, 71, 49, 168, ILI9341_WHITE);
    tft.fillRect(81, 171, 17, 68, ILI9341_WHITE);
    tft.fillRect(138, 171, 22, 68, ILI9341_WHITE);
}
void showKey3() {
    tft.fillRect(175, 71, 50, 168, ILI9341_GREEN);
    tft.fillRect(161, 171, 17, 68, ILI9341_GREEN);
    tft.fillRect(226, 171, 13, 68, ILI9341_GREEN);
    tone(BUZZ, 440, 200);
    tft.fillRect(175, 71, 50, 168, ILI9341_WHITE);
    tft.fillRect(161, 171, 17, 68, ILI9341_WHITE);
    tft.fillRect(226, 171, 13, 68, ILI9341_WHITE);
}
void showKey4() {
    tft.fillRect(255, 71, 65, 168, ILI9341_GREEN);
    tft.fillRect(242, 171, 13, 68, ILI9341_GREEN);
    tone(BUZZ, 500, 200);
    tft.fillRect(255, 71, 65, 168, ILI9341_WHITE);
    tft.fillRect(242, 171, 13, 68, ILI9341_WHITE);
}
void showbKey1() {
    tft.fillRect(66, 71, 28, 98, ILI9341_GREEN);
    tone(BUZZ, 375, 200);
    tft.fillRect(66, 71, 28, 98, ILI9341_BLACK);
}
void showbKey2() {
    tft.fillRect(146, 71, 28, 98, ILI9341_GREEN);
    tone(BUZZ, 420, 200);
    tft.fillRect(146,71, 28, 98, ILI9341_BLACK);
}
void showbKey3() {
    tft.fillRect(226, 71, 28, 98, ILI9341_GREEN);
    tone(BUZZ, 470, 200);
    tft.fillRect(226, 71, 28, 98, ILI9341_BLACK);
}

/*
    Will play the upload song
*/
void playUpload() {
    // Counter for the upload delay array
    uint16_t counter = 0;
    // Loops for upload key array
    for (uint16_t i = 0; i < currentS.uploadSize; i++) {
        // Will play the appropriate key for the current value,
        // if it is 0, then the current delay will run
        if (upload[i] == 1) {
            showKey1();
        } else if (upload[i] == 2) {
            showKey2();
        } else if (upload[i] == 3) {
            showKey3();
        } else if (upload[i] == 4) {
            showKey4();
        } else if (upload[i] == 5) {
            showbKey1();
        } else if (upload[i] == 6) {
            showbKey2();
        } else if (upload[i] == 7) {
            showbKey3();
        } else if (upload[i] == 0) {
            delay(uploadDT[counter]);
            counter++;
        }
    }
}

/*
    This will compare what the user has entered vs what was selected as a song.
*/
void compareTunes() { 
    Var.userScore = 0;
    uint16_t max;
    int keys = 0, delays = 0;
    // Determines the maximum size of the for loop based on the size of the
    // user entered tune and the uploaded tune.
    max = (currentS.uploadSize > currentS.index)? currentS.index : currentS.uploadSize;
    // Determines if the userScore matches with what was uploaded
    for (uint16_t i = 0; i < max; i++) {
        if (stored[i] == upload[i] && upload[i] != 0) {
            keys++;
            Var.userScore++;
        }
    }
    // Determines the size for the delays
    max = (currentS.uploadDTSize > currentS.delayIndex)? currentS.delayIndex : currentS.uploadDTSize;
    // Increments userScore if the delay between button presses was within 200
    // of the uploaded delay.
    for (uint16_t i = 0; i < max; i++) {
        if (storedDT[i] < (uploadDT[i] + 200) && storedDT[i] > (uploadDT[i] - 200)) {
            delays++;
            Var.userScore++;
        }
    }
    // These are checks if the user pressed the correct keys, didnt press anything
    // or pressed too many keys- respectively.
    if (keys == 0 || delays == 0)
        Var.userScore = -1;
    if (currentS.index == 1)
        Var.userScore = 0;
    // Decrements the userScore by the number of extra keys they pressed.
    if (currentS.index > currentS.uploadSize)
        Var.userScore -= currentS.index - currentS.uploadSize;
}

/*
    This is simply a header. The function is down below.
*/
void menu();

/*
    Draws in the paino and the buttons.
*/
void piano() {
    Var.userScore = 0;
    tft.fillScreen(ILI9341_BLACK);
    // Sets the textsize for the program
    tft.setTextSize(1);
    // Sets wrapping to false
    tft.setTextWrap(false);
    // Sets the text color to white
    tft.setTextColor(ILI9341_WHITE);
    // Allows reading of the SD card
    card.init(SPI_HALF_SPEED, SD_CS);
    tft.fillRect(0, 0, 320, 69, 0xA6A1A1);
    // Draws in the white keys
    tft.drawRect(0, 70, 80, 170, ILI9341_BLUE);
    tft.drawRect(80, 70, 80, 170, ILI9341_BLUE);
    tft.drawRect(160, 70, 80, 170, ILI9341_BLUE);
    tft.drawRect(240, 70, 80, 170, ILI9341_BLUE);
    tft.fillRect(1, 71, 79, 168, ILI9341_WHITE);
    tft.fillRect(81, 71, 79, 168, ILI9341_WHITE);
    tft.fillRect(161, 71, 79, 168, ILI9341_WHITE);
    tft.fillRect(241, 71, 78, 168, ILI9341_WHITE);
    // Draws in the black keys
    tft.drawRect(65, 70, 30, 100, ILI9341_BLUE);
    tft.drawRect(145, 70, 30, 100, ILI9341_BLUE);
    tft.drawRect(225, 70, 30, 100, ILI9341_BLUE);
    tft.fillRect(66, 71, 29, 99, ILI9341_BLACK);
    tft.fillRect(145, 71, 29, 99, ILI9341_BLACK);
    tft.fillRect(225, 71, 29, 99, ILI9341_BLACK);
    // Depending on which button was pressed, the main piano screen must be
    // changed.
    if (!Var.saving) {
        // Creats the record/test button
        tft.drawRect(10, 10, 110, 50, 0xBCB6B6);
        tft.setCursor(14, 14);
        tft.print("TEST YOUR ABILITY");
        tft.fillTriangle(55, 50, 55, 30, 75, 40, 0xBCB6B6);
        // Creates the Var.menuToggle button
        tft.drawRect(200, 10, 110, 50, 0xBCB6B6);
        tft.setTextSize(4);
        tft.setCursor(210, 20);
        tft.print("MENU");
        // Creates the play button
        tft.fillTriangle(145, 60, 174, 60, 160, 30, 0xBCB6B6);
    }
    else {
        // Creats the record/test button
        tft.drawRect(10, 10, 110, 50, 0xBCB6B6);
        tft.setCursor(19, 14);
        tft.print("SAVE YOUR TUNE");
        tft.fillTriangle(55, 50, 55, 30, 75, 40, 0xBCB6B6);
        // Creates the Var.menuToggle button
        tft.drawRect(200, 10, 110, 50, 0xBCB6B6);
        tft.setTextSize(4);
        tft.setCursor(210, 20);
        tft.print("MENU");
    }
}

/*
    The following function group will play a sound when called. This is the
    piano. They take in no inputs and return nothing. 
*/
void key1() {
    addStored(1);
    tone(BUZZ, 350, 200);
}
void key2() {
    addStored(2);
    tone(BUZZ, 400, 200);
}
void key3() {
    addStored(3);
    tone(BUZZ, 440, 200);
}
void key4() {
    addStored(4);
    tone(BUZZ, 500, 200);
}
void bkey1() {
    addStored(5);
    tone(BUZZ, 375, 200);
}
void bkey2() {
    addStored(6);
    tone(BUZZ, 420, 200);
}
void bkey3() {
    addStored(7);
    tone(BUZZ, 570, 200);
}

/*
    This will display a black screen asking the user to confirm if they want to
    delete their tune.
*/
void deleteTune(uint8_t selected) {
    tft.fillRect(30, 30, 260, 180, ILI9341_BLACK);
    tft.drawRect(30, 30, 260, 180, ILI9341_GREEN);
    tft.setTextSize(2);
    tft.setCursor(85, 40);
    tft.print("YOU DELETING?");
    tft.setTextSize(1);
    tft.setCursor(50, 100);
    tft.print("Pressing 'Yes' will delete your tune!");
    tft.setTextSize(3);
    tft.setCursor(45, 180);
    tft.print("YES");
    tft.setCursor(240, 180);
    tft.print("NO");
    // This is a loop which will determine whether or not the user wants to
    // delete the tune.
    while (true) {
        TSPoint touch = ts.getPoint();
        int x = touch.x, y = touch.y, z = touch.z;
        // This is if they they clicked yes
        if (z >= MINP && x <= 760 && x >= 710 && y >= 730 && y <= 785) {
            overwriteSave(selected);
            menu();
            break;
        }
        // This is if they clicked no.
        else if (z >= MINP && x >= 710 && x <= 760 && y >= 199 && y <= 295) {
            menu();
            break;
        }
    }
}

/*
    This is the menu screen function. It will switch between the paino screen
    and back to the menu.
*/
void menu() {
    // Fills the screen with the red colour and draws the box for the user to
    // return to the main screen
    tft.fillScreen(0xA6A1A1);
    tft.drawRect(200, 10, 110, 50, 0xBCB6B6);
    tft.setTextSize(4);
    tft.setCursor(210, 20);
    tft.print("PLAY");
    // This will draw the "make your own" button
    tft.drawRect(50, 100, 210, 30, 0xBCB6B6);
    tft.setTextSize(2);
    tft.setCursor(70, 107);
    tft.print("Make your Own!");
    // This will draw the selected song name. So it will save the name which
    // was previously selected and redraw it. So if they selected HotCrossBuns
    // and click to the home screen and return to the manu, then it will still
    // display HotCrossBuns.
    tft.drawRect(35, 150, 240, 30, 0xBCB6B6);
    tft.setTextSize(2);
    tft.setCursor(45, 157);
    char* name;
    if (Names.songSelect > 3) {
        name = Names.getSaveFile(Names.songSelect - 3, false);
    } else {
        name = Names.getPreFile(Names.songSelect, false);
    }
    tft.print(name);
    tft.fillTriangle(5, 165, 30, 150, 30, 180, 0xBCB6B6);
    tft.fillTriangle(305, 165, 280, 150, 280, 180, 0xBCB6B6);
    // This will draw the delete button.
    tft.drawRect(100, 200, 100, 30, 0xBCB6B6);
    tft.setTextSize(2);
    tft.setCursor(115, 207);
    tft.print("Delete");
}

/*
    The following function group will give a letter grade for when the user
    presses the "test your ability" button
    Pauses for 3 seconds and then returns to the piano screen.
*/
void scoreA() {
    tft.print("A");
    delay(3000);
    piano();
}
void scoreB() {
    tft.print("B");
    delay(3000);
    piano();
}
void scoreC() {
    tft.print("C");
    delay(3000);
    piano();
}
void scoreD() {
    tft.print("D");
    delay(3000);
    piano();
}

/*
    This will change the play to a pause butotn (and back) when called.
    No parameters or outputs.
*/
void testButton() {
    if (!Var.saving) {
        // If it is displaying play, then we will display pause.
        if (!Var.pauseButton) {
            tft.fillTriangle(55, 50, 55, 30, 75, 40, 0xA6A1A1);
            tft.fillRect(55, 30, 5, 25, 0xBCB6B6);
            tft.fillRect(65, 30, 5, 25,0xBCB6B6);
            Var.lockFlag = true;
            Var.pauseButton = true;
            currentS.start = true;
        }
        // This will switch the button from pause to play.
        else {
            Var.beginFlag = false;
            tft.fillRect(55, 30, 5, 25, 0xA6A1A1);
            tft.fillRect(65, 30, 5, 25,0xA6A1A1);
            tft.fillTriangle(55, 50, 55, 30, 75, 40, 0xBCB6B6);
            Var.pauseButton = false;
            Var.lockFlag = false;
            currentS.start = false;
            // Displays the score screen
            tft.fillRect(30, 30, 260, 180, ILI9341_BLACK);
            tft.drawRect(30, 30, 260, 180, ILI9341_GREEN);
            tft.setTextSize(2);
            tft.setTextColor(ILI9341_GREEN);
            tft.setCursor(40, 40);
            tft.print("You Recieved...");
            compareTunes();
            // If the user does not try, they get a D-score
            Var.computerScore = (currentS.uploadSize/2)-1 + currentS.uploadDTSize;
            int user = Var.userScore, comp = Var.computerScore;
            // if the user does not try, they will recieve a D-grade
            // and a messages telling them no input was entered.
            if (user == 0) {
                tft.setTextSize(2);
                tft.setCursor(40, 80);
                tft.print("You didn't even try!");
                tft.setTextSize(10);
                tft.setCursor(135, 120);
                scoreD();
            }
            else if (user == -1) {
                // Prints the text to fit within the black box.
                tft.setTextSize(2);
                tft.setCursor(40, 80);
                tft.print("At least you tried?");
                tft.setTextSize(10);
                tft.setCursor(135, 120);
                scoreD();
            }
            // If the score is 90% corrent then they get an A
            else if (user >= comp - (comp * .1)) {
                // if the user gets a perfect score, then they get a custom
                // message.
                if (user != comp) {
                    tft.setTextSize(2);
                    tft.setCursor(40, 80);
                    tft.print("You were within 10%!");
                }
                else {
                    // prints the text to fit within the black box
                    tft.setTextSize(2);
                    tft.setCursor(37, 80);
                    tft.print("WOW! You Nailed it!");
                }
                tft.setTextSize(10);
                tft.setCursor(135, 120);
                // calls the scoreA function
                scoreA();
            }
            // If the score is within 70% then they get a B
            else if (user >= comp - (comp * .3)) {
                tft.setTextSize(2);
                tft.setCursor(40, 80);
                tft.print("You were within 30%!");
                tft.setTextSize(10);
                tft.setCursor(135, 120);
                scoreB();
            }
            // If the score is within 50% then they get a C
            else if (user >= comp - (comp * .5)) {
                tft.setTextSize(2);
                tft.setCursor(40, 80);
                tft.print("You were within 50%!");
                tft.setTextSize(10);
                tft.setCursor(135, 120);
                scoreC();
            }
            // If the score is further than anything mentioned above then they
            // get a D-score.
            else {
                // Prints the text to fit within the black box.
                tft.setTextSize(2);
                tft.setCursor(40, 80);
                tft.print("At least you tried?");
                tft.setTextSize(10);
                tft.setCursor(135, 120);
                scoreD();
            }
            deleteStored();
            // Resets the user score.
            Var.userScore = 0;
        }
    }
    else {
        // If the Var.pauseButton is currently displaying the play button, then
        // we switch to the pausebutton.
        if (!Var.pauseButton) {
            tft.fillTriangle(55, 50, 55, 30, 75, 40, 0xA6A1A1);
            tft.fillRect(55, 30, 5, 25, 0xBCB6B6);
            tft.fillRect(65, 30, 5, 25,0xBCB6B6);
            Var.lockFlag = true;
            Var.pauseButton = true;
            currentS.start = true;
        }
        // If the Var.pauseButton is not currently displaying play, then display it.
        else {
            // Erases the white rectangles
            tft.fillRect(55, 30, 5, 25, 0xA6A1A1);
            tft.fillRect(65, 30, 5, 25,0xA6A1A1);
            // Draws the 'play' button
            tft.fillTriangle(55, 50, 55, 30, 75, 40, 0xBCB6B6);
            Var.pauseButton = false;
            Var.lockFlag = false;
            // Draws a black box so the user can see if they want to save their
            // work or not.
            tft.fillRect(30, 30, 260, 180, ILI9341_BLACK);
            tft.drawRect(30, 30, 260, 180, ILI9341_GREEN);
            tft.setTextSize(2);
            tft.setCursor(40, 40);
            tft.print("DO YOU WANT TO SAVE?");
            tft.setTextSize(1);
            tft.setCursor(50, 100);
            tft.print("Pressing 'NO' will delete your tune!");
            tft.setCursor(50, 110);
            tft.print("If you have 5 saves, it will overwrite");
            tft.setCursor(50, 120);
            tft.print("the last save!");
            tft.setTextSize(3);
            tft.setCursor(45, 180);
            tft.print("YES");
            tft.setCursor(240, 180);
            tft.print("NO");
            currentS.start = false;
            // Determines if the user pressed the save button or not.
            while (true) {
                TSPoint touch = ts.getPoint();
                int x = touch.x, y = touch.y, z = touch.z;
                if (z >= MINP && x <= 760 && x >= 710 && y >= 730 && y <= 785) {
                    // YES Button: RALPH WE SAVE THE THING HERE
                    saveStored();
                    deleteStored();
                    piano();
                    break;
                }
                else if (z >= MINP && x >= 710 && x <= 760 && y >= 199 && y <= 295) {
                    // NO BUtton is pressed: NOW RALPH DELETE THE THING HERE
                    deleteStored();
                    piano();
                    break;
                }
            }
        }
    }
}

/*
    Function determines which piano key was pressed.
    Depending on the key pressed.
*/
void pressKey() {
    // Gets the position of where pressure is applied to
    TSPoint touch = ts.getPoint();
    int x = touch.x, y = touch.y;
    // Based off the location of where pressure is applied, a certain button
    // will be pressed. It also determines the time taken between pushes.
    if (y >= 760 && y <= 900 && x >= 375) {
        key1();
    }
    // All following "else if" follow what was mentioned above.
    else if (y <= 900 && y >= 720 && x >= 675) {
        key1();
    }
    else if (y <= 720 && y >= 510 && x >= 675) {
        key2();
    }
    else if (y <= 665 && y >= 540 && x >= 375) {
        key2();
    }
    else if (y <= 490 && y >= 295 && x >= 675) {
        key3();
    }
    else if (y <= 456 && y >= 330 && x >= 375) {
        key3();
    }
    else if (y <= 295 && y >= 95 && x >= 675) {
        key4();
    }
    else if (y <= 240 && y >= 95 && x >= 375) {
        key4();
    }
    else if (x >= 375 && x <= 675) {
        if (y <= 740 && y >= 680) {
            bkey1();
        }
        else if (y <= 530 && y >= 460) {
            bkey2();
        }
        else if (y <= 320 && y >= 251) {
            bkey3();
        }
    }
}

/*
    Determines which button the user has pressed.
*/
void pressButton() {
    // Gets the position of where pressure is applied to
    TSPoint touch = ts.getPoint();
    int x = touch.x, y = touch.y;
    // Checks to make sure that the user did not press the "test your aiblity"
    // button before the user presses the menu button.
    if (y <= 390 && y >= 110 && !Var.lockFlag && !Var.playFlag) {
        // Triggers the menu function to change the screen.
        if (!Var.menuToggle) {
            menu();
            Var.menuToggle = true;
            // This will determine where the user has pushed.
            while (true) {
                TSPoint touch = ts.getPoint();
                int x = touch.x, y = touch.y, z = touch.z;
                if (z >= MINP && y <= 390 && y >= 110 && x >= 178 && x <= 330) {
                    Var.menuToggle = false;
                    Var.saving = false;
                    readFile();
                    piano();
                    break;
                }
                // Changes the menu if the player wants to make their own tune.
                else if (z >= MINP && x >= 450 && x <= 550 && y >= 248 && y <= 795) {
                    Var.menuToggle = false;
                    Var.saving = true;
                    piano();
                    break;
                }
                // Moves the selected song left
                else if (z >= MINP && x >= 600 && x <= 710 && y >= 830 && y <= 903 && Names.songSelect > 1) {
                    char* name;
                    Names.songSelect--;
                    if (Names.songSelect > 3) {
                        name = Names.getSaveFile(Names.songSelect - 3, false);
                    } else {
                        name = Names.getPreFile(Names.songSelect, false);
                    }
                    tft.fillRect(36, 151, 238, 28, 0xA6A1A1);
                    tft.setTextSize(2);
                    tft.setCursor(45, 157);
                    tft.print(name);
                    delay(500);

                }
                // Moves the selected song right
                else if (z >= MINP && x >= 600 && x <= 710 && y >= 115 && y <= 190 && Names.songSelect < Names.songMax()) {
                    char* name;
                    Names.songSelect++;
                    if (Names.songSelect > 3) {
                        name = Names.getSaveFile(Names.songSelect - 3, false);
                    } else {
                        name = Names.getPreFile(Names.songSelect, false);
                    }
                    tft.setTextSize(2);
                    tft.setCursor(45, 157);
                    tft.fillRect(36, 151, 238, 28, 0xA6A1A1);
                    tft.print(name);
                    delay(500);
                }
                // The deleted button was pressed
                else if (z >= MINP && x >= 775 && x <= 865 && y >= 395 && y <= 642 && Names.songSelect > 3) {
                    deleteTune(Names.songSelect - 3);
                }
            }
        }
    }
    // trigger for the test your ability button.
    else if (y >= 550 && y <= 880 && !Var.menuToggle && !Var.playFlag) {
        testButton();
    }
    // test for the play button in the middle.
    else if (x >= 219 && y >= 462 && y <= 532 && !Var.menuToggle && !Var.lockFlag && !Var.playFlag && !Var.saving) {
        Var.playFlag = true;
        playUpload();
        Var.playFlag = false;
    }
}

/*
    This is the main function. Returns 0. Calls all other functions.
    No inputs required.
*/
int main() {
    // Calls the setup function.
    setup();
    checkFiles();
    // Flags are needed to make sure the button presses are not spammed.
    bool oldPress = false, newPress = false;
    long long time1, time2;
    // Forever loop allowing the program to run forever.
    while (true) {
        // Gets the position of where pressure is applied to
        TSPoint touch = ts.getPoint();
        int z = touch.z, x = touch.x;
        newPress = (z >= MINP)? true : false;
        if (newPress == true && oldPress == false) {
            // This will trigger if the user has pressed the "test your ability"
            // button on the main screen.
            if (currentS.start) {
                // This will add elements to the dynamic array and if needed,
                // it will double the size of the array.
                currentS.pause = !currentS.pause;
                if (currentS.delayIndex == currentS.delaySize) {
                    currentS.delaySize = 2*currentS.delaySize;
                    uint16_t * temp = new uint16_t[currentS.delaySize];
                    // this copies things from the temp array into the new
                    // stored array.
                    for(uint16_t i = 0; i < currentS.delaySize; i++) {
                        temp[i] = storedDT[i];
                    }
                    uint16_t * toDelete = storedDT;
                    storedDT = temp;
                    // delets the temporary array.
                    delete [] toDelete;
                }
                // This sets delay to be 60 seconds
                if (currentS.maxDelay) {
                    storedDT[currentS.delayIndex] = 60000;
                } else {
                    // determines the time delay between presses
                    storedDT[currentS.delayIndex] = millis() - time1;
                }
                currentS.maxDelay = false;
                currentS.delayIndex++;
            }
            // if the press was within the x-range then the keyboard was pressed
            if (x >= 375 && !Var.menuToggle) {
                pressKey();
            }
            // If the press was below a certain range then the buttons were
            // pressed- not the keyboard.
            else if (x <= 300) {
                pressButton();
            }
        } else {
            if (!currentS.pause && currentS.start) {
                // Adds a 0- which is a delay into the array
                currentS.pause = true;
                addStored(0);
                delay(50);
                time1 = millis();
                time2 = millis();
            } else {
                time2 = millis();
                if ((time2 - time1) > 60000) {
                    currentS.maxDelay = true;
                }
            }
        }
        oldPress = newPress;
        delay(75);
    }

    // Ends serial communication.
    Serial.end();
    return 0;
}
