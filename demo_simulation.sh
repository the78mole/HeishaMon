#!/bin/bash

echo "=== HeishaMon Native Simulation Demo ==="
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${YELLOW}Available simulation features:${NC}"
echo "✅ Arduino/ESP API mocking (Serial, WiFi, GPIO)"
echo "✅ MQTT client simulation with console output"
echo "✅ File system operations (LittleFS mock)"
echo "✅ Web server simulation"
echo "✅ JSON configuration parsing"
echo "✅ Real-time debugging output"

echo ""
echo -e "${BLUE}PlatformIO Configuration:${NC}"
echo "Environment: native"
echo "Platform: native (host simulation)"
echo "Build flags: -DNATIVE_SIM"
echo ""

if command -v pio &> /dev/null; then
    echo -e "${GREEN}Running HeishaMon simulation...${NC}"
    echo ""
    
    # Run the simulation
    pio run -e native --target exec
    
    echo ""
    echo -e "${GREEN}Simulation completed!${NC}"
    echo ""
    echo "Available commands:"
    echo "  pio run -e native           # Build simulation"
    echo "  pio run -e native -t exec   # Run simulation"
else
    echo -e "${RED}PlatformIO not found. Please install PlatformIO:${NC}"
    echo "  pip install platformio"
    echo ""
    echo -e "${YELLOW}Example console output during simulation:${NC}"
    echo "[SETUP] HeishaMon Native Simulation v3.5.0-simulation"
    echo "[SERIAL] Begin at 115200 baud"
    echo "[WIFI] Connecting to HeishaMon_WiFi"
    echo "[MQTT] Connected with ID: HeishaMon-12345"
    echo "[LITTLEFS] Started"
    echo "[WEBSERVER] Started on port 80"
    echo "[HEATPUMP] Simulating data read #1"
    echo "[DECODE] Processing heat pump data..."
    echo "[MQTT] Publishing to panasonic_heat_pump/main/Room_Thermostat_Temp: 25"
fi

echo ""
echo -e "${GREEN}Host simulation enables development and testing without hardware!${NC}"
echo "See README_SIMULATION.md for complete documentation."