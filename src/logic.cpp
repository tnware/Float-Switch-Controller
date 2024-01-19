/**
 * Logic
 * Description: This file contains the relay logic
 * Author: Tyler Woods
 * Date: 1/16/2024
 */
#include "logic.h"
#include "globals.h"

void activateRelayAndLED()
{
    if (currentRelayState != HIGH)
    {
        digitalWrite(relayPin, HIGH);
        digitalWrite(ledPin, HIGH); // Turn on the LED
        currentRelayState = HIGH;
        Serial.println("Relay and LED activated.");
    }
}

void deactivateRelayAndLED()
{
    if (currentRelayState != LOW)
    {
        digitalWrite(relayPin, LOW);
        digitalWrite(ledPin, LOW); // Turn off the LED
        currentRelayState = LOW;
        Serial.println("Relay and LED deactivated.");
    }
}
