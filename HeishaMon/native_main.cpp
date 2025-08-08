#ifdef NATIVE_SIM

#include "native_mocks.h"
#include <ArduinoJson.h>

// Version info (simplified)
#ifndef HEISHAMON_VERSION
#define HEISHAMON_VERSION "3.5.0-simulation"
#endif

// Global mock instances
MockSerial Serial;
MockSerial Serial1;
MockWiFi WiFi;
MockWebServer* server = nullptr;
MockMDNS MDNS;
MockLittleFS LittleFS;
MockArduinoOTA ArduinoOTA;
MockDNSServer dnsServer;

// Simplified HeishaMon simulation setup
void setup() {
    std::cout << "[SETUP] HeishaMon Native Simulation v" << HEISHAMON_VERSION << std::endl;
    
    // Initialize mock serial
    Serial.begin(115200);
    Serial1.begin(9600);
    
    // Mock WiFi setup
    WiFi.begin("HeishaMon_WiFi");
    std::cout << "[WIFI] Connected to " << WiFi.localIP() << std::endl;
    
    // Mock MQTT connection
    std::cout << "[MQTT] Connected with ID: HeishaMon-12345" << std::endl;
    
    // Mock filesystem
    LittleFS.begin();
    
    // Mock web server
    server = new MockWebServer(80);
    server->begin();
    
    std::cout << "[SETUP] Initialization complete" << std::endl;
}

// Simplified HeishaMon simulation loop
void loop() {
    static int loop_count = 0;
    static unsigned long last_data_time = 0;
    
    unsigned long current_time = millis();
    
    // Simulate periodic heat pump data reading every 1 second (faster for demo)
    if (current_time - last_data_time > 1000) {
        last_data_time = current_time;
        
        // Simulate receiving heat pump data
        std::cout << "[HEATPUMP] Simulating data read #" << (++loop_count) << std::endl;
        
        // Create mock heat pump data packet
        byte mock_data[203] = {0};
        mock_data[0] = 0x71; // Start byte
        mock_data[1] = 0x6c; // Command
        mock_data[2] = 0x01; // Sub-command
        mock_data[3] = 0x10; // Data length
        
        // Add some mock temperature values
        mock_data[11] = 25;  // Room temperature (25°C)
        mock_data[16] = 45;  // Outlet water temp (45°C)
        mock_data[17] = 35;  // Inlet water temp (35°C)
        
        // Simulate data processing
        std::cout << "[DECODE] Processing heat pump data..." << std::endl;
        std::cout << "[DECODE] Room temperature: 25°C" << std::endl;
        std::cout << "[DECODE] Outlet water: 45°C" << std::endl;
        std::cout << "[DECODE] Inlet water: 35°C" << std::endl;
        
        // Simulate MQTT publishing
        std::cout << "[MQTT] Publishing to panasonic_heat_pump/main/Room_Thermostat_Temp: 25" << std::endl;
        std::cout << "[MQTT] Publishing to panasonic_heat_pump/main/DHW_Temp: 45" << std::endl;
        std::cout << "[MQTT] Publishing to panasonic_heat_pump/main/Outlet_Water_Temp: 35" << std::endl;
        
        // Simulate JSON API
        DynamicJsonDocument json(1024);
        json["Room_Thermostat_Temp"] = 25;
        json["DHW_Temp"] = 45;
        json["Outlet_Water_Temp"] = 35;
        json["timestamp"] = current_time;
        
        std::string json_str;
        serializeJson(json, json_str);
        std::cout << "[JSON API] Data available at /json: " << json_str.substr(0, 100) << "..." << std::endl;
    }
    
    // Simulate web server handling
    if (server) {
        server->handleClient();
    }
    
    // Simulate command processing
    if (loop_count % 50 == 0) {
        std::cout << "[COMMANDS] Ready to process MQTT commands" << std::endl;
    }
    
    // Small delay to prevent excessive output
    delay(100);
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "HeishaMon Native Simulation Starting..." << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "This simulation demonstrates HeishaMon functionality" << std::endl;
    std::cout << "without requiring ESP8266/ESP32 hardware." << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Create the tmp directory for simulated filesystem
    system("mkdir -p /tmp/heishamon_sim");
    
    // Call Arduino-style setup
    setup();
    
    std::cout << "========================================" << std::endl;
    std::cout << "Starting main simulation loop..." << std::endl;
    std::cout << "Press Ctrl+C to exit." << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Run the main loop for a limited time for demo
    int loop_iterations = 0;
    const int max_loops = 50; // Limit for demo purposes
    
    while (loop_iterations < max_loops) {
        loop();
        loop_iterations++;
        
        if (loop_iterations % 10 == 0) {
            std::cout << "[INFO] Simulation running... (" << loop_iterations << "/" << max_loops << " loops)" << std::endl;
        }
    }
    
    std::cout << "========================================" << std::endl;
    std::cout << "HeishaMon Native Simulation Complete!" << std::endl;
    std::cout << "Ran for " << loop_iterations << " iterations" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Cleanup
    delete server;
    
    return 0;
}

#endif // NATIVE_SIM