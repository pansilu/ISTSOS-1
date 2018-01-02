#ifndef LOG_H
#define LOG_H

#include <SD.h>
#include "Codes.h"
#include "Settings.h"
#include <LiquidCrystal_I2C.h>

#define LCD_COLS 16
#define LCD_ROWS 2

// saving log file
extern File file;
extern int SDOK;
extern const int chipSelect;  // chip select pin for the SD module.it should be connected to 53 of module

// SD functions
void initSD();
void createFileSD(String fileName);
void writeFileSD(String fileName,String message);

// LCD functions
extern LiquidCrystal_I2C lcd;

void initLCD();
void printLCDDouble(double val,int i,int j);
void printLCDCharArray(char *f,int i,int j);  
void printLCDString(String f,int i,int j);
void printLCD(char *f);

// printers
void printValues(String name_index,double value);
void printValues(String name_index,String value);
void printError(String text);
void printStrOnDebug(String text);
void logData(String& text);
void printStr(String text);
void printStr(String text,String logTime,int DefinitionCode );
void printErrorCode(String text,String logTime,int DefinitionCode );
void printErrorCode(String text, int DefinitionCode);

// sound indicate
void soundIndicator(int count1,int count2);

#endif
