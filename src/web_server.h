#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "globals.h"
#include "index.h"

extern AsyncWebServer server; // Declare the server object

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

String getRelayStatusJSON()
{
    DynamicJsonDocument doc(256);
    bool isRelayActive = digitalRead(relayPin) == HIGH; // Assuming HIGH means active
    doc["relayStatus"] = isRelayActive ? "ON" : "OFF";
    String output;
    serializeJson(doc, output);
    return output;
}

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

void handleOverrideToggle(AsyncWebServerRequest *request)
{
    overrideMode = !overrideMode;
    Serial.println(overrideMode ? "Override mode activated." : "Override mode deactivated.");
    request->redirect("/");
}

void setupWebServer()
{
    // Move all the server.on(...) and related logic here
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/html", webpage); });
    server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request)
              { handleSwitchToggle(request); });

    server.on("/override", HTTP_GET, [](AsyncWebServerRequest *request)
              { handleOverrideToggle(request); });
    server.on("/getSwitchData", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    String response = getSwitchDataJSON(); // Function to generate JSON response
    request->send(200, "application/json", response); });
    server.on("/getRelayStatus", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    String response = getRelayStatusJSON();
    request->send(200, "application/json", response); });

    server.on("/getGlobalStatus", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    String response = getGlobalStatusJSON();
    request->send(200, "application/json", response); });
}
