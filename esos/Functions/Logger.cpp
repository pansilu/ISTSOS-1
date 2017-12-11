

#include "Logger.h";

/*
 basic LCD functions 
*/

// constructor

LiquidCrystal lcd(LCD_RS, LCD_Enable, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

void Logger::Logger(){
    // LCD initialization
    lcd.begin(LCD_COLS, LCD_ROWS);
    
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
  
