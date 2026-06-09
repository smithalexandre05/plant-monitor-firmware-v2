#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFiS3.h>

bool connectWiFi();
bool isWiFiConnected();
void printWiFiStatus();
void checkFirmwareVersion();
extern WiFiSSLClient client;
WiFiSSLClient& retrieveClientInstance();

#endif