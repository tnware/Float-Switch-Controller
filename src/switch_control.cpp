/**
 * Switch Control
 * Description: This file contains the switch control
 * Author: Tyler Woods
 * Date: 1/16/2024
 */
#include "switch_control.h"
#include "globals.h"
#include <HTTPClient.h>

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

// New array to store the timestamp of the last OPEN to CLOSED transition for each switch
unsigned long lastCloseTimestamps[MAX_SWITCHES] = {0};

void updateSwitchStates(bool &relayShouldBeActive, int &openSwitches, bool &stateChanged)
{
    unsigned long currentTimestamp = millis();  // Get the current timestamp
    const unsigned long debouncePeriod = 10000; // 10 seconds cooldown period for OPEN to CLOSED transition

    if (!overrideMode)
    {
        for (int i = 0; i < MAX_SWITCHES; ++i)
        {
            bool currentState = digitalRead(floatSwitchPins[i]) == LOW; // Read current state (CLOSED if LOW)

            // If switch goes from OPEN to CLOSED, apply debounce
            if (!lastSwitchStates[i] && currentState)
            {
                if (currentTimestamp - lastCloseTimestamps[i] > debouncePeriod)
                {
                    // Accept the CLOSED state after debounce period
                    lastSwitchStates[i] = currentState;
                    lastCloseTimestamps[i] = currentTimestamp; // Update last CLOSED timestamp
                    // No need to send webhook or increment counter here
                }
            }
            // Immediate response for CLOSED to OPEN transition
            else if (lastSwitchStates[i] && !currentState)
            {
                lastSwitchStates[i] = currentState; // Update the last state
                tripCounters[i]++;                  // Increment trip counter
                stateChanged = true;
                // sendDiscordWebhook("Float switch " + String(i) + " has been tripped! Tripped " + String(tripCounters[i]) + " time(s)!");
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
