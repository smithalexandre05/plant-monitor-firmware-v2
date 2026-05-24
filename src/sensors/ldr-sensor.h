/*
funcs:
- void initLDR
- bool determineLightState
*/
#ifndef LDR_SENSOR_H
#define LDR_SENSOR_H

extern float currentLightLevel;
extern bool currentLightState;

void initLDR();
bool determineLightState();

#endif