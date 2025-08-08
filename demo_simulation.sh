#!/bin/bash

echo "=== HeishaMon Native Simulation Demo ==="
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}Testing basic mock functionality...${NC}"
make -f Makefile.native test-mocks

echo ""
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
    echo -e "${GREEN}PlatformIO is available. You can use:${NC}"
    echo "  pio run -e native           # Build simulation"
    echo "  pio run -e native -t exec   # Run simulation"
else
    echo -e "${YELLOW}PlatformIO not found. Using Makefile for demonstration.${NC}"
fi

echo ""
echo -e "${BLUE}Example console output during simulation:${NC}"
echo "[SERIAL] Begin at 115200 baud"
echo "[WIFI] Connecting to HeishaMon_WiFi"
echo "[MQTT] Connected with ID: HeishaMon-12345"
echo "[GPIO] pinMode(5, 1)"
echo "[LITTLEFS] Started"
echo "[WEBSERVER] Started on port 80"

echo ""
echo -e "${GREEN}Host simulation allows development and testing without hardware!${NC}"
echo "See README_SIMULATION.md for complete documentation."