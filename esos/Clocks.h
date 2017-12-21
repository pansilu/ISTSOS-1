#ifndef CLOCK_H
#define CLOCK_H

#include <RTClib.h>
#include "Codes.h"
#include "Log.h"
#include "Settings.h"

extern RTC_DS1307 rtc; 
extern String localDateTime;

void initRTC();
String getLocalTime();
void setTimeFromPC();

#endif