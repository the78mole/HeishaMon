#ifndef ARDUINOJSON_H
#define ARDUINOJSON_H

#ifdef NATIVE_SIM

#include <string>
#include <map>
#include <vector>
#include <iostream>

// Simple mock for ArduinoJson - provides basic functionality for simulation
namespace ArduinoJson {
    
class JsonDocument {
public:
    std::map<std::string, std::string> data;
    
    std::string& operator[](const char* key) {
        return data[key];
    }
    
    const std::string& operator[](const char* key) const {
        static std::string empty;
        auto it = data.find(key);
        return (it != data.end()) ? it->second : empty;
    }
    
    void clear() { data.clear(); }
    
    bool containsKey(const char* key) const {
        return data.find(key) != data.end();
    }
    
    size_t memoryUsage() const { return data.size() * 100; } // Mock value
    
    std::string serializeToString() const {
        std::string result = "{";
        bool first = true;
        for (const auto& pair : data) {
            if (!first) result += ",";
            result += "\"" + pair.first + "\":\"" + pair.second + "\"";
            first = false;
        }
        result += "}";
        return result;
    }
};

// Mock types
typedef JsonDocument DynamicJsonDocument;
typedef JsonDocument StaticJsonDocument;

// Mock functions
inline size_t serializeJson(const JsonDocument& doc, char* buffer, size_t size) {
    std::string result = doc.serializeToString();
    strncpy(buffer, result.c_str(), size - 1);
    buffer[size - 1] = '\0';
    return result.length();
}

inline size_t serializeJson(const JsonDocument& doc, std::string& output) {
    output = doc.serializeToString();
    return output.length();
}

inline void deserializeJson(JsonDocument& doc, const char* input) {
    std::cout << "[JSON] Mock deserialize: " << input << std::endl;
    // Very basic mock - just store the input as a single key
    doc.data["raw"] = input;
}

} // namespace ArduinoJson

using namespace ArduinoJson;

#else
#error "ArduinoJson mock is only for NATIVE_SIM builds"
#endif

#endif // ARDUINOJSON_H