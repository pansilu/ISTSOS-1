#include "log.h"

/*
    SD card Functions
*/

void initSD(){

    if(SDOK==0){
        if (SD.begin(chipSelect)) 
            printErrorCode(F("SD_INIT_DONE"),SD_INIT_DONE);
        else
            printErrorCode(F("SD_INIT_ERROR"),SD_INIT_ERROR);
        SDOK=1;
    }
}

// file extention
void createFileSD(String fileName)
{
  if (!SD.exists(fileName)) 
  {
    file = SD.open(fileName, FILE_WRITE);
    file.close();
 
    if (SD.exists(fileName)) 
        printErrorCode(F("SD_FILE_CREATION_ERROR"),SD_FILE_CREATION_ERROR);
  }
}

//Write the message on the Log
void writeFileSD(String fileName,String message)
{
    createFileSD(fileName);
    file = SD.open(fileName, FILE_WRITE);
    if (file) 
    {
        file.println(message);
        file.close();
    } 
    else 
    {
        Serial.println("SD_FILE_OPEN_ERROR" + fileName);
    }
}

/*
    LCD Functions
*/

void initLCD(){
    lcd.begin(LCD_COLS, LCD_ROWS);
}

void printLCDDouble(double val,int i,int j){ 
    lcd.setCursor(i,j);
    lcd.print(String(val,2));
}
  
void printLCDCharArray(char *f,int i,int j){
    lcd.setCursor(i,j);
    lcd.print(f);
}
  
void printLCDString(String f,int i,int j){
    lcd.setCursor(i,j);
    lcd.print(f);
}
  
void printLCD(char *f){
    lcd.clear();
    lcd.print(f);
}

/*
    Print Functions 
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
    Serial.println(text);
    writeFileSD("ErrorLog.txt",text);
    lcd.clear();
    printLCDN(name_index,0,0);
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

