/**
 * Switch Control
 * Description: This file contains the switch control
 * Author: Tyler Woods
 * Date: 1/16/2024
 */
#include "switch_control.h"
#include "globals.h"

void updateSwitchStates(bool &relayShouldBeActive, int &openSwitches, bool &stateChanged)
{
    if (!overrideMode)
    {
        for (int i = 0; i < MAX_SWITCHES; ++i)
        {
            bool currentState = digitalRead(floatSwitchPins[i]) == LOW; // Read current state (CLOSED if LOW)

            // Check for transition from CLOSED (true) to OPEN (false)
            if (lastSwitchStates[i] && !currentState)
            {
                tripCounters[i]++;   // Increment trip counter for this switch
                stateChanged = true; // Mark that a change has occurred
            }

            // Update the last state and manage relay and open switches count
            if (currentState != lastSwitchStates[i])
            {
                lastSwitchStates[i] = currentState; // Update the last state
            }
            if (!currentState) // If current state is OPEN (LOW, true)
            {
                relayShouldBeActive = false;
                openSwitches++;
                stateChanged = true;
            }
        }
    }
    else
    {
        // In override mode, logic remains the same
        for (int i = 0; i < MAX_SWITCHES; ++i)
        {
            if (!lastSwitchStates[i]) // If the switch is OPEN (HIGH, false)
            {
                relayShouldBeActive = false;
                openSwitches++;
            }
        }
    }
}
