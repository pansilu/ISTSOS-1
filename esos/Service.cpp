#include "Service.h"

Sim800 server = Sim800(Serial1, APN, USERNAME, PASSWORD);

void ServiceBegin(){
    uint8 t= server.begin();
    if(t)
        printStr("SERVICE_OK",getLocalTime(),SERVICE_OK);
    else
        printErrorCode("SERVICE_ERROR",getLocalTime(),SERVICE_ERROR);
}