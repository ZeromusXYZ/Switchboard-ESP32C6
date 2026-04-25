#include <Wire.h>
#include <rgb_lcd.h>

rgb_lcd lcd;

const int colorR = 128;
const int colorG = 128;
const int colorB = 128;

const int PIN_BTN0 = 0;
const int PIN_BTN1 = 1;
int btn0Press = 0;
int btn1Press = 0;

const int maxConsoles = 4;
String names[maxConsoles] = {
//  "0123456789ABCDEF"
    "NES             ",
    "Super NES       ",
    "Wii U           ",
    "XBox 360        "
};
int consoleSelection = 0;
int lastSelection = 0;


void setup() {
    // Set buttons
    pinMode(PIN_BTN0, INPUT);
    pinMode(PIN_BTN1, INPUT);

    // Seeed Studio ESP32-C6 with Grove breakout on D4 + D5 makes for non-default pins to be used.
    Wire.begin(22, 23); 

    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
    
    // Our White on Blue LCD display doesn't use the RGB for light
    lcd.setRGB(colorR, colorG, colorB);

    // Print a message to the LCD.
    lcd.clear();
    lcd.setCursor(0, 0);
    //  print("0123456789ABCDEF");
    lcd.print(" Retro Selector ");
    lcd.setCursor(0, 1);
    lcd.print("Version  0.0.0.1");
    delay(2000);
    lcd.clear();
}

void loop() {
    lastSelection = consoleSelection;
    // set the cursor to column 0, line 1
    // (note: line 1 is the second row, since counting begins with 0):
    lcd.setCursor(0, 1);
    // print the number of seconds since reset:
    lcd.print("Time ");
    lcd.print(millis() / 1000);
    delay(250);

    btn0Press = digitalRead(PIN_BTN0);
    btn1Press = digitalRead(PIN_BTN1);

    if (btn0Press == HIGH)
    {
        consoleSelection++ ;
        if (consoleSelection >= maxConsoles)
        {
            consoleSelection = 0;
        }
    }

    if (btn1Press == HIGH)
    {
        consoleSelection-- ;
        if (consoleSelection < 0)
        {
            consoleSelection = maxConsoles - 1;
        }
    }

    lcd.setCursor(0,0);
    lcd.print(names[consoleSelection]);

    // Add delay if selection changed
    if (lastSelection != consoleSelection)
    {
        delay(500);
    }
}