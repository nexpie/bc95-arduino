#include <Arduino.h>
#include <AltSoftSerial.h>
#include "BC95Udp.h"
#include "CoAP.h"

AltSoftSerial bc95serial;

BC95UDP udp;
Coap coap(udp);

void responseHandler(CoapPacket *packet, IPAddress remoteIP, int remotePort) {
    char buff[6];

    Serial.print("CoAP Response Code: ");
    sprintf(buff, "%d.%02d \n", packet->code >> 5, packet->code & 0b00011111);
    Serial.print(buff);

    for (int i=0; i< packet->payloadlen; i++) {
        Serial.print((char) (packet->payload[i]));
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

    coap.response(responseHandler);
    coap.start();
    coap.get("coap.me", 5683, "/hello");
}

void loop() {
    coap.loop();
}
