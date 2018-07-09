#include <Arduino.h>
#include <AltSoftSerial.h>
#include "NTPClient.h"

AltSoftSerial bc95serial;

BC95UDP udp;
NTPClient ntpclient(udp);

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

    ntpclient.begin();
    ntpclient.update();
    Serial.print("The current GMT time is : ");
    Serial.println(ntpclient.getFormattedTime());
}

void loop() {
  
}

