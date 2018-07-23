/*
BC95 UDP adapter class for Arduino.
Author: Chavee Issariyapat
E-mail: chavee@nexpie.com

This software is released under the MIT License.
*/

#include "settings.h"
#include "Dns.h"
#include <Udp.h>
#include "BC95.h"
#include "BC95Udp.h"

#if BC95UDP_SHARE_GLOBAL_BUFFER == 1
#if BC95UDP_USE_EXTERNAL_BUFFER == 1
uint8_t *pbuffer = NULL;
size_t pbuffersize = 0;
#else
uint8_t pbuffer[BC95UDP_BUFFER_SIZE];
#define pbuffersize BC95UDP_BUFFER_SIZE
#endif
uint16_t pbufferlen;
#endif

BC95UDP::BC95UDP() {
    _bc95 = &BC95;
    pbufferlen = 0;

    #if BC95_USE_EXTERNAL_BUFFER == 1
    _bc95->setExternalBuffer(pbuffer, pbuffersize);
    #endif
}

#if BC95UDP_USE_EXTERNAL_BUFFER == 1
void BC95UDP::setExternalBuffer(uint8_t *sbuffer, size_t sbuffersize) {
    pbuffer = sbuffer;
    pbuffersize = sbuffersize;

    #if BC95UDP_USE_EXTERNAL_BUFFER == 1
    _bc95->setExternalBuffer((char *)sbuffer, sbuffersize);
    #endif
}
#endif

uint8_t BC95UDP::begin(uint16_t port) {
    socket = _bc95->createSocket(port);
}

void BC95UDP::stop() {
    _bc95->closeSocket(socket);
}

int BC95UDP::beginPacket(const char *host, uint16_t port) {
    int ret = 0;
    DNSClient *dns;
    IPAddress remote_addr;

    dns = new DNSClient();
    dns->begin();
    ret = dns->getHostByName(host, remote_addr);
    delete dns;

    if (ret == 1) {
        return beginPacket(remote_addr, port);
    }
    else return ret;
}

int BC95UDP::beginPacket(IPAddress ip, uint16_t port) {
    flush();
    dip = ip;
    dport = port;
    return 1;
}

size_t BC95UDP::write(const uint8_t *buffer, size_t size) {
    if (pbufferlen + size <= pbuffersize) {
        memcpy(pbuffer+pbufferlen, buffer, size);
        pbufferlen += size;
        return size;
    }
    else return 0;
}

size_t BC95UDP::write(uint8_t byte) {
    write(&byte, 1);
    return 1;
}

size_t BC95UDP::write(const __FlashStringHelper *buffer, size_t size) {
    if (pbufferlen + size <= pbuffersize) {
        memcpy_P(pbuffer+pbufferlen, buffer, size);
        pbufferlen += size;
        return size;
    }
    else return 0;
}

int BC95UDP::endPacket() {
    int result;
    result = _bc95->sendPacket(socket, dip, dport, pbuffer, pbufferlen);
    flush();
    return result;
}

int BC95UDP::parsePacket() {
    uint8_t *p,*q, *k, field;
    uint8_t i;
    size_t len;
    if (pbufferlen > 0) return pbufferlen;

    pbufferlen = 0;
    memset(pbuffer, 0, pbuffersize);
    q = pbuffer;
    do {
        len = 0;
        #if BC95_USE_EXTERNAL_BUFFER == 1
        _bc95->setExternalBuffer(q, pbuffersize-(q-pbuffer));
        #endif

        char *msg = _bc95->fetchSocketPacket(socket, BC95UDP_SERIAL_READ_CHUNK_SIZE);

        if (msg == NULL) {
            break;
        }
        p = msg;
        field = 0;
        while (field<4) {
            if (*p == ',') {
                field++;
                if (field==3) k = p+1;   // point to the beginning of returned size
                else if (field == 4) {
                    *p = '\0';
                    len = atoi((char *)k);
                    // shift p and break to avoid terminator checking below
                    p++;
                    break;
                }
            }

            if (*p=='\x00' || *p=='\x0D' || *p=='\x0A') {
                *pbuffer = '\0';
                break;
            }
            p++;
        }
        while (*p != '\0' && *p != ',') {
            *q = 0;
            for (i=0,*q=0; i<2; i++,p++) {
                *q = 16*(*q) + ((*p>='A' && *p<='F')?(*p-'A'+10):((*p>='0' && *p<='9')?(*p-'0'):0));
            }
            pbufferlen++;
            q++;
        }
        if (len < socket->bc95_msglen) socket->bc95_msglen -= len;
        else  socket->bc95_msglen = 0;

    } while (socket->bc95_msglen > 0);

    socket->buff_msglen = pbufferlen;
    return pbufferlen;
}

int BC95UDP::available() {
    return pbufferlen;
}

int BC95UDP::read() {
    unsigned char c;
    read(&c,1);
    return c;
}

int BC95UDP::read(unsigned char* buffer, size_t len) {
    uint16_t rlen;
    rlen = len>pbufferlen?pbufferlen:len;

    memcpy(buffer, pbuffer, rlen);
    memmove(pbuffer, pbuffer+rlen, pbufferlen-rlen);
    pbufferlen -= rlen;
    return rlen;
}

int BC95UDP::read(char* buffer, size_t len) {
    return read((unsigned char *)buffer, len);
}

int BC95UDP::peek() {
    if (pbufferlen > 0) return (int)*pbuffer;
    else return -1;
}

void BC95UDP::flush() {
    if (pbufferlen > 0) {
        memset(pbuffer, 0, pbuffersize);
        pbufferlen = 0;
    }
}

IPAddress BC95UDP::remoteIP() {
    return dip;
}

uint16_t BC95UDP::remotePort() {
    return dport;
}
