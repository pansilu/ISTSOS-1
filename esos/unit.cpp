#include "unit.h"

void unitRun(){
    if(RTC_TEST){
        initRTC();  // RTC init
        
        Serial.println(getLocalTime());
        Serial.println(getGrinichTime());
    }

    if(SERVICE_TEST){
        ServiceBegin();
        double testVar = 0.5;
        String s= getLocalTime();
        String p= String("07ec7356-9b13-48");
        //Serial.println(executeRequest(&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,GET_REQUEST,s,p));
    }

    if(SERVICE_TEST_IST){
        ServiceBegin();
        double testVar = 0.1;
        Serial.print(getLocalTime());
        String s= getGrinichTime();
        Serial.print(s);
        String p= String("bb3a14a0988311e78b760800273cbaca");
        //Serial.println(executeRequest(&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,POST_REQUEST,s,p));
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
        //Serial.println(executeRequest(&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,&testVar,JSON_POST_REQUEST,s,p));
    }
    
    if(NTP_TEST){
        DateTime now = ntpUpdate();
        char str_time[30];
        sprintf(str_time, "%04d-%02d-%02d %02d:%02d:%02d", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());     
        Serial.println(String(str_time));
    }

    if(RSSI_TEST){
      ServiceBegin();
      initLCD();
      int x=0;
      for(;;){
        x=readRSSI();
        Serial.println(x);
        showStrength(x);
        delay(1000);  
      }
    }

    if(SD_TEST){
      Serial.println(getFileNameTime());
      delay(3000);
      initSD();
      //writeFileSD("MEM_LOG/ISTSOS/",getFileNameTime(),"b269adf01ba411e8a93608002745029a;"+getGrinichTime()+",60.00,28.65,100.90,36.27,1023.00,0.00,94.07,19.53,0.00");
      //writeFileSD("MEM_LOG/SLPIOT/",getFileNameTime(),"{\"GUID\":\"5bf82c59-7ec0-4f\",\"dt\":\""+getLocalTime()+"\",\"H\":\"60.00\",\"TE\":\"28.65\",\"L\":\"7.00\",\"TI\":\"27.00\",\"WS\":\"19.53\",\"WD\":\"94.07\",\"RG\":\"0.00\",\"P\":\"100895.01\",\"SM\":\"1023.00\",\"WL\":\"0.00\",\"AT\":\"36.27\",\"BV\":\"12.53\"}");
      Serial.println(getAPN());
       Serial.println(getAPNUser());
        Serial.println(getAPNPass());
      sendLogData();
    }

    
    while(1);
}

