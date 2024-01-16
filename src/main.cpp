#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

// Initialize the U8G2 library for SSD1306 OLED display
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

// Array of GPIO pins used for the float switches
const int floatSwitchPins[] = {4, 5, 15, 18, 19};
const int numSwitches = sizeof(floatSwitchPins) / sizeof(floatSwitchPins[0]);
bool lastSwitchStates[numSwitches]; // Array to store the last state of each switch

const int relayPin = 23;      // GPIO pin used for the relay
const int ledPin = 2;         // GPIO pin used for the LED
bool currentRelayState = LOW; // Current state of the relay

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

    // Read and check the state of each float switch
    for (int i = 0; i < numSwitches; ++i)
    {
        bool isClosed = digitalRead(floatSwitchPins[i]) == LOW;

        // Check if state has changed
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

    // Update the OLED only if a change in state has occurred
    if (stateChanged)
    {
        drawStatusScreen(openSwitches == 0, openSwitches);
    }

    // Control the relay and LED based on the float switches status
    if (relayShouldBeActive && currentRelayState == LOW)
    {
        if (currentRelayState != HIGH)
        { // Check if the relay state needs to change
            digitalWrite(relayPin, HIGH);
            digitalWrite(ledPin, HIGH); // Turn on the LED
            currentRelayState = HIGH;
            Serial.println("Relay and LED activated.");
        }
    }
    else if (!relayShouldBeActive && currentRelayState == HIGH)
    {
        if (currentRelayState != LOW)
        { // Check if the relay state needs to change
            digitalWrite(relayPin, LOW);
            digitalWrite(ledPin, LOW); // Turn off the LED
            currentRelayState = LOW;
            Serial.println("Relay and LED deactivated.");
        }
    }

    delay(1000); // Wait for a second before reading again
}
