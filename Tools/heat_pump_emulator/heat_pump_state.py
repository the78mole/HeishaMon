"""
Heat Pump State Management and Simulation

Manages the internal state of the simulated heat pump and provides
realistic behavior simulation.
"""

import json
import logging
import math
import time
from typing import Dict, Any


class HeatPumpState:
    """Manages heat pump state and simulation."""
    
    def __init__(self):
        self.logger = logging.getLogger(__name__)
        
        # Model information
        self.model_name = "CU-E12DKUA / CS-E12DKUA"  # Default model
        self.model_bytes = [0xe2, 0xce, 0x0d, 0x71, 0x81, 0x72, 0xce, 0x0c, 0x92, 0x81]
        
        # Basic states
        self.heatpump_on = True
        self.force_dhw = False
        self.holiday_mode = False
        self.scheduler_on = False
        
        # Operating mode
        self.operating_mode = "heat+dhw"  # heat, cool, auto, dhw_only, heat+dhw, cool+dhw, auto+dhw
        
        # Control modes
        self.quiet_mode = 0  # 0=off, 1-3=levels
        self.powerful_mode = 0  # 0=off, 30/60/90=minutes
        
        # Temperature setpoints (°C)
        self.z1_heat_shift = 0    # Zone 1 heat shift (-5 to +5)
        self.z1_cool_shift = 0    # Zone 1 cool shift (-5 to +5)
        self.dhw_target = 45      # DHW target temperature (40-75°C)
        
        # Current temperatures (°C)
        self.z1_temp = 22.0           # Zone 1 room temperature
        self.z1_target_temp = 22.0    # Zone 1 target temperature
        self.dhw_temp = 45.0          # DHW actual temperature
        self.outdoor_temp = 10.0      # Outdoor temperature
        self.inlet_temp = 35.0        # Water inlet temperature
        self.outlet_temp = 40.0       # Water outlet temperature
        self.outlet_target_temp = 40.0 # Outlet target temperature
        
        # Operating parameters
        self.compressor_freq = 45     # Compressor frequency (Hz)
        self.pump_flow = 12.5         # Pump flow (L/min)
        self.pump_speed = 1400        # Pump speed (RPM)
        self.fan1_speed = 450         # Fan 1 speed (RPM)
        self.fan2_speed = 0           # Fan 2 speed (RPM, often 0)
        
        # Counters and times
        self.operations_counter = 2238   # Number of starts
        self.operations_hours = 2852     # Operating hours
        
        # Power consumption/production (Watts)
        self.heat_power_consumption = 1200
        self.heat_power_production = 4800
        self.dhw_power_consumption = 1800
        self.dhw_power_production = 3600
        
        # Simulation state
        self.last_update = time.time()
        self.simulation_speed = 1.0  # Simulation speed multiplier
        
        # 3-way valve and defrost states
        self.three_way_valve = 0  # 0=room, 1=DHW
        self.defrost_active = False
        
        # Error state
        self.error_code = 0
        self.error_type = 0
        
        # Zone and pump states
        self.zone1_active = True
        self.zone2_active = False
        self.zone1_pump_on = False
        self.zone2_pump_on = False
        
        self.logger.info("Heat pump state initialized")
    
    def load_from_config(self, config: Dict[str, Any]):
        """Load configuration from JSON config."""
        try:
            if "model" in config:
                model_config = config["model"]
                self.model_name = model_config.get("name", self.model_name)
                if "bytes" in model_config:
                    self.model_bytes = model_config["bytes"]
            
            if "initial_state" in config:
                state = config["initial_state"]
                self.heatpump_on = state.get("heatpump_on", self.heatpump_on)
                self.operating_mode = state.get("operating_mode", self.operating_mode)
                self.dhw_target = state.get("dhw_target", self.dhw_target)
                self.outdoor_temp = state.get("outdoor_temp", self.outdoor_temp)
                
            if "simulation" in config:
                sim = config["simulation"]
                self.simulation_speed = sim.get("speed", self.simulation_speed)
                
            self.logger.info("Configuration loaded successfully")
        except Exception as e:
            self.logger.error(f"Error loading configuration: {e}")
    
    def get_model_info(self) -> str:
        """Get model information string."""
        return f"{self.model_name}"
    
    def apply_command(self, command_info: Dict, raw_data: bytes):
        """Apply a command to update the heat pump state."""
        if not command_info or "parameters" not in command_info:
            return
            
        params = command_info["parameters"]
        
        # Update heat pump state
        if "heatpump_state" in params:
            old_state = self.heatpump_on
            self.heatpump_on = params["heatpump_state"] == "on"
            if old_state != self.heatpump_on:
                self.logger.info(f"Heat pump {'ON' if self.heatpump_on else 'OFF'}")
        
        # Update operating mode
        if "mode" in params:
            old_mode = self.operating_mode
            self.operating_mode = params["mode"]
            if old_mode != self.operating_mode:
                self.logger.info(f"Operating mode changed to: {self.operating_mode}")
        
        # Update force DHW
        if "force_dhw" in params:
            self.force_dhw = params["force_dhw"]
            self.logger.info(f"Force DHW: {'ON' if self.force_dhw else 'OFF'}")
        
        # Update holiday mode
        if "holiday_mode" in params:
            self.holiday_mode = params["holiday_mode"]
            self.logger.info(f"Holiday mode: {'ON' if self.holiday_mode else 'OFF'}")
        
        # Update quiet mode
        if "quiet_mode" in params:
            old_quiet = self.quiet_mode
            self.quiet_mode = params["quiet_mode"]
            if old_quiet != self.quiet_mode:
                self.logger.info(f"Quiet mode: {self.quiet_mode}")
        
        # Update powerful mode
        if "powerful_mode" in params:
            old_power = self.powerful_mode
            self.powerful_mode = params["powerful_mode"]
            if old_power != self.powerful_mode:
                self.logger.info(f"Powerful mode: {self.powerful_mode} minutes")
        
        # Update temperature setpoints
        if "z1_heat_shift" in params:
            self.z1_heat_shift = max(-5, min(5, params["z1_heat_shift"]))
            self.logger.info(f"Zone 1 heat shift: {self.z1_heat_shift}°C")
        
        if "z1_cool_shift" in params:
            self.z1_cool_shift = max(-5, min(5, params["z1_cool_shift"]))
            self.logger.info(f"Zone 1 cool shift: {self.z1_cool_shift}°C")
        
        if "dhw_target" in params:
            old_target = self.dhw_target
            self.dhw_target = max(40, min(75, params["dhw_target"]))
            if old_target != self.dhw_target:
                self.logger.info(f"DHW target temperature: {self.dhw_target}°C")
    
    def update_simulation(self):
        """Update the heat pump simulation state."""
        current_time = time.time()
        dt = (current_time - self.last_update) * self.simulation_speed
        self.last_update = current_time
        
        if dt <= 0:
            return
        
        # Simulate heat pump operation
        if self.heatpump_on:
            self._simulate_heating_operation(dt)
            self._simulate_dhw_operation(dt)
            self._simulate_operating_parameters(dt)
        else:
            self._simulate_shutdown(dt)
        
        # Update counters (very slowly)
        if current_time % 3600 < dt:  # Once per simulated hour
            self.operations_hours += 1
    
    def _simulate_heating_operation(self, dt: float):
        """Simulate heating operation."""
        if "heat" in self.operating_mode or "auto" in self.operating_mode:
            # Calculate target based on outdoor temperature and heat curve
            base_target = 40.0  # Base water temperature
            
            # Simple heat curve: colder outside = hotter water
            if self.outdoor_temp < 0:
                base_target = 50.0
            elif self.outdoor_temp < 10:
                base_target = 45.0
            elif self.outdoor_temp > 20:
                base_target = 35.0
            
            # Apply zone shifts
            self.outlet_target_temp = base_target + self.z1_heat_shift
            
            # Simulate water temperature following target
            temp_diff = self.outlet_target_temp - self.outlet_temp
            self.outlet_temp += temp_diff * 0.05 * dt  # Slow approach to target
            
            # Inlet is typically 5°C lower than outlet
            self.inlet_temp = self.outlet_temp - 5.0
            
            # Zone temperature follows water temperature (with thermal mass)
            zone_target = 22.0 + self.z1_heat_shift
            zone_diff = zone_target - self.z1_temp
            self.z1_temp += zone_diff * 0.01 * dt  # Very slow thermal mass
            self.z1_target_temp = zone_target
    
    def _simulate_dhw_operation(self, dt: float):
        """Simulate DHW heating operation."""
        if "dhw" in self.operating_mode or self.force_dhw:
            # DHW heating
            temp_diff = self.dhw_target - self.dhw_temp
            if abs(temp_diff) > 0.5:  # Only heat if significant difference
                self.dhw_temp += temp_diff * 0.02 * dt  # Moderate heating rate
                self.three_way_valve = 1  # Switch to DHW
            else:
                self.three_way_valve = 0  # Switch back to heating
        else:
            # DHW cooling down slowly when not heating
            if self.dhw_temp > self.outdoor_temp + 5:
                self.dhw_temp -= 0.5 * dt  # Slow heat loss
    
    def _simulate_operating_parameters(self, dt: float):
        """Simulate compressor, pumps, and fans."""
        # Compressor frequency based on load
        target_freq = 30  # Base frequency
        
        if "heat" in self.operating_mode:
            temp_diff = abs(self.outlet_target_temp - self.outlet_temp)
            target_freq = 30 + min(40, temp_diff * 5)  # Higher freq for larger diff
        
        if self.three_way_valve == 1:  # DHW mode
            dhw_diff = abs(self.dhw_target - self.dhw_temp)
            target_freq = max(target_freq, 35 + min(30, dhw_diff * 3))
        
        # Apply quiet mode reduction
        if self.quiet_mode > 0:
            reduction = [0, 0.8, 0.6, 0.4][self.quiet_mode]  # Reduce by 20/40/60%
            target_freq *= reduction
        
        # Apply powerful mode boost
        if self.powerful_mode > 0:
            target_freq *= 1.3  # 30% boost
        
        # Simulate frequency changes
        freq_diff = target_freq - self.compressor_freq
        self.compressor_freq += freq_diff * 0.1 * dt
        self.compressor_freq = max(20, min(90, self.compressor_freq))
        
        # Pump flow and speed based on frequency
        self.pump_flow = 8.0 + (self.compressor_freq - 20) * 0.15
        self.pump_speed = 1000 + (self.compressor_freq - 20) * 15
        
        # Fan speed based on outdoor temperature and frequency
        if self.outdoor_temp > 25:  # Hot weather = faster fans
            self.fan1_speed = 300 + (self.compressor_freq - 20) * 8
        else:
            self.fan1_speed = 200 + (self.compressor_freq - 20) * 5
        
        # Power consumption/production
        base_power = 800 + self.compressor_freq * 10
        self.heat_power_consumption = base_power
        self.heat_power_production = base_power * 3.5  # COP of 3.5
        
        if self.three_way_valve == 1:  # DHW mode typically higher power
            self.dhw_power_consumption = base_power * 1.2
            self.dhw_power_production = base_power * 3.0
        else:
            self.dhw_power_consumption = 0
            self.dhw_power_production = 0
    
    def _simulate_shutdown(self, dt: float):
        """Simulate heat pump shutdown behavior."""
        # Temperatures slowly equalize with outdoor
        self.outlet_temp += (self.outdoor_temp - self.outlet_temp) * 0.005 * dt
        self.inlet_temp += (self.outdoor_temp - self.inlet_temp) * 0.005 * dt
        self.z1_temp += (self.outdoor_temp - self.z1_temp) * 0.001 * dt
        
        # DHW slowly cools
        if self.dhw_temp > self.outdoor_temp + 5:
            self.dhw_temp -= 1.0 * dt
        
        # Operating parameters go to minimum
        self.compressor_freq = 0
        self.pump_flow = 0
        self.pump_speed = 0
        self.fan1_speed = 0
        self.heat_power_consumption = 0
        self.heat_power_production = 0
        self.dhw_power_consumption = 0
        self.dhw_power_production = 0