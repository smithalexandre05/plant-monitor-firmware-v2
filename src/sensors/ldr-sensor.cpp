/*
funcs:
- int retrieveLDRReading
- bool determineLightState
*/

#include "config.h"
#include <Arduino.h>

float currentLightLevel;
bool currentLightState = false;

void initLDR() {
    pinMode(LDR_PIN, INPUT);
}

float retrieveLDRReading() {
    return analogRead(LDR_PIN);
}

bool determineLightState() {
    currentLightLevel = retrieveLDRReading();
    if (currentLightLevel < LDRLowerLightThreshold) {
        currentLightState = true;
        return true; // turn light ON...
    } else if (currentLightLevel > LDRUpperLightThreshold) {
        currentLightState = false;
        return false;
    }
}