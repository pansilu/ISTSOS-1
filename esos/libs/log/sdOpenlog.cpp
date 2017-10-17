#include "sdOpenlog.h"

OpenLog::OpenLog(Stream &serial)
{
    this->openLog = &serial;
};

bool OpenLog::begin(){
    pinMode(RESET_OPENLOG, OUTPUT);
    delay(200);

    //Reset OpenLog
    digitalWrite(RESET_OPENLOG, LOW);
    delay(500);
    digitalWrite(RESET_OPENLOG, HIGH);

    //Wait for OpenLog to respond with '<' to indicate it is alive and recording to a file
    return this->waitToCommand();
};

void OpenLog::gotoCommandLine(){

    if (this->command)
        return;

    this->openLog->write(36);
    this->openLog->write(36);
    this->openLog->write(36);

    this->waitToCommand();
};

void OpenLog::reset()
{
};

void OpenLog::init()
{
    //this->sendCommand("reset");
    this->sendCommandTest(F("reset"));
    this->waitToCommand();
}

bool OpenLog::createFile(const String& fileName)
{
    // check if file already exists
    int size = this->getFileSize(fileName);
    if (size == -1)
        return true;

    this->sendCommandTest(F("new "), fileName);
    this->waitToCommand();
    delay(5);
    return true;
};



String OpenLog::readFile(const String& fileName)
{

    if(DEBUG_SD)
    {
        Serial.print(F("Read file: "));
        Serial.println(fileName);
    }

    this->sendCommandTest(F("read "), fileName);

    while(1)
    {
        if(this->openLog->available())
            if(this->openLog->read() == '\n') break;
    }
    String message = "";

    for(int timeOut = 0 ; timeOut < 1000 ; timeOut++)
    {
        while(this->openLog->available())
        {
            while(this->openLog->available())
            {
                char c = this->openLog->read();
                if (c != '>')
                    message += c;
            }
            timeOut = 0;
        }
        delay(1);
    }

    return message;
};

template<typename... Args>
void OpenLog::sendCommandTest(Args... commands)
{
    this->gotoCommandLine();
    this->writeCommand(commands...);
    this->openLog->println();
}

template<typename T, typename... Args>
void OpenLog::writeCommand(T com, Args... command)
{
    this->openLog->print(com);
    this->writeCommand(command...);
}

template<typename T>
void OpenLog::writeCommand(T com)
{
    this->openLog->print(com);
}


void OpenLog::clearBuffer()
{
    while(this->openLog->available() > 0) {
        this->openLog->read();
        delay(1);
    }
}

int OpenLog::getFileSize(const String& fileName)
{
    this->sendCommandTest(F("size "), fileName);

    int no = 0;
    String bytes = "0";
    for(int timeOut = 0 ; timeOut < 1000 ; timeOut++)
    {
        while(this->openLog->available())
        {
            while(this->openLog->available())
            {
                char c = this->openLog->read();
                if (c == '\n'){
                    no++;
                }
                else if (no >= 1 && isdigit(c)){
                    bytes += c;
                }else if (c == '>'){
                    this->command = true;
                    break;
                }
            }
            timeOut = 0;
        }
        delay(1);
    }

    if (DEBUG_SD)
        Serial.println("string: " + bytes + " end string");
    delay(5);
    return bytes.toInt();
}

void OpenLog::writeLine(const String& file, const String& line)
{
    int size = this->getFileSize(file);

    if(DEBUG_SD)
    {
        Serial.print(F("Size: "));
        Serial.println(size);
    }

    if (size == 0)
    {
        this->createFile(file);
    }

    this->sendCommandTest(F("write "), file, F(" "), size);
    this->waitForEdit();

    this->openLog->println(line);
    this->openLog->println();


    this->command = true;
    delay(10);
}

void OpenLog::removeFile(const String& file)
{

    this->sendCommandTest(F("rm "), file);
    this->waitToCommand();
}

String OpenLog::ls()
{
    this->sendCommandTest(F("ls"));

    String message = "";

    for(int timeOut = 0 ; timeOut < 1000 ; timeOut++)
    {
        while(this->openLog->available())
        {
            while(this->openLog->available())
            {
                String tmp = getValue(this->openLog->readStringUntil('\n'), ' ', 0);

                if (tmp[0] != '>' && tmp[0] != '\r')
                {
                    if(message == "")
                        message += tmp;
                    else
                        if (tmp.length() != 0 && (tmp.charAt(tmp.length() -1 ) != '/'))
                            message += "," + tmp;
                }
            }
            timeOut = 0;
        }
        delay(1);
    }
    return message;
}

String OpenLog::getFirstFileName()
{
    this->sendCommandTest(F("ls"));

    for(int timeOut = 0 ; timeOut < 1000 ; timeOut++)
    {
        while(this->openLog->available())
        {
            while(this->openLog->available())
            {
                String tmp = getValue(this->openLog->readStringUntil('\n'), ' ', 0);
                tmp.replace("\r", "");
                char lastChar = tmp.charAt(tmp.length() -1 );
                if (lastChar != '>' && tmp.length() != 0 && ( lastChar != '/'))
                {
                    this->waitToCommand();
                    return tmp;
                }
            }
            timeOut = 0;
        }
        delay(1);
    }

    this->clearBuffer();
    return String("");
}

bool OpenLog::openFile(const String& fileName)
{
    this->createFile(fileName);
    this->sendCommandTest(F("append "), fileName);
    this->waitForEdit();
    return true;
}

bool OpenLog::createDirectory(const String& dirName)
{
    this->sendCommandTest(F("md "), dirName);
    this->waitToCommand();
    return true;
}

bool OpenLog::copyToLog(const String& fileName)
{

    String content = readFile(fileName);

    if(DEBUG_SD)
    {
        Serial.print(F("File content: "));
        Serial.println(content);
    }

    this->cd(PARENT_DIR);
    this->cd(LOG_DIR);
    this->createFile(fileName);
    this->writeLine(fileName, content);
    this->cd(PARENT_DIR);
    this->cd(TMP_DIR);
    return true;
}

void OpenLog::cd(const String& dirName)
{
    this->sendCommandTest(F("cd "), dirName);
    this->waitToCommand();
    delay(5);
}

bool OpenLog::waitToCommand()
{
    unsigned long timeout = millis();

    bool flag = false;
    while ((unsigned long)(millis() - timeout) < SD_TIMEOUT) {

        if(this->openLog->available())
        {
            if(this->openLog->read() == '>')
            {
                this->command = true;
                flag = true;
                return true;
            }
        }
    }
    return flag;
}

bool OpenLog::waitForEdit()
{

    unsigned long timeout = millis();

    while ((unsigned long)(millis() - timeout) < SD_TIMEOUT) {

        if(this->openLog->available())
        {
            if(this->openLog->read() == '<')
            {
                this->command = false;
                return true;
            }
        }
    }
    return false;
}
