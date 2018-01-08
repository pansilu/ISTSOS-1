#include "Clocks.h"

RTC_DS3231 rtc;  

// begin RTC 
void initRTC(){
    if(!rtc.begin())
    printError(F("RTC_NOT_CONNECTED"));
    else if(rtc.lostPower()){
        printError(F("RTC_ADJESTING..."));
        
        for(int i=0;i<5;i++)
          setTimeFromPC();
        printError(F("RTC_SUCCESSFULL"));
    }
    
    else
    printStr(F("RTC_INIT_DONE ")); 
}

// void set Time from PC
void setTimeFromPC(){
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void setTimeExternal(DateTime dt){
    rtc.adjust(dt);
}

// get the local time

String getLocalTime(){
   DateTime now = rtc.now();

   char str_time[30];
   sprintf(str_time, "%04d-%02d-%02d %02d:%02d:%02d", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());

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
