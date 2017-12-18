

#include "Logger.h";

/*
 basic LCD functions 
*/

LiquidCrystal lcd(LCD_RS, LCD_Enable, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

void Logger::begin(Stream &com){
    this->com_port = &com;
    // LCD initialization
    lcd.begin(LCD_COLS, LCD_ROWS);
}

void Logger::begin(){
    
}


void Logger::LcdPrintDouble(double val,int i,int j){
    String s = String(val,LCD_DOUBLE_DECIMAL_PLACES);  
    lcd.setCursor(i,j);
    lcd.print(s);
}
  
void Logger::LcdPrintCharArray(char *f,int i,int j){
    lcd.setCursor(i,j);
    lcd.print(f);
}
  
void LcdPrintString(String f,int i,int j){
    lcd.setCursor(i,j);
    lcd.print(f);
}

// tone genarator
void Logger::soundIndicator(int higher_code,int lower_code){ // 1KHz 100ms out 1

    // long tones
    while(higher_code >0){
        tone(BUZZER,1000);
        delay(200);
        noTone(BUZZER);
        delay(10);
        higher_code--;
    }   
    // short tones
    while(lower_code >0){
        tone(BUZZER,1000);
        delay(200);
        noTone(BUZZER);
        delay(10);
        lower_code--;
    }  
}

void Logger::Indicate(int soundCode){
  this-> soundIndicator(soundCode/10 , soundCode%10);
}

/*
  SD card Log Functions
*/

void Logger::createFileSD(String fileName)
{
  if (!SD.exists(fileName)) 
  {
    logFile = SD.open(fileName, FILE_WRITE);
    logFile.close();
    if (!SD.exists(fileName)) 
    {
      Serial.println(SD_FILE_CREATE_FAILED);
      this->Indicate(SD_FILE_CREATE_FAILED_B);  
    }
  }
}
  
