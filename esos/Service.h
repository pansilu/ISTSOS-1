#ifndef SERVICE_H
#define SERVICE_H

#include <sim800.h>
#include "Settings.h"
#include "log.h"
#include "Clocks.h"

// SIM800
extern Sim800 server ;

void ServiceBegin();
String getRequestString(double* externalHum,
    double *externalTemp,
    double *internalTemp,
    double *light_intensity,
    double *windSpeed,
    double *windDirection,
    double *rainFall,
    double *pressure,
    double *soilMoisture,
    double *altitude,
    double *battry,
    int type,
    String TimeStamp,
    String Guid);

uint8_t executePostRequest(char server[],char url[],String &data,uint8_t auth=0);
int readRSSI();

DateTime ntpUpdate();

#endif
