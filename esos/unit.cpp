#include "unit.h"

void unitRun(){
    if(RTC_TEST){
        initRTC();  // RTC init
        if(RTC_SET_LOCAL_TIME)
        setTimeFromPC();
        Serial.print(getLocalTime());
    }

    
    
    while(1);
}

