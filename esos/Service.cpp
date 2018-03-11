#include "Service.h"

const String server_url = String(SERVER);
Sim800 simServer = Sim800(Serial2, APN, USERNAME, PASSWORD, BASIC_AUTH);

const char istserver[] = IST_SERVER;
const char isturi[] = POSTREQ;
uint8_t temp = 0;

void ServiceBegin()
{
  printString("SERVICE CHECK",APN);
	uint8_t temp = simServer.begin();
	if (temp)
		printSystemLog(SUCCESSFULL,APN,SERVICE_OK);
	else
		printErrorCode(SUCCESS_ERROR,APN, SERVICE_ERROR);
}

uint8_t executeRequest(double *externalHum,
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
											 String &TimeStamp,
											 String &Guid)
{
	String req;
	if (type == POST_REQUEST)
	{
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
		logData(req);

		char charBuf[req.length()];
		req.toCharArray(charBuf, req.length());

		temp = simServer.executePost(istserver, isturi, req);
		if (temp)
			printStr(F("DATA_SEND_SUCCESSFULLY:ISTSOS"), getLocalTime(), DATA_SEND_SUCCESSFULLY);
		else
		{
			printErrorCode(F("ISTSOS:RESENDING..."), getLocalTime(), DATA_SEND_ERROR);
			writeErrorLogData(req);
		}
	}
	else if (type == JSON_POST_REQUEST)
	{

		String req = String(F("{"));
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
		req.concat("\"WL\":\"");
		req.concat(0);
		req.concat("\",");
		req.concat("\"AT\":\"");
		req.concat(*altitude);
		req.concat("\",");
		req.concat("\"BV\":\"");
		req.concat(*battry);
		req.concat("\"}");
		logData(req);

		char charBuf[req.length()];
		req.toCharArray(charBuf, req.length());

		temp = executePostRequest("www.slpiot.org", "/insert_data4.php", req);
		if (temp)
			printStr(F("DATA_SEND_SUCCESSFULLY:SLPIOT"), getLocalTime(), DATA_SEND_SUCCESSFULLY);
		else
		{
			printErrorCode(F("SLPIOT:RESENDING..."), getLocalTime(), DATA_SEND_ERROR);
			writeErrorLogData(req);
		}
	}
	else
	{

		printErrorCode(F("SENDING_PARAMETERS_ARE_NOT_DEFINED"), getLocalTime(), DATA_SEND_ERROR);
		return 1;
	}
	return temp;
}

uint8_t executePostRequest(char server[], char url[], String &data, uint8_t auth)
{
  if(auth)
    return simServer.executePost(server, url, data);
	return simServer.executePostPure(server, url, data);
}

DateTime ntpUpdate()
{
	uint32_t *result = simServer.ntpUpdate("metasntp11.admin.ch", 0);
	uint8_t tmp = 5;
  long last= millis();
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
