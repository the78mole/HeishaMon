#pragma once

#ifdef HOST_SIM

#include "Arduino.h"
#include "OneWire.h"

// Temperature conversion constants
#define TEMP_9_BIT  0x1F //  9 bit
#define TEMP_10_BIT 0x3F // 10 bit
#define TEMP_11_BIT 0x5F // 11 bit
#define TEMP_12_BIT 0x7F // 12 bit

// Device address type
typedef uint8_t DeviceAddress[8];

// DallasTemperature mock
class DallasTemperature {
public:
    DallasTemperature() : _oneWire(nullptr) {}
    DallasTemperature(OneWire* oneWire) : _oneWire(oneWire) {}
    
    void setOneWire(OneWire* oneWire) {
        _oneWire = oneWire;
    }
    
    bool begin() {
        printf("DallasTemperature.begin()\n");
        return true;
    }
    
    uint8_t getDeviceCount() {
        printf("DallasTemperature.getDeviceCount() -> 0\n");
        return 0;
    }
    
    bool validAddress(const uint8_t* deviceAddress) {
        printf("DallasTemperature.validAddress()\n");
        (void)deviceAddress;
        return false;
    }
    
    bool getAddress(uint8_t* deviceAddress, uint8_t index) {
        printf("DallasTemperature.getAddress(%d)\n", index);
        (void)deviceAddress;
        return false;
    }
    
    void setResolution(uint8_t newResolution) {
        printf("DallasTemperature.setResolution(%d)\n", newResolution);
    }
    
    void setResolution(const uint8_t* deviceAddress, uint8_t newResolution) {
        printf("DallasTemperature.setResolution(addr, %d)\n", newResolution);
        (void)deviceAddress;
    }
    
    uint8_t getResolution(const uint8_t* deviceAddress) {
        printf("DallasTemperature.getResolution() -> 12\n");
        (void)deviceAddress;
        return 12;
    }
    
    void requestTemperatures() {
        printf("DallasTemperature.requestTemperatures()\n");
    }
    
    void requestTemperaturesByAddress(const uint8_t* deviceAddress) {
        printf("DallasTemperature.requestTemperaturesByAddress()\n");
        (void)deviceAddress;
    }
    
    void requestTemperaturesByIndex(uint8_t index) {
        printf("DallasTemperature.requestTemperaturesByIndex(%d)\n", index);
    }
    
    float getTempC(const uint8_t* deviceAddress) {
        printf("DallasTemperature.getTempC() -> 20.0\n");
        (void)deviceAddress;
        return 20.0; // Mock temperature in Celsius
    }
    
    float getTempCByIndex(uint8_t index) {
        printf("DallasTemperature.getTempCByIndex(%d) -> 20.0\n", index);
        return 20.0; // Mock temperature in Celsius
    }
    
    float getTempF(const uint8_t* deviceAddress) {
        printf("DallasTemperature.getTempF() -> 68.0\n");
        (void)deviceAddress;
        return 68.0; // Mock temperature in Fahrenheit
    }
    
    float getTempFByIndex(uint8_t index) {
        printf("DallasTemperature.getTempFByIndex(%d) -> 68.0\n", index);
        return 68.0; // Mock temperature in Fahrenheit
    }
    
    bool isConnected(const uint8_t* deviceAddress) {
        printf("DallasTemperature.isConnected() -> false\n");
        (void)deviceAddress;
        return false;
    }
    
    void setWaitForConversion(bool flag) {
        printf("DallasTemperature.setWaitForConversion(%s)\n", flag ? "true" : "false");
    }
    
    bool getWaitForConversion() {
        return true;
    }
    
    void setCheckForConversion(bool flag) {
        printf("DallasTemperature.setCheckForConversion(%s)\n", flag ? "true" : "false");
    }
    
    bool getCheckForConversion() {
        return true;
    }
    
private:
    OneWire* _oneWire;
};

#endif // HOST_SIM