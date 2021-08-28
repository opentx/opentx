local INTERVAL          = 50         -- in 1/100th seconds

local MSP_TX_INFO       = 186

local lastRunTS
local sensorId = -1
local dataInitialised = false
local data_init = nil

local function getSensorValue()
    if sensorId == -1 then
        local sensor = getFieldInfo(protocol.stateSensor)
        if type(sensor) == "table" then
            sensorId = sensor['id'] or -1
        end
    end
    return getValue(sensorId)
end

local function modelActive(sensorValue)
    return type(sensorValue) == "number" and sensorValue > 0
end

local function init()
    lastRunTS = 0
end

local function run_bg()
    -- run in intervals
    if lastRunTS == 0 or lastRunTS + INTERVAL < getTime() then
        local sensorValue = getSensorValue()
        if modelActive(sensorValue) then
            -- Send data when the telemetry connection is available
            -- assuming when sensor value higher than 0 there is an telemetry connection
            if not dataInitialised then
                if data_init == nil then
                    data_init = assert(loadScript(SCRIPT_HOME .. "/data_init.lua"))()
                end

                dataInitialised = data_init.init();

                if dataInitialised then
                    data_init = nil

                    collectgarbage()
                end
            else
                local rssi, alarm_low, alarm_crit = getRSSI()
                -- Scale the [0, 85] (empirical) RSSI values to [0, 255]
                rssi = rssi * 3
                if rssi > 255 then
                    rssi = 255
                end

                values = {}
                values[1] = rssi

                protocol.mspWrite(MSP_TX_INFO, values)
            end
        else
            dataInitialised = false
        end

        lastRunTS = getTime()
    end

    -- process queue
    mspProcessTxQ()
end

return { init=init, run_bg=run_bg }
