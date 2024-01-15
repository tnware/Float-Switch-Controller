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

void drawCheckmark()
{
    // Draw a simple checkmark
    u8g2.drawLine(10, 30, 20, 40); // First part of the checkmark
    u8g2.drawLine(20, 40, 40, 20); // Second part of the checkmark
}

void drawCross()
{
    // Draw a simple cross
    u8g2.drawLine(10, 20, 40, 50); // First diagonal
    u8g2.drawLine(40, 20, 10, 50); // Second diagonal
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

        if (isClosed != lastSwitchStates[i])
        {                                   // Check if state has changed
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
        u8g2.clearBuffer(); // Clear the buffer

        if (openSwitches == 0)
        {
            drawCheckmark();
            u8g2.setFont(u8g2_font_ncenB08_tr); // Choose a suitable font
            u8g2.drawStr(50, 30, "OKAY");       // Place text next to the checkmark
            Serial.println("System Status: OKAY");
        }
        else
        {
            // Log system status to the console
            Serial.print("ALERT: ");
            Serial.print(openSwitches);
            Serial.println(" switch(es) OPEN.");
            drawCross();
            int yPosition = 40;
            for (int i = 0; i < numSwitches; ++i)
            {
                if (!lastSwitchStates[i])
                {
                    u8g2.setFont(u8g2_font_ncenB08_tr); // Choose a suitable font
                    u8g2.setCursor(50, yPosition);      // Set cursor position next to the cross
                    u8g2.print("#");
                    u8g2.print(i + 1);
                    u8g2.print(": OPEN");
                    yPosition += 15;
                }
            }
        }

        u8g2.sendBuffer(); // Transfer buffer to the display
    }

    // Control the relay and LED based on the float switches status
    if (relayShouldBeActive && currentRelayState == LOW)
    {
        digitalWrite(relayPin, HIGH);
        digitalWrite(ledPin, HIGH); // Turn on the LED
        currentRelayState = HIGH;
    }
    else if (!relayShouldBeActive && currentRelayState == HIGH)
    {
        digitalWrite(relayPin, LOW);
        digitalWrite(ledPin, LOW); // Turn off the LED
        currentRelayState = LOW;
    }

    delay(1000); // Wait for a second before reading again
}
