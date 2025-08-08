#ifndef CLIENT_H
#define CLIENT_H

#ifdef NATIVE_SIM

#include <iostream>
#include <cstdint>
#include "IPAddress.h"

// Mock Client class for native simulation
class Client {
public:
    virtual ~Client() {}
    
    virtual int connect(IPAddress ip, uint16_t port) {
        std::cout << "[CLIENT] connect(" << ip.toString() << ", " << port << ")" << std::endl;
        return 1; // Success
    }
    
    virtual int connect(const char* host, uint16_t port) {
        std::cout << "[CLIENT] connect(" << host << ", " << port << ")" << std::endl;
        return 1; // Success
    }
    
    virtual size_t write(uint8_t data) {
        std::cout << "[CLIENT] write(" << (int)data << ")" << std::endl;
        return 1;
    }
    
    virtual size_t write(const uint8_t* buf, size_t size) {
        std::cout << "[CLIENT] write(buffer, " << size << ")" << std::endl;
        return size;
    }
    
    virtual int available() {
        return 0; // No data available
    }
    
    virtual int read() {
        return -1; // No data
    }
    
    virtual int read(uint8_t* buf, size_t size) {
        return 0; // No data read
    }
    
    virtual int peek() {
        return -1; // No data
    }
    
    virtual void flush() {
        std::cout << "[CLIENT] flush()" << std::endl;
    }
    
    virtual void stop() {
        std::cout << "[CLIENT] stop()" << std::endl;
    }
    
    virtual uint8_t connected() {
        return 1; // Always connected for simulation
    }
    
    virtual operator bool() {
        return true; // Always valid for simulation
    }
};

#endif // NATIVE_SIM
#endif // CLIENT_H