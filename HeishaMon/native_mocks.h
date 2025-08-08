#ifndef NATIVE_MOCKS_H
#define NATIVE_MOCKS_H

#ifdef NATIVE_SIM

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <sys/stat.h>
#include <unistd.h>

// Arduino core compatibility types
typedef uint8_t byte;
typedef bool boolean;

// Mock Serial class for native simulation
class MockSerial {
public:
    void begin(int baud) { 
        std::cout << "[SERIAL] Begin at " << baud << " baud" << std::endl; 
    }
    void end() { 
        std::cout << "[SERIAL] End" << std::endl; 
    }
    void println(const char* str) { 
        std::cout << "[SERIAL] " << str << std::endl; 
    }
    void println(const std::string& str) { 
        std::cout << "[SERIAL] " << str << std::endl; 
    }
    void print(const char* str) { 
        std::cout << "[SERIAL] " << str; 
    }
    void print(unsigned long value) { 
        std::cout << "[SERIAL] " << value; 
    }
    void printf(const char* format, ...) {
        va_list args;
        va_start(args, format);
        std::cout << "[SERIAL] ";
        vprintf(format, args);
        va_end(args);
    }
    int available() { return 0; }
    int read() { return -1; }
    void write(uint8_t data) { std::cout << (char)data; }
    void flush() { std::cout.flush(); }
};

// Mock WiFi class for native simulation
class MockWiFi {
public:
    int status() { return 3; } // WL_CONNECTED
    std::string localIP() { return "192.168.1.100"; }
    bool isConnected() { return true; }
    void begin(const char* ssid, const char* password = nullptr) {
        std::cout << "[WIFI] Connecting to " << ssid << std::endl;
    }
    void printDiag(MockSerial& serial) {
        serial.println("Mock WiFi Diagnostics");
    }
    void disconnect(bool wifioff = false) {
        std::cout << "[WIFI] Disconnected" << std::endl;
    }
    void mode(int mode) {
        std::cout << "[WIFI] Mode set to " << mode << std::endl;
    }
    void softAP(const char* ssid) {
        std::cout << "[WIFI] SoftAP started: " << ssid << std::endl;
    }
    void softAPConfig(const std::string& ip, const std::string& gateway, const std::string& subnet) {
        std::cout << "[WIFI] SoftAP configured" << std::endl;
    }
    int softAPgetStationNum() { return 0; }
    int scanComplete() { return 1; }
    void setScanMethod(int method) {}
    std::string softAPSSID() { return ""; }
};

// Mock Client class
class MockClient {
public:
    bool connected() { return true; }
    void stop() {}
    int read() { return -1; }
    int available() { return 0; }
    void write(const uint8_t* data, size_t len) {}
};

// Mock WebServer class
class MockWebServer {
public:
    MockWebServer(int port) : _port(port) {}
    void begin() { std::cout << "[WEBSERVER] Started on port " << _port << std::endl; }
    void handleClient() {}
    void on(const char* path, void (*handler)()) {}
    void send(int code, const char* content_type, const char* content) {}
    std::string arg(const char* name) { return ""; }
    bool hasArg(const char* name) { return false; }
private:
    int _port;
};

// Mock mDNS class
class MockMDNS {
public:
    bool begin(const char* hostname) { 
        std::cout << "[MDNS] Started with hostname: " << hostname << std::endl;
        return true; 
    }
    void addService(const char* service, const char* proto, int port) {}
};

// Mock LittleFS class
class MockLittleFS {
public:
    bool begin(bool format = false) { 
        std::cout << "[LITTLEFS] Started" << std::endl;
        return true; 
    }
    bool exists(const char* path) { 
        struct stat buffer;
        std::string fullPath = std::string("/tmp/heishamon_sim") + path;
        return (stat(fullPath.c_str(), &buffer) == 0); 
    }
    bool remove(const char* path) { 
        std::string fullPath = std::string("/tmp/heishamon_sim") + path;
        return (unlink(fullPath.c_str()) == 0); 
    }
    // Mock File class
    class MockFile {
    public:
        MockFile() : _valid(false) {}
        MockFile(const std::string& path, const char* mode) {
            std::string fullPath = std::string("/tmp/heishamon_sim") + path;
            system("mkdir -p /tmp/heishamon_sim");
            _file = fopen(fullPath.c_str(), mode);
            _valid = (_file != nullptr);
        }
        ~MockFile() { if (_file) fclose(_file); }
        operator bool() const { return _valid; }
        void close() { if (_file) { fclose(_file); _file = nullptr; _valid = false; } }
        size_t write(const uint8_t* data, size_t len) {
            return _file ? fwrite(data, 1, len, _file) : 0;
        }
        size_t read(uint8_t* data, size_t len) {
            return _file ? fread(data, 1, len, _file) : 0;
        }
        std::string readString() {
            if (!_file) return "";
            fseek(_file, 0, SEEK_END);
            long len = ftell(_file);
            fseek(_file, 0, SEEK_SET);
            std::string content(len, '\0');
            fread(&content[0], 1, len, _file);
            return content;
        }
        void print(const char* str) { if (_file) fputs(str, _file); }
        void println(const char* str) { if (_file) { fputs(str, _file); fputs("\n", _file); } }
    private:
        FILE* _file = nullptr;
        bool _valid;
    };
    
    MockFile open(const char* path, const char* mode = "r") {
        return MockFile(path, mode);
    }
};

// Mock ArduinoOTA class
class MockArduinoOTA {
public:
    void setHostname(const char* hostname) {}
    void setPassword(const char* password) {}
    void onStart(void (*fn)()) {}
    void onEnd(void (*fn)()) {}
    void onProgress(void (*fn)(unsigned int, unsigned int)) {}
    void onError(void (*fn)(int)) {}
    void begin() { std::cout << "[OTA] Started" << std::endl; }
    void handle() {}
};

// Mock NeoPixel class
class MockAdafruitNeoPixel {
public:
    MockAdafruitNeoPixel(int numPixels, int pin) {}
    void begin() {}
    void setPixelColor(int pixel, uint8_t r, uint8_t g, uint8_t b) {}
    void setPixelColor(int pixel, uint32_t color) {}
    void show() {}
    uint32_t Color(uint8_t r, uint8_t g, uint8_t b) { return (r << 16) | (g << 8) | b; }
};

// Mock DNS Server
class MockDNSServer {
public:
    void setErrorReplyCode(int code) {}
    void start(int port, const char* domain, const std::string& ip) {}
    void processNextRequest() {}
};

// Arduino-like functions
inline unsigned long millis() {
    static auto start = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
}

inline void delay(unsigned long ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

inline void yield() {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

inline void pinMode(int pin, int mode) {
    std::cout << "[GPIO] pinMode(" << pin << ", " << mode << ")" << std::endl;
}

inline void digitalWrite(int pin, int value) {
    std::cout << "[GPIO] digitalWrite(" << pin << ", " << value << ")" << std::endl;
}

inline int digitalRead(int pin) {
    return 1; // Always HIGH for simulation
}

// Additional Arduino-like functions needed
inline void attachInterrupt(int pin, void (*handler)(), int mode) {
    std::cout << "[INTERRUPT] attachInterrupt(" << pin << ", " << mode << ")" << std::endl;
}

inline void detachInterrupt(int pin) {
    std::cout << "[INTERRUPT] detachInterrupt(" << pin << ")" << std::endl;
}

inline unsigned long micros() {
    static auto start = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(now - start).count();
}

// Mock implementations of Arduino string functions
inline void strcpy_P(char* dest, const char* src) {
    strcpy(dest, src);
}

inline void strcat_P(char* dest, const char* src) {
    strcat(dest, src);
}

inline int strlen_P(const char* str) {
    return strlen(str);
}

inline int sprintf_P(char* buffer, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int result = vsprintf(buffer, format, args);
    va_end(args);
    return result;
}

inline int snprintf_P(char* buffer, size_t size, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int result = vsnprintf(buffer, size, format, args);
    va_end(args);
    return result;
}

// Global mock objects
extern MockSerial Serial;
extern MockSerial Serial1;
extern MockWiFi WiFi;
extern MockWebServer* server;
extern MockMDNS MDNS;
extern MockLittleFS LittleFS;
extern MockArduinoOTA ArduinoOTA;
extern MockDNSServer dnsServer;

// WiFi constants
#define WL_CONNECTED 3
#define WL_DISCONNECTED 6
#define WL_STOPPED 7
#define WIFI_AP 2
#define WIFI_MODE_AP 2
#define WIFI_ALL_CHANNEL_SCAN 1

// Pin mode constants
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2
#define FUNCTION_0 0

// Interrupt constants
#define RISING 1
#define FALLING 2
#define CHANGE 3

// Other constants
#define HIGH 1
#define LOW 0
#define LED_BUILTIN 2

// PROGMEM and related macros
#define PROGMEM
#define PSTR(s) (s)
#define F(string_literal) (string_literal)
#define _F(string_literal) (string_literal)

// DNS constants
#define DNS_PORT 53
enum DNSReplyCode {
    NoError = 0
};

// ESP specific mocks
namespace ESP {
    struct rst_info {
        uint32_t reason;
        uint32_t exccause;
    };
    
    inline rst_info* getResetInfoPtr() {
        static rst_info info = {1, 0}; // Normal reset
        return &info;
    }
    
    inline uint32_t getFreeHeap() { return 50000; }
}

// Mock time functions
inline time_t time(time_t* t) {
    time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    if (t) *t = now;
    return now;
}

// SNTP mocks
inline void sntp_stop() {}
inline void sntp_setoperatingmode(int mode) {}
inline void sntp_init() {}
#define SNTP_OPMODE_POLL 0

// IPAddress mock
class IPAddress {
public:
    IPAddress(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {}
    std::string toString() const { return "192.168.1.1"; }
};

#endif // NATIVE_SIM
#endif // NATIVE_MOCKS_H