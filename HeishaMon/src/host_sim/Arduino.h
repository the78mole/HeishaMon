#pragma once

#ifdef HOST_SIM

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <iostream>
#include <string>
#include <cmath>
#include <chrono>
#include <thread>

#include "pgmspace.h"

// Forward declarations
class IPAddress;

// Arduino basic types
typedef uint8_t byte;
typedef bool boolean;

// Arduino constants
#define HIGH 1
#define LOW 0
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2

// Math constants
#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105

// Arduino min/max macros - protected to avoid conflicts with std library
#ifdef min
#undef min
#endif
#ifdef max 
#undef max
#endif
#ifdef abs
#undef abs
#endif
#ifdef round
#undef round
#endif

// Arduino-compatible functions without macro conflicts
template<typename T>
inline T arduino_min(T a, T b) { return (a < b) ? a : b; }
template<typename T>
inline T arduino_max(T a, T b) { return (a > b) ? a : b; }
template<typename T>
inline T arduino_abs(T x) { return (x >= 0) ? x : -x; }
inline long arduino_round(double x) { return (x >= 0) ? (long)(x + 0.5) : (long)(x - 0.5); }
template<typename T>
inline T constrain(T amt, T low, T high) { return (amt < low) ? low : ((amt > high) ? high : amt); }
#define radians(deg) ((deg)*DEG_TO_RAD)
#define degrees(rad) ((rad)*RAD_TO_DEG)
#define sq(x) ((x)*(x))

// Arduino String class mock
class String {
public:
    String() : data("") {}
    String(const char* str) : data(str ? str : "") {}
    String(const std::string& str) : data(str) {}
    String(const String& other) : data(other.data) {}
    String(const __FlashStringHelper* str) : data(reinterpret_cast<const char*>(str) ? reinterpret_cast<const char*>(str) : "") {}
    String(int val) : data(std::to_string(val)) {}
    String(unsigned int val) : data(std::to_string(val)) {}
    String(long val) : data(std::to_string(val)) {}
    String(unsigned long val) : data(std::to_string(val)) {}
    String(float val) : data(std::to_string(val)) {}
    String(double val) : data(std::to_string(val)) {}
    
    const char* c_str() const { return data.c_str(); }
    size_t length() const { return data.length(); }
    size_t size() const { return data.size(); }
    
    String operator+(const String& other) const { return String(data + other.data); }
    String operator+(const char* other) const { return String(data + (other ? other : "")); }
    String& operator+=(const String& other) { data += other.data; return *this; }
    String& operator+=(const char* other) { if(other) data += other; return *this; }
    String& operator=(const String& other) { data = other.data; return *this; }
    String& operator=(const char* other) { data = (other ? other : ""); return *this; }
    
    bool operator==(const String& other) const { return data == other.data; }
    bool operator!=(const String& other) const { return data != other.data; }
    
    int indexOf(const String& str) const { 
        size_t pos = data.find(str.data);
        return (pos != std::string::npos) ? (int)pos : -1;
    }
    int indexOf(char c) const { 
        size_t pos = data.find(c);
        return (pos != std::string::npos) ? (int)pos : -1;
    }
    
    String substring(int start) const {
        if (start >= (int)data.length()) return String();
        return String(data.substr(start));
    }
    String substring(int start, int end) const {
        if (start >= (int)data.length()) return String();
        if (end > (int)data.length()) end = data.length();
        if (end <= start) return String();
        return String(data.substr(start, end - start));
    }
    
    void replace(const String& find, const String& replace) {
        size_t pos = 0;
        while ((pos = data.find(find.data, pos)) != std::string::npos) {
            data.replace(pos, find.data.length(), replace.data);
            pos += replace.data.length();
        }
    }
    
    void trim() {
        data.erase(0, data.find_first_not_of(" \t\n\r\f\v"));
        data.erase(data.find_last_not_of(" \t\n\r\f\v") + 1);
    }
    
    void toLowerCase() {
        for (char& c : data) {
            if (c >= 'A' && c <= 'Z') c = c - 'A' + 'a';
        }
    }
    
    void toUpperCase() {
        for (char& c : data) {
            if (c >= 'a' && c <= 'z') c = c - 'a' + 'A';
        }
    }
    
    int toInt() const {
        return atoi(data.c_str());
    }
    
    float toFloat() const {
        return atof(data.c_str());
    }
    
    double toDouble() const {
        return atof(data.c_str());
    }
    
    uint8_t concat(const String& str) {
        data += str.data;
        return 1;
    }
    
    uint8_t concat(const char* cstr) {
        if (cstr) data += cstr;
        return 1;
    }
    
    uint8_t concat(char c) {
        data += c;
        return 1;
    }
    
private:
    std::string data;
};

// Print base class for Arduino compatibility
class Print {
public:
    virtual size_t write(uint8_t byte) = 0;
    virtual size_t write(const uint8_t *buffer, size_t size) {
        size_t n = 0;
        while (size--) {
            if (write(*buffer++)) n++;
            else break;
        }
        return n;
    }
    
    size_t print(const char* str) { 
        if (!str) return 0;
        return write((const uint8_t*)str, strlen(str)); 
    }
    size_t print(const String& str) { 
        return write((const uint8_t*)str.c_str(), str.length()); 
    }
    size_t print(int val) { 
        char buf[12];
        sprintf(buf, "%d", val);
        return print(buf);
    }
    size_t print(unsigned int val) { 
        char buf[12];
        sprintf(buf, "%u", val);
        return print(buf);
    }
    size_t print(long val) { 
        char buf[20];
        sprintf(buf, "%ld", val);
        return print(buf);
    }
    size_t print(unsigned long val) { 
        char buf[20];
        sprintf(buf, "%lu", val);
        return print(buf);
    }
    size_t print(float val) { 
        char buf[20];
        sprintf(buf, "%.2f", val);
        return print(buf);
    }
    size_t print(double val) { 
        char buf[20];
        sprintf(buf, "%.2f", val);
        return print(buf);
    }
    
    size_t println() { return print("\n"); }
    size_t println(const char* str) { 
        size_t n = print(str);
        n += println();
        return n;
    }
    size_t println(const String& str) { 
        size_t n = print(str);
        n += println();
        return n;
    }
    size_t println(int val) { 
        size_t n = print(val);
        n += println();
        return n;
    }
    size_t println(unsigned int val) { 
        size_t n = print(val);
        n += println();
        return n;
    }
    size_t println(long val) { 
        size_t n = print(val);
        n += println();
        return n;
    }
    size_t println(unsigned long val) { 
        size_t n = print(val);
        n += println();
        return n;
    }
    size_t println(float val) { 
        size_t n = print(val);
        n += println();
        return n;
    }
    size_t println(double val) { 
        size_t n = print(val);
        n += println();
        return n;
    }
    
    virtual void flush() {}
};

// Serial class mock
class SerialClass : public Print {
public:
    void begin(unsigned long baud) { (void)baud; }
    void end() {}
    
    virtual size_t write(uint8_t byte) override {
        printf("%c", byte);
        return 1;
    }
    
    size_t print(const char* str) { printf("%s", str); return strlen(str); }
    size_t print(const String& str) { printf("%s", str.c_str()); return str.length(); }
    size_t print(int val) { return printf("%d", val); }
    size_t print(unsigned int val) { return printf("%u", val); }
    size_t print(long val) { return printf("%ld", val); }
    size_t print(unsigned long val) { return printf("%lu", val); }
    size_t print(float val) { return printf("%.2f", val); }
    size_t print(double val) { return printf("%.2f", val); }
    
    size_t println() { printf("\n"); return 1; }
    size_t println(const char* str) { printf("%s\n", str); return strlen(str) + 1; }
    size_t println(const String& str) { printf("%s\n", str.c_str()); return str.length() + 1; }
    size_t println(int val) { return printf("%d\n", val); }
    size_t println(unsigned int val) { return printf("%u\n", val); }
    size_t println(long val) { return printf("%ld\n", val); }
    size_t println(unsigned long val) { return printf("%lu\n", val); }
    size_t println(float val) { return printf("%.2f\n", val); }
    size_t println(double val) { return printf("%.2f\n", val); }
    
    int available() { return 0; }
    int read() { return -1; }
    void flush() { fflush(stdout); }
};

// Stream class for Arduino compatibility
class Stream : public Print {
public:
    virtual int available() = 0;
    virtual int read() = 0;
    virtual int peek() { return -1; }
    virtual void flush() {}
    
    // Find functions
    bool find(const char *target) { (void)target; return false; }
    bool findUntil(const char *target, const char *terminator) { 
        (void)target; (void)terminator; return false; 
    }
    
    // Read functions
    String readString() { return String(""); }
    String readStringUntil(char terminator) { (void)terminator; return String(""); }
    int readBytes(char *buffer, size_t length) { 
        (void)buffer; (void)length; return 0; 
    }
    
    // Timeout functions
    void setTimeout(unsigned long timeout) { (void)timeout; }
    unsigned long getTimeout() { return 1000; }
};

// Printable class for Arduino compatibility
class Printable {
public:
    virtual size_t printTo(Print& p) const = 0;
};

extern SerialClass Serial;
extern SerialClass Serial1;

// Arduino functions
void pinMode(int pin, int mode);
void digitalWrite(int pin, int value);
int digitalRead(int pin);
int analogRead(int pin);
void analogWrite(int pin, int value);

unsigned long millis();
unsigned long micros();
void delay(unsigned long ms);
void delayMicroseconds(unsigned int us);

// Math functions
long map(long value, long fromLow, long fromHigh, long toLow, long toHigh);
unsigned int makeWord(unsigned int w);
unsigned int makeWord(unsigned char h, unsigned char l);

// Math functions from cmath
using std::sin;
using std::cos;
using std::tan;
using std::fmod;
using std::sqrt;
using std::pow;

// Setup and loop functions that need to be implemented
void setup();
void loop();

#endif // HOST_SIM