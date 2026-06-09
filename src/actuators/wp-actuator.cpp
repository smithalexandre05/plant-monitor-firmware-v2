#include "config.h"

bool waterCooldownOver = true;
bool currentPumpStatus = false;
bool pumpHasActivated = false;

void initWP() {
    pinMode(WP_PIN, OUTPUT);
}

void waterPumpON() {
    digitalWrite(WP_PIN, HIGH);
}

void waterPumpOFF() {
    digitalWrite(WP_PIN, LOW);
}