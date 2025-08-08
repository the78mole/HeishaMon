#!/bin/bash
"""
Installation script for Panasonic Heat Pump Emulator

This script installs the emulator as a system service on Linux systems.
"""

set -e

# Configuration
INSTALL_DIR="/opt/heishamon/heat_pump_emulator"
SERVICE_USER="heishamon"
SERVICE_NAME="heat_pump_emulator"

echo "Installing Panasonic Heat Pump Emulator..."

# Check if running as root
if [ "$EUID" -ne 0 ]; then
    echo "Please run as root (use sudo)"
    exit 1
fi

# Install Python dependencies
echo "Installing Python dependencies..."
apt-get update
apt-get install -y python3 python3-pip
pip3 install pyserial

# Create user if it doesn't exist
if ! id "$SERVICE_USER" &>/dev/null; then
    echo "Creating service user: $SERVICE_USER"
    useradd --system --shell /bin/false --home /opt/heishamon --create-home "$SERVICE_USER"
fi

# Add user to dialout group for serial port access
usermod -a -G dialout "$SERVICE_USER"

# Create installation directory
echo "Creating installation directory: $INSTALL_DIR"
mkdir -p "$INSTALL_DIR"

# Copy files
echo "Copying emulator files..."
cp -r . "$INSTALL_DIR/"
chown -R "$SERVICE_USER:$SERVICE_USER" "$INSTALL_DIR"
chmod +x "$INSTALL_DIR/heat_pump_emulator.py"

# Install systemd service
echo "Installing systemd service..."
cp "$INSTALL_DIR/heat_pump_emulator.service" "/etc/systemd/system/"
systemctl daemon-reload

# Enable but don't start the service (user needs to configure port first)
systemctl enable "$SERVICE_NAME"

echo ""
echo "Installation completed!"
echo ""
echo "Configuration:"
echo "1. Edit /etc/systemd/system/heat_pump_emulator.service to set correct serial port"
echo "2. Optionally edit $INSTALL_DIR/config/default.json for heat pump model"
echo ""
echo "Usage:"
echo "  sudo systemctl start $SERVICE_NAME    # Start the emulator"
echo "  sudo systemctl stop $SERVICE_NAME     # Stop the emulator"
echo "  sudo systemctl status $SERVICE_NAME   # Check status"
echo "  journalctl -u $SERVICE_NAME -f        # View logs"
echo ""
echo "Manual usage:"
echo "  cd $INSTALL_DIR"
echo "  python3 heat_pump_emulator.py --port /dev/ttyUSB0"
echo ""