# HeishaMon Script Engine

HeishaMon now includes a Lua-based script engine that replaces the previous custom rules engine. This provides more powerful automation capabilities with a standard scripting language.

## Features

- **Lua scripting**: Use standard Lua syntax for automation logic
- **Event-driven**: Scripts are triggered by heat pump events, temperature changes, timers, etc.
- **WebDAV support**: Edit scripts remotely using Visual Studio Code or any WebDAV client
- **File system storage**: Scripts are stored in LittleFS on the ESP32/ESP8266
- **HeishaMon API**: Access sensor data and send commands to the heat pump from scripts

## Getting Started

### 1. Enable Script Engine

The script engine is automatically started when HeishaMon boots. Scripts are stored in the `/scripts/` directory on the device's file system.

### 2. WebDAV Access

Scripts can be edited remotely via WebDAV:

- **URL**: `http://your-heishamon-ip:8080/scripts/`
- **Mount in VS Code**: Use the WebDAV extension to mount the scripts folder
- **Direct editing**: Access via any WebDAV client (Windows Explorer, macOS Finder, etc.)

### 3. Script Types

Scripts are triggered by different events:

#### Boot Script
- **File**: `/scripts/boot.lua`
- **Trigger**: When HeishaMon starts up
- **Use**: Initialize variables, set startup conditions

#### Timer Scripts
- **File**: `/scripts/timer_N.lua` (where N is the timer number)
- **Trigger**: Periodic execution based on configured timers
- **Use**: Scheduled automation, monitoring, maintenance tasks

#### Sensor Event Scripts
- **File**: `/scripts/ds18b20#ADDRESS.lua`
- **Trigger**: When DS18B20 temperature sensor values change
- **Use**: React to temperature changes, alerts

#### OpenTherm Event Scripts
- **File**: `/scripts/?EVENT_NAME.lua`
- **Trigger**: OpenTherm data changes (e.g., `?chEnable.lua`, `?roomtemp.lua`)
- **Use**: React to thermostat and heating system changes

## API Reference

### Sensor Data Functions

```lua
-- Get sensor data by name
temperature = getSensorData("ds18b20#2800000000000000")  -- DS18B20 sensor
outdoor_temp = getSensorData("@OutdoorAirTemperature")   -- Heat pump data
room_temp = getSensorData("?roomtemp")                   -- OpenTherm data
```

### Command Functions

```lua
-- Send commands to the heat pump
sendCommand("Z1HeatRequestTemperature", "21")  -- Set zone 1 temperature
sendCommand("DHWForced", "1")                  -- Force DHW heating
sendCommand("operationMode", "0")              -- Set operation mode
```

### Time Functions

```lua
-- Get current time components
hour = getTime("hour")      -- 0-23
minute = getTime("minute")  -- 0-59
day = getTime("day")        -- 1-7 (Monday=1)
month = getTime("month")    -- 1-12
```

### Logging

```lua
-- Log messages (appears in HeishaMon logs)
log("Script executed successfully")
print("Debug information")  -- Also logged
```

### Event Data (Context)

```lua
-- Get event-specific data (when available)
sensor_address = getEventData("address")  -- For temperature events
event_name = getEventData("name")         -- For OpenTherm events
```

## Example Scripts

See the `examples/scripts/` directory for sample scripts:

- `boot.lua` - Startup script with time-based initialization
- `timer_1.lua` - Hourly monitoring and scheduling
- `ds18b20#temperature_change.lua` - Temperature event handling
- `?opentherm_event.lua` - OpenTherm event handling

## Migration from Rules Engine

The new script engine runs alongside the existing rules engine during the transition period. To fully migrate:

1. Convert existing rules to Lua scripts
2. Test thoroughly with both systems running
3. Disable the old rules engine once migration is complete

## Platform Support

- **ESP32**: Full support with plenty of memory for complex scripts
- **ESP8266**: Basic support (memory limitations may restrict script complexity)
- **PlatformIO**: Recommended build system for easier dependency management
- **Arduino IDE**: Supported with manual library installation

## WebDAV Client Setup

### Visual Studio Code
1. Install the "WebDAV" extension
2. Add mount point: `http://your-heishamon-ip:8080/scripts/`
3. Edit scripts with full IDE support

### Windows Explorer
1. Map network drive to `http://your-heishamon-ip:8080/scripts/`
2. Edit scripts with any text editor

### macOS Finder
1. Connect to server: `http://your-heishamon-ip:8080/scripts/`
2. Edit scripts with any text editor

## Troubleshooting

- **Scripts not executing**: Check HeishaMon logs for error messages
- **WebDAV not accessible**: Ensure HeishaMon is connected to WiFi
- **Syntax errors**: Use Lua syntax checker or VS Code Lua extension
- **Memory issues**: Simplify scripts or reduce number of active scripts (ESP8266)

## Advanced Features

- **Persistent variables**: Store data across script executions
- **HTTP requests**: Make web API calls (future enhancement)
- **MQTT publishing**: Send custom MQTT messages (future enhancement)
- **File operations**: Read/write custom data files (future enhancement)