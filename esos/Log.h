#ifndef LOG_H
#define LOG_H

#include <SD.h>
#include "Codes.h"
#include "Settings.h"

// saving log file
File file;
int SDOK=0;
const int chipSelect = 53;  // chip select pin for the SD module.it should be connected to 53 of module

#define LCD_COLS 16
#define LCD_ROWS 2

// SD initialization
void initSD();
void createFileSD(String fileName);
void writeFileSD(String fileName,String message);

// LCD begin
void initLCD();

// printers
void printValues(String name_index,double value);
void printValues(String name_index,String value);
void printError(String text);
void printStr(String text);

void printErrorCode(String text, int DefinitionCode);

// sound indicate
void soundIndicator(int count1,int count2);

#endif