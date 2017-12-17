/*
    This is a library basic logging functionalities of the system
    such as
        LCD print,
        Serial Print,
        Beep Indicator
        Errorindicator
        LOG file writer
    Written by Kolitha Warnakulasooriya. 2017.

*/

#include "Settings.h"

#define LCD_DOUBLE_DECIMAL_PLACES 2
#define LCD_COLS 16
#define LCD_ROWS 2

File logFile;
const int SDOK=0;
const int chipSelect = 53;

class Logger{
  
private:
    Logger();
    void LcdPrintDouble(double val,int i,int j);
    void LcdPrintCharArray(char *f,int i,int j);
    void LcdPrintString(String f,int i,int j);
    void soundIndicator(int higher_code,int lower_code);
    void createFileSD(String fileName);

public:
    void Indicate(int soundCode);
    
};
