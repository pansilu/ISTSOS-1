#ifndef CLOCK_H
#define CLOCK_H

#define DS1307 1

#include "RTClib.h"
#include "Codes.h"
#include "Log.h"
#include "Settings.h"

extern RTC_DS3231 rtc;

extern String localDateTime;

void initRTC();
String getLocalTime();
String getLocalTimeHHMM();
String getGrinichTime();
String getFileNameDate();
String getFileNameTime();
void setTimeFromPC();
void setTimeExternal(DateTime dt);
void setNTPTime();
DateTime getCurruntRTCDate();
unsigned long getUnixTime();

#endif
