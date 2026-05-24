#include "config.h"
#include "sensors/ldr-sensor.h"
#include <Arduino.h>

void initGL() {
    pinMode(GL_PIN, OUTPUT);
    if (currentLightState == false) {
        digitalWrite(GL_PIN, LOW);
    }
}

void turnGrowLightON() {
    digitalWrite(GL_PIN, HIGH);
}

void turnGrowLightOFF() {
    digitalWrite(GL_PIN, LOW);
}