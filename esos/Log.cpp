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
    Watchdog.enable(WATCHDOG_TIME_OUT);
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
    delay(100);
    SD.mkdir("DT_LOG");
    delay(100);
    SD.mkdir("SYS_LOG");
    delay(100);
    
    SD.mkdir("MEM_LOG/SLPIOT");
    delay(100);
    SD.mkdir("MEM_LOG/ISTSOS");
    delay(100);
    SD.mkdir("DT_LOG/SLPIOT");
    delay(100);
    SD.mkdir("DT_LOG/ISTSOS");
    delay(100);
    Watchdog.disable();
}

String getAPN(){
  String res = readFileSD("","config.txt");
  char c;
  String apn;
  int index = res.indexOf("APN:");
  for(int i=index+4;i<res.length();i++){
    c = res.charAt(i);
    apn.concat(c);
    if(c=='\n'){
      break;
    } 
  }
  return apn;
}

String getAPNUser(){
  String res = readFileSD("","config.txt");
  char c;
  String apn;
  int index = res.indexOf("USER:");
  for(int i=index+5;i<res.length();i++){
    c = res.charAt(i);
    apn.concat(c);
    if(c=='\n'){
      break;
    } 
  }
  return apn;
}

String getAPNPass(){
  String res = readFileSD("","config.txt");
  char c;
  String apn;
  int index = res.indexOf("PASS:");
  for(int i=index+5;i<res.length();i++){
    c = res.charAt(i);
    apn.concat(c);
    if(c=='\n'){
      break;
    } 
  }
  return apn;
}

void sendLogData(){

    // send ISTSOS
    #ifdef ISTSOS
    File dir =SD.open(F("MEM_LOG/ISTSOS"));
    while(1){
        File reader = dir.openNextFile();
        if(!reader)
          break;
        printString(F("ISTSOS RESENDING"),reader.name());
        String req = readFileSD(F("MEM_LOG/ISTSOS/"),reader.name());
        Serial.println(req);
        const char istserver[] = IST_SERVER;
        const char isturi[] = POSTREQ;
        if(sendRequstMessage(istserver,isturi,req,1)== SEND_SUCCESS){
          if(removeFile(F("MEM_LOG/ISTSOS/"),reader.name()))
            Serial.println(String(reader.name()) + " Removed");
          printString(SUCCESSFULL,reader.name());
          writeFileSD(F("DT_LOG/ISTSOS/"),getFileNameDate(),req);
          continue;
        }
    }
    #endif

    #ifdef SLPIOT
    dir =SD.open(F("MEM_LOG/SLPIOT"));
    while(1){
        File reader = dir.openNextFile();
        if(!reader)
          break;
        printString(F("SLPIOT RESENDING"),reader.name());
        String req = readFileSD(F("MEM_LOG/SLPIOT/"),reader.name());
        Serial.println(req);
        const char slpserver[] = SERVER;
        const char slpuri[] = REQ_STR;
        if(sendRequstMessage(slpserver,slpuri,req,0)== SEND_SUCCESS){
          if(removeFile(F("MEM_LOG/SLPIOT/"),reader.name()))
            Serial.println(String(reader.name()) + " Removed");
          printString(SUCCESSFULL,reader.name());
          writeFileSD(F("DT_LOG/SLPIOT/"),getFileNameDate(),req);
          continue;
        }
    }
    #endif
}

uint8_t sendRequstMessage(char server[],char uri[],String message,uint8_t auth){
  int count = ERROR_REPEATE_COUNT;
  printSystemLog(F("SENDING..."),String(server));
  while(count>0){
      int tmp = executePostRequest(server, uri, message,auth);
      // network failiur
      if(tmp == REQUEST_SUCCESS){
        printSystemLog(SUCCESSFULL,String(server),DATA_SEND_SUCCESSFULLY);
        return SEND_SUCCESS;
      }else if(tmp == NETWORK_FAILURE){
        printSystemLog(F("ERROR"),F("NETWORK_FAILURE"),DATA_SEND_ERROR);
        return SEND_ERROR;
      }else if(tmp == GPRS_FAILURE){
        printSystemLog(F("ERROR"),F("GPRS_FAILURE"),DATA_SEND_ERROR);
        return SEND_ERROR;
      }
      printSystemLog(F("RESENDING..."),String(server));
      count--;
  }
  printSystemLog(F("SEND ERROR"),String(server),DATA_SEND_ERROR);
  return SEND_ERROR;
}

/*
 * Basics
 */

uint8_t removeFile(String folderpath,String fileName){
  Watchdog.enable(WATCHDOG_TIME_OUT);
  fileName = folderpath + fileName;
  long last = millis();
  while((millis()-last)<10000UL){
    if(SD.remove(fileName))
      return 1;
  }
  Watchdog.disable();
  return 0;
}

String readFileSD (String folderpath,String filename){
  Watchdog.enable(WATCHDOG_TIME_OUT);
  filename = folderpath +filename;
  File file = SD.open(filename,FILE_READ);
  String strRead = String("");
  while(file.available()){
     strRead.concat((char)file.read());    
  }
  file.close();
  Watchdog.disable();
  return strRead; 
}

//Write the message on the Log
void writeFileSD(String folderpath,String fileName,String message)
{
    Watchdog.enable(WATCHDOG_TIME_OUT);
    fileName = folderpath + fileName;
    filef = SD.open(fileName, FILE_WRITE);
    delay(100);
    if (filef) 
    {
        filef.println(message);
        filef.close();
    } 
    else 
    {
        Serial.println("SD_FILE_OPEN_ERROR :" + fileName);
    }
    Watchdog.disable();
}


void printString(String topLayer,String bottomLayer){
    Serial.println(getLocalTimeHHMM()+" : "+topLayer + ":" + bottomLayer);
    lcd.clear();
    printLCDString(topLayer,0,0);
    printLCDString(bottomLayer,0,1);
    delay(1000);
}

void printString(String topLayer,String bottomLayer,int DefinitionCode){
    Serial.println(getLocalTimeHHMM()+" : "+topLayer + ":" + bottomLayer);
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
    writeFileSD("SYS_LOG/",getFileNameDate(),getLocalTimeHHMM()+" : "+ topLayer + " " + bottomLayer);
    delay(1000);
}

void printSystemLog(String topLayer,String bottomLayer,int DefinitionCode ){
    Serial.println(getLocalTimeHHMM()+" : "+ topLayer + " " + bottomLayer);
    lcd.clear();
    printLCDString(topLayer,0,0);
    printLCDString(bottomLayer,0,1);
    writeFileSD("SYS_LOG/",getFileNameDate(),getLocalTimeHHMM()+" : "+ topLayer + " " + bottomLayer);
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

    uint8_t signal_0[8]  = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1f};
    uint8_t signal_1[8]  = {0x0, 0x0, 0x0, 0x0, 0x0, 0x1f, 0x1f};
    uint8_t signal_2[8]  = {0x0, 0x0, 0x0, 0x1f, 0x1f, 0x1f, 0x1f};
    uint8_t signal_3[8]  = {0x0, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f};
    uint8_t signal_4[8]  = {0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f};

//    uint8_t up_dev_left_upper_corner[8]  = {0x1f, 0x1f, 0x10, 0x18, 0x1C, 0x1E, 0x1f};
//    uint8_t up_dev_left_lower_corner[8]  = {0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f};
//    uint8_t up_dev_left_lower_corner_1[8]  = {0x10, 0x10, 0x18, 0x18, 0x1C, 0x1E, 0x1f};
//    uint8_t upper_dev[8]  = {0x1f, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
//    uint8_t low_dev[8]  = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1f};
//    uint8_t up_dev_right_upper_corner[8]  = {0x1f, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
//    uint8_t up_dev_right_lower_corner[8]  = {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x1f};
//    uint8_t node_low[8]  = {0x0, 0x0, 0x0, 0x0, 0x1C, 0x1C, 0x1C};
//    uint8_t node_upper[8]  = {0x1C, 0x1C, 0x1C,0x0, 0x0, 0x0, 0x0};

void initLCD(){

   

    lcd.begin();
    lcd.backlight();

    lcd.createChar(0, signal_0);
    lcd.createChar(1, signal_1);
    lcd.createChar(2, signal_2);
    lcd.createChar(3, signal_3);
    lcd.createChar(4, signal_4);
    
    lcd.home();
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
//
//void showBattryLowMark(){
//  
//
//    lcd.createChar(5, up_dev_left_upper_corner);
//    lcd.createChar(6, up_dev_left_lower_corner);
//    lcd.createChar(7, upper_dev);
//    lcd.createChar(8, low_dev);
//    lcd.createChar(9, up_dev_right_upper_corner);
//    lcd.createChar(10, up_dev_right_lower_corner);
//    lcd.createChar(11, node_low);
//    lcd.createChar(12, node_upper);
//    lcd.createChar(13, up_dev_left_lower_corner_1);
//    
//  lcd.clear();
//  lcd.setCursor(2,0);
//  lcd.write(5);
//  lcd.write(7);
//  lcd.write(7);
//  lcd.write(7);
//  lcd.write(9);
//  lcd.write(11);
//  lcd.print(F(" BATTERY"));
//  lcd.setCursor(2,1);
//  lcd.write(6);
//  lcd.write(13);
//  lcd.write(8);
//  lcd.write(8);
//  lcd.write(10);
//  lcd.write(12);
//  lcd.print(F(" LOW!"));
//  delay(1000);
//  lcd.clear();
//  delay(1000);
//  lcd.createChar(0, signal_0);
//    lcd.createChar(1, signal_1);
//    lcd.createChar(2, signal_2);
//    lcd.createChar(3, signal_3);
//    lcd.createChar(4, signal_4);
//
//  
//}

// log requests in temporary

