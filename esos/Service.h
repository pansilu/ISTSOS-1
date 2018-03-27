#ifndef SERVICE_H
#define SERVICE_H

#include <sim800.h>
#include "Settings.h"
#include "log.h"
#include "Clocks.h"

// SIM800
extern Sim800 server ;

void ServiceBegin();
void sendRequestString(double* externalHum,
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

int readRSSI();
uint8_t sendRequstMessage(char server_url[],char url[],String message,bool auth);
DateTime ntpUpdate();

#endif
