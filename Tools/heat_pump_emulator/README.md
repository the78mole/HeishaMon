# Panasonic Heat Pump Emulator

A Python-based emulator that simulates a Panasonic Aquarea heat pump for testing HeishaMon without requiring real hardware.

## Features

- **Serial Communication**: Communicates via configurable serial port (e.g., `/dev/ttyUSB0`)
- **Protocol Compliance**: Implements the documented Panasonic protocol (9600 8E1)
- **Realistic Simulation**: Simulates temperature changes, operating modes, and system responses
- **Command Processing**: Responds to HeishaMon commands and updates internal state
- **Multiple Models**: Support for different heat pump models via configuration files
- **Comprehensive Data**: Provides all sensor readings and status information

## Installation

### Requirements

- Python 3.7 or later
- `pyserial` library

### Install Dependencies

```bash
pip install pyserial
```

### Virtual Serial Ports (Optional)

For testing without physical hardware, you can create virtual serial port pairs:

#### Linux (socat)
```bash
# Install socat
sudo apt-get install socat

# Create virtual serial port pair
socat -d -d pty,raw,echo=0 pty,raw,echo=0
# This will create two linked ports, e.g., /dev/pts/1 and /dev/pts/2
```

#### Linux (systemctl)
```bash
# Load null modem module
sudo modprobe tty0tty

# This creates /dev/tnt0 and /dev/tnt1 as a connected pair
```

## Usage

### Basic Usage

```bash
# Run with default settings
python3 heat_pump_emulator.py

# Specify serial port
python3 heat_pump_emulator.py --port /dev/ttyUSB0

# Use specific configuration
python3 heat_pump_emulator.py --config l_series.json

# Enable verbose logging
python3 heat_pump_emulator.py --verbose
```

### Configuration Files

The emulator supports different heat pump models through JSON configuration files in the `config/` directory:

- `default.json` - Standard E-series heat pump
- `l_series.json` - L-series heat pump with different characteristics

### Example Configuration

```json
{
    "model": {
        "name": "CU-E12DKUA / CS-E12DKUA",
        "series": "E-series", 
        "capacity": "12kW"
    },
    "initial_state": {
        "heatpump_on": true,
        "operating_mode": "heat+dhw",
        "dhw_target": 45,
        "outdoor_temp": 10.0
    },
    "simulation": {
        "speed": 1.0,
        "thermal_mass_factor": 0.01
    }
}
```

## Testing with HeishaMon

### 1. Setup Virtual Serial Ports

```bash
# Create virtual port pair
socat -d -d pty,raw,echo=0 pty,raw,echo=0
# Note the created ports, e.g., /dev/pts/1 and /dev/pts/2
```

### 2. Start the Emulator

```bash
# Start emulator on one port
python3 heat_pump_emulator.py --port /dev/pts/1 --verbose
```

### 3. Configure HeishaMon

- Connect HeishaMon to the other port (`/dev/pts/2`)
- Configure HeishaMon to use 9600 baud, 8E1 settings
- The emulator will respond to queries and commands

### 4. Monitor Communication

The emulator logs all communication:

```
2024-01-15 10:30:15 - INFO - Starting Panasonic Heat Pump Emulator
2024-01-15 10:30:15 - INFO - Model: CU-E12DKUA / CS-E12DKUA
2024-01-15 10:30:16 - DEBUG - Processing heat pump status query
2024-01-15 10:30:16 - INFO - Processing command: settings_change
2024-01-15 10:30:16 - INFO - DHW target temperature: 48°C
```

## Protocol Details

### Message Format

- **Query (HeishaMon → Heat Pump)**: `71 6c 01 10 + 107 bytes`
- **Response (Heat Pump → HeishaMon)**: `71 c8 01 10 + 199 bytes`  
- **Command (HeishaMon → Heat Pump)**: `f1 6c 01 10 + 107 bytes`

### Serial Settings

- **Baud Rate**: 9600
- **Data Bits**: 8
- **Parity**: Even
- **Stop Bits**: 1
- **Flow Control**: None

### Checksum

All messages use 8-bit checksum validation where `sum(all_bytes) & 0xFF == 0`.

## Supported Features

### Operating Modes
- Heat only
- Cool only  
- Auto (heat/cool)
- DHW only
- Combined modes (heat+DHW, cool+DHW, auto+DHW)

### Temperature Control
- Zone 1 heating/cooling temperature shifts (-5°C to +5°C)
- DHW target temperature (40°C to 75°C)
- Realistic temperature simulation with thermal mass

### System States
- Quiet mode (off, level 1-3)
- Powerful mode (30/60/90 minutes)
- Holiday mode
- Weekly scheduler
- Force DHW heating

### Sensor Data
- Indoor/outdoor temperatures
- Water inlet/outlet temperatures  
- DHW temperature
- Compressor frequency
- Pump flow and speed
- Fan speeds
- Power consumption/production
- Operating counters and hours

## Architecture

The emulator consists of three main components:

1. **`heat_pump_emulator.py`** - Main application and serial communication
2. **`panasonic_protocol.py`** - Protocol encoding/decoding and message handling
3. **`heat_pump_state.py`** - State management and realistic simulation

## Troubleshooting

### Serial Port Issues

```bash
# Check available ports
ls /dev/tty*

# Check port permissions
sudo chmod 666 /dev/ttyUSB0

# Add user to dialout group
sudo usermod -a -G dialout $USER
```

### Communication Issues

- Verify baud rate and parity settings
- Check serial cable connections
- Enable verbose logging to see message details
- Verify checksum calculations

### No Response from HeishaMon

- Ensure HeishaMon is configured for correct serial port
- Check that emulator is running and listening
- Verify virtual serial port setup
- Check for conflicting applications using the port

## Development

### Adding New Commands

1. Update command parsing in `panasonic_protocol.py`
2. Add command handling in `heat_pump_state.py`
3. Test with known command examples from the protocol documentation

### Creating New Heat Pump Models

1. Create new JSON configuration file in `config/`
2. Update model identification bytes
3. Adjust sensor ranges and characteristics
4. Test with HeishaMon

## License

This project is part of HeishaMon and released under the GPL-3.0 license.

## Contributing

Contributions are welcome! Please:

1. Test thoroughly with real HeishaMon setups
2. Follow the existing code style
3. Update documentation for new features
4. Add configuration files for new heat pump models