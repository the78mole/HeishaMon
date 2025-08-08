# HeishaMon Native Host Simulation

This directory contains a complete Host Simulation environment for HeishaMon, allowing the firmware to run on development machines (PC/Mac/Linux) for testing and debugging without requiring ESP8266/ESP32 hardware.

## Features

- **✅ Native PlatformIO Target**: Complete `[env:native]` configuration for host simulation
- **✅ Comprehensive Mock System**: Arduino/ESP API mocking including WiFi, Serial, GPIO, LittleFS, MQTT, and web server
- **✅ Cross-Platform Development**: Runs on any platform that supports PlatformIO
- **✅ Real-Time Debugging**: All hardware operations output to console with detailed logging
- **✅ Heat Pump Simulation**: Mock heat pump data processing and MQTT publishing
- **✅ JSON API Simulation**: Complete JSON API endpoint simulation
- **✅ Clean Build System**: Proper source filtering and library management

## Quick Start

### Demo the Simulation

```bash
# Run the complete demonstration
./demo_simulation.sh
```

### Using PlatformIO

```bash
# Build the simulation
pio run -e native

# Run the simulation
pio run -e native --target exec
```

## Simulation Output

The simulation provides rich console output showing all operations:

```
========================================
HeishaMon Native Simulation Starting...
========================================
[SETUP] HeishaMon Native Simulation v3.5.0-simulation
[SERIAL] Begin at 115200 baud
[WIFI] Connecting to HeishaMon_WiFi
[MQTT] Connected with ID: HeishaMon-12345
[LITTLEFS] Started
[WEBSERVER] Started on port 80

[HEATPUMP] Simulating data read #1
[DECODE] Processing heat pump data...
[DECODE] Room temperature: 25°C
[DECODE] Outlet water: 45°C
[DECODE] Inlet water: 35°C
[MQTT] Publishing to panasonic_heat_pump/main/Room_Thermostat_Temp: 25
[JSON API] Data available at /json: {"Room_Thermostat_Temp":25,...}
```

## Architecture

### Mock System

The simulation uses conditional compilation with `NATIVE_SIM` to replace ESP-specific code:

```cpp
#ifdef NATIVE_SIM
#include "native_mocks.h"  // Host simulation
#else
#include <ESP8266WiFi.h>   // Real hardware
#endif
```

### Core Components

- **`native_main.cpp`**: Simplified main simulation loop
- **`native_mocks.h/cpp`**: Complete Arduino/ESP API mocking
- **Mock Headers**: Individual library mocks (OneWire.h, LittleFS.h, etc.)
- **Source Filtering**: PlatformIO build filters to include only simulation-compatible code

### Simulated Operations

- **WiFi Management**: Connection simulation with mock IP addresses
- **MQTT Communication**: Message publishing with console output
- **File System**: LittleFS operations using `/tmp/heishamon_sim`
- **Heat Pump Protocol**: Mock data packet processing and decoding
- **JSON API**: ArduinoJson integration for configuration data
- **Web Server**: Mock HTTP server for configuration interface

## Current Status

- ✅ **Complete Native Simulation**: Fully functional host environment
- ✅ **PlatformIO Integration**: Native target builds and runs successfully
- ✅ **Arduino API Mocking**: Serial, WiFi, GPIO, File System operations
- ✅ **MQTT Simulation**: Publisher simulation with realistic output
- ✅ **Heat Pump Data Processing**: Mock sensor data generation and parsing
- ✅ **JSON API**: Configuration data serialization/deserialization
- ✅ **Build System**: Clean source filtering and dependency management
- ✅ **Demo Script**: Complete demonstration with `./demo_simulation.sh`

## Development Benefits

### Rapid Development Cycles
- No need to flash hardware for testing basic functionality
- Instant feedback on code changes
- Real-time debugging with console output

### Testing Capabilities
- Configuration logic validation
- MQTT communication protocol testing
- JSON API endpoint verification
- Heat pump data processing validation

### Continuous Integration
- Automated testing without hardware requirements
- Build verification for multiple targets
- Protocol validation in CI pipelines

## Configuration

The native environment is configured in `platformio.ini`:

```ini
[env:native]
platform = native
targets = exec
build_flags = -DNATIVE_SIM -std=c++11 -pthread
lib_deps = bblanchon/ArduinoJson@^6.21.3
build_src_filter = +<native_main.cpp> +<native_mocks.cpp> -<*.ino> ...
```

## Limitations & Design Decisions

- **Simplified Architecture**: Focuses on core functionality rather than complete hardware emulation
- **Mock Hardware**: No real network operations or hardware timing constraints
- **Limited Scope**: OpenTherm and complex hardware features are excluded for simplicity
- **Demo Focus**: Designed to demonstrate capability rather than replace hardware testing

## Future Enhancements

- **Extended Hardware Mocking**: Add support for more ESP-specific features
- **Real Network Testing**: Optional real MQTT broker connectivity
- **Configuration Persistence**: File-based configuration storage
- **Test Framework Integration**: Automated unit testing capabilities
- **Protocol Emulation**: More sophisticated heat pump protocol simulation

## Usage in Development

This simulation environment enables:

1. **Algorithm Development**: Test heat pump data processing logic
2. **Configuration Testing**: Validate JSON configuration parsing
3. **MQTT Integration**: Verify message publishing and topic structures
4. **API Development**: Test web interface and JSON API endpoints
5. **CI/CD Integration**: Automated testing without hardware dependencies

The simulation provides a practical development environment that bridges the gap between pure unit tests and full hardware integration testing.