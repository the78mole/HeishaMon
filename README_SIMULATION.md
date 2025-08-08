# HeishaMon Native Host Simulation

This directory contains the Host Simulation environment for HeishaMon, allowing the firmware to run on a development PC for testing and debugging without requiring actual ESP8266/ESP32 hardware.

## Features

- **Mock Hardware APIs**: Complete simulation of Arduino/ESP functions including WiFi, Serial, GPIO, and file system operations
- **MQTT Simulation**: Mock MQTT client with console output for testing communication protocols  
- **Web Interface Simulation**: Mock web server for testing configuration interfaces
- **Cross-platform**: Runs on Linux, macOS, and Windows development machines
- **Real-time Console Output**: All hardware operations are logged to console for debugging

## Quick Start

### Using PlatformIO (Recommended)

```bash
# Build the native simulation target
pio run -e native

# Run the simulation  
pio run -e native --target exec
```

### Using Make (Alternative)

```bash
# Test basic mock functionality
make -f Makefile.native test-mocks

# Build core simulation (work in progress)
make -f Makefile.native build-core
```

## Architecture

### Mock System

The simulation environment provides complete mocking of ESP hardware APIs:

- **`native_mocks.h/cpp`**: Core mock implementations for Arduino APIs
- **`Arduino.h`**: Mock Arduino header with NATIVE_SIM conditionals
- **`ArduinoJson.h`**: Mock JSON library for configuration parsing
- **`PubSubClient.h`**: Mock MQTT client for testing communication

### Conditional Compilation

Code is conditionally compiled using the `NATIVE_SIM` preprocessor macro:

```cpp
#ifdef NATIVE_SIM
#include "native_mocks.h"
#else
#include <ESP8266WiFi.h>  // Real hardware
#endif
```

### Simulated Hardware Operations

All hardware operations produce console output for debugging:

```
[SERIAL] Begin at 115200 baud
[WIFI] Connecting to test_ssid
[MQTT] Connected with ID: HeishaMon
[GPIO] pinMode(5, 1)
[LITTLEFS] Started
```

## Current Status

- ✅ Basic mock infrastructure complete
- ✅ PlatformIO native environment configured  
- ✅ Arduino API mocking (Serial, WiFi, GPIO, LittleFS)
- ✅ MQTT and JSON mocking
- 🚧 Integration with full HeishaMon source code
- 🚧 Complete library dependency mocking
- 🚧 Heat pump protocol simulation

## Development

### Adding New Mocks

To add support for additional libraries:

1. Create mock header in `HeishaMon/LibraryName.h`
2. Add `#ifdef NATIVE_SIM` conditional to existing includes
3. Implement mock functionality with console output
4. Update build configuration as needed

### Testing Changes

The simulation environment is ideal for:

- Testing configuration logic without hardware
- Debugging MQTT communication protocols  
- Validating web interface functionality
- Unit testing individual components
- Continuous integration testing

### Limitations

- No actual heat pump communication (mocked)
- No real network operations (simulated)
- Limited to functionality that doesn't require hardware timing
- Some ESP-specific features may not be fully supported

## Future Enhancements

- Complete heat pump protocol emulation
- Real network socket support for MQTT testing
- Web interface with actual HTTP server
- Configuration file persistence
- Automated testing framework integration