#include "log.h"

void initSD(){

    if(SDOK==0){
        if (SD.begin(chipSelect)) 
            printErrorCode("SD_INIT_DONE",SD_INIT_DONE);
        else
            printErrorCode("SD_INIT_ERROR",SD_INIT_ERROR);
        SDOK=1;
    }
}


/*
    Print Values 

*/

void printValues(String name_index,double value){
    Serial.print(name_index);
    Serial.println(value);
    // lcd.clear();
    // printLCDN(name_index,0,0);
    // printLCD(value,0,1);
    // delay(1000);
}

void printValues(String name_index,String value){
    Serial.print(name_index);
    Serial.println(value);
    // lcd.clear();
    // printLCDN(name_index,0,0);
    // printLCDN(value,0,1);
    // delay(1000);
}

void printError(String text){
  Serial.println(text); 
//   lcd.clear();
//   printLCDN(f,0,0);
//   delay(1000);
}

void printStr(String text){
    Serial.print(text);
    // lcd.clear();
    // printLCDN(name_index,0,0);
}

void printErrorCode(String text, int DefinitionCode){
    printError(text);
    soundIndicator(DefinitionCode/10,DefinitionCode%10);
}

// sound soundIndicator
void soundIndicator(int count1,int count2){

    //long turn
    while(count1>0){
        tone(BUZZER,1000);
        delay(200);
        noTone(BUZZER);
        delay(10);
        count1--;
    }
    //long turn
    while(count2>0){
        tone(BUZZER,1000);
        delay(100);
        noTone(BUZZER);
        delay(10);
        count2--;
    }
    
  }

