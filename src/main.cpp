#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <WiFi.h>
#include <WebServer.h>
#include "credentials.h"

// Replace with your network credentials

WebServer server(80); // HTTP server on port 80
// Initialize the U8G2 library for SSD1306 OLED display
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

// Array of GPIO pins used for the float switches
const int floatSwitchPins[] = {4, 5, 15, 18, 19};
const int numSwitches = sizeof(floatSwitchPins) / sizeof(floatSwitchPins[0]);
bool lastSwitchStates[numSwitches]; // Array to store the last state of each switch

const int relayPin = 23;      // GPIO pin used for the relay
const int ledPin = 2;         // GPIO pin used for the LED
bool currentRelayState = LOW; // Current state of the relay

bool overrideMode = false; // Global variable to track the override mode

void handleSwitchToggle()
{
    if (server.hasArg("switch"))
    {
        int switchNum = server.arg("switch").toInt();
        if (switchNum >= 0 && switchNum < numSwitches)
        {
            // Toggle the state of the switch
            lastSwitchStates[switchNum] = !lastSwitchStates[switchNum];
            Serial.print("Toggled Switch ");
            Serial.print(switchNum + 1);
            Serial.println(lastSwitchStates[switchNum] ? ": CLOSED" : ": OPEN");
        }
    }
    server.sendHeader("Location", "/");
    server.send(303); // Redirect back to the root page
}

void handleOverrideToggle()
{
    overrideMode = !overrideMode; // Toggle the override mode
    Serial.println(overrideMode ? "Override mode activated." : "Override mode deactivated.");
    server.sendHeader("Location", "/");
    server.send(303); // Redirect back to the root page
}

void handleRoot()
{
    String html = "<html><head><title>Float Switch Status</title></head><body><h1>Float Switch Status</h1>";
    html += "<form action='/override' method='get'><button>Toggle Override Mode</button></form><br>";
    html += overrideMode ? "<b>Override Mode is ON</b><br>" : "<b>Override Mode is OFF</b><br>";
    for (int i = 0; i < numSwitches; ++i)
    {
        html += "Switch " + String(i + 1) + ": " + (lastSwitchStates[i] ? "CLOSED" : "OPEN");
        html += " <form action='/toggle' method='get'><button name='switch' value='" + String(i) + "'>Toggle</button></form><br>";
    }
    html += "</body></html>";
    server.send(200, "text/html", html);
}

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

    // Connect to WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected.");

    // Start the server
    server.on("/", handleRoot);
    server.on("/toggle", handleSwitchToggle);
    server.on("/override", handleOverrideToggle);
    server.begin();
    Serial.println("HTTP server started.");
    Serial.println(WiFi.localIP());
}

void drawStatusScreen(bool allClosed, int openSwitchCount)
{
    // Clear the buffer
    u8g2.clearBuffer();

    // Draw the top status bar
    for (int i = 0; i < numSwitches; ++i)
    {
        if (lastSwitchStates[i])
        {
            // Draw a filled rectangle for closed switch
            u8g2.drawBox(i * 25 + 1, 0, 24, 8);
        }
        else
        {
            // Draw an unfilled rectangle for open switch
            u8g2.drawFrame(i * 25 + 1, 0, 24, 8);
        }
    }

    // Set the base line y-coordinate for the status text
    int baseY = 24;

    // Choose a suitable font for status message
    u8g2.setFont(u8g2_font_ncenB08_tr); // Choose a suitable font

    // Display the system status as text
    if (allClosed)
    {
        u8g2.drawStr(0, baseY, "OKAY");
    }
    else
    {
        char alertMsg[30];
        sprintf(alertMsg, "ALERT: %d OPEN", openSwitchCount);
        u8g2.drawStr(0, baseY, alertMsg);
    }

    // Choose a suitable font for the icons
    u8g2.setFont(u8g2_font_unifont_t_77);

    // Position for the icon in the bottom right corner
    int iconX = u8g2.getDisplayWidth() - 18; // 18 pixels from the right edge
    int iconY = u8g2.getDisplayHeight() - 2; // 2 pixels from the bottom edge

    // Draw the appropriate icon
    if (allClosed)
    {
        // Draw water droplet icon
        u8g2.drawGlyph(iconX, iconY, 0x26c6);
    }
    else
    {
        // Draw alert icon
        u8g2.drawGlyph(iconX, iconY, 0x26a0);
    }

    // Send buffer to the display
    u8g2.sendBuffer();
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
        digitalWrite(relayPin, HIGH);
        digitalWrite(ledPin, HIGH); // Turn on the LED
        currentRelayState = HIGH;
        Serial.println("Relay and LED activated.");
    }
    else if (!relayShouldBeActive && currentRelayState != LOW)
    {
        digitalWrite(relayPin, LOW);
        digitalWrite(ledPin, LOW); // Turn off the LED
        currentRelayState = LOW;
        Serial.println("Relay and LED deactivated.");
    }

    delay(50);             // Wait for a second before reading again
    server.handleClient(); // Handle client requests
}
