#ifndef STREAM_H
#define STREAM_H

#ifdef NATIVE_SIM

#include <iostream>
#include <cstdint>

// Mock Stream class for native simulation
class Stream {
public:
    virtual ~Stream() {}
    
    virtual int available() = 0;
    virtual int read() = 0;
    virtual int peek() = 0;
    virtual void flush() = 0;
    
    virtual size_t write(uint8_t data) {
        std::cout << (char)data;
        return 1;
    }
    
    virtual size_t write(const char* str) {
        std::cout << str;
        return strlen(str);
    }
    
    virtual size_t write(const uint8_t* buffer, size_t size) {
        for (size_t i = 0; i < size; i++) {
            std::cout << (char)buffer[i];
        }
        return size;
    }
    
    size_t print(const char* str) {
        return write(str);
    }
    
    size_t print(int value) {
        std::string str = std::to_string(value);
        return write(str.c_str());
    }
    
    size_t print(float value) {
        std::string str = std::to_string(value);
        return write(str.c_str());
    }
    
    size_t println(const char* str) {
        size_t len = write(str);
        len += write('\n');
        return len;
    }
    
    size_t println() {
        return write('\n');
    }
    
    // Read functions
    String readString() {
        return String("");
    }
    
    String readStringUntil(char terminator) {
        return String("");
    }
    
    int parseInt() {
        return 0;
    }
    
    float parseFloat() {
        return 0.0f;
    }
    
    // Timeout functions
    void setTimeout(unsigned long timeout) {}
    unsigned long getTimeout() { return 1000; }
};

#endif // NATIVE_SIM
#endif // STREAM_H