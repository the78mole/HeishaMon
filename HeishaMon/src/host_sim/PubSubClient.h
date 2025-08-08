#pragma once

#ifdef HOST_SIM

#include "Arduino.h"
#include "WiFi.h"

// MQTT client mock
class PubSubClient {
public:
    PubSubClient() {}
    
    bool connected() { return true; }
    bool connect(const char* id) { 
        printf("MQTT.connect('%s')\n", id);
        return true; 
    }
    bool connect(const char* id, const char* user, const char* pass) {
        printf("MQTT.connect('%s', '%s', '***')\n", id, user);
        return true;
    }
    bool connect(const char* id, const char* willTopic, uint8_t willQos, bool willRetain, const char* willMessage) {
        printf("MQTT.connect('%s', will='%s')\n", id, willTopic);
        return true;
    }
    bool connect(const char* id, const char* user, const char* pass, const char* willTopic, uint8_t willQos, bool willRetain, const char* willMessage) {
        printf("MQTT.connect('%s', '%s', will='%s')\n", id, user, willTopic);
        return true;
    }
    
    void disconnect() {
        printf("MQTT.disconnect()\n");
    }
    
    bool publish(const char* topic, const char* payload) {
        printf("MQTT.publish('%s', '%s')\n", topic, payload);
        return true;
    }
    bool publish(const char* topic, const char* payload, bool retained) {
        printf("MQTT.publish('%s', '%s', %s)\n", topic, payload, retained ? "retained" : "not-retained");
        return true;
    }
    bool publish(const char* topic, const uint8_t* payload, unsigned int length) {
        printf("MQTT.publish('%s', binary, %u bytes)\n", topic, length);
        return true;
    }
    bool publish(const char* topic, const uint8_t* payload, unsigned int length, bool retained) {
        printf("MQTT.publish('%s', binary, %u bytes, %s)\n", topic, length, retained ? "retained" : "not-retained");
        return true;
    }
    
    bool subscribe(const char* topic) {
        printf("MQTT.subscribe('%s')\n", topic);
        return true;
    }
    bool subscribe(const char* topic, uint8_t qos) {
        printf("MQTT.subscribe('%s', qos=%d)\n", topic, qos);
        return true;
    }
    
    bool unsubscribe(const char* topic) {
        printf("MQTT.unsubscribe('%s')\n", topic);
        return true;
    }
    
    bool loop() {
        // Mock - do nothing
        return true;
    }
    
    void setServer(const char* domain, uint16_t port) {
        printf("MQTT.setServer('%s', %d)\n", domain, port);
    }
    void setServer(uint8_t* ip, uint16_t port) {
        printf("MQTT.setServer(IP, %d)\n", port);
    }
    void setServer(IPAddress ip, uint16_t port) {
        printf("MQTT.setServer('%s', %d)\n", ip.toString().c_str(), port);
    }
    
    typedef void (*MQTT_CALLBACK_SIGNATURE)(char* topic, byte* payload, unsigned int length);
    void setCallback(MQTT_CALLBACK_SIGNATURE callback) {
        printf("MQTT.setCallback()\n");
        (void)callback;
    }
    
    void setKeepAlive(uint16_t keepAlive) {
        printf("MQTT.setKeepAlive(%d)\n", keepAlive);
    }
    
    void setSocketTimeout(uint16_t timeout) {
        printf("MQTT.setSocketTimeout(%d)\n", timeout);
    }
    
    int state() {
        return 0; // MQTT_CONNECTED
    }
};

#endif // HOST_SIM