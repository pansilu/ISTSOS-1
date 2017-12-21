#include "unit.h"

void unitRun(){
    if(RTC_TEST){
        initRTC();  // RTC init
        if(RTC_SET_LOCAL_TIME)
        setTimeFromPC();
        Serial.print(getLocalTime());
    }

    if(SERVICE_TEST){
        ServiceBegin();
        double testVar = 0.0;
        String s= getLocalTime();
        String p= String("8b29c33e-9df0-44");

        Serial.println(executeRequest(&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,s,p));
    }
    
    while(1);
}

