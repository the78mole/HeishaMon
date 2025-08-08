#!/bin/bash
"""
Demo script for testing the Panasonic Heat Pump Emulator

This script demonstrates how to test the emulator using virtual serial ports
without requiring actual hardware.
"""

set -e

echo "Panasonic Heat Pump Emulator - Demo Setup"
echo "========================================"

# Check if socat is available
if ! command -v socat &> /dev/null; then
    echo "Error: socat is required for creating virtual serial ports"
    echo "Install with: sudo apt-get install socat"
    exit 1
fi

# Check if Python dependencies are installed
if ! python3 -c "import serial" 2>/dev/null; then
    echo "Error: pyserial is required"
    echo "Install with: pip3 install pyserial"
    exit 1
fi

echo "Creating virtual serial port pair..."

# Create virtual serial ports in background
socat -d -d pty,raw,echo=0,link=/tmp/heishamon_heatpump pty,raw,echo=0,link=/tmp/heishamon_controller 2>/tmp/socat.log &
SOCAT_PID=$!

# Wait a moment for the ports to be created
sleep 2

# Check if ports were created
if [ ! -e /tmp/heishamon_heatpump ] || [ ! -e /tmp/heishamon_controller ]; then
    echo "Error: Failed to create virtual serial ports"
    echo "socat log:"
    cat /tmp/socat.log
    kill $SOCAT_PID 2>/dev/null || true
    exit 1
fi

echo "Virtual serial ports created:"
echo "  Heat pump emulator: /tmp/heishamon_heatpump"
echo "  HeishaMon side:     /tmp/heishamon_controller"
echo ""

# Start the emulator in background
echo "Starting heat pump emulator..."
python3 heat_pump_emulator.py --port /tmp/heishamon_heatpump --verbose &
EMULATOR_PID=$!

# Give emulator time to start
sleep 3

echo ""
echo "Demo is running!"
echo ""
echo "You can now:"
echo "1. Connect HeishaMon to /tmp/heishamon_controller"
echo "2. Send test commands using a serial terminal"
echo "3. Monitor the emulator logs"
echo ""
echo "To send a test query manually:"
echo "  echo -ne '\\x71\\x6c\\x01\\x10' | cat - /dev/zero | head -c 111 > /tmp/heishamon_controller"
echo ""
echo "Press Ctrl+C to stop the demo..."

# Setup cleanup on exit
cleanup() {
    echo ""
    echo "Cleaning up..."
    kill $EMULATOR_PID 2>/dev/null || true
    kill $SOCAT_PID 2>/dev/null || true
    rm -f /tmp/heishamon_heatpump /tmp/heishamon_controller /tmp/socat.log
    echo "Demo stopped."
}

trap cleanup EXIT INT TERM

# Wait for user interrupt
wait $EMULATOR_PID 2>/dev/null || true