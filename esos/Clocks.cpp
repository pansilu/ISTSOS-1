#include "Clocks.h"

// begin RTC 
void initRTC(){
    if(!rtc.begin())
        printErrorCode(F("RTC_NOT_CONNECTED"),RTC_NOT_CONNECTED);
    else if(!rtc.isrunning())
        printErrorCode(F("RTC_FAILED"),RTC_FAILED);
    else
        printErrorCode(F("RTC_INIT_DONE"),RTC_INIT_DONE); 
}