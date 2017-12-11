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

#ifndef Logger_h
#define Logger_h

#include <Arduino.h>
#include <LiquidCrystal.h>
#include "Settings.h"

#define LCD_DOUBLE_DECIMAL_PLACES 2
#define LCD_COLS 16
#define LCD_ROWS 2

class Logger{
private:
    void Logger();
    void LcdPrintDouble(double val,int i,int j);
    void LcdPrintCharArray(char *f,int i,int j);
    void LcdPrintString(String f,int i,int j);

}