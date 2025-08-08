#ifdef NATIVE_SIM

#include "native_mocks.h"
#include <iostream>

// Simple test main function for native simulation
int main() {
    std::cout << "HeishaMon Native Simulation Starting..." << std::endl;
    
    // Test basic mock functionality
    Serial.begin(115200);
    Serial.println("Testing Serial mock");
    
    WiFi.begin("test_ssid", "test_password");
    std::cout << "WiFi IP: " << WiFi.localIP() << std::endl;
    
    std::cout << "Basic native simulation test completed." << std::endl;
    std::cout << "For full simulation, include the actual HeishaMon main loop." << std::endl;
    
    return 0;
}

#endif // NATIVE_SIM