/**
 * Web Server Configuration
 * Description: This file handles the setup and request handling for the web server.
 * Author: Tyler Woods
 * Date: 1/16/2024
 */
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "globals.h"
#include "index.h"

extern AsyncWebServer server; // Use the server object defined in main.cpp

// Serializes the states of the float switches into a JSON string
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

// Serializes the relay status into a JSON string
String getRelayStatusJSON()
{
    DynamicJsonDocument doc(256);
    bool isRelayActive = digitalRead(relayPin) == HIGH; // Assuming HIGH means active
    doc["relayStatus"] = isRelayActive ? "ON" : "OFF";
    String output;
    serializeJson(doc, output);
    return output;
}

// Serializes the global status into a JSON string
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

// Serializes the override status into a JSON string
String getOverrideStatusJSON()
{
    DynamicJsonDocument doc(256);
    doc["overrideStatus"] = overrideMode; // Assuming overrideMode is a boolean
    String output;
    serializeJson(doc, output);
    return output;
}

// Handles toggle requests for individual switches
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

// Handles requests to toggle override mode
void handleOverrideToggle(AsyncWebServerRequest *request)
{
    overrideMode = !overrideMode;
    Serial.println(overrideMode ? "Override mode activated." : "Override mode deactivated.");
    request->redirect("/");
}

// Sets up the web server routes and handlers
void setupWebServer()
{
    // Route for serving the main page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/html", webpage); });
    // Route for serving the toggle endpoint
    server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request)
              { handleSwitchToggle(request); });
    // Route for serving the override endpoint
    server.on("/override", HTTP_GET, [](AsyncWebServerRequest *request)
              { handleOverrideToggle(request); });
    // Route for serving the switch data endpoint
    server.on("/getSwitchData", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    String response = getSwitchDataJSON(); // Function to generate JSON response
    request->send(200, "application/json", response); });
    // Route for serving the relay status endpoint
    server.on("/getRelayStatus", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    String response = getRelayStatusJSON();
    request->send(200, "application/json", response); });
    // Route for serving the global status endpoint
    server.on("/getGlobalStatus", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    String response = getGlobalStatusJSON();
    request->send(200, "application/json", response); });
    // Route for serving the override status
    server.on("/getOverrideStatus", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        String response = getOverrideStatusJSON();
        request->send(200, "application/json", response); });
    // More routes...
}
