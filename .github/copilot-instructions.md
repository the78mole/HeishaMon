# HeishaMon - Panasonic Heat Pump Monitor

HeishaMon is an Arduino-based firmware project for ESP8266 and ESP32-S3 microcontrollers that interfaces with Panasonic Aquarea heat pump systems to provide MQTT monitoring and control capabilities.

Always reference these instructions first and fallback to search or bash commands only when you encounter unexpected information that does not match the info here.

## Working Effectively

### Environment Setup
- **IMPORTANT**: Arduino CLI setup requires internet connectivity to download packages
- Install Arduino CLI:
  ```bash
  curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
  export PATH=$PATH:./bin
  ```
- Configure Arduino CLI board manager URLs:
  ```bash
  export ARDUINO_BOARD_MANAGER_ADDITIONAL_URLS="https://arduino.esp8266.com/stable/package_esp8266com_index.json https://espressif.github.io/arduino-esp32/package_esp32_dev_index.json"
  ```
- Update core index and install platforms (requires internet):
  ```bash
  arduino-cli core update-index
  arduino-cli core install esp8266:esp8266
  # ESP32 core version is pinned to 3.0.7 for compatibility reasons.
  # If you wish to use a newer version, ensure you test thoroughly as later versions may introduce breaking changes or incompatibilities with this firmware.
  # Version 3.0.7 is the last known stable version tested with HeishaMon as of [last update]. Update this note if future versions are validated.
  arduino-cli core install esp32:esp32@3.0.7
  ```
  **Installation time**: 5-10 minutes depending on connection. NEVER CANCEL. Set timeout to 20+ minutes.
- Install required libraries (requires internet):
  ```bash
  arduino-cli lib install ringbuffer pubsubclient arduinojson dallastemperature onewire "Adafruit NeoPixel"
  ```
  **Installation time**: 2-3 minutes. NEVER CANCEL. Set timeout to 10+ minutes.
- Fix OneWire library for ESP32-3.0.0 compatibility:
  ```bash
  sed -i '/#include <driver\/rtc_io\.h>/a\
  #include <soc\/gpio_struct\.h>' \
  "$HOME/Arduino/libraries/OneWire/util/OneWire_direct_gpio.h"
  ```

### Building the Firmware
- **IMPORTANT**: This is embedded firmware, not a traditional application - it compiles to .bin files for flashing to microcontrollers
- **CRITICAL BUILD TIMING**: Arduino builds typically take 2-5 minutes. NEVER CANCEL builds. Set timeout to 10+ minutes minimum.
- Build for ESP8266 (Wemos D1 Mini):
  ```bash
  cd HeishaMon
  arduino-cli compile --output-dir . \
    --fqbn=esp8266:esp8266:d1_mini:xtal=160,vt=flash,ssl=basic,mmu=3216,non32xfer=fast,eesz=4M2M,ip=lm2f,dbg=Disabled,lvl=None____,wipe=none,baud=921600 \
    --warnings=none --verbose HeishaMon.ino
  ```
  Build time: 2-3 minutes. NEVER CANCEL. Set timeout to 10+ minutes.

- Build for ESP32-S3:
  ```bash
  cd HeishaMon
  arduino-cli compile --output-dir . \
    --fqbn=esp32:esp32:esp32s3:CDCOnBoot=cdc,PSRAM=enabled,PartitionScheme=min_spiffs \
    --warnings=none --verbose HeishaMon.ino
  ```
  Build time: 3-4 minutes. NEVER CANCEL. Set timeout to 10+ minutes.

- Alternative build using scripts:
  ```bash
  # For ESP8266
  ./scripts/build_esp8266.sh
  # For ESP32-S3
  ./scripts/build_esp32s3.sh
  ```

### Testing and Validation
- **IMPORTANT**: This project has NO unit tests - it's embedded firmware for specific hardware
- **Validation approach**: Use protocol checksum validator:
  ```bash
  node Tools/chksumChecker.js
  ```
  **Execution time**: ~40ms - validates communication protocol checksums for heat pump commands.

- **Manual validation scenarios**: 
  - **CRITICAL**: This is embedded firmware - full validation requires actual hardware
  - Connect to actual Panasonic heat pump hardware via CN-CNT or CN-NMODE connector
  - Test initial WiFi hotspot mode: Device creates "HeishaMon" hotspot, config at http://192.168.4.1
  - Test MQTT connectivity and data publishing to configured MQTT broker
  - Verify web interface functionality at device IP address
  - Test JSON API endpoint: http://device-ip/json should return all sensor data
  - Test command sending via MQTT topics (see MQTT-Topics.md)
  - **For OpenTherm boards**: Test thermostat bridge functionality
  - **For large boards**: Test relay control via MQTT (panasonic_heat_pump/gpio/relay/one|two)
  - **For 1-wire setups**: Verify DS18B20 sensor readings
  - **For S0 setups**: Test kWh meter pulse counting
  - **WARNING**: Cannot be fully tested without actual hardware - this is specialized IoT firmware

### Code Quality and Linting
- Format C/C++ code using clang-format:
  ```bash
  find HeishaMon -name "*.cpp" -o -name "*.h" | xargs clang-format -i
  ```
  **Formatting time**: ~3 seconds for all 72 source files. Set timeout to 30+ seconds.
- **ALWAYS run formatting before committing** - the CI will fail without proper formatting
- Check formatting without changes (dry run):
  ```bash
  find HeishaMon -name "*.cpp" -exec clang-format --dry-run {} \;
  ```
- Pre-commit hooks configuration available in `.pre-commit-config.yaml`
- Install pre-commit hooks (when pre-commit is available):
  ```bash
  pre-commit install
  ```

### Development Workflow
- **Primary development targets**: ESP8266 (Wemos D1 Mini) and ESP32-S3
- **Flash requirements**: Minimum 4MB flash memory on ESP8266
- **Runtime environment**: Connects to Panasonic heat pumps via TTL 5V UART 9600,8,E,1
- **Network capabilities**: WiFi hotspot mode for initial config, MQTT client for data publishing
- **Power requirements**: Powered from heat pump connector (5V) or external 5V supply for long distances

## Project Structure

### Key Directories and Files
```
HeishaMon/
├── HeishaMon.ino              # Main Arduino sketch
├── HeishaOT.cpp/.h           # OpenTherm functionality  
├── commands.cpp/.h           # Heat pump command handling
├── dallas.cpp/.h             # 1-wire temperature sensors
├── decode.cpp/.h             # Protocol decoding
├── gpio.cpp/.h               # GPIO control (relays, etc.)
├── rules.cpp/.h              # Rules engine for automation
├── s0.cpp/.h                 # S0 pulse counter for kWh meters
├── webfunctions.cpp/.h       # Web interface
├── htmlcode.h                # Web UI HTML content
├── version.h                 # Version information
└── src/                      # Library source code
    ├── common/               # Common utilities
    ├── opentherm/            # OpenTherm protocol
    └── rules/                # Rules engine components

.github/workflows/main.yml     # Build automation
scripts/                      # Build scripts
Tools/chksumChecker.js        # Protocol validation
```

### Important Documentation Files
- `README.md` - Main project documentation
- `MQTT-Topics.md` - Complete MQTT topic reference
- `ProtocolByteDecrypt.md` - Heat pump protocol documentation  
- `LIBSUSED.md` - Required Arduino libraries
- `HeatPumpType.md` - Compatible heat pump models
- `Integrations/` - Home automation platform examples

## Common Tasks

### Repository Commands
```bash
# Repository root contents
ls -la
# Shows: HeishaMon/ (source), scripts/, .github/, README.md, etc.

# Build artifacts location  
ls HeishaMon/
# After build: HeishaMon.ino.bin (ESP8266) or HeishaMon.ino.bin (ESP32)

# View main sketch
head -50 HeishaMon/HeishaMon.ino
# Arduino setup() and loop() functions
```

### Git Workflow
- **Main branch**: `main` contains stable releases
- **Development**: Feature branches merged via pull requests
- **CI/CD**: GitHub Actions automatically builds both ESP8266 and ESP32 targets
- **Artifacts**: Binary files available as GitHub Actions artifacts
- **Versioning**: Version updated automatically in non-main branches

### Hardware-Specific Information
- **ESP8266 target**: Wemos D1 Mini compatible boards, 4MB flash minimum
- **ESP32-S3 target**: Larger boards with PSRAM enabled
- **Connectivity**: 
  - GPIO13/GPIO15 for heat pump communication
  - GPIO2 for debug serial output
  - GPIO4 for 1-wire sensors (Dallas DS18B20)
  - GPIO12/GPIO14 for S0 pulse counters
  - GPIO21/GPIO47 for relays (ESP32-S3 only)

### Integration Points
- **MQTT Topics**: `panasonic_heat_pump/*` - see MQTT-Topics.md for complete list
- **Web Interface**: Configuration and monitoring at device IP address
- **JSON API**: All data available at `http://device-ip/json`
- **Rules Engine**: Internal automation scripting for advanced control
- **1-wire sensors**: External temperature monitoring
- **S0 counters**: kWh meter integration
- **OpenTherm**: Thermostat bridge functionality (on compatible boards)

### Troubleshooting
- **Build failures**: Check library versions and board package versions
- **Flash errors**: Ensure minimum 4MB flash and correct board selection
- **Communication issues**: Verify TTL voltage levels and wiring
- **Network problems**: Use serial debug output on GPIO2
- **Reset to defaults**: Double-reset procedure or web interface factory reset

### What NOT to Do
- **DO NOT try to "run" the firmware locally** - this is embedded code for microcontrollers
- **DO NOT expect unit tests** - embedded firmware testing requires hardware
- **DO NOT attempt to install as a service** - this flashes to microcontroller hardware
- **DO NOT use standard debugging tools** - use Serial output and hardware debug probes
- **DO NOT expect fast iteration** - each test cycle requires flashing to hardware
- **DO NOT send commands rapidly to heat pump** - EEPROM wear limits apply (few per hour max)
- **DO NOT modify networking code without understanding TTL voltage requirements**

## CI/CD Pipeline
The GitHub Actions workflow (`.github/workflows/main.yml`) automatically:
1. Updates version.h for non-main branches
2. Installs Arduino CLI and required platforms
3. Installs all library dependencies
4. Applies ESP32 library compatibility fixes
5. Compiles for both ESP8266 and ESP32-S3 targets
6. Generates MD5-labeled binary files
7. Uploads binaries as GitHub Actions artifacts

**Build time in CI**: 5-8 minutes total for both targets. Production builds may take longer.

## Development Environment
- **DevContainer**: Configuration available in `.devcontainer/`
- **Docker image**: `ghcr.io/the78mole/heishamon-dev:latest`
- **IDE support**: Arduino IDE, PlatformIO, or VS Code with Arduino extension
- **Debugging**: Serial output on GPIO2, web console logs

This is specialized IoT firmware - treat it as an embedded project, not a traditional software application.