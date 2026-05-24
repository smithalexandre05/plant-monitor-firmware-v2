#ifndef CMS_SENSOR_H
#define CMS_SENSOR_H

enum SoilState {
    DRY,
    MOIST,
    WET,
    INVALID
};

 extern float currentSoilState;
 extern SoilState currentSoilLevel;

void initCMS();
SoilState determineSoilState();
String soilStateToString(SoilState state);

#endif