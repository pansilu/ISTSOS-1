#include "log.h"

LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);
// saving log file
File filef;
int SDOK=0;
const int chipSelect = 53;  // chip select pin for the SD module.it should be connected to 53 of module


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
    filef = SD.open(fileName, FILE_WRITE);
    filef.close();
 
    if (SD.exists(fileName)) 
        printErrorCode(F("SD_FILE_CREATION_ERROR : "),SD_FILE_CREATION_ERROR);
  }
}

//Write the message on the Log
void writeFileSD(String fileName,String message)
{
    createFileSD(fileName);
    filef = SD.open(fileName, FILE_WRITE);
    if (filef) 
    {
        filef.println(message);
        filef.close();
    } 
    else 
    {
        Serial.println("SD_FILE_OPEN_ERROR :" + fileName);
    }
}

/*
    LCD Functions
*/

void initLCD(){
    lcd.begin();
    lcd.backlight();
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
    Serial.print(name_index + ":");
    Serial.println(value);
    lcd.clear();
    printLCDString(name_index,0,0);
    printLCDDouble(value,0,1);
    delay(1000);
}

void printValues(String name_index,String value){
    Serial.print(name_index);
    Serial.println(value);
    lcd.clear();
    printLCDString(name_index,0,0);
    printLCDString(value,0,1);
    delay(1000);
}

void printValues(String name_index,String tx,double value){
    Serial.print(name_index + ":");
    Serial.println(value);
    lcd.clear();
    printLCDString(name_index,0,0);
    printLCDDouble(value,7,0);
    printLCDString(tx.substring(2),0,1);
    delay(1000);
}

void printValues(String name_index,String tx,String value){
    Serial.print(name_index);
    Serial.println(value);
    lcd.clear();
    printLCDString(name_index,0,0);
    printLCDString(value,7,0);
    printLCDString(tx.substring(2),0,1);
    delay(1000);
}

void printError(String text){
    Serial.println(text);
    lcd.clear();
    printLCDString(text,0,0);
}

void printStrOnDebug(String text){
    Serial.print(text);
}

void printStr(String text){
    Serial.println(text);
    lcd.clear();
    printLCDString(text,0,0);
}

void logData(String &text){
    Serial.println(text);
    writeFileSD("DTLOG.txt",text);
}

void writeErrorLogData(String &text){
  writeFileSD(F("SE.txt"),text);
}

void printStr(String text,String logTime,int DefinitionCode ){
    Serial.println(text);
    logTime.concat(" | ");
    logTime.concat(text);
    writeFileSD("SYSLOG.txt",logTime);
    lcd.clear();
    printLCDString(text,0,0);
    soundIndicator(DefinitionCode/10,DefinitionCode%10);
}

void printErrorCode(String text, int DefinitionCode){
    Serial.println(text);
    writeFileSD("ERRLog.txt",text);
    lcd.clear();
    printLCDString(text,0,0);
    soundIndicator(DefinitionCode/10,DefinitionCode%10);
}

void printErrorCode(String text,String logTime,int DefinitionCode ){
    Serial.println(text);
    logTime.concat(" | ");
    logTime.concat(text);
    writeFileSD("ERRLog.txt",logTime);
    lcd.clear();
    printLCDString(text,0,0);
    soundIndicator(DefinitionCode/10,DefinitionCode%10);
}

// sound soundIndicator
void soundIndicator(int count1,int count2){

    //long turn
    while(count1>0){
        tone(BUZZER,1000);
        delay(200);
        noTone(BUZZER);
        delay(100);
        count1--;
    }
    //long turn
    while(count2>0){
        tone(BUZZER,1000);
        delay(100);
        noTone(BUZZER);
        delay(100);
        count2--;
    }
    delay(1000);
  }

