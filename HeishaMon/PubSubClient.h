#ifndef PUBSUBCLIENT_H
#define PUBSUBCLIENT_H

#ifdef NATIVE_SIM

#include <string>
#include <functional>

class PubSubClient {
public:
    PubSubClient() {}
    PubSubClient(MockClient& client) {}
    
    bool connect(const char* id) { 
        std::cout << "[MQTT] Connected with ID: " << id << std::endl; 
        return true; 
    }
    bool connect(const char* id, const char* user, const char* pass) {
        std::cout << "[MQTT] Connected with ID: " << id << ", user: " << user << std::endl;
        return true;
    }
    void disconnect() { std::cout << "[MQTT] Disconnected" << std::endl; }
    bool connected() { return true; }
    
    bool publish(const char* topic, const char* payload) {
        std::cout << "[MQTT] Publish to " << topic << ": " << payload << std::endl;
        return true;
    }
    bool publish(const char* topic, const char* payload, bool retained) {
        std::cout << "[MQTT] Publish to " << topic << " (retain=" << retained << "): " << payload << std::endl;
        return true;
    }
    bool subscribe(const char* topic) {
        std::cout << "[MQTT] Subscribe to " << topic << std::endl;
        return true;
    }
    bool unsubscribe(const char* topic) {
        std::cout << "[MQTT] Unsubscribe from " << topic << std::endl;
        return true;
    }
    
    void setServer(const char* ip, uint16_t port) {
        std::cout << "[MQTT] Set server " << ip << ":" << port << std::endl;
    }
    void setCallback(std::function<void(char*, uint8_t*, unsigned int)> callback) {
        std::cout << "[MQTT] Set callback" << std::endl;
    }
    
    void loop() {
        // Mock MQTT processing
    }
    
    int state() { return 0; } // MQTT_CONNECTED
    
    // Constants
    static const int MQTT_CONNECTED = 0;
};

#else
#error "PubSubClient mock is only for NATIVE_SIM builds"
#endif

#endif // PUBSUBCLIENT_H