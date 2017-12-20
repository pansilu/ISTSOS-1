#ifndef LOG_H
#define LOG_H

#include <SD.h>
#include "Codes.h"
#include "Settings.h"
#include <LiquidCrystal.h>

#define LCD_COLS 16
#define LCD_ROWS 2

// saving log file
extern File file;
extern int SDOK=0;
extern const int chipSelect = 53;  // chip select pin for the SD module.it should be connected to 53 of module

// SD functions
void initSD();
void createFileSD(String fileName);
void writeFileSD(String fileName,String message);

// LCD functions
extern LiquidCrystal lcd(LCD_RS,LCD_EN,LCD_D4,LCD_D5,LCD_D6,LCD_D7);

void initLCD();
void printLCDDouble(double val,int i,int j);
void printLCDCharArray(char *f,int i,int j);  
void printLCDString(String f,int i,int j);
void printLCD(char *f);

// printers
void printValues(String name_index,double value);
void printValues(String name_index,String value);
void printError(String text);
void printStr(String text);

void printErrorCode(String text, int DefinitionCode);

// sound indicate
void soundIndicator(int count1,int count2);

#endif