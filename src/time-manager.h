#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include <Arduino.h>
#include "RTC.h"

extern bool insideLightWindow;

void initTime();
bool isInsideLightWindow();
bool isWiFiConnected();
bool syncTimeFromNTP();

#endif