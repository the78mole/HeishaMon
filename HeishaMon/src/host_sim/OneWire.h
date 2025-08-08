#pragma once

#ifdef HOST_SIM

#include "Arduino.h"

// OneWire mock
class OneWire {
public:
    OneWire(uint8_t pin) : _pin(pin) {
        printf("OneWire(%d)\n", pin);
    }
    
    void reset_search() {
        printf("OneWire.reset_search()\n");
    }
    
    uint8_t search(uint8_t *newAddr) {
        printf("OneWire.search()\n");
        (void)newAddr;
        return 0; // No more devices
    }
    
    static uint8_t crc8(const uint8_t *addr, uint8_t len) {
        printf("OneWire.crc8()\n");
        (void)addr; (void)len;
        return 0;
    }
    
    void select(const uint8_t rom[8]) {
        printf("OneWire.select()\n");
        (void)rom;
    }
    
    void skip() {
        printf("OneWire.skip()\n");
    }
    
    void write(uint8_t v, uint8_t power = 0) {
        printf("OneWire.write(0x%02X)\n", v);
        (void)power;
    }
    
    void write_bytes(const uint8_t *buf, uint16_t count, bool power = false) {
        printf("OneWire.write_bytes(%d bytes)\n", count);
        (void)buf; (void)power;
    }
    
    uint8_t read() {
        printf("OneWire.read() -> 0xFF\n");
        return 0xFF;
    }
    
    void read_bytes(uint8_t *buf, uint16_t count) {
        printf("OneWire.read_bytes(%d bytes)\n", count);
        memset(buf, 0xFF, count);
    }
    
    uint8_t reset() {
        printf("OneWire.reset() -> 1\n");
        return 1; // Device present
    }
    
private:
    uint8_t _pin;
};

#endif // HOST_SIM