#ifdef NATIVE_SIM

#include "native_mocks.h"

// Global mock object definitions
MockSerial Serial;
MockSerial Serial1;
MockWiFi WiFi;
MockWebServer* server = nullptr;
MockMDNS MDNS;
MockLittleFS LittleFS;
MockArduinoOTA ArduinoOTA;
MockDNSServer dnsServer;

// Global variables needed for simulation
std::string apIP = "192.168.4.1";

#endif // NATIVE_SIM