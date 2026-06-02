#include <Arduino.h>
#include "config.h"
#include "sensors/sht40-sensor.h"
#include "sensors/ldr-sensor.h"
#include "sensors/cms-sensor.h"
#include "actuators/gl-actuator.h"
#include "actuators/wp-actuator.h"
#include "wifi-manager.h"
#include "time-manager.h"

#include "communication/telemetry.h"
#include "communication/serverCommunication.h"

SHTReading currentSHTReading;
unsigned long lastSHTReadTime = 0;
unsigned long lastLDRReadTime = 0;
unsigned long lastWPRunTime = 0;
unsigned long lastCoolDownTime = 0;
unsigned long lastDBUpdate = 0;
unsigned long lastServerUpdate = 0;
unsigned long lastRTCUpdate = 0;

bool hasSHTReading = false;

void setup() {

  Serial.begin(9600);

  initSHT40();
  initLDR();
  initGL();
  initCMS();
  initWP();
  connectWiFi();
  if (isWiFiConnected() == true) {
    initTime();
  }
}

void loop() {

  unsigned long currentTime = millis();

  if (currentTime - lastSHTReadTime >= SHTReadInterval) {
    currentSHTReading = readSHT40();
    if (currentSHTReading.SHTValid) {
      hasSHTReading = true;
    }
    lastSHTReadTime = currentTime;
  }

  if (isInsideLightWindow()) {
    if (currentTime - lastLDRReadTime >= LDRReadInterval) {
      currentLightState = true;
      if (determineLightState() == true) {
        turnGrowLightON();
      } else { turnGrowLightOFF(); }
        lastLDRReadTime = currentTime;
      }
    } else { turnGrowLightOFF(); currentLightState = false; }

  if (waterCooldownOver) {
    currentSoilLevel = determineSoilState();
    if(currentSoilLevel == DRY) {
      waterPumpON();
      currentPumpStatus = true;
      waterCooldownOver = false;
      lastWPRunTime = currentTime;
    }
  }

  if ((currentPumpStatus) && (currentTime - lastWPRunTime >= WPRunTime)) {
    waterPumpOFF();
    currentPumpStatus = false;
    lastCoolDownTime = currentTime;
  }
  if ((!waterCooldownOver) && (!currentPumpStatus) && (currentTime - lastCoolDownTime >= WPCooldownPeriod)) {
    waterCooldownOver = true;
  }

  if (currentTime - lastRTCUpdate >= RTCDriftUpdateInterval) {
    syncTimeFromNTP();
    lastRTCUpdate = currentTime;
  }

  if (currentTime - lastServerUpdate >= ServerUpdateInterval) {
    if (hasSHTReading) {
      buildTelemetryJson(currentSHTReading, 
        currentLightLevel, 
        currentLightState, 
        currentSoilState, 
        currentSoilLevel, 
        currentPumpStatus,
        waterCooldownOver,
        insideLightWindow);
      sendTelemetry();
    }
    lastServerUpdate = currentTime;
  }
}