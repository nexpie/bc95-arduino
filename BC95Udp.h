/*
BC95 UDP adapter class for Arduino.
Author: Chavee Issariyapat
E-mail: chavee@nexpie.com

This software is released under the MIT License.
*/

#ifndef BC95Udp_h
#define BC95Udp_h

#include "BC95.h"
#include <Udp.h>

extern BC95Class BC95;

class BC95UDP : public UDP {
public:
    BC95UDP();

    virtual uint8_t begin(uint16_t);
    virtual void stop();
    virtual int beginPacket(IPAddress ip, uint16_t port);
    virtual int beginPacket(const char *host, uint16_t port);
    virtual int endPacket();

    virtual size_t write(uint8_t);
    virtual size_t write(const uint8_t *buffer, size_t size);
    virtual size_t write(const __FlashStringHelper *buffer, size_t size);

    virtual int parsePacket();
    virtual int available();
    virtual int read();
    virtual int read(unsigned char* buffer, size_t len);
    virtual int read(char* buffer, size_t len);
    virtual int peek();
    virtual void flush();

    virtual IPAddress remoteIP();
    virtual uint16_t remotePort();

    #if BC95UDP_USE_EXTERNAL_BUFFER == 1
    void setExternalBuffer(uint8_t *sbuffer, size_t sbuffersize);
    #endif
private:
    BC95Class *_bc95;
    IPAddress dip;
    uint16_t dport;
    SOCKD *socket;
    uint8_t *pktcur;
    #if BC95UDP_SHARE_GLOBAL_BUFFER == 0
    uint8_t pbuffer[BC95UDP_BUFFER_SIZE];
    uint16_t pbufferlen;
    #endif
};

#endif
