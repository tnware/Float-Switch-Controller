#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <WiFi.h>
#include <WebServer.h>
#include "credentials.h"
#include <ESPAsyncWebServer.h>
#include "index.h" // Include the HTML content
#include <ArduinoJson.h>
#include "display.h"
#include "globals.h"
#include "web_server.h"
#include "logic.h"
#include "network.h"

AsyncWebServer server(80);
// WebServer server(80); // HTTP server on port 80
// Initialize the U8G2 library for SSD1306 OLED display
// U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// Array of GPIO pins used for the float switches
const int floatSwitchPins[] = {4, 5, 15, 18, 19};
const int numSwitches = sizeof(floatSwitchPins) / sizeof(floatSwitchPins[0]);
bool lastSwitchStates[numSwitches]; // Array to store the last state of each switch

const int relayPin = 23;      // GPIO pin used for the relay
const int ledPin = 2;         // GPIO pin used for the LED
bool currentRelayState = LOW; // Current state of the relay

bool overrideMode = false; // Global variable to track the override mode

void setup()
{
    Serial.begin(9600); // Start serial communication at 9600 baud
    u8g2.begin();       // Initialize the OLED display

    // Initialize the float switch pins as inputs with pull-up resistors
    for (int i = 0; i < numSwitches; ++i)
    {
        pinMode(floatSwitchPins[i], INPUT_PULLUP);
        lastSwitchStates[i] = digitalRead(floatSwitchPins[i]); // Initialize the last state
    }

    // Initialize the relay and LED pins as outputs
    pinMode(relayPin, OUTPUT);
    pinMode(ledPin, OUTPUT);
    digitalWrite(relayPin, currentRelayState); // Set initial relay state
    digitalWrite(ledPin, currentRelayState);   // Set initial LED state
    setupWiFi();                               // Initialize WiFi connection
    setupWebServer();                          // Initialize web server setup
    server.begin();
    Serial.println("HTTP server started.");
    Serial.println(WiFi.localIP());
}

void loop()
{
    bool relayShouldBeActive = true;
    int openSwitches = 0;
    bool stateChanged = false;

    if (!overrideMode)
    {
        // Read and check the state of each float switch
        for (int i = 0; i < numSwitches; ++i)
        {
            bool isClosed = digitalRead(floatSwitchPins[i]) == LOW;
            if (isClosed != lastSwitchStates[i])
            {
                stateChanged = true;            // Mark that a change has occurred
                lastSwitchStates[i] = isClosed; // Update the last state
            }
            if (!isClosed)
            {
                relayShouldBeActive = false;
                openSwitches++;
            }
        }
    }
    else
    {
        // In override mode, count the number of open switches based on lastSwitchStates array
        for (int i = 0; i < numSwitches; ++i)
        {
            if (!lastSwitchStates[i])
            {
                relayShouldBeActive = false;
                openSwitches++;
            }
        }
    }

    // Update the OLED display if there is a change in state or if in override mode
    if (stateChanged || overrideMode)
    {
        drawStatusScreen(relayShouldBeActive, openSwitches);
    }

    // Control the relay and LED based on the float switches status
    if (relayShouldBeActive && currentRelayState != HIGH)
    {
        activateRelayAndLED();
    }
    else if (!relayShouldBeActive && currentRelayState != LOW)
    {
        deactivateRelayAndLED();
    }

    delay(50); // Wait for a second before reading again
}
