#include "time-manager.h"
#include "config.h"
#include "RTC.h"
#include <NTPClient.h>
#include <WiFiUdp.h>

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;

// Create a instance of the NTPClient class called timeClient and with Udp as input
NTPClient timeClient(Udp);

// Has the time been set?
bool rtcTimeValid = false;

bool insideLightWindow;

bool syncTimeFromNTP();

void initTime() {
    if (rtcTimeValid == false) {
        // Begin RTC
        RTC.begin();
        // Begin connection with NPT and Udp
        timeClient.begin();
        bool timeRetrievalSuccess = syncTimeFromNTP();
        if (timeRetrievalSuccess == true) {
            rtcTimeValid = true;
        } else {
            Serial.println("Error: Could not retrieve time with NTP...");
            rtcTimeValid = false;
        }
    }
}

bool syncTimeFromNTP() {

    // Essentially update time (when enough time has passed)
    bool timeUpdate = timeClient.forceUpdate();

    if (timeUpdate == true) {
        // We're 2 hours off UPC
        auto timeZoneOffsetHours = 2;

        // Unix is the amount of time since January 1, 1970, 00:00:00 (UTC) (also known as Epoch, the last known starting point)
        auto unixTime = timeClient.getEpochTime() + (timeZoneOffsetHours * 3600);

        // Var of type RTCTime called 'timeToSet' set to our new unixTime
        RTCTime timeToSet = RTCTime(unixTime);

        RTC.setTime(timeToSet);

        // Retrieve the date and time from the RTC and print them
        RTCTime currentTime;
        RTC.getTime(currentTime); 
        Serial.println("The RTC was just set to: " + String(currentTime));

        return true;
    } else {
        return false;
    }
}

bool isInsideLightWindow() {
    RTCTime windowTime;
    RTC.getTime(windowTime);
    int currentHour = windowTime.getHour();
    if (currentHour >= lowerWindowTimeLimit && currentHour <= upperWindowTimeLimit) {
        insideLightWindow = true;
        return true;
    } else { insideLightWindow = false; return false; }
}