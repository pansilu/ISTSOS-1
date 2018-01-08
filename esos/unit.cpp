#include "unit.h"

void unitRun(){
    if(RTC_TEST){
        initRTC();  // RTC init
        
        Serial.print(getLocalTime());
        Serial.print(getGrinichTime());
    }

    if(SERVICE_TEST){
        ServiceBegin();
        double testVar = 0.5;
        String s= getLocalTime();
        String p= String("07ec7356-9b13-48");

        Serial.println(executeRequest(&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,GET_REQUEST,s,p));
    }

    if(SERVICE_TEST_IST){
        ServiceBegin();
        double testVar = 0.1;
        Serial.print(getLocalTime());
        String s= getGrinichTime();
        Serial.print(s);
        String p= String("bb3a14a0988311e78b760800273cbaca");
        Serial.println(executeRequest(&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,POST_REQUEST,s,p));
    }

    if(SERVICE_TEST_SLPPOST){
      //String data= String("{\"A\":\"100\"}");
      //executePostRequest("slpiot.org","/sp.php",data);
      ServiceBegin();
        double testVar = 0.1;
        Serial.print(getLocalTime());
        String s= getLocalTime();
        Serial.print(s);
        String p= String("07ec7356-9b13-48");
        Serial.println(executeRequest(&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,JSON_POST_REQUEST,s,p));
    }
    
    if(NTP_TEST){
        DateTime now = ntpUpdate();
        char str_time[30];
        sprintf(str_time, "%04d-%02d-%02d %02d:%02d:%02d", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());     
        Serial.println(String(str_time));
    }
    while(1);
}

