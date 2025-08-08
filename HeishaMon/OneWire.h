#ifndef ONEWIRE_H
#define ONEWIRE_H

#ifdef NATIVE_SIM

#include <iostream>
#include <cstdint>

// Mock OneWire class for native simulation
class OneWire {
public:
    OneWire(int pin) : _pin(pin) {
        std::cout << "[ONEWIRE] OneWire initialized on pin " << pin << std::endl;
    }
    
    void reset_search() {
        std::cout << "[ONEWIRE] reset_search()" << std::endl;
    }
    
    bool search(uint8_t* newAddr) {
        // Return false to indicate no devices found
        return false;
    }
    
    uint8_t reset() {
        std::cout << "[ONEWIRE] reset()" << std::endl;
        return 1; // Device presence detected
    }
    
    void select(const uint8_t* addr) {
        std::cout << "[ONEWIRE] select()" << std::endl;
    }
    
    void write(uint8_t data) {
        std::cout << "[ONEWIRE] write(" << (int)data << ")" << std::endl;
    }
    
    uint8_t read() {
        std::cout << "[ONEWIRE] read()" << std::endl;
        return 0;
    }
    
    static uint8_t crc8(const uint8_t* addr, uint8_t len) {
        return 0; // Mock CRC
    }

private:
    int _pin;
};

#endif // NATIVE_SIM
#endif // ONEWIRE_H