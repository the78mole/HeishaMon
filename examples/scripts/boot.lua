-- HeishaMon Boot Script Example
-- This script runs when HeishaMon starts up

print("HeishaMon started with Lua script engine!")

-- Example: Set some initial variables
local startup_time = os.time()
print("System started at: " .. startup_time)

-- Example: Check if it's during nighttime hours  
local hour = tonumber(os.date("%H"))
if hour >= 22 or hour <= 6 then
    print("System started during night hours - enabling night mode")
    -- You could send commands to the heat pump here
    -- sendCommand("operationMode", "0")  -- Example command
end