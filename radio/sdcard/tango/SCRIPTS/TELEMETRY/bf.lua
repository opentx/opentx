SCRIPT_HOME = "/SCRIPTS/BF"

apiVersion = 0

protocol = assert(loadScript(SCRIPT_HOME.."/protocols.lua"))()
radio = assert(loadScript(SCRIPT_HOME.."/radios.lua"))()

assert(loadScript(radio.preLoad))()
assert(loadScript(protocol.transport))()
assert(loadScript(SCRIPT_HOME.."/MSP/common.lua"))()

isTelemetryScript = true

local run_ui = assert(loadScript(SCRIPT_HOME.."/ui.lua"))()
local background = assert(loadScript(SCRIPT_HOME.."/background.lua"))()

local MENU_TIMESLICE = 100

local lastMenuEvent = 0

function run(event)
  lastMenuEvent = getTime()

  run_ui(event)
end

function run_bg()
  if lastMenuEvent + MENU_TIMESLICE < getTime() then
    background.run_bg()
  end
end

return { init=background.init, run=run, background=run_bg }
