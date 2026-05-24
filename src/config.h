#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Pin out

constexpr int LDR_PIN = A1;
constexpr int GL_PIN = 12;
constexpr int CMS_PIN = A2;
constexpr int WP_PIN = 5;

// Timing Variables

// SHT40
constexpr const unsigned long SHTReadInterval = 5000;
// LDR
constexpr const unsigned long LDRReadInterval = 5000;
// CMS
constexpr const unsigned long CMSReadInterval = 30000;
// WP
constexpr const unsigned long WPCooldownPeriod = 1200000;
constexpr const unsigned long WPRunTime = 1000; // ~80mL
// DASHBOARD UPDATE
constexpr const unsigned long ServerUpdateInterval = 4000;

// Time Manager
constexpr int lowerWindowTimeLimit = 8;
constexpr int upperWindowTimeLimit = 21;

// Ldr
constexpr int LDRUpperLightThreshold = 1000;
constexpr int LDRLowerLightThreshold = 960;

// Grow Light
// constexpr bool currentLightState = false;

// Capacitive Moisture Sensor
constexpr int CMS_DRY_Min = 390;
constexpr int CMS_MOIST_Min = 250;
// constexpr int CMS_WET_Min = 200;

// Water Pump
constexpr bool wateringCooldownOver = true;

#endif