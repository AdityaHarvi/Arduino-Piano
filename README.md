# Arduino Piano
A custom-made piano capable of saving songs and allowing players to compare their skills to that song.
So if you were to create a song and save it to the program, you can then give the piano to a friend and they can try to match your song as best they can.
The program will then return a letter grade for how well they did.
  
![](https://github.com/AdityaHarvi/Arduino-Piano/blob/master/images/piano.png)
## Setup & Pre-req's
#### Accessories:
-   Arduino Mega Board (AMG)
-   Breadboard
-   Display
#### Wiring Instructions:
Arduino must be wired to display exactly as listed.
<pre>
Board Pin <---> Arduino Pin
GND             GND
Vin             5V
3Vo             NOT CONNECTED
CLK             52
MISO            50
MOSI            51
CS              10
D/C             9
RST             NOT CONNECTED
Lite            NOT CONNECTED
Y+              A2 (analog pin)
X+              4  (digital pin)
Y-              5  (digital pin)
X-              A3 (analog pin)
IM0 - IM3       NOT CONNECTED (they expect a 3.3v signal, DON'T CONNECT!)
CCS             6
CD              NOT CONNECTED


BUZZER WIRING
Arduino pin 24
</pre>
**It's safe to disregard the joystick from the following image:**
![](https://github.com/AdityaHarvi/Arduino-Piano/blob/master/images/wiring.png)
![](https://github.com/AdityaHarvi/Arduino-Piano/blob/master/images/wiring_close_up.png)
## Running Instructions

In order to run the program, make sure everything is wired correctly.\
Then enter into the directory containing all the files listed above.\
Once in the file directory and your Arduino is connected to the computer:\
Enter the following into the terminal:

`make && make upload`

#### What it does:
Now that the program is uploaded, the user will be presented with the main screen.
The main screen contains the piano on the bottom half and 3 keys on the top half.
-   Piano keys each play a different tune
-   The 3 buttons on the top are "Test your Ability", the single triangle, and
    the "Menu" button.
    -   The "Menu" button will take the user to the menu screen. The Menu screen
        contains 3 buttons: "Create your own", song selection, and the "delete" button.

        -   Song selection:
            -   Use the left and right buttons to cycle through the available songs

        -   Deleting:
            -   User cannot delete any of the 3 pre-recorded songs
            -   When deleting any of the Custom 1-5 files, simply select it, using
                the arrows from song selection and click the delete button
            -   When clicked on a valid file, a menu will appear asking for
                confirmation if the user wants to delete the selected custom file.
                -   If No is clicked, then the user returns to the menu.
                -   If Yes is clicked, the selected custom file will be deleted.
                -   If for example there are 5 custom files created and the user
                    deletes the custom 1 file- custom 1 will be deleted and all
                    other files will be shifted down in order to keep order.
                    So custom 2 will now become custom 1, custom 3 will be custom 2,
                    etc.

        -   Create Your Own:
            -   User will be taken to the song selection menu.
            -   The piano is seen on the bottom half
            -   A button on the top left will allow you to save your tune.
            To Utilize the button:
            -   Click on the button to begin the saving process.
            -   Play the piano- any key you press will be recorded.
            -   When finished, click on the button again to stop recording.
            -   A menu will pop up asking for confirmation if you want to save
                the tune you just played.
                -   Yes:
                    The tune will be saved into the next available Custom file.
                    If all custom files are already full- then the last customer file
                    will be overwritten.
                -   No:
                    The tune that was created will be deleted.
            -   User will now return to the create your own screen when either the
                Yes/No button is pressed.

    -   Single Triangle (in the middle of the screen):
        -   This button will show the user how to play the music by highlighting
            the keys which are to be pressed in green as well as play the song
            with the proper delays between the buttons.
        -   To utilize this feature, you first select a song- how to do this is mentioned above.
        -   Once a song is selected and you have returned to the home screen, just
            press the middle triangle button.
        -   The song will be played.

    -   The "Test your ability" button:
        -   allows the user to compare their score with a selected song. 
        -   To utilize this feature, select a song of your choice (how to do this
            is mentioned above).
        -   Return to the main screen (the one displaying the piano and the 3 buttons on the top)
        -   Press the "Test your ability" button so it displays a pause button.
        -   Now try your best to replicate the music that was selected.
        -   When finished, press the "Test your ability" button again to stop it.
        -   It will give you a letter score based on your ability to match the
            uploaded music. (Based on time delays and if the proper key was pressed).
        -   The way this button works is that it compares the keys you press with the
            uploaded keys as well as the delays between the key presses.
            So if you are pressing all the keys but too fast or too slow, then you will
            get a failing score.
        -   If you were off by one key from the very beginning, you will get no points
            for pressing the right keys, but maybe you might get points for the 
            timing between button presses (depends on how fast you press it).
        -   If you don't try, then it will tell you that "you didn't even try" and give
            you a fail score.
        -   If you press all the wrong keys then you will also get a failing score.
## Authors
- Aditya Harvi
- Ralph Milford
