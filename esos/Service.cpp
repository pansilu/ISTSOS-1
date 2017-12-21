#include "Service.h"

const String server_url=String(SERVER);
Sim800 simServer = Sim800(Serial1, APN, USERNAME, PASSWORD);

void ServiceBegin(){
    uint8_t temp= simServer.begin();
    if(temp)
        printStr("SERVICE_OK",getLocalTime(),SERVICE_OK);
    else
        printErrorCode("SERVICE_ERROR",getLocalTime(),SERVICE_ERROR);
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
                    double *waterlevel,
                    double *altitude,
                    double *battry,
                    String &TimeStamp,
                    String &Guid)
{
            String req = String(F("http://slpiot.org/insert_data.php?"));
            req.concat("H="); req.concat(*externalHum);
            req.concat("&TE=" ); req.concat(* externalTemp);
            req.concat("&L=" ); req.concat(*light_intensity);
            req.concat("&TI=" ); req.concat(*internalTemp);
            req.concat("&WS=" ); req.concat(*windSpeed);
            req.concat("&WS=" ); req.concat(*windDirection);
            req.concat("&RG=" ); req.concat(*rainFall);
            req.concat("&SM=" ); req.concat(*soilMoisture);
            req.concat("&WL=" ); req.concat(*waterlevel);
            req.concat("&AT=" ); req.concat(*altitude);
            req.concat("&BV=" ); req.concat(*battry);
            req.concat("&dt=" ); req += TimeStamp;
            req.concat("&GUID=" ); req += Guid;

            logData(req);

    char charBuf[req.length()];
    req.toCharArray(charBuf,req.length());

   uint8_t temp = simServer.executeGet(SERVER,charBuf);
   if(temp)
    printStr("DATA_SEND_SUCCESSFULLY",getLocalTime(),DATA_SEND_SUCCESSFULLY);
   else
    printErrorCode("DATA_SEND_ERROR",getLocalTime(),DATA_SEND_ERROR);

   return temp;
}

