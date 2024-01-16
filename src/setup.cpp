#include "setup.h"
#include "globals.h"

void initializePins()
{
    // Code to initialize GPIO pins
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
