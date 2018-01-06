#include "Clocks.h"

RTC_DS3231 rtc;  
String localDateTime;

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

// get the local time

String getLocalTime(){
   DateTime now = rtc.now();
   
   localDateTime=String(now.year(),DEC);
   localDateTime.concat('-');
   if(now.month()<10)localDateTime.concat("0");
   localDateTime.concat(String(now.month(), DEC));
   localDateTime.concat('-');
   if(now.day()<10)localDateTime.concat("0");
   localDateTime.concat(String(now.day(), DEC));
   localDateTime.concat('-');
   if(now.hour()<10)localDateTime.concat("0");
   localDateTime.concat(String(now.hour(), DEC));
   localDateTime.concat(':');
   if(now.minute()<10)localDateTime.concat("0");
   localDateTime.concat(String(now.minute(), DEC));
   localDateTime.concat(':');
   if(now.second()<10)localDateTime.concat("0");
   localDateTime.concat(String(now.second(), DEC));

   return localDateTime;
}

String getGrinichTime(){
   DateTime now = rtc.now();
   now =now - TimeSpan(0, 5, 30, 00);
   
   localDateTime=String(now.year(),DEC);
   localDateTime.concat('-');
   if(now.month()<10)localDateTime.concat("0");
   localDateTime.concat(String(now.month(), DEC));
   localDateTime.concat('-');
   if(now.day()<10)localDateTime.concat("0");
   localDateTime.concat(String(now.day(), DEC));
   localDateTime.concat('T');
   if(now.hour()<10)localDateTime.concat("0");
   localDateTime.concat(String(now.hour(), DEC));
   localDateTime.concat(':');
   if(now.minute()<10)localDateTime.concat("0");
   localDateTime.concat(String(now.minute(), DEC));
   localDateTime.concat(':');
   if(now.second()<10)localDateTime.concat("0");
   localDateTime.concat(String(now.second(), DEC));
   localDateTime.concat("+0000");
   return localDateTime;
}

DateTime getCurruntRTCDate(){
    return rtc.now();
}

unsigned long getUnixTime(){
    DateTime now = rtc.now();
    return now.unixtime();
}
