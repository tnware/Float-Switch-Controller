/**
 * Main Program File
 * Description: This file contains the main setup and loop functions for Float Switch Controller.
 * Author: Tyler Woods
 * Date: 1/16/2024
 */
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
#include <HTTPClient.h>

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
int tripCounters[MAX_SWITCHES] = {0};

void sendDiscordWebhook(String message)
{
    HTTPClient http;
    http.begin("https://discord.com/api/webhooks/1197747033667280936/agBmTfDwUQT9TyekZA7zhi35g071mccgXZ8G8z_v0kpAupEW0VehS9PBvaMCOiMtrRfM"); // Paste your webhook URL here
    http.addHeader("Content-Type", "application/json");

    String discordMessage = "{\"content\": \"" + message + "\"}";
    int httpResponseCode = http.POST(discordMessage);

    if (httpResponseCode > 0)
    {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
    }
    else
    {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    }

    http.end();
}

void setup()
{
    Serial.begin(9600); // Initialize serial communication
    u8g2.begin();       // Initialize the OLED display

    initializePins(); // Set up GPIO pins
    setupWiFi();      // Connect to WiFi
    setupWebServer(); // Initialize web server
    server.begin();   // Start the server

    Serial.println("HTTP server started.");
    Serial.println(WiFi.localIP());
}

void loop()
{
    // State variables for switch control
    bool relayShouldBeActive = true;
    int openSwitches = 0;
    bool stateChanged = false;

    updateSwitchStates(relayShouldBeActive, openSwitches, stateChanged);

    // Update the OLED display if there is a change in state or if in override mode
    if (stateChanged || overrideMode)
    {
        drawStatusScreen(relayShouldBeActive, openSwitches, overrideMode);
    }

    // Control relay and LED based on switch states
    if (relayShouldBeActive && currentRelayState != HIGH)
    {
        activateRelayAndLED();

        drawStatusScreen(relayShouldBeActive, openSwitches, overrideMode);

        sendDiscordWebhook("Relay Activated!");
    }
    else if (!relayShouldBeActive && currentRelayState != LOW)
    {
        deactivateRelayAndLED();

        drawStatusScreen(relayShouldBeActive, openSwitches, overrideMode);

        sendDiscordWebhook("Relay Deactivated!");
    }

    delay(50); // Short delay for debounce and rate control
}
