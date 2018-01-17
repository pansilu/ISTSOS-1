#ifndef SETTINGS_H
#define SETTINGS_H

/*
	GPRS Settings 
*/

// For Communication 

#define APN "dialog"
#define USERNAME ""
#define PASSWORD ""

// For SLPIOT Settings

#define SERVER "slpiot.org"
#define GUID_SLPIOT "e813b67c-3c7a-40"

// For ISTSOS Settings

#define POSTREQ "/istsos/wa/istsos/services/sl/operations/fastinsert"
#define PROCEDURE "a0303bb4f9e311e7953f0800273cbaca"

// for Enable Servers

#define ENABLE_ISTSOS 1
#define ENABLE_SLPIOT 1

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// definitins
#define EXTERNAL_TEMP_PIN 8  // External temperature pin
#define DHT11_IN_PIN 4       // internal temperature
#define BUZZER 12             // buzzer pin
#define SM_PIN A8              //  for SM sensor
#define BMP085_ADDRESS 0x77   // bmp sensor Address  
#define BATT A0                // get battry meter value
#define WIN_DIRECTION_PIN A2   // wind directionPin
#define WIND_DIRECTION_VOLTAGE_MIN 1.04  // minimum voltage comes from wind speed sensor
#define WIND_DIRECTION_VOLTAGE_MAX 5.02  // maximum voltage comes from wind speed sensor
// rain gauge
#define RAIN_GAUGE_PIN 2
#define RAIN_GAUGE_INT 0
#define RAIN_FACTOR 0.2       // rain factor for one tip bucket
#define TEMP_UP 37            // upeer temp for the fan
#define TEMP_DOWN 34          // lower temperature or fan
#define FAN_PIN 10            // fan pin
#define TIME_RATE 5           // set as sending after every Time (minutes)
#define RTC_UPDATE_TIME_RATE 604800UL
#define RTC_UPDATE_INTERVAL 1 // the inteval et ween 2 RTC update from ntp (days)
#define RTC_UPDATE_BY_NTP 1
#define ERROR_REPEATE_COUNT 3

#define WIN_SPEED_PIN A1       // wind speed pin
#define WIND_FACTOR 32.2   // 1024 --> 32.2ms-1   
#define WIND_VOLTAGE_MIN 10  // minimum voltage comes from wind speed sensor
#define WIND_VOLTAGE_MAX 1024  // maximum voltage comes from wind speed sensor        

#define GET_REQUEST 10
#define POST_REQUEST 20
#define JSON_POST_REQUEST 30

#define RF_TIMEOUT 5000

// LCD
#define LCD_ADDRESS 0x27


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
