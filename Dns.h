// BC95 DNS client -- modified from Arduino DNS client for WizNet5100-based Ethernet shield
// Author: Chavee Issariyapat
// E-mail: chavee@nexpie.com


// Arduino DNS client for WizNet5100-based Ethernet shield
// (c) Copyright 2009-2010 MCQN Ltd.
// Released under Apache License, version 2.0

#ifndef DNSClient_h
#define DNSClient_h

#include  "settings.h"
#include "BC95Udp.h"
#include "Arduino.h"

#if DNS_CACHE_SLOT > 0
struct DNS_CACHE_STRUCT {
    char domain[DNS_CACHE_SIZE];
    IPAddress ip;
};
typedef struct DNS_CACHE_STRUCT dns_cache_struct;
#endif

class DNSClient {
public:
    DNSClient();
    void begin ();
    void begin(const IPAddress& aDNSServer);

    /** Convert a numeric IP address string into a four-byte IP address.
        @param aIPAddrString IP address to convert
        @param aResult IPAddress structure to store the returned IP address
        @result 1 if aIPAddrString was successfully converted to an IP address,
                else error code
    */
    int inet_aton(const char *aIPAddrString, IPAddress& aResult);

    /** Resolve the given hostname to an IP address.
        @param aHostname Name to be resolved
        @param aResult IPAddress structure to store the returned IP address
        @result 1 if aIPAddrString was successfully converted to an IP address,
                else error code
    */
    int getHostByName(const char* aHostname, IPAddress& aResult);

protected:
    uint16_t BuildRequest(const char* aName);
    uint16_t ProcessResponse(uint16_t aTimeout, IPAddress& aAddress);

    #if DNS_CACHE_SLOT > 0
    void insertDNSCache(char* domain, IPAddress ip);
    void clearDNSCache();
    #endif

    IPAddress iDNSServer;
    uint16_t iRequestId;
    BC95UDP iUdp;
};

#endif
