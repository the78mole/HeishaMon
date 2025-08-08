#pragma once

#ifdef HOST_SIM

#include "Arduino.h"

// File system mock
class File {
public:
    File() : _valid(false) {}
    File(bool valid) : _valid(valid) {}
    
    operator bool() { return _valid; }
    
    size_t write(uint8_t byte) {
        printf("File.write(0x%02X)\n", byte);
        return 1;
    }
    
    size_t write(const uint8_t *buf, size_t size) {
        printf("File.write(buffer, %zu bytes)\n", size);
        return size;
    }
    
    size_t write(const char* str) {
        printf("File.write('%s')\n", str);
        return strlen(str);
    }
    
    int available() { return 0; }
    int read() { return -1; }
    
    size_t read(uint8_t* buffer, size_t length) {
        printf("File.read(buffer, %zu)\n", length);
        memset(buffer, 0xFF, length);
        return length;
    }
    
    size_t readBytes(char* buffer, size_t length) {
        printf("File.readBytes(buffer, %zu)\n", length);
        return 0;
    }
    
    String readString() {
        return String("");
    }
    
    String readStringUntil(char terminator) {
        printf("File.readStringUntil('%c')\n", terminator);
        return String("");
    }
    
    void flush() {
        printf("File.flush()\n");
    }
    
    void close() {
        printf("File.close()\n");
        _valid = false;
    }
    
    size_t size() {
        return 1024; // Mock file size
    }
    
    String name() {
        return String("mock_file.txt");
    }
    
    bool seek(uint32_t pos) {
        printf("File.seek(%u)\n", pos);
        return true;
    }
    
    uint32_t position() {
        return 0;
    }
    
private:
    bool _valid;
};

// File system class mock
class LittleFSClass {
public:
    bool begin() {
        printf("LittleFS.begin()\n");
        return true;
    }
    
    void end() {
        printf("LittleFS.end()\n");
    }
    
    File open(const char* path, const char* mode = "r") {
        printf("LittleFS.open('%s', '%s')\n", path, mode);
        return File(true);
    }
    
    File open(const String& path, const char* mode = "r") {
        return open(path.c_str(), mode);
    }
    
    bool exists(const char* path) {
        printf("LittleFS.exists('%s') -> true\n", path);
        return true;
    }
    
    bool exists(const String& path) {
        return exists(path.c_str());
    }
    
    bool remove(const char* path) {
        printf("LittleFS.remove('%s')\n", path);
        return true;
    }
    
    bool remove(const String& path) {
        return remove(path.c_str());
    }
    
    bool rename(const char* pathFrom, const char* pathTo) {
        printf("LittleFS.rename('%s', '%s')\n", pathFrom, pathTo);
        return true;
    }
    
    bool mkdir(const char* path) {
        printf("LittleFS.mkdir('%s')\n", path);
        return true;
    }
    
    bool rmdir(const char* path) {
        printf("LittleFS.rmdir('%s')\n", path);
        return true;
    }
    
    size_t totalBytes() {
        return 1024 * 1024; // 1MB mock filesystem
    }
    
    size_t usedBytes() {
        return 512 * 1024; // 512KB used
    }
};

extern LittleFSClass LittleFS;

#endif // HOST_SIM