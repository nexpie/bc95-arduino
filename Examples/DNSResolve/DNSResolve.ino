#include <Arduino.h>
#include <AltSoftSerial.h>
#include "Dns.h"

AltSoftSerial bc95serial;

DNSClient dns;
IPAddress remoteip;

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

    dns.begin();
    dns.getHostByName("google.com", remoteip);

    Serial.print("The resolved IP address is : ");
    Serial.println(remoteip);
}

void loop() {
  
}

