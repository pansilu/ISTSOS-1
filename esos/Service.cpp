#include "Service.h"

Sim800 simServer = Sim800(Serial2, APN, USERNAME, PASSWORD, BASIC_AUTH);;

const char slpserver[] = SERVER;
const char slpuri[] = REQ_STR;
const char istserver[] = IST_SERVER;
const char isturi[] = POSTREQ;

uint8_t temp = 0;

File dir;
File reader;

uint8_t log_send_error_count=LOG_SEND_ERROR_COUNT;

void ServiceBegin()
{
  
  printString("SERVICE CHECK",APN);
	uint8_t temp = simServer.begin();
	if (temp)
		printSystemLog(SUCCESSFULL,"SIM 800",SERVICE_OK);
	else
		printSystemLog(SUCCESS_ERROR,"SIM 800", SERVICE_ERROR);
}

void sendRequestString(double *externalHum,
											 double *externalTemp,
											 double *internalTemp,
											 double *light_intensity,
											 double *windSpeed,
											 double *windDirection,
											 double *rainFall,
											 double *pressure,
											 double *soilMoisture,
											 double *altitude,
											 double *battry,
											 int type,
											 String TimeStamp,
											 String Guid)
{
	String req;


	if (type == ISTSOS_REQUEST)
	{

   #ifdef SAVE_LOG_AND_SEND
    
    dir =SD.open(F("MEM_LOG/ISTSOS"));
    while(log_send_error_count>0){
        reader = dir.openNextFile();
        if(!reader)
          break;
        printString(F("ISTSOS RESENDING"),reader.name());
        req= readFileSD(F("MEM_LOG/ISTSOS/"),reader.name());
        Serial.println(req);
        if(sendRequstMessage(istserver,isturi,req,true)== SEND_SUCCESS){
          if(removeFile(F("MEM_LOG/ISTSOS/"),reader.name()))
            Serial.println(String(reader.name()) + " Removed");
          printString(SUCCESSFULL,reader.name());
          writeFileSD(F("DT_LOG/ISTSOS/"),getFileNameDate(),req);
          log_send_error_count=LOG_SEND_ERROR_COUNT;
          continue;
        }else{
          log_send_error_count--;  
        }
    }
      
    #endif

  
		req = String(Guid);
		req.concat(";");
		req += TimeStamp;
		if (EXT_HUM_ENABLE)
		{
			req.concat(",");
			req.concat(*externalHum);
		}
		if (EXT_TEMP_ENABLE)
		{
			req.concat(",");
			req.concat(*externalTemp);
		}
		if (PRESSURE_ENABLE)
		{
			req.concat(",");
			req.concat(*pressure / 1000);
		}
		if (ALTITUDE_ENABLE)
		{
			req.concat(",");
			req.concat(*altitude);
		}
		if (SM_ENABLE)
		{
			req.concat(",");
			req.concat(*soilMoisture);
		}
		if (WD_ENABLE)
		{
			req.concat(",");
			req.concat(*windDirection);
		}
		if (WS_ENABLE)
		{
			req.concat(",");
			req.concat(*windSpeed);
		}
		if (RG_ENABLE)
		{
			req.concat(",");
			req.concat(*rainFall);
		}
		if (LUX_ENABLE)
		{
			req.concat(",");
			req.concat(*light_intensity / 1000);
		}

    if(sendRequstMessage(istserver,isturi,req,true)== SEND_SUCCESS){
      writeFileSD(F("DT_LOG/ISTSOS/"),getFileNameDate(),req);
    }else{
      writeFileSD(F("MEM_LOG/ISTSOS/"),getFileNameTime() ,req);
    }
    
	}
	else if (type == SLPIOT_REQUEST)
	{
    #ifdef SAVE_LOG_AND_SEND
    
      dir =SD.open(F("MEM_LOG/SLPIOT"));
      while(log_send_error_count>0){
      reader = dir.openNextFile();
      if(!reader)
        break;
      printString(F("SLPIOT RESENDING"),reader.name());
      req = readFileSD(F("MEM_LOG/SLPIOT/"),reader.name());
      Serial.println(req);
      if(sendRequstMessage(slpserver,slpuri,req,false)== SEND_SUCCESS){
        if(removeFile(F("MEM_LOG/SLPIOT/"),reader.name()))
          Serial.println(String(reader.name()) + " Removed");
        printString(SUCCESSFULL,reader.name());
        writeFileSD(F("DT_LOG/SLPIOT/"),getFileNameDate(),req);
        log_send_error_count=LOG_SEND_ERROR_COUNT;
        continue;
      }else{
        log_send_error_count--;
      }
      }
    #endif
    
		req = String(F("{"));
		req.concat("\"GUID\":\"");
		req += Guid;
		req.concat("\",");
		req.concat("\"dt\":\"");
		req += TimeStamp;
		req.concat("\",");
		req.concat("\"H\":\"");
		req.concat(*externalHum);
		req.concat("\",");
		req.concat("\"TE\":\"");
		req.concat(*externalTemp);
		req.concat("\",");
		req.concat("\"L\":\"");
		req.concat(*light_intensity);
		req.concat("\",");
		req.concat("\"TI\":\"");
		req.concat(*internalTemp);
		req.concat("\",");
		req.concat("\"WS\":\"");
		req.concat(*windSpeed);
		req.concat("\",");
		req.concat("\"WD\":\"");
		req.concat(*windDirection);
		req.concat("\",");
		req.concat("\"RG\":\"");
		req.concat(*rainFall);
		req.concat("\",");
		req.concat("\"P\":\"");
		req.concat(*pressure);
		req.concat("\",");
		req.concat("\"SM\":\"");
		req.concat(*soilMoisture);
		req.concat("\",");
		req.concat("\"AT\":\"");
		req.concat(*altitude);
		req.concat("\",");
		req.concat("\"BV\":\"");
		req.concat(*battry);
		req.concat("\"}");

    if(sendRequstMessage(slpserver,slpuri,req,false)== SEND_SUCCESS){
      writeFileSD(F("DT_LOG/SLPIOT/"),getFileNameDate(),req);
    }else{
      writeFileSD(F("MEM_LOG/SLPIOT/"),getFileNameTime(),req);
    }
	}
	else
	{
		printSystemLog("ERROR","PARMS");
	}
}

uint8_t sendRequstMessage(char server_url[],char url[],String message,bool auth){
  
  uint8_t count = ERROR_REPEATE_COUNT;
  String server = String(server_url); 
  printSystemLog(F("SENDING..."),server);
  
  while(count>0){
      uint8_t tmp = auth==true? simServer.executePost(server_url, url, message):simServer.executePostPure(server_url, url, message);
      // network failiur
      if(tmp == REQUEST_SUCCESS){
        printSystemLog(SUCCESSFULL,server,DATA_SEND_SUCCESSFULLY);
        return SEND_SUCCESS;
      }else if(tmp == NETWORK_FAILURE){
        printSystemLog(F("ERROR"),F("NETWORK_FAILURE"),DATA_SEND_ERROR);
        return SEND_ERROR;
      }else if(tmp == GPRS_FAILURE){
        printSystemLog(F("ERROR"),F("GPRS_FAILURE"),DATA_SEND_ERROR);
        return SEND_ERROR;
      }else{
        printSystemLog(F("ERROR"),F("REMOTE ERROR"),DATA_SEND_ERROR);
      }
      printSystemLog(F("RESENDING..."),server);
      count--;
  }
  
  printSystemLog(F("SEND ERROR"),server,DATA_SEND_ERROR);
  return SEND_ERROR;
}

DateTime ntpUpdate()
{
	uint32_t *result = simServer.ntpUpdate("metasntp11.admin.ch", 0);
	uint8_t tmp = 5;
  long last= millis();
  if(result == 0)
    tmp=0;
	while (tmp > 0 && result[0] < 2018)
	{
		result = simServer.ntpUpdate("metasntp11.admin.ch", 0);
		delay(100);
    if((millis()-last) > 10000UL){
      tmp=0; 
      break;
    }
      
	}
	if (tmp == 0)
	{
		DateTime dt = DateTime(2017, 01, 01, 00, 00, 00);
		return dt;
	}

	DateTime dt = DateTime((uint16_t)result[0], (uint8_t)result[1], (uint8_t)result[2], (uint8_t)result[3], (uint8_t)result[4], (uint8_t)result[5]);
	dt = dt + TimeSpan(0, 5, 30, 0);
	return dt;
}

int readRSSI()
{
	return simServer.readRSSI();
}
