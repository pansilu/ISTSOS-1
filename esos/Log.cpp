#include "log.h"

void initSD(){

    if(SDOK==0){
    if (!SD.begin(chipSelect)) 
    {
      printError("SD Error ... !");
      soundIndicator(2,0);
      setup();
    }
    else
    {
      SDOK=1;
      Serial.println("SD Initializes.");
    }  
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


