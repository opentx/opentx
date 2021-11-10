SCRIPT_HOME = "/SCRIPTS/BF"

assert(loadScript(SCRIPT_HOME.."/MSP/messages.lua"))()

local msg_p = {
  intro = "p.wav",
  readoutValues = {1, 2, 3},
}

local msg_i = {
  intro = "i.wav",
  readoutValues = {4, 5, 6},
}

local msg_d = {
  intro = "d.wav",
  readoutValues = {7, 8},
}

local msg_dsetpt = {
  intro = "dsetpt.wav",
  readoutValues = {1, 2},
}

local pidSelectorField = nil

local function init_pids()
    pidSelectorField = getFieldInfo("trim-thr")
end

local function run_pids()
    local pidSelector = getValue(pidSelectorField.id)
    if pidSelector > 33 and pidSelector < 99 then
        readoutMsp(MSP_PID_FORMAT, msg_p)
    elseif pidSelector > 99 and pidSelector < 165 then
        readoutMsp(MSP_PID_FORMAT, msg_i)
    elseif pidSelector > 165 and pidSelector < 231 then
        readoutMsp(MSP_PID_FORMAT, msg_d)
    elseif pidSelector > -99 and pidSelector < -33 then
        readoutMsp(MSP_PID_ADVANCED_FORMAT, msg_dsetpt)
    end
end

return { init = init_pids, run = run_pids }