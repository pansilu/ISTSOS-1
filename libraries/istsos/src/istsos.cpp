
#include "istsos.h"

Istsos::Istsos(ILog &sd, ICom &com, const char* server, const char* resource, const char* procedure)
{
    this->sd = &sd;
    this->com = &com;

    this->server = server;
    this->resource = resource;
    this->procedure = procedure;
}

void Istsos::begin()
{
    bool flag = this->sd->begin();
    if(flag)
        Serial.println(F("Sd ready..."));
    this->com->begin();
    Serial.println(F("Com ready..."));
    delay(2000);
}

bool Istsos::sendData()
{

    this->sd->cd("TMP");
    String files = this->sd->ls();

    int size = getArraySize(files, ',');

    if (DEBUG_ISTSOS)
        Serial.println("Files: " + files + " no: " + size);

    for(int i = 0; i < size; i++)
    {
        String fileName = getValue(files, ',', i);

        if (fileName == "\r")
            continue;

        if (DEBUG_ISTSOS)
        {
            Serial.print(F("Working on file: "));
            Serial.println(fileName);
        }

        bool flag = this->sendSingleFile(fileName);

        if(flag)
        {
            if (DEBUG_ISTSOS)
                Serial.println(F("Transmission ok, delete file"));
            this->sd->copyToLog(fileName);
            this->sd->removeFile(fileName);
            //TODO: this->sd->logSuccess();
        }else{
            break;
        }
    }

    this->sd->cd("..");
    return  true;
}

bool Istsos::sendDataTest()
{

    this->sd->cd("TMP");
    String files = this->sd->ls();

    bool flag = false;

    while(1)
    {
        String fileName = this->sd->getFirstFileName();

        if(fileName == String(""))
        {
            flag = true;
            break;
        }

        if (DEBUG_ISTSOS)
        {
            Serial.print(F("Working on file: "));
            Serial.println(fileName);
        }
        bool flag = this->sendSingleFile(fileName);

        if(flag)
        {
            if (DEBUG_ISTSOS)
                Serial.println(F("Transmission ok, delete file"));
            this->sd->copyToLog(fileName);
            this->sd->removeFile(fileName);
            flag = true;
            //TODO: this->sd->logSuccess();
        }else{
            flag = false;
            break;
        }
    }

    this->sd->cd("..");
    return  flag;
}

bool Istsos::sendSingleFile(const String& fileName)
{
    // TODO: only to test
    //return true;
    String message = this->procedure;

    message += ";" + sd->readFile(fileName);

    // format for istsos requests;
    message.replace("\r\n", "");
    message.replace("\n\r", "@");
    message.remove(message.length() - 1);


    if (DEBUG_ISTSOS)
    {
        Serial.print(F("Procedure id: "));
        Serial.println(this->procedure);
        Serial.println(F("Message to send: "));
        Serial.println(message);
    }

    int code = this->com->executePost(this->server, this->resource, message);
    if (code == 0)
    {
        return true;
    }

    Serial.print(F("Error code: "));
    Serial.println(code);

    return false;
}

void Istsos::logData(const String& message)
{

    this->sd->cd("TMP");

    String tmp = getValue(message, ',', 0);
    String date = getValue(tmp, 'T', 0);
    String time = getValue(tmp, 'T', 1);

    tmp.remove(0);

    String fileName = "L";
    fileName += getValue(date, '-', 0).substring(2);
    fileName += String(getValue(date, '-', 1).toInt(), HEX);
    fileName += getValue(date, '-', 2);
    fileName += getValue(time, ':', 0);
    fileName += ".TXT";

    if (DEBUG_ISTSOS)
    {
        Serial.print(F("Filename: "));
        Serial.println(fileName);
    }

    //this->sd->createFile(fileName);
    this->sd->writeLine(fileName, message);

    this->sd->cd("..");
}

void Istsos::logging(const String& date, const String& message)
{
    String tmpDate = getValue(date, 'T', 0);


    String fileName = "SLOG";
    fileName += getValue(tmpDate, '-', 0).substring(2);
    fileName += String(getValue(tmpDate, '-', 1).toInt(), HEX);
    //fileName += getValue(tmpDate, '-', 2);
    fileName += ".TXT";

    String logMessage = date + ": " + String(message);

    this->sd->cd("SLOG");

    this->sd->writeLine(fileName, logMessage);

    this->sd->cd("..");

}

void Istsos::tikLogging(const String& date)
{
    String tmpDate = getValue(date, 'T', 0);


    String fileName = "TL";
    fileName += getValue(tmpDate, '-', 0).substring(2);
    fileName += String(getValue(tmpDate, '-', 1).toInt(), HEX);
    fileName += getValue(tmpDate, '-', 2);
    fileName += ".TXT";

    // String logMessage = date + ": tick";

    this->sd->cd("TLOG");

    this->sd->writeLine(fileName, date);

    this->sd->cd("..");

}

uint32_t* Istsos::ntpUpdate(const char ntpServer[] = "", int GMT = 0)
{
    return this->com->ntpUpdate(ntpServer, GMT);
}

String Istsos::listFile()
{
    this->sd->cd("TMP");
    return this->sd->ls();
}
