#include "serverCommunication.h"
#include "wifi-manager.h"
#include "telemetry.h"
#include <ArduinoHttpClient.h>

char SERVER_ADDRESS[] = "192.168.0.117"; // Change after Vercel implementation
int SERVER_PORT = 3000; // Change after Vercel implementation
String ENDPOINT = "/api/telemetry";
String contentType = "application/json";

bool sendTelemetry() {
    WiFiClient& client = retrieveClientInstance();
    HttpClient requestClient(client, SERVER_ADDRESS, SERVER_PORT);
    requestClient.post(ENDPOINT, contentType, readyData);

    int statusCode = requestClient.responseStatusCode();
    String response = requestClient.responseBody();

    if (statusCode == 200 || statusCode == 201) {
        Serial.println("POST Successful");
        return true;
    } else {
        Serial.print("POST Failed, status: ");
        Serial.println(statusCode);
        return false;
    }
}