// includes
#include <DallasTemperature.h>
#include <OneWire.h>
#include <dht.h>
#include <BH1750.h> 
#include <RTClib.h>
#include "SD.h"
#include <istsos.h>
#include <com/sim800.h>
#include <LiquidCrystal.h>
#include <Seeed_BME280.h>
#include <Wire.h>
#include "Codes.h"
#include "Functions/Logger.h"

//LCD
#define LCD_RS 3
#define LCD_Enable 8
#define LCD_D4 4
#define LCD_D5 5
#define LCD_D6 6
#define LCD_D7 7

#define EXTERNAL_TEMP_PIN 11  // External temperature pin
#define DHT11_IN_PIN 13       // internal temperature
#define BUZZER 12             // buzzer pin
#define SM_PIN 8              //  for SM sensor
#define BMP085_ADDRESS 0x77   // bmp sensor Address  
#define BATT 0                // get battry meter value
#define WIN_DIRECTION_PIN 1   // wind directionPin
#define WIND_DIRECTION_VOLTAGE_MIN 1.96  // minimum voltage comes from wind speed sensor
#define WIND_DIRECTION_VOLTAGE_MAX 3.13  // maximum voltage comes from wind speed sensor
// rain gauge
#define RAIN_GAUGE_PIN 2
#define RAIN_GAUGE_INT 0
#define RAIN_FACTOR 0.2       // rain factor for one tip bucket
#define POWER_UP_GSM 9        // powerup pin
#define TEMP_UP 38            // upeer temp for the fan
#define TEMP_DOWN 36          // lower temperature or fan
#define FAN_PIN 10            // fan pin
#define SERVER_SETUP 0        // if SERVER_SETUP==0 SLPIOT.org else SERVER_SETUP=1 esos
#define TIME_RATE 2           // set as sending after every Time rate=15minute

#define WIN_SPEED_PIN 2       // wind speed pin
#define WIND_FACTOR 32.2      // 1024 --> 32.2ms-1   
#define WIND_VOLTAGE_MIN 10   // minimum voltage comes from wind speed sensor
#define WIND_VOLTAGE_MAX 1024 // maximum voltage comes from wind speed sensor        
// GPRS SETTINGS FOR ISTSOS

#define APN "mobitel"
#define USERNAME ""
#define PASSWORD ""
#define PROCEDURE "bb3a14a0988311e78b760800273cbaca"
#define POSTREQ "/istsos/wa/istsos/services/sl/operations/fastinsert"

#define RF_TIMEOUT 5000
