#ifndef CODES_H
#define CODES_H

// SD
#define SD_INIT_DONE 0
#define SD_INIT_ERROR 12
#define SD_FILE_CREATION_ERROR 22 
#define SD_FILE_OPEN_ERROR 23

// RTC
#define RTC_INIT_DONE 0
#define RTC_NOT_CONNECTED 13
#define RTC_FAILED 14
#define RTC_UPDATED 33

// BME
#define BME_NOT_INIT 31
#define BME_I2C_ERROR 32

#define INIT_DONE 1

// Service
#define SERVICE_OK 1
#define SERVICE_ERROR 11

#define DATA_SEND_SUCCESSFULLY 2
#define DATA_SEND_ERROR 21

// Light 
#define LIGHT_I2C_ERROR 15

// DS18B20

#define DS18B20_ERROR 16

//sm
#define SM_ERROR 41

#endif
