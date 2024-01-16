#include "switch_control.h"
#include "globals.h"

void updateSwitchStates(bool &relayShouldBeActive, int &openSwitches, bool &stateChanged)
{
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
}
