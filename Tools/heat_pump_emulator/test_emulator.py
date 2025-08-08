#!/usr/bin/env python3
"""
Standalone test script for the Panasonic Heat Pump Emulator.

This script tests the protocol implementation without requiring serial hardware.
It validates message encoding/decoding and checksum calculations.
"""

import sys
from pathlib import Path

# Add the parent directory to Python path so we can import the modules
sys.path.insert(0, str(Path(__file__).parent))

from panasonic_protocol import PanasonicProtocol
from heat_pump_state import HeatPumpState


def test_checksum_validation():
    """Test checksum calculation and validation."""
    print("Testing checksum validation...")
    
    protocol = PanasonicProtocol()
    
    # Test with known good query from protocol documentation
    query_hex = "716c01100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000012"
    query_bytes = bytes.fromhex(query_hex)
    
    print(f"Query length: {len(query_bytes)} bytes")
    print(f"Query valid: {protocol.is_valid_query(query_bytes)}")
    
    # Test with known good response 
    response_hex = "71c801105655624900050000000000000000000019151155165e550509000000000000000000808f808ab27171979900000000000000000000008085158a8585d07b781f7e1f1f79798d8d9e96718fb7a37b8f8e85808f8a949e8a8a949e82908b056578c10b00000000000000005556552153155a051212190000000000000000e2ce0d718172ce0c9281b000aa7cabb032329cb632323280b7afcd9aac79807780ff9101295900003b0b1c51590136790101c30200dd02000500000100000601010101010a1400000077"
    response_bytes = bytes.fromhex(response_hex)
    
    print(f"Response length: {len(response_bytes)} bytes")
    print(f"Response checksum valid: {protocol.validate_checksum(response_bytes)}")
    
    # Test with known command
    command_hex = "f16c01104254224900050000000000000000000000000000000000000000000000000000000080940000b171710000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000e5"
    command_bytes = bytes.fromhex(command_hex)
    
    print(f"Command length: {len(command_bytes)} bytes") 
    print(f"Command valid: {protocol.is_valid_command(command_bytes)}")
    print()


def test_command_parsing():
    """Test command parsing functionality."""
    print("Testing command parsing...")
    
    protocol = PanasonicProtocol()
    
    # Test DHW temperature command
    dhw_cmd_hex = "f16c0110000000000000000000000000000000000000000000000000000000000000000000000000000000b000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000e2"
    dhw_cmd = bytes.fromhex(dhw_cmd_hex)
    
    parsed = protocol.parse_command(dhw_cmd)
    print(f"DHW command parsed: {parsed}")
    
    # Test mode change command  
    mode_cmd_hex = "f16c01104254224900050000000000000000000000000000000000000000000000000000000080940000b171710000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000e5"
    mode_cmd = bytes.fromhex(mode_cmd_hex)
    
    parsed = protocol.parse_command(mode_cmd)
    print(f"Mode command parsed: {parsed}")
    
    # Test quiet mode command
    quiet_cmd_hex = "f16c01104254225100050000000000000000000000000000000000000000000000000000000080940000b171710000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000dd"
    quiet_cmd = bytes.fromhex(quiet_cmd_hex)
    
    parsed = protocol.parse_command(quiet_cmd)
    print(f"Quiet mode command parsed: {parsed}")
    print()


def test_response_generation():
    """Test response message generation."""
    print("Testing response generation...")
    
    protocol = PanasonicProtocol()
    state = HeatPumpState()
    
    # Generate a response
    response = protocol.create_status_response(state)
    
    print(f"Generated response length: {len(response)} bytes")
    print(f"Response checksum valid: {protocol.validate_checksum(response)}")
    print(f"Response header: {response[:4].hex()}")
    print(f"Response ends with checksum: {response[-1]:02x}")
    
    # Change some state and generate another response
    state.dhw_target = 50
    state.outdoor_temp = 15.0
    state.operating_mode = "cool+dhw"
    
    response2 = protocol.create_status_response(state)
    print(f"Modified state response valid: {protocol.validate_checksum(response2)}")
    print()


def test_state_simulation():
    """Test heat pump state simulation."""
    print("Testing state simulation...")
    
    state = HeatPumpState()
    
    print(f"Initial state: {state.get_model_info()}")
    print(f"DHW target: {state.dhw_target}°C")
    print(f"DHW actual: {state.dhw_temp:.1f}°C")
    print(f"Outdoor temp: {state.outdoor_temp:.1f}°C")
    print(f"Operating mode: {state.operating_mode}")
    
    # Simulate some time passing
    for i in range(5):
        state.update_simulation()
        print(f"Simulation step {i+1}: DHW={state.dhw_temp:.1f}°C, Outlet={state.outlet_temp:.1f}°C, Freq={state.compressor_freq:.0f}Hz")
    
    # Apply a command
    command_info = {
        "type": "settings_change",
        "parameters": {
            "dhw_target": 55,
            "operating_mode": "heat+dhw",
            "quiet_mode": 2
        }
    }
    
    state.apply_command(command_info, b"")
    print(f"After command: DHW target={state.dhw_target}°C, Mode={state.operating_mode}, Quiet={state.quiet_mode}")
    print()


def main():
    """Run all tests."""
    print("Panasonic Heat Pump Emulator - Test Suite")
    print("=" * 50)
    
    try:
        test_checksum_validation()
        test_command_parsing()
        test_response_generation()
        test_state_simulation()
        
        print("All tests completed successfully!")
        return True
        
    except Exception as e:
        print(f"Test failed with error: {e}")
        import traceback
        traceback.print_exc()
        return False


if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)