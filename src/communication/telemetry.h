#ifndef TELEMETRY_H
#define TELEMETRY_H

#include "sensors/sht40-sensor.h"
#include "sensors/ldr-sensor.h"
#include "sensors/cms-sensor.h"

extern String readyData;
String buildTelemetryJson(SHTReading sht40, 
    float currentLightLevel, 
    bool currentLightState, 
    float currentSoilState, 
    SoilState currentSoilLevel, 
    bool currentPumpStatus,
    bool waterCooldownOver);

#endif