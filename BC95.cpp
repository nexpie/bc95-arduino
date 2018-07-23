/*
BC95 adapter class for Arduino.
Author: Chavee Issariyapat
E-mail: chavee@nexpie.com

This software is released under the MIT License.
*/

#include "BC95.h"

#if BC95_USE_EXTERNAL_BUFFER == 1
    char *buffer = NULL;
    size_t buffersize = 0;
#else
    char buffer[BC95_BUFFER_SIZE];
    #define buffersize c
#endif

const char STOPPER[][STOPPERLEN] = {END_LINE, END_OK, END_ERROR};

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))
#define STOPPERCOUNT COUNT_OF(STOPPER)

#if BC95_USE_EXTERNAL_BUFFER == 1
void BC95Class::setExternalBuffer(char *sbuffer, size_t sbuffersize) {
    buffer = sbuffer;
    buffersize = sbuffersize;
}
#endif

void BC95Class::begin(Stream &serial) {
    BC95Serial = &serial;
    memset(socketpool, 0, sizeof(SOCKD)*MAXSOCKET);
}

char* BC95Class::fetchSocketPacket(SOCKD *socket, uint16_t len) {
    char *p;
    if((p = getSerialResponse("\x0D\x0A", 0))) return p;

    cmdPrint(F("AT+NSORF="));
    cmdPrint(socket->sockid);
    cmdPrint(",");
    cmdPrintln(len);

    p = getSerialResponse("\x0D\x0A");
    return p;
}

// return a null-terminated response text from AT-command
char* BC95Class::getSerialResponse(char *prefix) {
    return getSerialResponse(prefix, BC95_DEFAULT_SERIAL_TIMEOUT);
}

char* BC95Class::getSerialResponse(char *prefix, uint32_t timeout) {

    if (readUntilDone(buffer, timeout, buffersize) >= 0) {
        char *p,*r;
        uint8_t sno;
        size_t plen;

        p = strstr((const char *)buffer, "+NSONMI:");
        if (p != NULL) {
            #ifdef LOG_DEBUG
                Serial.println(p);
            #endif

            p = p+8; // skip +NSONMI:
            plen = 0;
            for (r=p+2; *r>='0' && *r<='9'; r++) {
                plen = 10*plen + *r-'0';
            }
            sno = (unsigned char)*p - '0';
            if (sno>=0 && sno<MAXSOCKET)
                socketpool[sno].bc95_msglen = plen;

            // if +NSONMI is unexpectly found, read serial again
            if (readUntilDone(buffer, timeout, buffersize) >= 0) {
                return parseResponse(buffer, prefix);
            }
            else {
                return NULL;
            }
        }
        else {
            #ifdef LOG_DEBUG
                Serial.print("Incoming data : ");
                Serial.println(buffer);
            #endif
            return parseResponse(buffer, prefix);
        }
    }
    else {
        return NULL;
    }
}

// parse text with a prefix
char* BC95Class::parseResponse(char *rawstr, char* pref) {
    uint8_t i;
	char *r, *p;

    if (pref==NULL || *pref=='\0') {
        return rawstr;
    }
    p = strstr((const char *)rawstr, (const char *)pref);
    if (p != NULL) {
		p += strlen(pref);
        // skip STOPPER[0]

        for (i=1; i<STOPPERCOUNT; i++) {
            r = strstr((const char *)p, (const char *)STOPPER[i]);
            if (r != NULL) {
                *r = '\0';
    	    	r += strlen(STOPPER[i]);
                return p;
            }
        }
	}
	return NULL;
}

int BC95Class::readUntilDone(char* buff, uint32_t timeout, size_t maxsize) {
    uint8_t i = 0;
    char *p = buff;
	int h[STOPPERCOUNT] = {0};
    unsigned long starttm = millis();

    memset(buff, 0, maxsize);
	while(true) {
        if (BC95Serial->available()) {
            *p = BC95Serial->read();
        }
        else {
            if (millis() - starttm > timeout) {
                *p = '\0';
                return READ_TIMEOUT;
            }
            else {
                delay(1);
                continue;
            }
        }

    	for (i=0; i<STOPPERCOUNT; i++) {
    		if (STOPPER[i][h[i]] != *p) h[i] = 0;
    		else h[i]++;

    		if (STOPPER[i][h[i]] == '\0') {
                // terminate when END_OK and END_ERROR found. and also when END_LINE found after +NSONMI:
                if (i > 0 || (i==0 && strstr(buff, "+NSONMI:")!=NULL)) {
        			*(p+1) = '\0';
        			return i;
                }
    		}
            if (p - buff > maxsize) {
                *(p+1) = '\0';
                return READ_OVERFLOW;
            }
            if (*p == '\0') {
                return READ_INCOMPLETE;
        	}
    	}
    	p++;
    }
	return buff;
}

void BC95Class::reset() {
    cmdPrintln(F("AT+NRB"));
    delay(2000);
    getSerialResponse("REBOOTING");
    cmdPrintln(F("AT+CFUN=1"));
    delay(2000);
    cmdPrintln(F("AT"));
    delay(2000);
    while(BC95Serial->available() > 0) BC95Serial->read();
}

bool BC95Class::attachNetwork() {
  cmdPrintln(F("AT+CGATT=1"));
  cmdPrintln(F("AT+CGATT?"));
  return getSerialResponse("+CGATT:1");
}

char* BC95Class::getIMEI() {
    cmdPrintln(F("AT+CGSN=1"));
    return getSerialResponse("+CGSN:");
}

char* BC95Class::getIMSI() {
    cmdPrintln(F("AT+CIMI"));
    return getSerialResponse("\x0D\x0A");
}

/* This function needs BC95_BUFFER_SIZE > 200 */
char* BC95Class::getManufacturerModel() {
    cmdPrintln(F("AT+CGMM"));
    return getSerialResponse("\x0D\x0A");
}

char* BC95Class::getManufacturerRevision() {
    cmdPrintln(F("AT+CGMR"));
    return getSerialResponse("\x0D\x0A");
}

char* BC95Class::getIPAddress() {
    cmdPrintln(F("AT+CGPADDR=0"));
    return getSerialResponse("+CGPADDR:0,");
}

int8_t BC95Class::getSignalStrength() {
    char *p;
    int8_t r = 0, i;
    cmdPrintln(F("AT+CSQ"));
    p = getSerialResponse("+CSQ:");
    for (i=0; i<2; i++) {
        if (*p>='0' && *p<='9') r=10*r + *p-'0';
        p++;
    }
    return 2*r-113;
}

SOCKD* BC95Class::createSocket(uint16_t port){
    uint8_t no;
    cmdPrint(F("AT+NSOCR=DGRAM,17,"));
    cmdPrint( port );
    cmdPrintln(F(",1"));
    no = *(getSerialResponse("\x0D\x0A")) - '0';

    if (no>=0 && no<MAXSOCKET) {
        socketpool[no].sockid = no;
        socketpool[no].status = 1;
        socketpool[no].port = port;
        return &socketpool[no];
    }
    else {
        return NULL;
    }
}

int BC95Class::sendPacket(SOCKD *socket, IPAddress destIP, uint16_t destPort, uint8_t *payload, size_t size) {
    uint8_t i;
    unsigned char *p;
    cmdPrint(F("AT+NSOST="));
    cmdPrint(socket->sockid);
    cmdPrint(",");
    cmdPrint(destIP);
    cmdPrint(",");
    cmdPrint(destPort);
    cmdPrint(",");
    cmdPrint(size);
    cmdPrint(",");
    for (p=payload, i=0; i<size; p++,i++) {
        if (*p < 16) cmdPrint("0");
        cmdPrint(*p, HEX);
    }
    cmdPrint("\x0D\x0A");
    getSerialResponse("\x0D\x0A");
    return 1;
}

int BC95Class::sendPacket(SOCKD *socket, IPAddress destIP, uint16_t destPort, char* payload) {
    uint8_t len = strlen(payload);
    sendPacket(socket, destIP, destPort, payload, len);
}

void BC95Class::closeSocket(SOCKD *socket) {
    cmdPrint(F("AT+NSOCL="));
    cmdPrintln(socket->sockid);
    getSerialResponse("");
    socketpool[socket->sockid].status = 0;
}

uint8_t* getBuffer() {
    return (uint8_t *)buffer;
}

#if !defined NO_GLOBAL_INSTANCES && !defined NO_GLOBAL_BC95
BC95Class BC95;
#endif
