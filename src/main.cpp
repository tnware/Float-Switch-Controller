#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <WiFi.h>
#include <WebServer.h>
#include "credentials.h"
#include <ESPAsyncWebServer.h>
#include "index.h" // Include the HTML content
#include <ArduinoJson.h>
AsyncWebServer server(80);
// WebServer server(80); // HTTP server on port 80
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

String getSwitchDataJSON()
{
    DynamicJsonDocument doc(1024);
    for (int i = 0; i < numSwitches; ++i)
    {
        doc[String(i)] = lastSwitchStates[i] ? "CLOSED" : "OPEN";
    }
    String output;
    serializeJson(doc, output);
    return output;
}

String getRelayStatusJSON()
{
    DynamicJsonDocument doc(256);
    bool isRelayActive = digitalRead(relayPin) == HIGH; // Assuming HIGH means active
    doc["relayStatus"] = isRelayActive ? "ON" : "OFF";
    String output;
    serializeJson(doc, output);
    return output;
}

String getGlobalStatusJSON()
{
    DynamicJsonDocument doc(256);
    bool allClosed = true;
    for (int i = 0; i < numSwitches; ++i)
    {
        if (!lastSwitchStates[i])
        {
            allClosed = false;
            break;
        }
    }
    doc["globalStatus"] = allClosed ? "OKAY" : "ALERT";
    String output;
    serializeJson(doc, output);
    return output;
}

void handleSwitchToggle(AsyncWebServerRequest *request)
{
    if (request->hasArg("switch"))
    {
        int switchNum = request->arg("switch").toInt();
        if (switchNum >= 0 && switchNum < numSwitches)
        {
            lastSwitchStates[switchNum] = !lastSwitchStates[switchNum];
            Serial.print("Toggled Switch ");
            Serial.print(switchNum + 1);
            Serial.println(lastSwitchStates[switchNum] ? ": CLOSED" : ": OPEN");
        }
    }
    request->redirect("/");
}

void handleOverrideToggle(AsyncWebServerRequest *request)
{
    overrideMode = !overrideMode;
    Serial.println(overrideMode ? "Override mode activated." : "Override mode deactivated.");
    request->redirect("/");
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

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/html", webpage); });
    server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request)
              { handleSwitchToggle(request); });

    server.on("/override", HTTP_GET, [](AsyncWebServerRequest *request)
              { handleOverrideToggle(request); });
    server.on("/getSwitchData", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    String response = getSwitchDataJSON(); // Function to generate JSON response
    request->send(200, "application/json", response); });
    server.on("/getRelayStatus", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    String response = getRelayStatusJSON();
    request->send(200, "application/json", response); });

    server.on("/getGlobalStatus", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    String response = getGlobalStatusJSON();
    request->send(200, "application/json", response); });

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

    delay(50); // Wait for a second before reading again
    // server.handleClient(); // Handle client requests
}
