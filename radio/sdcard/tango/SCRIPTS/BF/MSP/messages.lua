MSP_PID_FORMAT = {
   read           = 112, -- MSP_PID
   write          = 202, -- MSP_SET_PID
   minBytes       = 8,
   fields = {
      -- P
      { vals = { 1 } },
      { vals = { 4 } },
      { vals = { 7 } },
      -- I
      { vals = { 2 } },
      { vals = { 5 } },
      { vals = { 8 } },
      -- D
      { vals = { 3 } },
      { vals = { 6 } },
   },
}

MSP_PID_ADVANCED_FORMAT = {
   read           = 94, -- MSP_PID_ADVANCED
   write          = 95, -- MSP_SET_PID_ADVANCED
   minBytes       = 23,
   fields = {
	  -- weight
      { vals = { 10 }, scale = 100 },
	  -- transition
      { vals = { 9 }, scale = 100 },
   },
}

local INTRO_DELAY = 1600
local READOUT_DELAY = 500

function extractMspValues(cmd, rx_buf, msgFormat, msgValues)
    if cmd == nil or rx_buf == nil then
        return
    end
    if cmd ~= msgFormat.read then
        return
    end
    if #(rx_buf) > 0 then
        msgValues.raw = {}
        for i=1,#(rx_buf) do
            msgValues.raw[i] = rx_buf[i]
        end

		msgValues.values = {}
        for i=1,#(msgFormat.fields) do
            if (#(msgValues.raw) or 0) >= msgFormat.minBytes then
               local f = msgFormat.fields[i]
               if f.vals then
                  local value = 0;
                  for idx=1, #(f.vals) do
                     local raw_val = msgValues.raw[f.vals[idx]]
                     raw_val = bit32.lshift(raw_val, (idx-1)*8)
                     value = bit32.bor(value, raw_val)
                  end
                  msgValues.values[i] = value/(f.scale or 1)
               end
            end
        end
    end
end

function readoutMsp(msgFormat, msg)
    local t = getTime()
    if msg.lastTrigger == nil or msg.lastTrigger + INTRO_DELAY <= t then
        playFile(msg.intro)
        msg.lastTrigger = t
    elseif msg.reqTS == nil or msg.reqTS + READOUT_DELAY <= t then
        protocol.mspRead(msgFormat.read)
        msg.reqTS = t
    else
        local cmd, rx_buf = mspPollReply()
        extractMspValues(cmd, rx_buf, msgFormat, msg)
        if msg.raw then
            for i=1,#(msg.readoutValues) do
                playNumber(msg.values[msg.readoutValues[i]], 0)
            end
            msg.raw = nil
        end
    end
    mspProcessTxQ()
end
