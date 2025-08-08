#ifdef HOST_SIM

#include <chrono>
#include <thread>
#include <unistd.h>
#include "Arduino.h"

// Global instances
SerialClass Serial;
SerialClass Serial1;

// Arduino function implementations
void pinMode(int pin, int mode) {
    // Mock implementation - just print for debugging
    printf("pinMode(%d, %d)\n", pin, mode);
}

void digitalWrite(int pin, int value) {
    // Mock implementation - just print for debugging
    printf("digitalWrite(%d, %d)\n", pin, value);
}

int digitalRead(int pin) {
    // Mock implementation - always return LOW
    printf("digitalRead(%d) -> 0\n", pin);
    return LOW;
}

int analogRead(int pin) {
    // Mock implementation - return a dummy value
    printf("analogRead(%d) -> 512\n", pin);
    return 512;
}

void analogWrite(int pin, int value) {
    // Mock implementation - just print for debugging
    printf("analogWrite(%d, %d)\n", pin, value);
}

unsigned long millis() {
    static auto start = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
    return duration.count();
}

unsigned long micros() {
    static auto start = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - start);
    return duration.count();
}

void delay(unsigned long ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void delayMicroseconds(unsigned int us) {
    std::this_thread::sleep_for(std::chrono::microseconds(us));
}

long map(long value, long fromLow, long fromHigh, long toLow, long toHigh) {
    return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
}

unsigned int makeWord(unsigned int w) {
    return w;
}

unsigned int makeWord(unsigned char h, unsigned char l) {
    return (h << 8) | l;
}

// Host simulation main function
int main() {
    printf("HeishaMon Host Simulation Starting...\n");
    
    // Call Arduino setup
    setup();
    
    printf("Setup complete, entering main loop...\n");
    
    // Run Arduino loop
    while (true) {
        loop();
        // Small delay to prevent 100% CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    return 0;
}

#endif // HOST_SIM