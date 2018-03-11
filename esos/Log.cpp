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
    printString(F("INITIALIZING"),F("MEMORY"));
    if(SDOK==0){
        if (SD.begin(chipSelect)) 
            printString(F(SUCCESSFULL),F("MEMORY"),SD_INIT_DONE); 
        else
            printString(F(SUCCESS_ERROR),F("MEMORY"),SD_INIT_ERROR);
        SDOK=1;
    }
    
    // create Folders
    SD.mkdir("MEM_LOG");
    SD.mkdir("DT_LOG");
    SD.mkdir("SYS_LOG");

    SD.mkdir("MEM_LOG/SLPIOT");
    SD.mkdir("MEM_LOG/ISTSOS");
}

void sendLogData(){

    // send ISTSOS
    #ifdef ISTSOS
    File dir =SD.open("MEM_LOG/ISTSOS");
    while(1){
        File reader = dir.openNextFile();
        if(!reader)
          break;
        Serial.println(reader.name());
        String req = readFileSD("MEM_LOG/ISTSOS/",reader.name());
        Serial.println(req);
        const char istserver[] = IST_SERVER;
        const char isturi[] = POSTREQ;
        if(sendRequstMessage(istserver,isturi,req,1)== SEND_SUCCESS){
          if(removeFile("MEM_LOG/ISTSOS/",reader.name()))
            Serial.println(String(reader.name()) + " Removed");
           continue;
        }
    }
    #endif

    #ifdef SLPIOT
    dir =SD.open("MEM_LOG/SLPIOT");
    while(1){
        File reader = dir.openNextFile();
        if(!reader)
          break;
        Serial.println(reader.name());
        String req = readFileSD("MEM_LOG/SLPIOT/",reader.name());
        Serial.println(req);
        const char slpserver[] = SERVER;
        const char slpuri[] = REQ_STR;
        if(sendRequstMessage(slpserver,slpuri,req,0)== SEND_SUCCESS){
          if(removeFile("MEM_LOG/SLPIOT/",reader.name()))
            Serial.println(String(reader.name()) + " Removed");
           continue;
        }
    }
    #endif
}

uint8_t sendRequstMessage(char server[],char uri[],String message,uint8_t auth){
  int count = ERROR_REPEATE_COUNT;
  printString("SENDING...",String(server));
  while(count>0){
      if(executePostRequest(server, uri, message,auth)){
        printSystemLog(SUCCESSFULL,String(server),DATA_SEND_SUCCESSFULLY);
        return SEND_SUCCESS;
      }
      printString("RESENDING...",String(server));
      count--;
  }
  printSystemLog("SEND ERROR",String(server),DATA_SEND_ERROR);
  return SEND_ERROR;
}

uint8_t removeFile(String folderpath,String fileName){
  fileName = folderpath + fileName;
  long last = millis();
  while((millis()-last)<10000UL){
    if(SD.remove(fileName))
      return 1;
  }
  return 0;
}

String readFileSD (String folderpath,String filename){
  filename = folderpath +filename;
  File file = SD.open(filename,FILE_READ);
  String strRead = String("");
  while(file.available()){
     strRead.concat((char)file.read());    
  }
  file.close();
  return strRead; 
}

//Write the message on the Log
void writeFileSD(String folderpath,String fileName,String message)
{
    fileName = folderpath + fileName;
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


void printString(String topLayer,String bottomLayer){
    Serial.println(topLayer + "\n" + bottomLayer);
    lcd.clear();
    printLCDString(topLayer,0,0);
    printLCDString(bottomLayer,0,1);
    delay(1000);
}

void printString(String topLayer,String bottomLayer,int DefinitionCode){
    Serial.println(topLayer + "\n" + bottomLayer);
    lcd.clear();
    printLCDString(topLayer,0,0);
    printLCDString(bottomLayer,0,1);
    delay(1000);
    soundIndicator(DefinitionCode/10,DefinitionCode%10);
}

void printSystemLog(String topLayer,String bottomLayer ){
    Serial.println(getLocalTimeHHMM()+" : "+ topLayer + " " + bottomLayer);
    lcd.clear();
    printLCDString(topLayer,0,0);
    printLCDString(bottomLayer,0,1);
    writeFileSD("SYS_LOG/",getFileNameDate(),topLayer);
    delay(1000);
}

void printSystemLog(String topLayer,String bottomLayer,int DefinitionCode ){
    Serial.println(getLocalTimeHHMM()+" : "+ topLayer + " " + bottomLayer);
    lcd.clear();
    printLCDString(topLayer,0,0);
    printLCDString(bottomLayer,0,1);
    writeFileSD("SYS_LOG/",getFileNameDate(),topLayer);
    delay(1000);
    soundIndicator(DefinitionCode/10,DefinitionCode%10);
}

void printValuesOnPanel(String name_index,double value,String unit){
    lcd.clear();
    String topLayer = name_index+" "+ String(value).substring(0,5) +" "+unit;
    printLCDString(topLayer,0,0);
    showStrength(readRSSI());
    printLCDString(getLocalTimeHHMM().substring(2),0,1);
    delay(1000);
}

/*
    LCD Functions
*/

void initLCD(){

    uint8_t signal_0[8]  = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1f};
    uint8_t signal_1[8]  = {0x0, 0x0, 0x0, 0x0, 0x0, 0x1f, 0x1f};
    uint8_t signal_2[8]  = {0x0, 0x0, 0x0, 0x1f, 0x1f, 0x1f, 0x1f};
    uint8_t signal_3[8]  = {0x0, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f};
    uint8_t signal_4[8]  = {0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f};

    lcd.begin();
    lcd.backlight();

    lcd.createChar(0, signal_0);
    lcd.createChar(1, signal_1);
    lcd.createChar(2, signal_2);
    lcd.createChar(3, signal_3);
    lcd.createChar(4, signal_4);
    lcd.home();
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

void clearLCD(){
  lcd.clear();  
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
    printLCDString("           ",0,0);
    printLCDString("           ",1,0);
    printLCDString(name_index,0,0);
    printLCDDouble(value,5,0);
    printLCDString(tx.substring(2),0,1);
    
    delay(1000);
}

void printValues(String name_index,String tx,String value){
    Serial.print(name_index);
    Serial.println(value);
    printLCDString("           ",0,0);
    printLCDString("           ",1,0);
    printLCDString(value,5,0);
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
}

void writeErrorLogData(String &text){
  
}

void printStr(String text,String logTime,int DefinitionCode ){
    Serial.println(text);
    logTime.concat(" | ");
    logTime.concat(text);
    //writeFileSD("SYSLOG.txt",logTime);
    lcd.clear();
    printLCDString(text,0,0);
    soundIndicator(DefinitionCode/10,DefinitionCode%10);
}

void printErrorCode(String text, int DefinitionCode){
    Serial.println(text);
    //writeFileSD("ERRLog.txt",text);
    lcd.clear();
    printLCDString(text,0,0);
    soundIndicator(DefinitionCode/10,DefinitionCode%10);
}

void printErrorCode(String text,String logTime,int DefinitionCode ){
    Serial.println(text);
    logTime.concat(" | ");
    logTime.concat(text);
    //writeFileSD("ERRLog.txt",logTime);
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


void showStrength(int x){
  lcd.setCursor(12,0);
    lcd.write(0);
    lcd.write(0);
    lcd.write(0);
    lcd.write(0);
  lcd.setCursor(12,0);
  if(x< 10 && x>= 2)
    lcd.write(1);
  else if(x< 15 && x>= 10){
    lcd.write(1);
    lcd.write(2);
  }
  else if(x< 20 && x>= 15){
    lcd.write(1);
    lcd.write(2);
    lcd.write(3);
  }
  else if( x>= 20){
    lcd.write(1);
    lcd.write(2);
    lcd.write(3);
    lcd.write(4);
  }else
    lcd.write(0);
}

// log requests in temporary

