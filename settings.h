/*
BC95 library for Arduino.
Author: Chavee Issariyapat
E-mail: chavee@nexpie.com

This software is released under the MIT License.
*/

#ifndef SETTINGS_h
#define SETTINGS_h

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

#define COAP_ENABLE_ACK_CALLBACK        0

#endif
