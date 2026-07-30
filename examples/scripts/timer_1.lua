-- HeishaMon Timer Script Example
-- This script runs every hour (timer 1)

print("Hourly timer script executed")

-- Get current time
local hour = getTime("hour")
local minute = getTime("minute")

print("Current time: " .. hour .. ":" .. minute)

-- Example: Daily schedule management
if hour == 6 and minute == 0 then
    -- Morning routine
    print("Morning routine - setting day temperatures")
    sendCommand("Z1HeatRequestTemperature", "21")
    
elseif hour == 22 and minute == 0 then
    -- Evening routine  
    print("Evening routine - setting night temperatures")
    sendCommand("Z1HeatRequestTemperature", "18")
    
elseif hour == 23 and minute == 0 then
    -- Late evening - hot water heating
    print("Late evening - heating hot water")
    sendCommand("DHWForced", "1")
end

-- Example: Monitor heat pump efficiency
local cop = getSensorData("@COP")
local power = getSensorData("@ConsumedElectricalPower")

if cop and power then
    print("Current COP: " .. cop .. ", Power consumption: " .. power .. "W")
    
    if cop < 2.0 and power > 2000 then
        print("WARNING: Low efficiency detected!")
        -- You could send a notification or adjust settings
    end
end