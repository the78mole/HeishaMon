"""
Panasonic Protocol Handler

Handles encoding and decoding of Panasonic heat pump communication protocol.
Based on ProtocolByteDecrypt.md documentation.
"""

import logging
from typing import Dict, Optional, Tuple


class PanasonicProtocol:
    """Handles Panasonic heat pump protocol message encoding and decoding."""
    
    def __init__(self):
        self.logger = logging.getLogger(__name__)
        
        # Protocol constants
        self.QUERY_HEADER = 0x71
        self.COMMAND_HEADER = 0xf1
        self.LENGTH_QUERY = 0x6c  # 108 bytes data + 3 header = 111 total
        self.LENGTH_RESPONSE = 0xc8  # 200 bytes data + 3 header = 203 total
        self.FIXED_BYTES = [0x01, 0x10]  # Bytes 2-3 are always 01 10
        
        # Command mappings for easier parsing
        self.COMMAND_MAP = {
            # Temperature adjustments (byte 38)
            0x7b: "temp_shift_-5",
            0x7c: "temp_shift_-4", 
            0x7d: "temp_shift_-3",
            0x7e: "temp_shift_-2",
            0x7f: "temp_shift_-1",
            0x80: "temp_shift_0",
            0x81: "temp_shift_+1",
            0x82: "temp_shift_+2",
            0x83: "temp_shift_+3",
            0x84: "temp_shift_+4",
            0x85: "temp_shift_+5",
            
            # DHW temperature setpoints (byte 42)
            0xa8: "dhw_40c",
            0xaf: "dhw_47c", 
            0xb0: "dhw_48c",
            0xb1: "dhw_49c",
            0xca: "dhw_75c",
            
            # Mode changes need multiple byte analysis
        }
    
    def calculate_checksum(self, data: bytes) -> int:
        """Calculate 8-bit checksum for message validation."""
        return (256 - (sum(data) & 0xFF)) & 0xFF
    
    def validate_checksum(self, data: bytes) -> bool:
        """Validate message checksum."""
        return (sum(data) & 0xFF) == 0
    
    def is_valid_query(self, data: bytes) -> bool:
        """Check if received data is a valid query message."""
        if len(data) != 111:
            return False
        
        if (data[0] != self.QUERY_HEADER or 
            data[1] != self.LENGTH_QUERY or
            data[2] != self.FIXED_BYTES[0] or
            data[3] != self.FIXED_BYTES[1]):
            return False
            
        return self.validate_checksum(data)
    
    def is_valid_command(self, data: bytes) -> bool:
        """Check if received data is a valid command message."""
        if len(data) != 111:
            return False
        
        if (data[0] != self.COMMAND_HEADER or 
            data[1] != self.LENGTH_QUERY or
            data[2] != self.FIXED_BYTES[0] or
            data[3] != self.FIXED_BYTES[1]):
            return False
            
        return self.validate_checksum(data)
    
    def parse_command(self, data: bytes) -> Optional[Dict]:
        """Parse a command message and extract command information."""
        if not self.is_valid_command(data):
            return None
        
        command_info = {
            "type": "unknown",
            "parameters": {}
        }
        
        # Analyze key command bytes
        # Byte 4: Force DHW, Heat pump on/off
        if data[4] != 0x00:
            if data[4] == 0x42:  # Normal operation
                command_info["parameters"]["heatpump_state"] = "on"
            elif data[4] == 0x41:  # Heat pump off
                command_info["parameters"]["heatpump_state"] = "off"
            elif data[4] == 0x82:  # Force DHW
                command_info["parameters"]["force_dhw"] = True
        
        # Byte 5: Holiday mode, scheduler
        if data[5] != 0x00:
            if data[5] & 0x10:  # Holiday mode bit
                command_info["parameters"]["holiday_mode"] = True
        
        # Byte 6: Operating mode
        if data[6] != 0x00:
            mode_bits = data[6] & 0x0F
            dhw_bits = (data[6] >> 2) & 0x03
            
            modes = {
                0x01: "dhw_only",
                0x02: "heat", 
                0x03: "cool",
                0x08: "auto_heat",
                0x09: "auto_cool"
            }
            
            if mode_bits in modes:
                base_mode = modes[mode_bits]
                if dhw_bits == 0x02:  # DHW on
                    command_info["parameters"]["mode"] = f"{base_mode}+dhw"
                else:
                    command_info["parameters"]["mode"] = base_mode
        
        # Byte 7: Quiet mode and powerful mode  
        if data[7] != 0x00:
            quiet_level = (data[7] >> 3) & 0x0F
            power_level = data[7] & 0x07
            
            if quiet_level in [0x0A, 0x0B, 0x0C, 0x11]:  # Quiet levels 1-3, scheduled
                quiet_map = {0x0A: 1, 0x0B: 2, 0x0C: 3, 0x11: "scheduled"}
                command_info["parameters"]["quiet_mode"] = quiet_map[quiet_level]
            
            if power_level in [0x02, 0x03, 0x04]:  # Power mode 30/60/90 min
                power_map = {0x02: 30, 0x03: 60, 0x04: 90}
                command_info["parameters"]["powerful_mode"] = power_map[power_level]
        
        # Temperature setpoints
        if data[38] != 0x80:  # Zone 1 heat shift (default 0x80 = 0°C)
            command_info["parameters"]["z1_heat_shift"] = data[38] - 128
        
        if data[39] != 0x80:  # Zone 1 cool shift  
            command_info["parameters"]["z1_cool_shift"] = data[39] - 128
            
        if data[42] != 0x00:  # DHW target temperature
            command_info["parameters"]["dhw_target"] = data[42] - 128
        
        # Set command type based on what was found
        if command_info["parameters"]:
            command_info["type"] = "settings_change"
        
        return command_info
    
    def create_status_response(self, state) -> bytes:
        """Create a status response message from current heat pump state."""
        # Start with response header
        response = bytearray([
            self.QUERY_HEADER,      # 0x71
            self.LENGTH_RESPONSE,   # 0xc8 (200 bytes data)
            self.FIXED_BYTES[0],    # 0x01
            self.FIXED_BYTES[1]     # 0x10
        ])
        
        # Add 199 bytes of status data
        status_data = self._build_status_data(state)
        response.extend(status_data)
        
        # Calculate and append checksum
        checksum = self.calculate_checksum(response)
        response.append(checksum)
        
        return bytes(response)
    
    def _build_status_data(self, state) -> bytearray:
        """Build the 199-byte status data payload."""
        data = bytearray(199)
        
        # Byte 4: Heat pump state, force DHW
        data[0] = 0x56  # Default: heat pump on, no force DHW
        if state.heatpump_on:
            data[0] |= 0x01
        if state.force_dhw:
            data[0] |= 0x40
        
        # Byte 5: Holiday mode, scheduler, heater states
        data[1] = 0x55  # Default values
        if state.holiday_mode:
            data[1] |= 0x10
        if state.scheduler_on:
            data[1] |= 0x04
        
        # Byte 6: Operating mode and zone states
        data[2] = 0x62  # Default: heat mode + DHW, zone 1 on
        mode_map = {
            "heat": 0x02, "cool": 0x03, "auto": 0x08, "dhw_only": 0x01,
            "heat+dhw": 0x22, "cool+dhw": 0x23, "auto+dhw": 0x28
        }
        if state.operating_mode in mode_map:
            data[2] = mode_map[state.operating_mode]
        
        # Byte 7: Quiet and powerful mode
        data[3] = 0x49  # Default: quiet off, power off
        if state.quiet_mode > 0:
            data[3] = 0x49 + (state.quiet_mode << 3)
        if state.powerful_mode > 0:
            power_map = {30: 0x02, 60: 0x03, 90: 0x04}
            if state.powerful_mode in power_map:
                data[3] = (data[3] & 0xF8) | power_map[state.powerful_mode]
        
        # Skip bytes 4-33 (mostly configuration data)
        
        # Temperature setpoints (bytes 34-47 in status = bytes 38-51 in protocol doc)
        data[34] = state.z1_heat_shift + 128      # Zone 1 heat shift
        data[35] = state.z1_cool_shift + 128      # Zone 1 cool shift  
        data[36] = 0x80                           # Zone 2 heat shift (not used)
        data[37] = 0x80                           # Zone 2 cool shift (not used)
        data[38] = state.dhw_target + 128         # DHW target temperature
        
        # Skip bytes 43-134 (various settings)
        
        # Current temperatures (bytes 135-162)
        data[135] = int(state.z1_temp) + 128      # Zone 1 temperature
        data[136] = 0x00                          # Zone 2 temperature (not used)
        data[137] = int(state.dhw_temp) + 128     # DHW temperature
        data[138] = int(state.outdoor_temp) + 128 # Outdoor temperature  
        data[139] = int(state.inlet_temp) + 128   # Inlet water temperature
        data[140] = int(state.outlet_temp) + 128  # Outlet water temperature
        data[141] = 0x32                          # Zone 1 water temp (placeholder)
        data[142] = 0x32                          # Zone 2 water temp (placeholder)
        data[143] = int(state.z1_target_temp) + 128  # Zone 1 target
        data[144] = 0x32                          # Zone 2 target (placeholder)
        data[145] = 0x32                          # Buffer temp (not used)
        data[146] = 0x32                          # Solar temp (not used)
        data[147] = 0x32                          # Pool temp (not used)
        data[148] = 0x80                          # Water shift including buffer delta
        data[149] = int(state.outlet_target_temp) + 128  # Outlet target
        
        # Operating parameters (bytes 159-178)
        data[159] = int(state.compressor_freq) + 1     # Compressor frequency
        data[166] = int(state.pump_flow * 256) % 256   # Pump flow (high byte)
        data[167] = int(state.pump_flow) % 256         # Pump flow (low byte)  
        data[168] = int(state.pump_speed / 50) + 1     # Pump speed
        data[169] = int(state.fan1_speed / 10) + 1     # Fan 1 speed
        data[170] = int(state.fan2_speed / 10) + 1     # Fan 2 speed
        
        # Operating counters (bytes 175-189)
        ops_count = state.operations_counter
        data[175] = (ops_count - 1) & 0xFF        # Operations counter low
        data[176] = ((ops_count - 1) >> 8) & 0xFF # Operations counter high
        
        ops_hours = state.operations_hours  
        data[178] = (ops_hours - 1) & 0xFF        # Operating hours low
        data[179] = ((ops_hours - 1) >> 8) & 0xFF # Operating hours high
        
        # Power consumption/generation (bytes 189-194)
        data[189] = max(1, int(state.heat_power_consumption / 200))  # Heat consumption
        data[190] = max(1, int(state.heat_power_production / 200))   # Heat production
        data[191] = 1  # Cool consumption (placeholder)
        data[192] = 1  # Cool production (placeholder)
        data[193] = max(1, int(state.dhw_power_consumption / 200))   # DHW consumption  
        data[194] = max(1, int(state.dhw_power_production / 200))    # DHW production
        
        return data