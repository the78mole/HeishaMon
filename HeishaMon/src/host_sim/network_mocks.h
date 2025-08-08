#pragma once

#ifdef HOST_SIM

#include "Arduino.h"

// Mock DNS server for host simulation
class DNSServer {
public:
    bool start(uint16_t port, const String& domainName, const IPAddress& resolvedIP) {
        printf("DNSServer.start(%d, '%s', %s)\n", port, domainName.c_str(), resolvedIP.toString().c_str());
        return true;
    }
    
    void processNextRequest() {
        // Mock - do nothing
    }
    
    void stop() {
        printf("DNSServer.stop()\n");
    }
};

// Mock UDP class
class WiFiUDP {
public:
    uint8_t begin(uint16_t port) {
        printf("UDP.begin(%d)\n", port);
        return 1;
    }
    
    void stop() {
        printf("UDP.stop()\n");
    }
    
    int beginPacket(IPAddress ip, uint16_t port) {
        printf("UDP.beginPacket(%s, %d)\n", ip.toString().c_str(), port);
        return 1;
    }
    
    int beginPacket(const char *host, uint16_t port) {
        printf("UDP.beginPacket('%s', %d)\n", host, port);
        return 1;
    }
    
    int endPacket() {
        printf("UDP.endPacket()\n");
        return 1;
    }
    
    size_t write(uint8_t byte) {
        printf("UDP.write(0x%02X)\n", byte);
        return 1;
    }
    
    size_t write(const uint8_t *buffer, size_t size) {
        printf("UDP.write(buffer, %zu)\n", size);
        return size;
    }
    
    int parsePacket() {
        return 0; // No packets available
    }
    
    int available() {
        return 0;
    }
    
    int read() {
        return -1;
    }
    
    int read(unsigned char* buffer, size_t len) {
        (void)buffer; (void)len;
        return 0;
    }
    
    IPAddress remoteIP() {
        return IPAddress(0, 0, 0, 0);
    }
    
    uint16_t remotePort() {
        return 0;
    }
};

#include "WiFi.h"

#endif // HOST_SIM