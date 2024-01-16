#ifndef GLOBALS_H
#define GLOBALS_H
#include <Arduino.h>

extern const int floatSwitchPins[];
extern const int numSwitches;
extern bool lastSwitchStates[];
extern const int relayPin;
extern const int ledPin;
extern bool currentRelayState;
extern bool overrideMode;

#endif
