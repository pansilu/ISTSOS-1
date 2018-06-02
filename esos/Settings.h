#ifndef SETTINGS_H
#define SETTINGS_H

/*
	GPRS Settings 
*/

// For Communication 

#define APN "mobitel"
#define USERNAME ""
#define PASSWORD ""
#define BASIC_AUTH "YWRtaW46QlYzWGp2clA="

// For SLPIOT Settings

#define SERVER "slpiot.org"
#define REQ_STR "/api/insertData/"
#define GUID_SLPIOT "5bf82c59-7ec0-4f"

// For ISTSOS Settings

#define IST_SERVER "geoservice.ist.supsi.ch"
#define POSTREQ "/4onse/wa/istsos/services/lka/operations/fastinsert"
#define PROCEDURE "492e6c460ca311e8a93608002745029a"

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
#define RAIN_FACTOR 0.5       // rain factor for one tip bucket
#define TEMP_UP 37            // upeer temp for the fan
#define TEMP_DOWN 34          // lower temperature or fan
#define FAN_PIN 10            // fan pin
#define TIME_RATE 10           // set as sending after every Time (minutes)
#define ERROR_REPEATE_COUNT 1 // how much times shoulw try to send
#define LOG_SEND_ERROR_COUNT 3
#define NTP_UPDATE 604800UL   // NTP update 7 days

#define WIN_SPEED_PIN A1       // wind speed pin
#define WIND_FACTOR 32.2   // 1024 --> 32.2ms-1   
#define WIND_VOLTAGE_MIN 10  // minimum voltage comes from wind speed sensor
#define WIND_VOLTAGE_MAX 1023  // maximum voltage comes from wind speed sensor        

#define ISTSOS_REQUEST 1
#define SLPIOT_REQUEST 2

#define RF_TIMEOUT 5000
#define WATCHDOG_TIME_OUT 8000

// message
#define SUCCESSFULL "SUCCESSFULL"
#define SUCCESS_ERROR "ERROR"

#define REQUEST_SUCCESS 1
#define REQUEST_FAILURE 0
#define NETWORK_FAILURE 3
#define GPRS_FAILURE 4

// LCD
#define LCD_ADDRESS 0x27

//*****************************************************************************

// for Enable Servers

#define ISTSOS 
#define SLPIOT 
#define NTP

//#define SAVE_LOG_AND_SEND

// Sensor Enabling
#define EXT_TEMP_ENABLE 1
#define INT_TEMP_ENABLE 1
#define EXT_HUM_ENABLE 1
#define INT_HUM_ENABLE 1
#define SM_ENABLE 0
#define PRESSURE_ENABLE 1
#define ALTITUDE_ENABLE 1
#define LUX_ENABLE 1
#define WS_ENABLE 1
#define WD_ENABLE 1
#define RG_ENABLE 0
#define BT_ENABLE 1



#endif
