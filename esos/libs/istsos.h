#ifndef ISTSOS_H
#define ISTSOS_H

#include "log/log.h"
#include "com/comunication.h"
#include "utils.h"

#define MAX_RETRY 3

#define DEBUG_ISTSOS 0

class Istsos
{
    private:
        ILog* sd;
        ICom* com;
        const char* server;
        const char* resource;
        const char* procedure;

        bool sendSingleFile(const String& fileName);
        String listFile();

    public:

        /**
            constructor.

            @param sd
            @param com
            @param server
            @param resource
            @param procedure

        */
        Istsos(ILog &sd, ICom &com, const char* server, const char* resource, const char* procedure);

        /**
            initialize enviroment, call it inside setup().
        */
        void begin();

        /**
            Send available data to the server.

            @return bool success or failure
        */
        bool sendData();

        /**
            Send available data to the server (testing).

            @return bool success or failure
        */
        bool sendDataTest();

        /**
            Log message to the SD card.

            @param message message to log
        */
        void logData(const String& message);
        /**
            this function log some message i.e. last send date.

            @param message massage to save
        */
        void logging(const String& date, const String& message);

        /**

        */
        void tikLogging(const String& date);

        /**
            Get current time and date

            @param ntpServer
        */
        uint32_t* ntpUpdate(const char ntpServer[] = "", int GMT = 0);

};

#endif
