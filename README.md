# bc95-arduino

UDP client library ที่พัฒนาขึ้นเพื่ออำนวยความสะดวกในการใช้งาน BC95 NB-IOT module ทำให้ไม่จำเป็นต้องเขียนโปรแกรมติดต่อ AT command เอง 
ในขณะเดียวกัน API สำหรับใช้งานถูกสร้างขึ้นตามแบบอย่างของ Arduino UDP class ทำให้สามารถนำไปใช้เป็น UDP stack ของ protocol library อื่นๆของ Arduino ได้เลย เช่น DNS, NTP และ CoAP

## ความเข้ากันได้

Arduino : UNO, MEGA2560

NB-IOT shield/module : True NB-IOT shield, AIS NB-IOT shield, Quectel BC95-B8

**คำแนะนำ**
สามารถปรับแต่งค่า config ต่างๆในไฟล์ settings.h ได้ เช่น ขนาด buffer, เลือกให้ใช้ buffer จากภายนอก หรือให้ library ทำการ allocate memory ขึ้นมาเอง, การตั้งค่า default dns และ time server เป็นต้น 

```
#define DATA_BUFFER_SIZE                128

#define BC95_USE_EXTERNAL_BUFFER        1
#define BC95_PRINT_DEBUG                0
#define BC95_DEFAULT_SERIAL_TIMEOUT     500
#define BC95_BUFFER_SIZE                DATA_BUFFER_SIZE

#define BC95UDP_USE_EXTERNAL_BUFFER     0
#define BC95UDP_SHARE_GLOBAL_BUFFER     1
#define BC95UDP_SERIAL_READ_CHUNK_SIZE  7
#define BC95UDP_BUFFER_SIZE             DATA_BUFFER_SIZE

#define DNS_MAX_RETRY                   5
#define DNS_DEFAULT_SERVER              IPAddress(8,8,8,8)

#define NTP_DEFAULT_SERVER              "time.nist.gov"

#define COAP_ENABLE_ACK_CALLBACK        1
```

## ตัวอย่างโค้ดบน Arduino UNO

ตัวอย่างการนำ BC95UDP ไปใช้เป็น UDP stack ร่วมกับ NTP protocol ในการ sync เวลากับ time server ผ่าน NB-IOT

```C++
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
        Serial.print(".");
        delay(1000);
    }
    Serial.println(F("\nNB-IOT attached.."));

    ntpclient.begin();
    ntpclient.update();
    Serial.print("The current GMT time is : ");
    Serial.println(ntpclient.getFormattedTime());
}

void loop() {
  
}

```

เรายังสามาถใช้รับส่ง UDP packet ในรูปแบบเดียวกับ UDP class มาตรฐานของ Arduino โค้ดข้างล่างเป็นตัวอย่างการส่ง UDP packet ของ DNS query ไป resolve IP address กับ domain name server ของ Google ซึ่งมี IP เป็น 8.8.8.8 

```C++
#include <Arduino.h>
#include <AltSoftSerial.h>
#include "BC95Udp.h"

AltSoftSerial bc95serial;

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
    Serial.print(F("Attach Network..."));
    while (!BC95.attachNetwork()) {
        Serial.print(".");
        delay(1000);
    }

    udpclient.begin(8053);
    udpclient.beginPacket(SERVER_IP, SERVER_PORT);    
    udpclient.write(udpdata, 28);
    udpclient.endPacket();

    while (udpclient.parsePacket() == 0) {
        delay(500);
    }

    size_t len = udpclient.read(buff, 64);

    Serial.println(F("\n\nReceive UDP payload : "));
    printHEX(buff, len);
}

void loop() {
  
}

```

library มีฟีเจอร์ resolve domain name อัตโนมัติผ่าน NB-IOT ทำให้สามารถใช้ hostname แทน IP address ใน UDP API ได้เลย เช่น

```C++
udpclient.beginPacket(udp.server.com", 5683);  
```

แต่เพื่อให้เกิดการใช้ memory อย่างมีประสิทธิภาพ ซึ่งสำคัญมากกับบอร์ดที่มี memory น้อยอย่างเช่น UNO จึงขอแนะนำให้ resolve IP ก่อนผ่าน DNS client จึงค่อยนำไปใช้งาน แบบในโค้ดต่อไปนี้

```C++
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
        Serial.print(".");
        delay(1000);
    }
    Serial.println(F("\nNB-IOT attached.."));

    dns.begin();
    dns.getHostByName("google.com", remoteip);

    Serial.print("The resolved IP address is : ");
    Serial.println(remoteip);
}

void loop() {
  
}

```

นอกจากนี้ เรายังสามารถใช้ BC95UDP ในการรับส่ง CoAP protocol กับ CoAP server ตามตัวอย่างนี้ responseHandler() เป็น callback function ที่เราต้อง register ไว้เพื่อรอรับเวลามี CoAP response เข้ามาจาก server โค้ดตัวอย่างเป็นการเรียก CoAP GET /hello ไปที่ coap.me ที่ port 5683 ซึ่ง server จะตอบคำว่า world กลับมา และมี response code เป็น 2.05

```C++
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
```
