#!/usr/bin/env python3
"""
Panasonic Heat Pump Emulator for HeishaMon Testing

This emulator simulates a Panasonic Aquarea heat pump system to allow testing
of HeishaMon without requiring actual hardware. It communicates via serial
interface using the documented Panasonic protocol.

Usage:
    python3 heat_pump_emulator.py [--port /dev/ttyUSB0] [--config default.json]

Author: HeishaMon Team
License: GPL-3.0
"""

import argparse
import json
import logging
import signal
import sys
import time
from pathlib import Path
from threading import Event, Thread

import serial

from panasonic_protocol import PanasonicProtocol
from heat_pump_state import HeatPumpState


class HeatPumpEmulator:
    """Main emulator class that handles serial communication and state management."""
    
    def __init__(self, port: str, config_file: str = None):
        self.port = port
        self.serial_conn = None
        self.protocol = PanasonicProtocol()
        self.state = HeatPumpState()
        self.running = False
        self.stop_event = Event()
        
        # Load configuration if provided
        if config_file:
            self.load_config(config_file)
        
        # Setup logging
        logging.basicConfig(
            level=logging.INFO,
            format='%(asctime)s - %(levelname)s - %(message)s'
        )
        self.logger = logging.getLogger(__name__)
    
    def load_config(self, config_file: str):
        """Load heat pump configuration from JSON file."""
        try:
            config_path = Path(__file__).parent / "config" / config_file
            with open(config_path, 'r') as f:
                config = json.load(f)
            self.state.load_from_config(config)
            self.logger.info(f"Loaded configuration from {config_file}")
        except Exception as e:
            self.logger.warning(f"Failed to load config {config_file}: {e}")
    
    def setup_serial(self):
        """Initialize serial connection with proper settings for Panasonic protocol."""
        try:
            self.serial_conn = serial.Serial(
                port=self.port,
                baudrate=9600,
                bytesize=serial.EIGHTBITS,
                parity=serial.PARITY_EVEN,
                stopbits=serial.STOPBITS_ONE,
                timeout=1.0,
                write_timeout=1.0
            )
            self.logger.info(f"Serial connection established on {self.port}")
            return True
        except Exception as e:
            self.logger.error(f"Failed to open serial port {self.port}: {e}")
            return False
    
    def handle_query(self, data: bytes) -> bytes:
        """Process a query from HeishaMon and return response."""
        if not self.protocol.is_valid_query(data):
            self.logger.warning("Received invalid query")
            return None
        
        self.logger.debug("Processing heat pump status query")
        response = self.protocol.create_status_response(self.state)
        self.logger.debug(f"Sending response: {response.hex()}")
        return response
    
    def handle_command(self, data: bytes) -> bool:
        """Process a command from HeishaMon and update state."""
        if not self.protocol.is_valid_command(data):
            self.logger.warning("Received invalid command")
            return False
        
        command_type = self.protocol.parse_command(data)
        if command_type:
            self.logger.info(f"Processing command: {command_type}")
            self.state.apply_command(command_type, data)
            return True
        
        return False
    
    def communication_loop(self):
        """Main communication loop that handles serial I/O."""
        buffer = bytearray()
        
        while not self.stop_event.is_set():
            try:
                # Read available data
                if self.serial_conn.in_waiting > 0:
                    chunk = self.serial_conn.read(self.serial_conn.in_waiting)
                    buffer.extend(chunk)
                    self.logger.debug(f"Received {len(chunk)} bytes: {chunk.hex()}")
                
                # Process complete messages
                while len(buffer) >= 111:  # Minimum message length
                    # Look for message start
                    start_idx = -1
                    for i in range(len(buffer) - 3):
                        if buffer[i] in [0x71, 0xf1] and buffer[i+1] == 0x6c and buffer[i+2] == 0x01 and buffer[i+3] == 0x10:
                            start_idx = i
                            break
                    
                    if start_idx == -1:
                        # No valid header found, clear buffer
                        buffer.clear()
                        break
                    
                    # Remove any data before the header
                    if start_idx > 0:
                        buffer = buffer[start_idx:]
                    
                    # Check if we have a complete message
                    if len(buffer) >= 111:
                        message = bytes(buffer[:111])
                        buffer = buffer[111:]
                        
                        # Process the message
                        if message[0] == 0x71:  # Query
                            response = self.handle_query(message)
                            if response:
                                self.serial_conn.write(response)
                                self.serial_conn.flush()
                        elif message[0] == 0xf1:  # Command
                            self.handle_command(message)
                    else:
                        # Wait for more data
                        break
                
                # Small delay to prevent busy waiting
                time.sleep(0.01)
                
            except Exception as e:
                self.logger.error(f"Communication error: {e}")
                time.sleep(1)
    
    def simulation_loop(self):
        """Background loop that updates heat pump state simulation."""
        while not self.stop_event.is_set():
            try:
                self.state.update_simulation()
                time.sleep(1.0)  # Update every second
            except Exception as e:
                self.logger.error(f"Simulation error: {e}")
                time.sleep(5)
    
    def start(self):
        """Start the emulator."""
        if not self.setup_serial():
            return False
        
        self.running = True
        self.logger.info("Starting Panasonic Heat Pump Emulator")
        self.logger.info(f"Model: {self.state.get_model_info()}")
        
        # Start background threads
        comm_thread = Thread(target=self.communication_loop, daemon=True)
        sim_thread = Thread(target=self.simulation_loop, daemon=True)
        
        comm_thread.start()
        sim_thread.start()
        
        try:
            # Main loop - just wait for shutdown
            while self.running and not self.stop_event.is_set():
                time.sleep(1)
        except KeyboardInterrupt:
            self.logger.info("Received shutdown signal")
        
        self.stop()
        return True
    
    def stop(self):
        """Stop the emulator."""
        self.logger.info("Stopping Heat Pump Emulator")
        self.running = False
        self.stop_event.set()
        
        if self.serial_conn:
            self.serial_conn.close()
            self.logger.info("Serial connection closed")


def signal_handler(signum, frame):
    """Handle system signals for graceful shutdown."""
    logging.getLogger(__name__).info(f"Received signal {signum}")
    sys.exit(0)


def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(description="Panasonic Heat Pump Emulator for HeishaMon Testing")
    parser.add_argument("--port", "-p", default="/dev/ttyUSB0",
                        help="Serial port to use (default: /dev/ttyUSB0)")
    parser.add_argument("--config", "-c", default="default.json",
                        help="Configuration file to use (default: default.json)")
    parser.add_argument("--verbose", "-v", action="store_true",
                        help="Enable verbose logging")
    
    args = parser.parse_args()
    
    if args.verbose:
        logging.getLogger().setLevel(logging.DEBUG)
    
    # Setup signal handlers
    signal.signal(signal.SIGINT, signal_handler)
    signal.signal(signal.SIGTERM, signal_handler)
    
    # Create and start emulator
    emulator = HeatPumpEmulator(args.port, args.config)
    
    try:
        success = emulator.start()
        sys.exit(0 if success else 1)
    except Exception as e:
        logging.error(f"Fatal error: {e}")
        sys.exit(1)


if __name__ == "__main__":
    main()