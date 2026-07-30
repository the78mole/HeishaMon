-- HeishaMon OpenTherm Event Script
-- This script runs when OpenTherm data changes

local event_name = getEventData("name")  -- e.g., "chEnable", "roomtemp", etc.
print("OpenTherm event: " .. event_name)

-- React to different OpenTherm events
if event_name == "chEnable" then
    local ch_enabled = getSensorData("?chEnable")
    print("Central heating enabled: " .. tostring(ch_enabled))
    
    if ch_enabled then
        print("Central heating turned on - monitoring temperatures")
    else
        print("Central heating turned off")
    end
    
elseif event_name == "roomtemp" then
    local room_temp = getSensorData("?roomtemp")
    local setpoint = getSensorData("?roomtempset")
    
    print("Room temperature: " .. room_temp .. "°C, Setpoint: " .. setpoint .. "°C")
    
    -- Example: Adaptive temperature control
    local temp_diff = setpoint - room_temp
    if temp_diff > 3.0 then
        print("Large temperature difference - check heating curves")
    end
    
elseif event_name == "dhwEnable" then
    local dhw_enabled = getSensorData("?dhwEnable")
    print("DHW enabled: " .. tostring(dhw_enabled))
    
    if dhw_enabled then
        -- You could implement smart DHW scheduling here
        local hour = getTime("hour")
        if hour >= 1 and hour <= 5 then
            print("DHW request during night hours - checking if necessary")
        end
    end
end