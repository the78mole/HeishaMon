#ifdef HOST_SIM

#include "src/host_sim/Arduino.h"
#include "src/host_sim/WiFi.h"
#include "src/host_sim/PubSubClient.h"
#include <ArduinoJson.h>

// HeishaMon Host Simulation
// This is a simplified version of HeishaMon that can run on a host PC for testing and development

// Simulation configuration
struct SimConfig {
    const char* wifi_ssid = "HeishaMon-Sim";
    const char* wifi_password = "simulator";
    const char* mqtt_server = "localhost";
    int mqtt_port = 1883;
    const char* mqtt_username = "heishamon";
    const char* mqtt_password = "password";
    bool debug_output = true;
};

SimConfig config;
PubSubClient mqttClient;
unsigned long lastStatusUpdate = 0;
unsigned long lastHeatPumpQuery = 0;
const unsigned long STATUS_INTERVAL = 30000; // 30 seconds
const unsigned long HEATPUMP_QUERY_INTERVAL = 5000; // 5 seconds

// Mock heat pump data
struct HeatPumpData {
    float outdoor_temp = 5.5;
    float inlet_temp = 25.0;
    float outlet_temp = 45.0;
    float target_temp = 45.0;
    bool heating_active = true;
    bool pump_active = true;
    int compressor_frequency = 35;
    float dhw_temp = 55.0;
    float dhw_target = 60.0;
    int operation_mode = 4; // Heat+DHW
    bool defrost_active = false;
};

HeatPumpData heatPumpData;

void initializeHostSim() {
    printf("=== HeishaMon Host Simulation Starting ===\n");
    printf("Version: Host-Sim v1.0\n");
    printf("Compile time: %s %s\n", __DATE__, __TIME__);
    printf("Configuration:\n");
    printf("  WiFi SSID: %s\n", config.wifi_ssid);
    printf("  MQTT Server: %s:%d\n", config.mqtt_server, config.mqtt_port);
    printf("  Debug Output: %s\n", config.debug_output ? "enabled" : "disabled");
    printf("==========================================\n\n");
}

void connectWiFi() {
    if (config.debug_output) {
        printf("Connecting to WiFi: %s\n", config.wifi_ssid);
    }
    
    WiFi.begin(config.wifi_ssid, config.wifi_password);
    
    // Simulate connection delay
    delay(2000);
    
    if (config.debug_output) {
        printf("WiFi connected! IP: %s\n", WiFi.localIP().toString().c_str());
    }
}

void connectMQTT() {
    if (config.debug_output) {
        printf("Connecting to MQTT server: %s:%d\n", config.mqtt_server, config.mqtt_port);
    }
    
    mqttClient.setServer(config.mqtt_server, config.mqtt_port);
    
    if (mqttClient.connect("HeishaMon-Sim", config.mqtt_username, config.mqtt_password)) {
        if (config.debug_output) {
            printf("MQTT connected successfully\n");
        }
        
        // Subscribe to command topics
        mqttClient.subscribe("panasonic_heat_pump/commands/+");
        mqttClient.subscribe("panasonic_heat_pump/SetHeatpump");
        mqttClient.subscribe("panasonic_heat_pump/SetPump");
        
        if (config.debug_output) {
            printf("Subscribed to command topics\n");
        }
    } else {
        if (config.debug_output) {
            printf("MQTT connection failed\n");
        }
    }
}

void simulateHeatPumpData() {
    // Simulate realistic heat pump behavior
    static float time_factor = 0.0;
    time_factor += 0.1;
    
    // Simulate outdoor temperature variation (sine wave between -5 and 15°C)
    heatPumpData.outdoor_temp = 5.0 + 10.0 * sin(time_factor * 0.1);
    
    // Simulate compressor frequency based on outdoor temp
    if (heatPumpData.outdoor_temp < 0) {
        heatPumpData.compressor_frequency = 45 + (int)(10 * sin(time_factor * 0.5));
    } else {
        heatPumpData.compressor_frequency = 25 + (int)(15 * sin(time_factor * 0.3));
    }
    
    // Simulate outlet temperature based on compressor activity
    heatPumpData.outlet_temp = heatPumpData.target_temp + 
                               (heatPumpData.compressor_frequency - 30) * 0.2 + 
                               2.0 * sin(time_factor * 0.8);
    
    // Simulate inlet temperature (always lower than outlet when heating)
    heatPumpData.inlet_temp = heatPumpData.outlet_temp - 5.0 - 2.0 * sin(time_factor * 0.6);
    
    // Simulate DHW temperature
    heatPumpData.dhw_temp = heatPumpData.dhw_target - 3.0 + 6.0 * sin(time_factor * 0.2);
    
    // Simulate defrost cycle (occasional)
    heatPumpData.defrost_active = (heatPumpData.outdoor_temp < -2.0) && 
                                  (fmod(time_factor, 20.0) < 2.0);
    
    // Pump is active when heating
    heatPumpData.pump_active = heatPumpData.heating_active && !heatPumpData.defrost_active;
}

void publishHeatPumpStatus() {
    // Create JSON status message
    DynamicJsonDocument doc(1024);
    
    doc["outdoor_temp"] = heatPumpData.outdoor_temp;
    doc["inlet_temp"] = heatPumpData.inlet_temp;
    doc["outlet_temp"] = heatPumpData.outlet_temp;
    doc["target_temp"] = heatPumpData.target_temp;
    doc["heating_active"] = heatPumpData.heating_active;
    doc["pump_active"] = heatPumpData.pump_active;
    doc["compressor_frequency"] = heatPumpData.compressor_frequency;
    doc["dhw_temp"] = heatPumpData.dhw_temp;
    doc["dhw_target"] = heatPumpData.dhw_target;
    doc["operation_mode"] = heatPumpData.operation_mode;
    doc["defrost_active"] = heatPumpData.defrost_active;
    doc["timestamp"] = millis();
    
    String jsonString;
    serializeJson(doc, jsonString);
    
    mqttClient.publish("panasonic_heat_pump/main/values", jsonString.c_str());
    
    // Also publish individual values
    char temp_str[10];
    
    sprintf(temp_str, "%.1f", heatPumpData.outdoor_temp);
    mqttClient.publish("panasonic_heat_pump/main/Outdoor_Temp", temp_str);
    
    sprintf(temp_str, "%.1f", heatPumpData.inlet_temp);
    mqttClient.publish("panasonic_heat_pump/main/Inlet_Temp", temp_str);
    
    sprintf(temp_str, "%.1f", heatPumpData.outlet_temp);
    mqttClient.publish("panasonic_heat_pump/main/Outlet_Temp", temp_str);
    
    sprintf(temp_str, "%d", heatPumpData.compressor_frequency);
    mqttClient.publish("panasonic_heat_pump/main/Compressor_Freq", temp_str);
    
    mqttClient.publish("panasonic_heat_pump/main/Pump_Flow", heatPumpData.pump_active ? "1" : "0");
    mqttClient.publish("panasonic_heat_pump/main/Defrost_Operation", heatPumpData.defrost_active ? "1" : "0");
    
    if (config.debug_output) {
        printf("Published heat pump status: Outdoor=%.1f°C, Outlet=%.1f°C, Compressor=%d%%, Defrost=%s\n",
               heatPumpData.outdoor_temp, heatPumpData.outlet_temp, 
               heatPumpData.compressor_frequency, 
               heatPumpData.defrost_active ? "ON" : "OFF");
    }
}

void processCommands() {
    // Process any incoming MQTT commands
    if (mqttClient.connected()) {
        mqttClient.loop();
    }
}

void setup() {
    initializeHostSim();
    connectWiFi();
    connectMQTT();
    
    // Publish initial status
    publishHeatPumpStatus();
    
    printf("Host simulation setup complete. Entering main loop...\n\n");
}

void loop() {
    unsigned long currentTime = millis();
    
    // Simulate heat pump data continuously
    simulateHeatPumpData();
    
    // Process MQTT commands
    processCommands();
    
    // Publish status updates at regular intervals
    if (currentTime - lastStatusUpdate >= STATUS_INTERVAL) {
        publishHeatPumpStatus();
        lastStatusUpdate = currentTime;
    }
    
    // Simulate heat pump query cycle
    if (currentTime - lastHeatPumpQuery >= HEATPUMP_QUERY_INTERVAL) {
        if (config.debug_output && (currentTime % 30000 < 100)) { // Log every 30 seconds
            printf("Heat pump simulation running... (uptime: %lu seconds)\n", currentTime / 1000);
        }
        lastHeatPumpQuery = currentTime;
    }
    
    // Small delay to prevent excessive CPU usage
    delay(100);
}

#endif // HOST_SIM