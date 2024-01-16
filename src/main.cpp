// Standard Library Includes
#include <Arduino.h>

// External Libraries
#include <U8g2lib.h>
#include <ESPAsyncWebServer.h>

// Project-Specific Headers
#include "credentials.h"
#include "index.h"          // HTML content
#include "display.h"        // Display-related functions
#include "globals.h"        // Global variables and constants
#include "logic.h"          // Business logic
#include "network.h"        // Network-related functions
#include "switch_control.h" // Switch control functions
#include "web_server.h"     // Web server setup and handlers
#include "setup.h"          // Setup-related functions

AsyncWebServer server(80);
// Initialize the U8G2 library for SSD1306 OLED display
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

    initializePins();
    setupWiFi();      // Initialize WiFi connection
    setupWebServer(); // Initialize web server setup
    server.begin();
    Serial.println("HTTP server started.");
    Serial.println(WiFi.localIP());
}

void loop()
{
    bool relayShouldBeActive = true;
    int openSwitches = 0;
    bool stateChanged = false;

    updateSwitchStates(relayShouldBeActive, openSwitches, stateChanged);

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
