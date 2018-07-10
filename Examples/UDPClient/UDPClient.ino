#include <Arduino.h>
#include <AltSoftSerial.h>
#include "BC95Udp.h"

AltSoftSerial bc95serial;

// 8.8.8.8 is the Google's public DNS server.
#define SERVER_IP     IPAddress(8, 8, 8, 8)
#define SERVER_PORT   53

// This binary string represents a UDP paylaod of the DNS query for the domain name nexpie.com
uint8_t udpdata[] = "\xC0\x5B\x01\x00\x00\x01\x00\x00\x00\x00\x00\x00\x06\x6E\x65\x78\x70\x69\x65\x03\x63\x6F\x6D\x00\x00\x01\x00\x01";


BC95UDP udpclient;
uint8_t buff[64];

void printHEX(uint8_t *buff, size_t len) {
    for (int i=0; i<len; i++) {
        if (buff[i]<16) Serial.print(" 0");
        else Serial.print(" ");
        Serial.print(buff[i], HEX);
    }
}

void setup() {
    bc95serial.begin(9600);
    BC95.begin(bc95serial);
    BC95.reset();

    Serial.begin(9600);
    Serial.println(F("Starting..."));

    while (!BC95.attachNetwork()) {
        Serial.println("...");
        delay(1000);
    }
    Serial.println(F("NB-IOT attached.."));

    Serial.print(F("NB-IOT module IP address : "));
    Serial.println(BC95.getIPAddress());

    udpclient.begin(8053);
    udpclient.beginPacket(SERVER_IP, SERVER_PORT);

    Serial.println("\n\nSending UDP payload : ");
    Serial.println(F("                                         n  e  x  p  i  e     c  o  m        A    IN"));
    printHEX(udpdata, 28);
    
    udpclient.write(udpdata, 28);
    udpclient.endPacket();

    while (udpclient.parsePacket() == 0) {
        delay(500);
    }

    size_t len = udpclient.read(buff, 64);

    Serial.println(F("\n\nReceive UDP payload : "));
    printHEX(buff, len);

    Serial.print(F("\n\nThe last 4 bytes encodes IP address of the requested domain : "));

    IPAddress hostip = IPAddress(buff[len-4], buff[len-3], buff[len-2], buff[len-1]);
    Serial.print(hostip);
}

void loop() {
  
}

