/*
BC95 adapter class for Arduino.
Author: Chavee Issariyapat
E-mail: chavee@nexpie.com

This software is released under the MIT License.
*/

#ifndef BC95_h
#define BC95_h

#include "settings.h"
#include <Arduino.h>
#include <IPAddress.h>

#if BC95_PRINT_DEBUG == 1
#define ECHO_DEBUG
#define LOG_DEBUG
#endif

#define MAXSOCKET 7

#define READ_TIMEOUT      -1
#define READ_OVERFLOW     -2
#define READ_INCOMPLETE   -3
#define STOPPERLEN        12

#define END_LINE        "\x0D\x0A"
#define END_OK          "\x0D\x0A\x0D\x0A\x4F\x4B\x0D\x0A"
#define END_ERROR       "\x0D\x0A\x0D\x0A\x45\x52\x52\x4F\x52\x0D\x0A"

#define CMDPRINTMACRO(_1,_2,NAME,...) NAME
#define cmdPrint(...) CMDPRINTMACRO(__VA_ARGS__, cmdPrint2, cmdPrint1)(__VA_ARGS__)

#define CMDPRINTLNMACRO(_1,_2,NAME,...) NAME
#define cmdPrintln(...) CMDPRINTMACRO(__VA_ARGS__, cmdPrintln2, cmdPrintln1)(__VA_ARGS__)

#ifdef ECHO_DEBUG
    #define cmdPrint1(CMD) if(1) { \
        BC95Serial->print(CMD);   \
        Serial.print(CMD);        \
    }

    #define cmdPrintln1(CMD) if(1) { \
        BC95Serial->println(CMD);    \
        Serial.println(CMD);         \
    }

    #define cmdPrint2(CMD, ARG) if(1) { \
        BC95Serial->print(CMD,ARG);    \
        Serial.print(CMD,ARG);         \
    }

    #define cmdPrintln2(CMD, ARG) if(1) { \
        BC95Serial->println(CMD,ARG);     \
        Serial.println(CMD,ARG);          \
    }
#else
    #define cmdPrint1(CMD) BC95Serial->print(CMD);
    #define cmdPrintln1(CMD) BC95Serial->println(CMD);
    #define cmdPrint2(CMD, ARG) BC95Serial->print(CMD,ARG);
    #define cmdPrintln2(CMD, ARG) BC95Serial->println(CMD,ARG);
#endif

typedef struct SOCKD SOCKD;
struct SOCKD {
    uint8_t sockid;
    uint8_t status;
    uint16_t port;
    uint16_t msglen;
    uint16_t buff_msglen;
    uint16_t bc95_msglen;
};

class BC95Class {
public:
    BC95();

    #if BC95_USE_EXTERNAL_BUFFER == 1
    void BC95Class::setExternalBuffer(char *sbuffer, size_t sbuffersize);
    #endif

    SOCKD socketpool[MAXSOCKET];
    void begin(Stream &);
    char* getIMEI();
    char* getIMEI(char *);
    char* getIMSI();
    int8_t getSignalStrength();
    char* getManufacturerModel();
    char* getManufacturerRevision();

    bool attachNetwork();
    char* getIPAddress();
    SOCKD* createSocket(uint16_t);
    void reset();
    int sendPacket(SOCKD* socket, IPAddress, uint16_t destPort, char *payload);
    int sendPacket(SOCKD* socket, IPAddress, uint16_t destPort, uint8_t *payload, size_t);
    char* fetchSocketPacket(SOCKD *socket, uint16_t len);
    void closeSocket(SOCKD *socket);

    uint8_t* getBuffer();

private:
    Stream* BC95Serial;
    int readUntilDone(char* buff, uint32_t timeout, size_t);
    char* parseResponse(char* rawstr, char* pref);
    char* getSerialResponse(char *prefix);
    char* getSerialResponse(char *prefix, uint32_t timeout);
};

#endif
