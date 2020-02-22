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

local version = "v1.2"

local VALUE = 0
local COMBO = 1

local edit = false
local page = 1
local current = 1  --row
local refreshState = 0
local refreshIndex = 0
local pageOffset = 0
local pages = {}
local fields = {}
local modifications = {}
local margin = 1
local spacing = 8
local appId = 0

local function drawScreenTitle(title,page, pages)
  if math.fmod(math.floor(getTime()/100),10) == 0 then
    title = version
  end
  if LCD_W == 480 then
    lcd.drawFilledRectangle(0, 0, LCD_W, 30, TITLE_BGCOLOR)
    lcd.drawText(1, 5, title, MENU_TITLE_COLOR)
    lcd.drawText(LCD_W-40, 5, page.."/"..pages, MENU_TITLE_COLOR)
  else
    lcd.drawScreenTitle(title, page, pages)
  end
end

local settingsFields = {
  {"SBEC OUTPUT (V)", VALUE, 0x80, nil, 50, 84 },
  {"Physical ID", VALUE, 0x01, nil, 0, 26 },
  {"Application IDgroup", VALUE, 0x0D, nil, 0, 15 },
  {"Data rate(*100ms)", VALUE, 0x22, nil, 1, 255 },
}

-- Change display attribute to current field
local function addField(step)
  local field = fields[current]
  local min, max
  if field[2] == VALUE then
    min = field[5]
    max = field[6]
  elseif field[2] == COMBO then
    min = 0
    max = #(field[5]) - 1
  end
  if field[2] ~= FLPOT then    --only read ?
    if field[2] == VALUE and field[6] > 256 then
      if field[4] > 1000 then
        step = step * 50
        field[4] = math.floor(field[4]/50)
        field[4] = field[4] *50
      elseif field[4] > 500 then
        step = step * 20
        field[4] = math.floor(field[4]/20)
        field[4] = field[4] *20
      elseif field[4] > 200 then
        step = step * 10
        field[4] = math.floor(field[4]/10)
        field[4] = field[4] *10
      else
        step = step * 1
        field[4] = math.floor(field[4]/1)
        field[4] = field[4] *1
      end
    end
    if (step < 0 and field[4] > min) or (step > 0 and field[4] < max) then
      field[4] = field[4] + step
    end
  else
    field[4] = field[4]
  end
end


-- Select the next or previous editable field
local function selectField(step)
  current = current + step
  if current > #fields then
    current = #fields
  elseif current < 1 then
    current = 1
  end
  if current > 7 + pageOffset then
    pageOffset = current - 7
  elseif current <= pageOffset then
    pageOffset = current - 1
  end
end

local function drawProgressBar()
  if LCD_W == 480 then
    local width = (300 * refreshIndex) / #fields
    lcd.drawRectangle(100, 10, 300, 6)
    lcd.drawFilledRectangle(102, 13, width, 2);
  else
    local width = (60 * refreshIndex) / #fields
    lcd.drawRectangle(45, 1, 60, 6)
    lcd.drawFilledRectangle(47, 3, width, 2);
  end
end

-- Redraw the current page
local function redrawFieldsPage()
  lcd.clear()
  drawScreenTitle("SBEC", page, #pages)
  if refreshIndex < #fields then
    drawProgressBar()
  end
  for index = 1, 7, 1 do
    local field = fields[pageOffset+index]
    if field == nil then
      break
    end
    local attr = current == (pageOffset+index) and ((edit == true and BLINK or 0) + INVERS) or 0
    lcd.drawText(1, margin+ spacing * index, field[1])
    if field[4] == nil then
      lcd.drawText(LCD_W, margin+ spacing * index, "---", attr + RIGHT)
    else
      if field[2] == VALUE then
        if field[3] == 0x80 then
          lcd.drawNumber(LCD_W, margin+ spacing * index, field[4] , attr + RIGHT + PREC1)
        else
          lcd.drawNumber(LCD_W, margin+ spacing * index, field[4] , attr + RIGHT)
        end
      end
    end
  end
end

local function telemetryRead(fieldx)
  return sportTelemetryPush(0x17, 0x30, appId, fieldx)
end

local function telemetryIdle(field)
  return sportTelemetryPush(0x17, 0x21, appId, field)
end

local function telemetryUnIdle(field)
  return sportTelemetryPush(0x17, 0x20, appId, field)
end

local function telemetryWrite(fieldx, valuex)
  return sportTelemetryPush(0x17, 0x31, appId, fieldx + valuex*256)
end

local telemetryPopTimeout = 0
local function refreshNext()
  if refreshState == 0 then
    if #modifications > 0 then
      local modificationstmp = modifications[1][2]
      telemetryWrite(modifications[1][1], modificationstmp)
      refreshIndex = 0
      modifications[1] = nil
    elseif refreshIndex < #fields then
      local field = fields[refreshIndex + 1]
      if telemetryRead(field[3]) == true then
        refreshState = 1
        telemetryPopTimeout = getTime() + 80
      end
    end
  elseif refreshState == 1 then
    local physicalId, primId, dataId, value = sportTelemetryPop()
    if primId == 0x32 and dataId == appId then
      local fieldId = value % 256
      local field = fields[refreshIndex + 1]
      if fieldId == field[3] then
        local value = math.floor(value / 256)
        if field[2] == VALUE  then
          value = value
        end

        fields[refreshIndex + 1][4] = value
        refreshIndex = refreshIndex + 1
        refreshState = 0
      end
    elseif getTime() > telemetryPopTimeout then
      refreshState = 0
    end
  end
end

local function updateField(field)
  local value = field[4]
  if field[2] == VALUE  and #field == 6 then
    value = value
  end
  modifications[#modifications+1] = {field[3], value}
end

-- Main1
local function runFieldsPage(event)
  if event == EVT_VIRTUAL_EXIT then
    telemetryUnIdle(0x80)
    return 2
  elseif event == EVT_VIRTUAL_ENTER then
    if fields[current][4] ~= nil then
      edit = not edit
      if edit == false then
        updateField(fields[current])
      end
    end
  elseif edit then
    if event == EVT_VIRTUAL_INC or event == EVT_VIRTUAL_INC_REPT then
      addField(1)
    elseif event == EVT_VIRTUAL_DEC or event == EVT_VIRTUAL_DEC_REPT then
      addField(-1)
    end
  else
    if event == EVT_VIRTUAL_NEXT then
      selectField(1)
    elseif event == EVT_VIRTUAL_PREV then
      selectField(-1)
    end
  end
  redrawFieldsPage()
  return 0
end

local function runSettingsPage(event)
  fields = settingsFields
  return runFieldsPage(event)
end

-- Init
local function init()
  current, edit, refreshState, refreshIndex = 1, false, 0, 0
  if LCD_W == 480 then
    margin = 10
    spacing = 20
  end

  pages = {
    runSettingsPage,
  }

  for index = 1, 40, 1 do
    local sensor = model.getSensor(index)
    if sensor ~= nil and sensor.id >= 0x0e50 and sensor.id <= 0x0e5f then
      appId = sensor.id
      break
    end
  end

  if appId == 0 then
    error("No SBEC sensor in this model!")
  end

  telemetryIdle(0x80)
end

local function run(event)
  if event == nil then
    error("Cannot run as a model script!")
    return 2
  end
  local result = pages[page](event)

  refreshNext()

  return result
end

return { init=init, background=background, run=run }
