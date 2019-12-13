
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

local version = "v1.1"

local VALUE = 0
local COMBO = 1
local FLPOI = 2


local edit = false
local page = 1
local current = 1  --row
local refreshState = 0
local refreshIndex = 0
local refreshIndex3 = 0
local pageOffset = 0
local pages = {}
local fields = {}
local modifications = {}
local thistime = getTime()
local lastTime = thistime
local margin = 1
local spacing = 8
local configFields = {}
local counter = 0
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

local interfaceconfig = {
  {"Sensor group select", VALUE, appId, nil, 0, 15},
}

local settingsFields = {
  {"Software version", FLPOT, 0x0c, nil, 1, 100 },
  {"Physical ID", VALUE, 0x01, nil, 0, 26 },
  {"Application IDgroup", VALUE, 0x0D, nil, 0, 15 },
  {"Data rate(*100ms)", VALUE, 0x22, nil, 1, 255 },
}

local telemetryFields = {
  {"TEMP1(C/F)", VALUE, 0x90, nil, -30, 600},
  {"TEMP2(C/F)", VALUE, 0x91, nil, -30, 600},
  {"SPEED(r/min)", VALUE, 0x92, nil, 0, 100000},
  {"Residual Volume(mL)", VALUE, 0x93, nil, 0, 60000},
  {"Residual Percent(%)", VALUE, 0x94, nil, 0, 100},
  {"FLOW(mL/min)", VALUE, 0x95, nil, 0, 2000},
  {"Max Flow(mL/min)", VALUE, 0x96, nil, 0, 2000},
  {"Avg Flow(mL/min)", VALUE, 0x97, nil, 0, 2000},
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

-- Select the next or previous page
local function selectPage(step)
  page = 1 + ((page + step - 1 + #pages) % #pages)
  refreshIndex = 0
  pageOffset = 0
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
  drawScreenTitle("GasSuit", page, #pages)
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
        if (field[3] == 0x90 or field[3] == 0x91) and field[4] == 500 then
          lcd.drawText(LCD_W, margin+ spacing * index, "---", attr + RIGHT)
        else

          lcd.drawNumber(LCD_W, margin+ spacing * index, field[4] , attr + RIGHT)
        end
      elseif field[2] == COMBO then
        if field[4] >= 0 and field[4] < #(field[5]) then
          lcd.drawText(LCD_W, margin+ spacing * index, field[5][1+field[4]], attr + RIGHT)
        end
      elseif field[2] == FLPOT then
        lcd.drawText(LCD_W, margin+ spacing * index, field[4], attr + RIGHT)
      end
    end
  end
end

local function telemetryRead(fieldx)
  return sportTelemetryPush(0x1b, 0x30, appId, fieldx)
end

local function telemetryListen(fieldx)
  return sportTelemetryPush(0x1b,  0,  0, 0)
end

local function telemetryWrite(fieldx, valuex)
  return sportTelemetryPush(0x1b, 0x31, appId, fieldx + valuex*256)
end

local telemetryPopTimeout = 0
local function refreshNext()
  if refreshState == 0 and page ~= 3 then  --
    if #modifications > 0 then  --
      if modifications[1][1] ~= 0x0c then
        if modifications[1][1] == 0x22 then
          modifications[1][2] = modifications[1][2] + 0xf00
        end
        local modificationstmp = modifications[1][2]
        if modifications[1][1] == 0x8a or modifications[1][1] == 0x8b then
          if configFields[11][4]== 1 then
            modificationstmp = (modifications[1][2] - 32)*10
            modificationstmp = math.floor(modificationstmp/18)
          end
        end
        telemetryWrite(modifications[1][1], modificationstmp)
        refreshIndex = 0
      end
      modifications[1] = nil
    elseif refreshIndex < #fields then
      local field = fields[refreshIndex + 1]
      if telemetryRead(field[3]) == true then
        refreshState = 1
        telemetryPopTimeout = getTime() + 80
      end
    end
  elseif refreshState == 0 and page == 3 then
    if #modifications > 0 then
      if modifications[1][1] == 0x96 or modifications[1][1] == 0x97 or modifications[1][1] == 0x93  then
        telemetryWrite(modifications[1][1], 0)
      end
      modifications[1] = nil
    elseif refreshIndex < #fields then
      local field = fields[refreshIndex + 1]
      if telemetryRead(field[3]) == true then
        refreshState = 1
        telemetryPopTimeout = getTime() + 20
      end
    elseif refreshIndex >= #fields then
      refreshIndex = 0
      refreshState = 0
    end
  elseif refreshState == 1  and page ~= 3  then
    local physicalId, primId, dataId, value = sportTelemetryPop()
    if  primId == 0x32 and dataId >= 0x0d00 and dataId <= 0x0d7f then
      local fieldId = value % 256
      local field = fields[refreshIndex + 1]
      if fieldId == field[3] then
        local value = math.floor(value / 256)
        if field[2] == COMBO then
          for index = 1, #(field[6]), 1 do
            if value == field[6][index] then
              value = index - 1
              break
            end
          end
        elseif field[2] == VALUE  then
          value = value -- - field[8] + field[5]
        end
        if field[1] == "Software version" then
          local flo_string = string.char(value/16%16 + 48).."."..string.char(value%16 + 48)
          fields[refreshIndex + 1][4] = flo_string
        else
          fields[refreshIndex + 1][4] = value
        end
        refreshIndex = refreshIndex + 1
        refreshState = 0
      end
    elseif getTime() > telemetryPopTimeout then
      refreshState = 0
    end
  elseif  refreshState == 1 and page == 3  then
    local pageID3 = 0
    local physicalId3, primId3, dataId3, value3 = sportTelemetryPop()
    if value3 ~= nil then
      pageID3 = value3 % 256
      value3 = math.floor(value3 / 0x100)
    end
    if primId3 == 0x32 and value3 ~= nil then
      if pageID3 == fields[refreshIndex + 1][3] then
        local field = fields[refreshIndex + 1]
        if field[2] == COMBO and #field == 6 then
          for index = 1, #(field[6]), 1 do
            if value3 == field[6][index] then
              value3 = index - 1
              break
            end
          end
        elseif field[2] == VALUE  then
          if field[3] == 0x90 or field[3] == 0x91 then
            value3 = math.floor(value3 % 0x10000)
            if value3 > 0xf000 then
              value3 = value3 - 0x10000
            end
            if configFields[11][4]== 1 and (field[3] == 0x90 or field[3] == 0x91) then
              value3 =(value3*18)
              value3 = math.floor(value3/10)+32
            end
          else
            value3 = value3
          end
        end
        fields[refreshIndex + 1][4] = value3
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
  if field[2] == COMBO and #field == 6 then
    value = field[6][1+value]
  elseif field[2] == VALUE  and #field == 6 then
    value = value -- + field[8] - field[5]
  elseif field[2] == FLPOT then
    value = 0
  end
  modifications[#modifications+1] = {field[3], value}
end

-- Main1
local function runFieldsPage(event)
  if event == EVT_VIRTUAL_EXIT then
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

local function runConfigPage(event)
  fields = configFields
  local result = runFieldsPage(event)
  return result
end

local function runSettingsPage(event)
  fields = settingsFields
  return runFieldsPage(event)
end

local function runTelemetryPage(event)
  fields = telemetryFields
  return runFieldsPage(event)
end

-- Init
local function init()
  current, edit, refreshState, refreshIndex = 1, false, 0, 0
  if LCD_W == 480 then
    margin = 10
    spacing = 20
  end
  if LCD_W == 128 then
    configFields = {
      {"CDI off speed limit", VALUE, 0x81, nil, 10,10000}, 				-- 1
      {"Milliliter per pulse", VALUE, 0x80, nil, 1, 2000}, 				-- 2
      {"Volume", VALUE, 0x83, nil, 10,60000},              				-- 3
      --	{"Flow trigger", VALUE, 0x84, nil, 5, 50 },
      --	{"Flow Reset", COMBO, 0x85, nil, { "ON", "OFF" }, {1 , 0}},
      {"Auto Reset", COMBO, 0x8d, nil, { "ON", "OFF" }, {0 , 1}},         -- 4
      {"Reset settings", COMBO, 0x86, nil,{ "YES", "NO" }, {1 , 0} },     -- 5
      {"Volume alarm(%)", VALUE, 0x87, nil, 0, 90 }, 						-- 6
      {"Max.Flow alarm", VALUE, 0x88, nil, 0, 2000 },						-- 7
      {"Over speed alarm", VALUE, 0x89, nil, 0, 10000 },					-- 8
      {"Over temp1 alarm", VALUE, 0x8a, nil, 0, 600 },					-- 9
      {"Over temp2 alarm", VALUE, 0x8b, nil, 0, 600 },					-- 10
      {"Temperature C/F", COMBO, 0x8c, nil,{ "C", "F" }, {0 , 1} },		-- 11
    }
  else
    configFields = {
      {"CDI off speed limit(*100RPM)", VALUE, 0x81, nil, 10,10000},
      {"Milliliter per pulse(*0.001mL/pul)", VALUE, 0x80, nil, 1, 2000},
      {"Volume(mL)", VALUE, 0x83, nil, 10,60000},
      --	  {"Flow trigger(mL/min)", VALUE, 0x84, nil, 5, 50 },
      --   {"Flow Reset", COMBO, 0x85, nil, { "ON", "OFF" }, {1 , 0}},
      {"Auto Reset", COMBO, 0x8d, nil, { "ON", "OFF" }, {0 , 1}},
      {"Reset to factory settings", COMBO, 0x86, nil,{ "YES", "NO" }, {1 , 0} },
      {"Volume alarm(%)", VALUE, 0x87, nil, 0, 90 },
      {"Max.Flow alarm(mL/min)", VALUE, 0x88, nil, 0, 2000 },
      {"Over speed alarm(*100RPM)", VALUE, 0x89, nil, 0, 10000 },
      {"Over temperature1 alarm(C/F)", VALUE, 0x8a, nil, 0, 600 },
      {"Over temperature2 alarm(C/F)", VALUE, 0x8b, nil, 0, 600 },
      {"Temperature Celsius/Fahrenheit", COMBO, 0x8c, nil,{ "C", "F" }, {0 , 1} },
    }
  end

  pages = {
    runConfigPage,
    runSettingsPage,
    runTelemetryPage,
  }

  -- Warning : GaSuite tool requires Temp2 to be connected and discovered for script to work
  for index = 1, 40, 1 do
    local sensor = model.getSensor(index)
    if sensor ~= nil and sensor.id >= 0x0d10 and sensor.id <= 0x0d1f then
      appId = sensor.id
      break
    end
  end

  if appId == 0 then
    error("No GasSuit sensor in this model!")
  end
end

local function background()
  local tonefrq,tonelength,tonepause
  thistime = getTime()
  lastTime = thistime
  refreshNext()
  if page == 3  then --alarm
    local alarmnum = 0
    if fields[3][4] ~= nil and configFields[9][4] ~= nil then
      local speedtest = fields[3][4]
      local speedover = configFields[9][4]*100
      if speedtest >= speedover then
        tonefrq = 800 + math.max(0,math.floor((speedtest - speedover)/10))
        tonelength = 50 + math.max(0,(150 - math.floor((speedtest - speedover)/10)))
        tonepause = tonelength
        playTone(tonefrq, tonelength, tonepause, PLAY_BACKGROUND,10)
        alarmnum = alarmnum + 1
      end
    end
    if  fields[5][4] ~= nil and configFields[7][4] ~= nil then
      local  Residualtest = fields[5][4]
      local  Residualline = configFields[6][4]
      if Residualtest < Residualline then
        tonefrq = 400
        tonelength = 100
        tonepause = 1000 + math.floor( Residualline - Residualtest )*30
        playTone(tonefrq, tonelength, tonepause, PLAY_BACKGROUND,10)
        alarmnum = alarmnum + 1
      end
    end
    if   fields[6][4] ~= nil and configFields[8][4] ~= nil then
      local  Flowtest = fields[6][4]
      local  Flowover = configFields[7][4]
      if Flowtest > Flowover then
        tonefrq = 400
        tonelength = 100
        tonepause = 200
        playTone(tonefrq, tonelength, tonepause, PLAY_BACKGROUND,10)
        alarmnum = alarmnum + 1
      end
    end
    if  fields[1][4] ~= nil and configFields[10][4] ~= nil then
      local  temp1test = fields[1][4]    --
      local  temp1over = configFields[9][4]
      if temp1test > temp1over and (temp1test ~= 500 and temp1test ~= 932) then
        tonefrq = 2000
        tonelength = 100
        tonepause = 900
        playTone(tonefrq, tonelength, tonepause, PLAY_BACKGROUND,10)
        alarmnum = alarmnum + 1
      end
    end
    if  fields[2][4] ~= nil and configFields[11][4] ~= nil then
      local  temp2test = fields[2][4]
      local  temp2over = configFields[10][4]
      if temp2test > temp2over and (temp2test ~= 500 and temp2test ~= 932) then
        tonefrq = 2000
        tonelength = 300
        tonepause = 700
        playTone(tonefrq, tonelength, tonepause, PLAY_BACKGROUND,10)
        alarmnum = alarmnum + 1
      end
    end
    if alarmnum > 1 then
      playTone(2000, 100, 100, PLAY_BACKGROUND,10)
    end
    alarmnum = 0
  end
end

local function run(event)
  if event == nil then
    error("Cannot be run as a sensor script!")
    return 2
  elseif event == EVT_VIRTUAL_NEXT_PAGE then
    selectPage(1)
  elseif event == EVT_VIRTUAL_PREV_PAGE then
    killEvents(event);
    selectPage(-1)
  end
  local result = pages[page](event)
  if page ~= 3 then
    refreshNext()
  end
  background()
  return result
end

return { init=init, background=background, run=run }
