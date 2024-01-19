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
                sendDiscordWebhook("Float switch " + String(i) + " has been tripped! Tripped " + String(tripCounters[i]) + " time(s)!");
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
