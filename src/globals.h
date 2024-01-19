/**
 * Globals
 * Description: This file contains the globals
 * Author: Tyler Woods
 * Date: 1/16/2024
 */
// globals.h
#ifndef GLOBALS_H
#define GLOBALS_H
#include <Arduino.h>
#define MAX_SWITCHES 5 // Adjust this to your maximum number of switches

extern const int floatSwitchPins[];
extern const int numSwitches;
extern bool lastSwitchStates[];
extern const int relayPin;
extern const int ledPin;
extern bool currentRelayState;
extern bool overrideMode;
extern int tripCounters[MAX_SWITCHES]; // Declare the array size with MAX_SWITCHES

#endif // GLOBALS_H
