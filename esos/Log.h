#ifndef LOG_H
#define LOG_H

#include <SD.h>
#include "Codes.h"
#include "Settings.h"
#include "Service.h"
#include <LiquidCrystal_I2C.h>
#include <Adafruit_SleepyDog.h>
#include <stdint.h>
#include <avr/pgmspace.h>

#define LCD_COLS 16
#define LCD_ROWS 2

#define SEND_ERROR 0
#define SEND_SUCCESS 1

//#define PRINT_RAM  // print free ram space on debug


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

void writeErrorLogData(String &text);
// LCD functions
extern LiquidCrystal_I2C lcd;

void initLCD();
void printLCDString(String f,uint8_t i,uint8_t j);
void printLCD(char *f);
void showStrength(uint8_t x);
void clearLCD();

// printers
void printString(String topLayer,String bottomLayer,char DefinitionCode=0);
void printSystemLog(String topLayer,String bottomLayer,char DefinitionCode =0);
void printValuesOnPanel(String name_index,double value,String unit);
void showBattryLowMark();



// sound indicate
void soundIndicator(uint8_t count1,uint8_t count2);

#endif
