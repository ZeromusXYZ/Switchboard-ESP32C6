#include <Wire.h>
#include <rgb_lcd.h> // Grove - LCD RGB Backlight by Seeed Studio
#include <Arduino.h>
#include <WiFi.h>
// #include "tapouser.h" // If you want to place your wifi/tapo settings in a different file
#include "tapo_device.h"

// #define TAPO_DEBUG_MODE // Comment this line to disable debug messages
#include "tapo_device.h"

const String WifiSSID = "my_wifi_ssid";
const String WifiPass = "wifipass";
const String TapoUser = "tapouser@somemail.ext";
const String TapoPass = "tapopassword";

rgb_lcd lcd;

const int colorR = 128;
const int colorG = 128;
const int colorB = 128;

const int PIN_BTN0  = 0;
const int PIN_BTN1  = 1;
const int PIN_START = 2;
int btn0Press = 0;
int btn1Press = 0;
int startPress = 0;

const int maxConsoles = 5;
String names[maxConsoles] = {
//  "0123456789ABCDEF"
    "NES            1",
    "Super NES      2",
    "Wii U          3",
    "XBox 360       4",
    "Playstation 1  5"
};

String tapoIP[maxConsoles] = {
//  "0123456789ABCDEF"
    "10.0.1.131",
    "10.0.1.132",
    "10.0.1.133",
    "10.0.1.134",
    "10.0.1.135"
};

TapoDevice tapo[maxConsoles];

int consoleSelection = 0;
int lastActiveSelection = -1;
long lastActiveSelectionStartTime = 0;
int lastSelection = 0;

void setup() {
    // Set buttons
    pinMode(PIN_BTN0, INPUT);
    pinMode(PIN_BTN1, INPUT);
    pinMode(PIN_START, INPUT);

    // Seeed Studio ESP32-C6 with Grove breakout on D4 + D5 makes for non-default pins to be used.
    Wire.begin(22, 23); // Top-Left connector on the related Grove board SDA4 + SCL5

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
    lcd.print("Version  0.0.0.2");
    delay(2000);
    // Connect to WiFi
    WiFi.begin(WifiSSID, WifiPass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(5000);
        lcd.setCursor(0, 1);
        //  print("0123456789ABCDEF");
        lcd.print("Waiting for WiFi");
    }
    lcd.setCursor(0, 1);
    //  print("0123456789ABCDEF");
    lcd.print("WiFi Connected !");

    // Init attached TAPOs
    for(int i = 0 ; i < maxConsoles ; i++)
    {
        // Initialize Tapo device
        // Example: tapo.begin("192.168.1.100", "abc@example.com", "abc123");
        while (!tapo[i].begin(tapoIP[i], TapoUser, TapoPass)) {
            lcd.setCursor(0, 0);
            //  print("0123456789ABCDEF");
            lcd.print(names[i]);
            lcd.setCursor(0, 1);
            //  print("0123456789ABCDEF");
            lcd.print("Tapo init failed");
            delay(2000);
        }
    }

    lcd.clear();
}

void changePlugState(int id, bool setOn)
{   
    if (setOn)
    {
        tapo[id].on();
    }
    else
    {
        tapo[id].off();
    }
}

void startSelectedConsole()
{
    if (lastActiveSelection >= 0)
    {
        changePlugState(lastActiveSelection, false);
        lastActiveSelection = -1;
        delay(1000);
    }

    changePlugState(consoleSelection, true);
    lastActiveSelection = consoleSelection;
    lastActiveSelectionStartTime = millis();
}

void loop() {
    lastSelection = consoleSelection;
    // set the cursor to column 0, line 1
    // (note: line 1 is the second row, since counting begins with 0):
    lcd.setCursor(0, 1);
    // print the number of seconds since reset:
    //  print("0123456789ABCDEF");
    if (lastActiveSelection >= 0)
    {
        long timeOn = millis() - lastActiveSelectionStartTime;
        long timeSeconds = timeOn / 1000;
        long timeMinutes = timeSeconds / 60;
        timeSeconds = timeSeconds % 60;
        //  print("0123456789ABCDEF");
        lcd.print("Playing ");
        lcd.print(timeMinutes);
        lcd.print("m ");
        lcd.print(timeSeconds);
        lcd.print("s    ");
    }
    else
    {
        lcd.print(" <<   >>   START");
    }
    delay(250);

    btn0Press = digitalRead(PIN_BTN0);
    btn1Press = digitalRead(PIN_BTN1);
    startPress = digitalRead(PIN_START);
    // stopPress = digitalRead(PIN_STOP);

    if (btn0Press == HIGH)
    {
        consoleSelection++ ;
        if (consoleSelection >= maxConsoles)
        {
            consoleSelection = -1;
        }
    }

    if (btn1Press == HIGH)
    {
        consoleSelection-- ;
        if (consoleSelection < -1)
        {
            consoleSelection = maxConsoles - 1;
        }
    }

    lcd.setCursor(0,0);
    if (consoleSelection >= 0)
    {
        lcd.print(names[consoleSelection]);
    }
    else
    {
        //  print("0123456789ABCDEF");
        lcd.print(" [Turn all OFF] ");
    }

    // Add delay if selection changed
    if (lastSelection != consoleSelection)
    {
        delay(500);
    }

    if (startPress == HIGH)
    {
        if (consoleSelection >= 0)
        {
            lcd.setCursor(0,1);
            //  print("0123456789ABCDEF");
            lcd.print("Starting        ");
            delay(500);
            startSelectedConsole();
        }
        else
        {
            lcd.setCursor(0,1);
            //  print("0123456789ABCDEF");
            lcd.print("Shutdown        ");
            if (lastActiveSelection >= 0)
            {
                changePlugState(lastActiveSelection, false);
                lastActiveSelection = -1;
                delay(3000);
            }
        }
    }
}