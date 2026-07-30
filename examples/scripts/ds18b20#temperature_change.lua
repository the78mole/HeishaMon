-- HeishaMon Temperature Change Event
-- This script runs when a DS18B20 temperature sensor value changes
-- The sensor address is available in the event context

-- Get the current temperature reading
local sensor_address = getEventData("address")  -- This would be provided by the engine
local temperature = getSensorData("ds18b20#" .. sensor_address)

print("Temperature sensor " .. sensor_address .. " changed to " .. temperature .. "°C")

-- Example: React to temperature changes
if temperature > 25.0 then
    print("High temperature detected - checking cooling settings")
    
    -- Get current outdoor temperature from heat pump
    local outdoor_temp = getSensorData("@OutdoorAirTemperature")
    
    if outdoor_temp and outdoor_temp > 30.0 then
        print("Very hot day - optimizing cooling")
        -- You could adjust heat pump settings here
        -- sendCommand("coolingSetpoint", "22")
    end
elseif temperature < 5.0 then
    print("Low temperature detected - freeze protection")
    -- You could enable freeze protection here
end