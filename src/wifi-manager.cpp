#include "wifi-secrets.h"
#include "wifi-manager.h"

// Vars to store ssid, pass
char ssid [] = SECRET_SSID;
char pass [] = SECRET_PASS;

// Wifi status:
int WiFiStatus = WL_IDLE_STATUS;

// Initialize the client library
WiFiSSLClient client;

bool connectWiFi() {
  // Check if module exists
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("WiFi module not found...");
  while (true) {
    Serial.println("WiFi failed");
    delay(100);
    }   
  }

  // check firmware version
  checkFirmwareVersion();

  // Attemp to connect to WiFi
  // This will need to change eventually when used in the main.cpp (currently the code blocks here)
  while (WiFiStatus != WL_CONNECTED) {
    WiFiStatus = WiFi.begin(ssid, pass);
    Serial.println("Attempting to connect to WiFi...");
    delay(3000);
  }

  // print status
  if (WiFi.status() == WL_CONNECTED) {
    printWiFiStatus();
    return true;
  } else {
    return false;
  }
}

bool isWiFiConnected() {
    if (WiFiStatus == WL_CONNECTED) {
        Serial.println("isWiFiConnected() - TRUE");
        return true;
    } else {
        Serial.println("isWiFiConnected() - FALSE");
        return false;
    }
}

void printWiFiStatus() {
  Serial.print("Connected to SSID: ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}

void checkFirmwareVersion() {
  String fv = WiFi.firmwareVersion();
  Serial.print("Firmware version: ");
  Serial.println(fv);

  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the WiFi firmware");
  }
}

WiFiSSLClient& retrieveClientInstance() {
  return client;
}