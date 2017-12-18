#ifndef CLOCK_H
#define CLOCK_H

#include <RTClib.h>
#include "Codes.h"
#include "Log.h"
#include "Settings.h"

RTC_DS1307 rtc; 

void initRTC();
String getLocalTime();

#endif