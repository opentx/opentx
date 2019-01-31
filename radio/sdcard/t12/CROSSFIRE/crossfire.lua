---- #########################################################################
---- #                                                                       #
---- # Copyright (C) OpenTX                                                  #
-----#                                                                       #
---- # License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html               #
---- #                                                                       #
---- # This program is free software; you can redistribute it and/or modify  #
---- # it under the terms of the GNU General Public License version 2 as     #
---- # published by the Free Software Foundation.                            #
---- #                                                                       #
---- # This program is distributed in the hope that it will be useful        #
---- # but WITHOUT ANY WARRANTY; without even the implied warranty of        #
---- # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
---- # GNU General Public License for more details.                          #
---- #                                                                       #
---- #########################################################################
local devices = { }
local lineIndex = 1
local pageOffset = 0

local function createDevice(id, name)
  local device = {
    id = id,
    name = name,
    timeout = 0
  }
  return device
end

local function getDevice(name)
  for i=1, #devices do
    if devices[i].name == name then
      return devices[i]
    end
  end
  return nil
end

local function parseDeviceInfoMessage(data)
  local id = data[2]
  local name = ""
  local i = 3
  while data[i] ~= 0 do
    name = name .. string.char(data[i])
    i = i + 1
  end
  local device = getDevice(name)
  if device == nil then
    device = createDevice(id, name)
    devices[#devices + 1] = device
  end
  local time = getTime()
  device.timeout = time + 3000 -- 30s
  if lineIndex == 0 then
    lineIndex = 1
  end
end

local devicesRefreshTimeout = 0
local function refreshNext()
  local command, data = crossfireTelemetryPop()
  if command == nil then
    local time = getTime()
    if time > devicesRefreshTimeout then
      devicesRefreshTimeout = time + 100 -- 1s
      crossfireTelemetryPush(0x28, { 0x00, 0xEA })
    end
  elseif command == 0x29 then
    parseDeviceInfoMessage(data)
  end
end

local function selectDevice(step)
  lineIndex = 1 + ((lineIndex + step - 1 + #devices) % #devices)
end

-- Init
local function init()
  lineIndex = 0
  pageOffset = 0
end

-- Main
local function run(event)
  if event == nil then
    error("Cannot be run as a model script!")
    return 2
  elseif event == EVT_EXIT_BREAK then
    return 2
  elseif event == EVT_ROT_LEFT or event == EVT_DOWN_BREAK then
    selectDevice(1)
  elseif event == EVT_ROT_RIGHT or event == EVT_UP_BREAK then
    selectDevice(-1)
  end

  lcd.clear()
  lcd.drawScreenTitle("CROSSFIRE SETUP", 0, 0)
  if #devices == 0 then
    lcd.drawText(24, 28, "Waiting for Crossfire devices...")
  else
    for i=1, #devices do
      local attr = (lineIndex == i and INVERS or 0)
      if (event == EVT_ROT_BREAK or event == EVT_RIGHT_BREAK) and attr == INVERS then
          crossfireTelemetryPush(0x28, { devices[i].id, 0xEA })
          return "device.lua"
      end
      lcd.drawText(0, i*8+9, devices[i].name, attr)
    end
  end

  refreshNext()

  return 0
end

return { init=init, run=run }
