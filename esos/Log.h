#ifndef LOG_H
#define LOG_H

#include <SD.h>
#include "Codes.h"
#include "Settings.h"
#include "Service.h"
#include <LiquidCrystal_I2C.h>
#include <Adafruit_SleepyDog.h>
#include <MemoryUsage.h>
#include <stdint.h>

#define LCD_COLS 16
#define LCD_ROWS 2

#define SEND_ERROR 0
#define SEND_SUCCESS 1

#define PRINT_RAM  // print free ram space on debug


extern uint8_t *__brkval;
extern uint8_t *__heap_start;

// saving log file
extern File file;
extern int SDOK;
extern const int chipSelect;  // chip select pin for the SD module.it should be connected to 53 of module

// SD functions
void initSD();
int get_freeRam();
void printFreeRam();

//basic
void writeFileSD(String filderpath,String fileName,String message);
uint8_t removeFile(String folderpath,String fileName);
String readFileSD(String folderpath,String fileName);

void sendLogData();
uint8_t sendRequstMessage(char server[],char uri[],String message,uint8_t auth);

void writeErrorLogData(String &text);
// LCD functions
extern LiquidCrystal_I2C lcd;

void initLCD();
void printLCDDouble(double val,int i,int j);
void printLCDCharArray(char *f,int i,int j);  
void printLCDString(String f,int i,int j);
void printLCD(char *f);
void showStrength(int x);
void clearLCD();

// printers
void printString(String topLayer,String bottomLayer);
void printString(String topLayer,String bottomLayer,int DefinitionCode);
void printSystemLog(String topLayer,String bottomLayer );
void printSystemLog(String topLayer,String bottomLayer,int DefinitionCode );
void printValuesOnPanel(String name_index,double value,String unit);
String getAPN();
String getAPNUser();
String getAPNPass();
void showBattryLowMark();



// sound indicate
void soundIndicator(int count1,int count2);

#endif
