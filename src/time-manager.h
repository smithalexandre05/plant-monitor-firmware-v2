#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include <Arduino.h>
#include "RTC.h"

void initTime();
bool isInsideLightWindow();
bool isWiFiConnected();

#endif