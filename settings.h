/*
BC95 library for Arduino.
Author: Chavee Issariyapat
E-mail: chavee@nexpie.com

This software is released under the MIT License.
*/

#ifndef SETTINGS_h
#define SETTINGS_h

#define BC95_PRINT_DEBUG                0
#define BC95_MAX_BUFFER_SIZE            128
#define BC95_DEFAULT_SERIAL_TIMEOUT     500

#define BC95UDP_SHARE_GLOBAL_BUFFER     0
#define BC95UDP_SERIAL_READ_CHUNK_SIZE  7
#define BC95UDP_MAX_PAYLOAD_SIZE        160

#define DNS_MAX_RETRY                   5
#define DNS_DEFAULT_SERVER              IPAddress(8,8,8,8)

#define COAP_ENABLE_ACK_CALLBACK        0

#endif
