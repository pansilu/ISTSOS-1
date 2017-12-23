#ifndef SETTINGS_H
#define SETTINGS_H

// definitins
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
#define TEMP_UP 40            // upeer temp for the fan
#define TEMP_DOWN 34          // lower temperature or fan
#define FAN_PIN 10            // fan pin
#define SERVER_SETUP 0         // if SERVER_SETUP==0 SLPIOT.org else SERVER_SETUP=1 for esos ServerSetup=2 for Both 
#define TIME_RATE 5           // set as sending after every Time (minutes)
#define RTC_UPDATE_INTERVAL 1 // the inteval et ween 2 RTC update from ntp (days)

#define WIN_SPEED_PIN 2       // wind speed pin
#define WIND_FACTOR 32.2   // 1024 --> 32.2ms-1   
#define WIND_VOLTAGE_MIN 10  // minimum voltage comes from wind speed sensor
#define WIND_VOLTAGE_MAX 1024  // maximum voltage comes from wind speed sensor        
// GPRS SETTINGS FOR ISTSOS

#define APN "dialog"
#define USERNAME ""
#define PASSWORD ""

#define SERVER "slpiot.org"

#define RF_TIMEOUT 5000

// LCD

#define LCD_RS 3
#define LCD_EN 8
#define LCD_D4 4
#define LCD_D5 5
#define LCD_D6 6
#define LCD_D7 7


// Sensor Enabling
#define EXT_TEMP_ENABLE 1
#define INT_TEMP_ENABLE 1
#define EXT_HUM_ENABLE 1
#define INT_HUM_ENABLE 1
#define SM_ENABLE 1
#define PRESSURE_ENABLE 1
#define ALTITUDE_ENABLE 1
#define LUX_ENABLE 1
#define WS_ENABLE 1
#define WD_ENABLE 1
#define RG_ENABLE 1
#define BT_ENABLE 1

#endif