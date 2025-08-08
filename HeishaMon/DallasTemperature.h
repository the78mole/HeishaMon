#ifndef DALLASTEMPERATURE_H
#define DALLASTEMPERATURE_H

#ifdef NATIVE_SIM

#include "OneWire.h"
#include <iostream>
#include <cstdint>

// Temperature conversion constants
#define DEVICE_DISCONNECTED_C -127
#define DEVICE_DISCONNECTED_F -196.6
#define DEVICE_DISCONNECTED_RAW -7040

// Resolution constants
#define TEMP_9_BIT  0x1F
#define TEMP_10_BIT 0x3F
#define TEMP_11_BIT 0x5F
#define TEMP_12_BIT 0x7F

// Mock DallasTemperature class for native simulation
class DallasTemperature {
public:
    DallasTemperature(OneWire* oneWire) : _wire(oneWire), _deviceCount(0) {
        std::cout << "[DALLAS] DallasTemperature initialized" << std::endl;
    }
    
    void begin() {
        std::cout << "[DALLAS] begin()" << std::endl;
        _deviceCount = 2; // Mock 2 devices for simulation
    }
    
    int getDeviceCount() {
        return _deviceCount;
    }
    
    bool getAddress(uint8_t* deviceAddress, int index) {
        if (index < _deviceCount) {
            // Mock device addresses
            for (int i = 0; i < 8; i++) {
                deviceAddress[i] = 0x28 + index + i;
            }
            return true;
        }
        return false;
    }
    
    void setResolution(uint8_t* deviceAddress, uint8_t newResolution) {
        std::cout << "[DALLAS] setResolution()" << std::endl;
    }
    
    void setResolution(uint8_t newResolution) {
        std::cout << "[DALLAS] setResolution(" << (int)newResolution << ")" << std::endl;
    }
    
    void requestTemperatures() {
        std::cout << "[DALLAS] requestTemperatures()" << std::endl;
    }
    
    float getTempC(uint8_t* deviceAddress) {
        // Return mock temperature values
        return 21.5f + (deviceAddress[7] % 10); // Different temps for different devices
    }
    
    float getTempCByIndex(int deviceIndex) {
        if (deviceIndex < _deviceCount) {
            return 21.5f + deviceIndex; // Mock temperature
        }
        return DEVICE_DISCONNECTED_C;
    }
    
    float getTempF(uint8_t* deviceAddress) {
        float tempC = getTempC(deviceAddress);
        return tempC * 9.0f / 5.0f + 32.0f;
    }
    
    float getTempFByIndex(int deviceIndex) {
        float tempC = getTempCByIndex(deviceIndex);
        if (tempC == DEVICE_DISCONNECTED_C) {
            return DEVICE_DISCONNECTED_F;
        }
        return tempC * 9.0f / 5.0f + 32.0f;
    }
    
    bool isConnected(uint8_t* deviceAddress) {
        return true; // All mock devices are connected
    }
    
    bool isParasitePowerMode() {
        return false; // Not using parasite power
    }

private:
    OneWire* _wire;
    int _deviceCount;
};

#endif // NATIVE_SIM
#endif // DALLASTEMPERATURE_H