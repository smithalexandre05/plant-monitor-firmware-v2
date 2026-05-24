/*\
JSON EX

{
  "deviceId": "plant-monitor-01",
  "timestamp": "2026-05-14T20:00:00Z",
  "sensors": {
    "soilRaw": 312,
    "soilState": "MOIST",
    "lightRaw": 940,
    "temperature": 22.4,
    "humidity": 55.2
  },
  "actuators": {
    "pumpRunning": false,
    "growLightOn": true
  },
  "system": {
    "insideLightWindow": true,
    "wateringCooldownOver": false
  }
}
*/

#include <Arduino.h>
#include "telemetry.h"
#include <ArduinoJson.h>
#include "RTC.h"

JsonDocument root;
RTCTime currentTime;
String readyData;

String RTCDateFormatting() {
  RTC.getTime(currentTime);
  return String(currentTime);
}

String buildTelemetryJson(SHTReading sht40, 
  float currentLightLevel, 
  bool currentLightState, 
  float currentSoilState, 
  SoilState currentSoilLevel,
  bool currentPumpStatus,
  bool waterCooldownOver,
  bool insideLightWindow) {

  JsonDocument doc;

  doc["deviceId"] = "plant-monitor-01";
  String RTCtimestamp = RTCDateFormatting();
  doc["timestamp"] = RTCtimestamp;

  JsonObject sensors = doc["sensors"].to<JsonObject>();

  sensors["temperature"] = sht40.SHTTemp;
  sensors["humidity"] = sht40.SHTHum;
  sensors["lightRaw"] = currentLightLevel;
  sensors["soilRaw"] = currentSoilState;
  sensors["soilState"] = soilStateToString(currentSoilLevel);

  JsonObject actuators = doc["actuators"].to<JsonObject>();

  actuators["growLightOn"] = currentLightState;
  actuators["pumpRunning"] = currentPumpStatus;

  JsonObject system = doc["system"].to<JsonObject>();

  system["wateringCooldownOver"] = waterCooldownOver;
  system["insideLightWindow"] = insideLightWindow;

  String jsonOutput;
  serializeJson(doc, jsonOutput);

  readyData = jsonOutput;

  return jsonOutput;
}