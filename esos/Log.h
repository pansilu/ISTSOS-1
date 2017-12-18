#ifndef LOG_H
#define LOG_H

#include <SD.h>

// saving log file
File myFile;
int SDOK=0;
const int chipSelect = 53;  // chip select pin for the SD module.it should be connected to 53 of module

// SD initialization
void initSD();
#endif