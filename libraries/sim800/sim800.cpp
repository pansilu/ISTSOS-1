#include "sim800.h"

Sim800::Sim800(Stream &serial, const char* apn, const char* user, const char* pass, const char* basic, const char* pin)
{
    this->serialAT = &serial;

    this->apn = apn;
    this->user = user;
    this->pass = pass;
    this->pin = pin;
    this->basic = basic;
}

uint8_t Sim800::begin()
{
    pinMode(SIM_PIN_STATUS, INPUT);
    pinMode(SIM_POWER, OUTPUT);

    if (!this->restart()){
        return false;
    }
	
	if(this->pin != "")
    {
        return this->unlockSim();
    }

    return true;
}

bool Sim800::restart()
{
    Serial.println(getStatus());
    if(!this->sendCmd(F("AT+CFUN=0\r\n"), 10000UL))
    {
        Serial.println(F("AT+CFUN=0 fail"));
        return false;
    }
    if(!this->sendCmd(F("AT+CFUN=1,1\r\n"), 10000UL))
    {
        Serial.println(F("AT+CFUN=1,1 fail"));
        return false;
    }

    delay(3000);


}

bool Sim800::unlockSim()
{
    if(this->pin != "")
        return this->sendCmd("AT+CPIN=\"" + String(this->pin) + "\"\r\n");
    else
        return true;
}

bool Sim800::autoBaud(unsigned long timeout) {
    for (unsigned long start = millis(); millis() - start < timeout; ) {
      if (this->sendCmd(F("\r\n"), 200UL))
      {
          delay(100);
          return true;
      }
      delay(100);
    }
    return false;
  }


bool Sim800::getStatus()
{
    bool res = this->sendCmd(F("AT\r\n"), 5000UL, "OK");
    this->serialAT->flush();
    return res;
}

void Sim800::wakeUpShield()
{
    if (!this->getStatus())
    {
        digitalWrite(SIM_POWER, HIGH);
        delay(2000);
        digitalWrite(SIM_POWER, LOW);
        delay(1000);
        this->restart();

    }
}

void Sim800::sleepMode()
{
    if (this->getStatus())
    {
        digitalWrite(SIM_POWER, HIGH);
        delay(2000);
        digitalWrite(SIM_POWER, LOW);
        delay(1000);
    }
}

void Sim800::serialFlush(){
    while(this->serialAT->available() > 0) {
        char t = this->serialAT->read();
    }
}

uint8_t Sim800::connectGprs()
{

    this->wakeUpShield();

    if (!this->waitForNetwork(TIMEOUT))
    {
        this->sleepMode();
        return NETWORK_FAILURE;
    }
    if (!this->connectToNetwork())
    {
        this->sleepMode();
        return GPRS_FAILURE;
    }


    return 0;
}

bool Sim800::waitForNetwork(unsigned long timeout)
{
    for (unsigned long start = millis(); millis() - start < timeout;)
    {
        short s = this->getRegStatus();
        if (s == TMP_REG_OK_HOME || s == TMP_REG_OK_ROAMING){
            return true;
        }
        delay(1000);
    }
    return false;
}

uint8_t Sim800::getRegStatus()
{
    this->serialAT->print(F("AT+CREG?\r\n"));
    this->serialAT->flush();

    unsigned long tmp = millis();

    uint8_t status = 0;

    while ((millis() - tmp) < 20000UL) {
        // Print available data
        while (this->serialAT->available()) {
            this->serialAT->readStringUntil(',');
            status = this->serialAT->readStringUntil('\n').toInt();
            this->waitResponse();
            return status;
        }
    }
    return 0;
}

bool Sim800::connectToNetwork()
{
    if(!this->sendCmd(F("AT+CIPSHUT\r\n"), 60000UL))
        return false;

    delay(5000);

    this->sendCmd(F("AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n"));
    this->writeCmd(F("AT+SAPBR=3,1,\"APN\",\""), String(this->apn), F("\"\r\n"));
    this->serialAT->flush();
    this->waitResponse();

    if(user)
    {
        this->writeCmd(F("AT+SAPBR=3,1,\"USER\",\""), String(this->user), F("\"\r\n"));
        this->serialAT->flush();
        this->waitResponse();
        this->writeCmd(F("AT+SAPBR=3,1,\"PWD\",\""), String(this->pass), F("\"\r\n"));
        this->serialAT->flush();
        this->waitResponse();
    }

    this->writeCmd(F("AT+CGDCONT=1,\"IP\",\""), String(this->apn) + F("\"\r\n"));
    this->serialAT->flush();
    this->waitResponse();

    // wait with delay 60sec
    this->sendCmd(F("AT+CGACT=1,1\r\n"), 60000UL);

    // wait with delay 85sec
    this->sendCmd(F("AT+SAPBR=1,1\r\n"), 85000UL);

    // wait with delay 30sec
    if (!this->sendCmd(F("AT+SAPBR=2,1\r\n"), 30000UL))
        return false;

    if(!this->sendCmd(F("AT+CGATT=1\r\n"), 60000UL))
        return false;

    if (!this->sendCmd(F("AT+CIPMUX=1\r\n")))
        return false;

    if (!this->sendCmd(F("AT+CIPQSEND=1\r\n")))
        return false;

    if (!this->sendCmd(F("AT+CIPRXGET=1\r\n")))
        return false;

    // wait with delay 60sec

    this->writeCmd(F("AT+CSTT=\""), String(this->apn), F("\",\""), String(this->user), F("\",\""), String(this->pass), F("\"\r\n"));
    this->serialAT->flush();

    if (!this->waitResponse(60000UL))
        return false;

    // wait with delay 60sec
    if(!this->sendCmd(F("AT+CIICR\r\n"), 60000UL))
        return false;

    if(!this->sendCmd(F("AT+CIFSR;E0\r\n"), 10000UL))
        return false;

    if(!this->sendCmd(F("AT+CDNSCFG=\"208.67.222.222\",\"208.67.220.220\"\r\n")))
        return false;

    return true;
}

uint8_t Sim800::waitResponse(uint32_t timeout, const String expected)
{

    unsigned long start = millis();

    String response = String("");
    bool status = false;
    bool check = false;

    if (expected != "")
        check = true;

    do
    {
        while(this->serialAT->available() > 0)
        {
            char c = this->serialAT->read();
            response += c;
			if(DEBUG_COM_ANS)
				Serial.print(c);
            if(check && response.endsWith(expected))
            {
                status = true;
                goto finish;
            }
            if(response.endsWith("OK"))
            {
                status = true;
                goto finish;
            }
            else if(response.endsWith("ERROR"))
            {
                goto finish;
            }
        }
    }while( millis() - start < timeout);

  finish:

    this->serialFlush();

    return status;
}

uint8_t Sim800::executeGet(const char server[], const char uri[]){

	/*this->serialAT->flush();
    int tmp = this->connectGprs();
    if( tmp != 0){
        return tmp;
    }
	
	this->writeCmd(F("AT+CIPSTART=\"TCP\","), String(server), String(",80"), F("\"\r\n"));
	this->waitResponse();
	this->sendCmd(F("AT+CIPSEND\r\n"));
	
	this->sendCmd(F("GET "));
	this->sendCmd(uri);
	this->sendCmd(F(" HTTP/1.1\r\nHost: "));
	this->sendCmd(server);
	this->sendCmd(F("\r\nConnection: close\r\n\r\n"));

    if (DEBUG_COM)
        Serial.println(F("wait for response..."));

    bool response = this->getResponse();
    return response;*/
	
	this->serialAT->flush();
    int tmp = this->connectGprs();

    if( tmp != 0){
        return tmp;
    }

    this->sendCmd(F("AT+HTTPINIT\r\n"));
    this->sendCmd(F("AT+HTTPPARA=\"CID\",1\r\n"));
	this->writeCmd(F("AT+HTTPPARA=\"URL\",\""), String(uri),String("\"\r\n"));
	this->sendCmd(F("AT+HTTPACTION=0\r\n"));
	this->waitResponse(5000UL, "OK");
	this->sendCmd(F("AT+HTTPREAD\r\n"));
	uint8_t response = this->waitResponse(5000UL, "OK");
	this->sendCmd(F("AT+HTTPTERM\r\n"));
	this->waitResponse(5000UL, "OK");
    this->disconnect();

    return response;
	
}

uint8_t Sim800::executePost(const char server[], const char uri[], const String& data)
{
    this->serialAT->flush();
    int tmp = this->connectGprs();

    if( tmp != 0){
        return tmp;
    }

    this->sendCmd(F("AT+HTTPINIT\r\n"));
	
    if(USE_SSL)
        this->writeCmd(F("AT+HTTPPARA=\"URL\",\"https://"), String(server), String(uri), F("\"\r\n"));
    else
        this->writeCmd(F("AT+HTTPPARA=\"URL\",\"http://"), String(server), String(uri), F("\"\r\n"));
    this->serialAT->flush();
    this->waitResponse();
    this->sendCmd(F("AT+HTTPPARA=\"CID\",1\r\n"));
    this->sendCmd(F("AT+HTTPPARA=\"REDIR\",1\r\n"));
    this->sendCmd(F("AT+HTTPPARA=\"CONTENT\",\"text/plain;charset=utf-8\"\r\n"));
    if (USE_SSL)
    {
        this->writeCmd(F("AT+HTTPPARA=\"USERDATA\",\"Authorization: Basic "), String(this->basic), F("\"\r\n"));
        this->waitResponse();
        delay(500);
        this->sendCmd(F("AT+HTTPSSL=1\r\n"));
    }

    this->writeCmd(F("AT+HTTPDATA="), String(data.length()), F(",5000\r\n"));
    this->serialAT->flush();
    this->waitResponse(20000UL, "DOWNLOAD");

    this->sendCmd(data, 20000UL);

    if(!this->sendCmd(F("AT+HTTPACTION=1\r\n")))
        return 1;

    delay(10000);

    this->serialAT->print(F("AT+HTTPREAD\r\n"));
    this->serialAT->flush();

    uint8_t response = this->getResponse();

    this->sendCmd(F("AT+HTTPTERM\r\n"));

    this->disconnect();

    return response;
}

template<typename T>
uint8_t Sim800::sendCmd(T command, uint32_t timeout, const String expected)
{
    this->writeCmd(command);
    this->serialAT->flush();
    delay(0);
    return this->waitResponse(timeout, expected);
}

template<typename T, typename... Args>
void Sim800::writeCmd(T comm, Args... command)  // , uint32_t timeout, const String expected)
{
    this->writeCmd(comm);
    this->writeCmd(command...);
}

template<typename T>
void Sim800::writeCmd(T command)
{
    this->serialAT->print(command);
}

uint32_t* Sim800::ntpUpdate(const char ntpServer[], int GMT){

    this->connectGprs();

    this->sendCmd("AT+CNTP=" + String(ntpServer) + "," + String(GMT) + "\r\n");
    this->writeCmd(F("AT+CNTP="), String(ntpServer), F(","), String(GMT), F("\r\n"));
    this->serialAT->flush();
    this->waitResponse();
    this->sendCmd(F("AT+CNTP\r\n"));


    this->serialAT->print(F("AT+CCLK?\r\n"));
    delay(200);

    String response = "";

    bool flag = false;
    if (this->serialAT->available()>0){
        while (this->serialAT->available()>0){
            char c = this->serialAT->read();
            if (c == '"'){
                flag = true;
            }else if(flag){
                if (c == '"')
                    flag = false;
                else
                    response += c;
            }
        }
    }

    this->serialFlush();

    uint32_t* result = new uint32_t[8];


    Serial.println(response);

    String dateStr = getValue(response, ',', 0);
    String timeStr = getValue(response, ',', 1);

    // get date and time
    result[0] = (uint32_t)(2000 + getValue(dateStr, '/', 0).toInt());
    result[1] = (uint32_t)getValue(dateStr, '/', 1).toInt();
    result[2] = (uint32_t)getValue(dateStr, '/', 2).toInt();
    result[3] = (uint32_t)getValue(timeStr, ':', 0).toInt();
    result[4] = (uint32_t)getValue(timeStr, ':', 1).toInt();
    result[5] = (uint32_t)getValue(getValue(timeStr, ':',2), '+', 0).toInt();

    // get timezone
    bool positive = false;
    uint32_t tmp = 0;
    if (timeStr.indexOf('+') > 0){
        positive = true;
        tmp = (uint32_t) getValue(timeStr, '+', 1).toInt();
    }else{
        tmp = (uint32_t) getValue(timeStr, '-', 1).toInt();
    }

    uint32_t h = tmp / 4;
    uint32_t min = tmp % 4;

    if(min == 0)
        min = 0;
    else if(min == 1)
        min = 15;
    else if(min == 2)
        min = 30;
    else
        min = 45;

    result[6] = h;

    result[7] = min;
    result[8] = (uint32_t) positive;

    this->disconnect();


    return result;
}

bool Sim800::getResponse(){

    String response = String("");
    bool exit = false;

    unsigned long timeout = millis();
    while ((millis() - timeout) < 100000UL && !exit) {
        // Print available data
        while (this->serialAT->available()) {
            char c = this->serialAT->read();
            if(DEBUG_COM_ANS)
                Serial.print(c);
            response += c;
			
            if (c == '}')
                exit = true;

            timeout = millis();
        }
    }

    this->serialFlush();

    if (DEBUG_COM)
        Serial.println(response);

    if (response.indexOf(F("\"success\": true")) < 0){
        return REQUEST_FAILURE;
    }
    return REQUEST_SUCCESS;
}

void Sim800::disconnect(){
    this->sendCmd(F("AT+CIPSHUT\r\n"), 60000UL);
    this->sleepMode();
}
