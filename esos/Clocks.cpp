#include "Clocks.h"

RTC_DS3231 rtc;  

// begin RTC 
void initRTC(){
    printString(F("INITIALIZING"),F("RTC"));
    if(!rtc.begin())
      printString(F("NOT CONNECTED"),F("RTC"));
    else
      printString(F(SUCCESSFULL),F("RTC"));
}

// void set Time from PC
void setTimeFromPC(){
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

uint8_t setTimeExternal(DateTime dt){
    if(dt.year()>=2018){
      rtc.adjust(dt);
      return 1;
    }
    else{
      return 0;
    }
}

void setNTPTime(){
  printString(F("REFRESH..."),F("RTC"));
  if(setTimeExternal(ntpUpdate()))
    printString(F(SUCCESSFULL),F("NTP"),RTC_UPDATED);
  else
    printString(F(SUCCESS_ERROR),F("NTP"),RTC_UPDATED);
  delay(1000);
}

// get the local time

String getLocalTime(){
   DateTime now = rtc.now();

   char str_time[30];
   sprintf(str_time, "%04d-%02d-%02d %02d:%02d:%02d", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());

   return String(str_time);
}

String getLocalTimeHHMM(){
   DateTime now = rtc.now();

   char str_time[30];
   sprintf(str_time, "%04d-%02d-%02d %02d:%02d", now.year(), now.month(), now.day(), now.hour(), now.minute());

   return String(str_time);
}

String getFileNameDate(){
   DateTime now = rtc.now();

   char str_time[30];
   sprintf(str_time, "%04d%02d%02d.TXT", now.year(), now.month(), now.day());

   return String(str_time);
}

String getFileNameTime(){
   DateTime now = rtc.now();

   char str_time[30];
   sprintf(str_time, "%02d%02d%02d%02.TXT", now.month(), now.day(), now.hour(), now.minute(),now.second());

   return String(str_time);
}

String getGrinichTime(){
   DateTime now = rtc.now();
   now =now - TimeSpan(0, 5, 30, 00);

   char str_time[30];
   sprintf(str_time, "%04d-%02d-%02dT%02d:%02d:%02d+0000", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());
   return String(str_time);
}

DateTime getCurruntRTCDate(){
    return rtc.now();
}

unsigned long getUnixTime(){
    DateTime now = rtc.now();
    return now.unixtime();
}
