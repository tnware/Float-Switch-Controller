/**
 * Setup
 * Description: This file contains the setup
 * Author: Tyler Woods
 * Date: 1/16/2024
 */
#include "setup.h"
#include "globals.h"

void initializePins()
{
    // Initialize the float switch pins as inputs with pull-up resistors
    for (int i = 0; i < numSwitches; ++i)
    {
        pinMode(floatSwitchPins[i], INPUT_PULLUP);
    }

    // Optionally, introduce a short delay before reading the initial states
    delay(100);

    // Read and debug the initial state of each switch
    // for (int i = 0; i < numSwitches; ++i)
    // {
    //     lastSwitchStates[i] = digitalRead(floatSwitchPins[i]);
    //     Serial.print("Initial state of switch ");
    //     Serial.print(i);
    //     Serial.print(": ");
    //     Serial.println(lastSwitchStates[i] == HIGH ? "HIGH (OPEN)" : "LOW (CLOSED)");
    // }

    // Initialize the relay and LED pins as outputs
    pinMode(relayPin, OUTPUT);
    pinMode(ledPin, OUTPUT);
    digitalWrite(relayPin, currentRelayState);
    digitalWrite(ledPin, currentRelayState);
}
