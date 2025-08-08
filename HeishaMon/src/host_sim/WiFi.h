#pragma once

#ifdef HOST_SIM

#include "Arduino.h"

// WiFi status constants
typedef enum {
    WL_NO_SHIELD = 255,
    WL_IDLE_STATUS = 0,
    WL_NO_SSID_AVAIL = 1,
    WL_SCAN_COMPLETED = 2,
    WL_CONNECTED = 3,
    WL_CONNECT_FAILED = 4,
    WL_CONNECTION_LOST = 5,
    WL_DISCONNECTED = 6
} wl_status_t;

// WiFi mode constants
typedef enum {
    WIFI_OFF = 0,
    WIFI_STA = 1,
    WIFI_AP = 2,
    WIFI_AP_STA = 3
} wifi_mode_t;

// WiFi power save constants
typedef enum {
    WIFI_PS_NONE = 0,
    WIFI_PS_MIN_MODEM = 1,
    WIFI_PS_MAX_MODEM = 2
} wifi_ps_type_t;

// IP Address mock
class IPAddress {
public:
    IPAddress() : _address(0) {}
    IPAddress(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
        _address = (uint32_t(a) << 24) | (uint32_t(b) << 16) | (uint32_t(c) << 8) | uint32_t(d);
    }
    IPAddress(uint32_t address) : _address(address) {}
    IPAddress(const char* address) {
        // Simple parser for IP strings
        uint8_t a, b, c, d;
        if (sscanf(address, "%hhu.%hhu.%hhu.%hhu", &a, &b, &c, &d) == 4) {
            _address = (uint32_t(a) << 24) | (uint32_t(b) << 16) | (uint32_t(c) << 8) | uint32_t(d);
        } else {
            _address = 0;
        }
    }
    
    String toString() const {
        return String((_address >> 24) & 0xFF) + "." +
               String((_address >> 16) & 0xFF) + "." +
               String((_address >> 8) & 0xFF) + "." +
               String(_address & 0xFF);
    }
    
    operator uint32_t() const { return _address; }
    
    uint8_t& operator[](int index) {
        return reinterpret_cast<uint8_t*>(&_address)[3-index];
    }
    
    uint8_t operator[](int index) const {
        return reinterpret_cast<const uint8_t*>(&_address)[3-index];
    }
    
private:
    uint32_t _address;
};

// WiFi class mock
class WiFiClass {
public:
    WiFiClass() : _status(WL_DISCONNECTED), _mode(WIFI_STA) {}
    
    void begin(const char* ssid, const char* passphrase = nullptr) {
        printf("WiFi.begin('%s', '%s')\n", ssid, passphrase ? passphrase : "");
        _ssid = ssid ? ssid : "";
        _status = WL_CONNECTED; // Simulate successful connection
    }
    
    void mode(wifi_mode_t mode) {
        _mode = mode;
        printf("WiFi.mode(%d)\n", mode);
    }
    
    wl_status_t status() {
        return _status;
    }
    
    void disconnect(bool wifioff = false) {
        _status = WL_DISCONNECTED;
        printf("WiFi.disconnect(%s)\n", wifioff ? "true" : "false");
    }
    
    IPAddress localIP() {
        // Return a mock IP address
        return IPAddress(192, 168, 1, 100);
    }
    
    IPAddress gatewayIP() {
        return IPAddress(192, 168, 1, 1);
    }
    
    IPAddress subnetMask() {
        return IPAddress(255, 255, 255, 0);
    }
    
    IPAddress dnsIP(uint8_t dns_no = 0) {
        (void)dns_no;
        return IPAddress(8, 8, 8, 8);
    }
    
    String SSID() {
        return _ssid;
    }
    
    int32_t RSSI() {
        return -50; // Mock signal strength
    }
    
    String macAddress() {
        return "AA:BB:CC:DD:EE:FF";
    }
    
    bool config(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dns1 = (uint32_t)0x00000000, IPAddress dns2 = (uint32_t)0x00000000) {
        printf("WiFi.config(%s, %s, %s)\n", local_ip.toString().c_str(), gateway.toString().c_str(), subnet.toString().c_str());
        (void)dns1; (void)dns2;
        return true;
    }
    
    void setSleep(wifi_ps_type_t type) {
        printf("WiFi.setSleep(%d)\n", type);
    }
    
    void hostname(const String& aHostname) {
        _hostname = aHostname;
        printf("WiFi.hostname('%s')\n", aHostname.c_str());
    }
    
    String hostname() {
        return _hostname;
    }
    
    bool softAP(const char* ssid, const char* passphrase = nullptr, int channel = 1, int ssid_hidden = 0, int max_connection = 4) {
        printf("WiFi.softAP('%s', '%s', %d, %d, %d)\n", ssid, passphrase ? passphrase : "", channel, ssid_hidden, max_connection);
        return true;
    }
    
    IPAddress softAPIP() {
        return IPAddress(192, 168, 4, 1);
    }
    
    void setOutputPower(float power) {
        printf("WiFi.setOutputPower(%.1f)\n", power);
    }
    
private:
    wl_status_t _status;
    wifi_mode_t _mode;
    String _ssid;
    String _hostname;
};

extern WiFiClass WiFi;

#endif // HOST_SIM