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
-- Delta Wizard pages
local ENGINE_PAGE = 0
local ELEVONS_PAGE = 1
local RUDDER_PAGE = 2
local CONFIRMATION_PAGE = 3

-- Navigation variables
local page = ENGINE_PAGE
local dirty = true
local edit = false
local field = 0
local fieldsMax = 0

-- Model settings
local engineMode = 1
local thrCH1 = 0
local elevCH1 = 0
local elevCH2 = 0
local elevonsMode = 0
local rudderMode = 0
local rudCH1 = 0
local servoPage = nil

-- Common functions
local lastBlink = 0
local function blinkChanged()
  local time = getTime() % 128
  local blink = (time - time % 64) / 64
  if blink ~= lastBlink then
    lastBlink = blink
    return true
  else
    return false
  end
end

local function fieldIncDec(event, value, max, force)
  if edit or force==true then
    if event == EVT_VIRTUAL_PREV or event == EVT_VIRTUAL_PREV_REPT then
      value = (value + max)
      dirty = true
    elseif event == EVT_VIRTUAL_NEXT or event == EVT_VIRTUAL_NEXT_REPT then
      value = (value + max + 2)
      dirty = true
    end
    value = (value % (max+1))
  end
  return value
end

local function valueIncDec(event, value, min, max)
  if edit then
    if event == EVT_VIRTUAL_INC or event == EVT_VIRTUAL_INC_REPT then
      if value < max then
        value = (value + 1)
        dirty = true
      end
    elseif event == EVT_VIRTUAL_DEC or event == EVT_VIRTUAL_DEC_REPT then
      if value > min then
        value = (value - 1)
        dirty = true
      end
    end
  end
  return value
end

local function navigate(event, fieldMax, prevPage, nextPage)
  if event == EVT_VIRTUAL_ENTER then
    edit = not edit
    dirty = true
  elseif edit then
    if event == EVT_VIRTUAL_EXIT then
      edit = false
      dirty = true
    elseif not dirty then
      dirty = blinkChanged()
    end
  else
    if event == EVT_VIRTUAL_NEXT then
      page = nextPage
      field = 0
      dirty = true
    elseif event == EVT_VIRTUAL_PREV then
      page = prevPage
      field = 0
      killEvents(event);
      dirty = true
    else
      field = fieldIncDec(event, field, fieldMax, true)
	end
  end
end

local function getFieldFlags(position)
  flags = 0
  if field == position then
    flags = INVERS
    if edit then
      flags = INVERS + BLINK
    end
  end
  return flags
end

local function channelIncDec(event, value)
  if not edit and event==EVT_VIRTUAL_MENU then
    servoPage = value
    dirty = true
  else
    value = valueIncDec(event, value, 0, 15)
  end
  return value
end

-- Init function
local function init()
  rudCH1 = defaultChannel(0)
  thrCH1 = defaultChannel(2)
  elevCH1 = defaultChannel(1)
  elevCH2 = defaultChannel(3)
end

-- Engine Menu
local engineModeItems = {"No", "Yes"}
local function drawEngineMenu()
  lcd.clear()
  if engineMode == 0 then
    -- No engine
    fieldsMax = 0
  else
    -- 1 channel
    lcd.drawText(5, 30, "Assign channel", 0);
    lcd.drawText(5, 40, ">>>", 0);
    lcd.drawSource(25, 40, MIXSRC_CH1+thrCH1, getFieldFlags(1))
    fieldsMax = 1
  end
  lcd.drawText(1, 0, "Got an engine?", 0)
  lcd.drawFilledRectangle(0, 0, LCD_W, 8, FILL_WHITE)
  lcd.drawCombobox(0, 8, LCD_W, engineModeItems, engineMode, getFieldFlags(0))
end

local function engineMenu(event)
  if dirty then
    dirty = false
    drawEngineMenu()
  end

  navigate(event, fieldsMax, page, page+1)

  if field==0 then
    engineMode = fieldIncDec(event, engineMode, 1)
  elseif field==1 then
    thrCH1 = channelIncDec(event, thrCH1)
  end
end

-- Elevons Menu
local elevonsModeItems = {"2 Channels"}
local function drawElevonsMenu()
  lcd.clear()
  lcd.drawText(1, 0, "Select elevon channnels", 0)
  lcd.drawFilledRectangle(0, 0, LCD_W, 8, FILL_WHITE)
  lcd.drawCombobox(0, 8, LCD_W, elevonsModeItems, elevonsMode, 0)
  lcd.drawText(5, 30, "Assign channels", 0);
  lcd.drawText(30, 40, "L", 0);
  lcd.drawText(65, 40, "R", 0);
  lcd.drawText(5, 50, ">>>", 0);
  lcd.drawSource(25, 50, MIXSRC_CH1+elevCH1, getFieldFlags(0))
  lcd.drawSource(60, 50, MIXSRC_CH1+elevCH2, getFieldFlags(1))
  fieldsMax = 1
end

local function elevonsMenu(event)
  if dirty then
    dirty = false
    drawElevonsMenu()
  end

  navigate(event, fieldsMax, page-1, page+1)

  if field==0 then
    elevCH1 = channelIncDec(event, elevCH1)
  elseif field==1 then
    elevCH2 = channelIncDec(event, elevCH2)
  end
end

-- Rudder menu
local rudderModeItems = {"No", "Yes"}

local function drawRudderMenu()
  lcd.clear()
  if rudderMode == 0 then
    -- No rudder
    fieldsMax = 0
  else
    -- 1 channel
    lcd.drawText(5, 30, "Assign channel", 0);
    lcd.drawText(5, 40, ">>>", 0);
    lcd.drawSource(25, 40, MIXSRC_CH1+rudCH1, getFieldFlags(1))
    fieldsMax = 1
  end
  lcd.drawText(1, 0, "Got a rudder?", 0)
  lcd.drawFilledRectangle(0, 0, LCD_W, 8, FILL_WHITE)
  lcd.drawCombobox(0, 8, LCD_W, rudderModeItems, rudderMode, getFieldFlags(0))
end

local function rudderMenu(event)
  if dirty then
    dirty = false
    drawRudderMenu()
  end

  navigate(event, fieldsMax, page-1, page+1)

  if field==0 then
    rudderMode = fieldIncDec(event, rudderMode, 1)
  elseif field==1 then
    rudCH1 = channelIncDec(event, rudCH1)
  end
end

-- Servo (limits) Menu
local function drawServoMenu(limits)
  lcd.clear()
  lcd.drawSource(1, 0, MIXSRC_CH1+servoPage, 0)
  lcd.drawText(25, 0, "servo min/max/center/direction?", 0)
  lcd.drawFilledRectangle(0, 0, LCD_W, 8, FILL_WHITE)
  lcd.drawText(LCD_W/2-19, LCD_H-8, ">>>", 0);
  lcd.drawNumber(140, 35, limits.min, PREC1+getFieldFlags(0));
  lcd.drawNumber(205, 35, limits.max, PREC1+getFieldFlags(1));
  lcd.drawNumber(170, 9, limits.offset, PREC1+getFieldFlags(2));
  if limits.revert == 0 then
    lcd.drawText(129, 50, "\126", getFieldFlags(3));
  else
    lcd.drawText(129, 50, "\127", getFieldFlags(3));
  end
  fieldsMax = 3
end

local function servoMenu(event)
  local limits = model.getOutput(servoPage)

  if dirty then
    dirty = false
    drawServoMenu(limits)
  end

  navigate(event, fieldsMax, page, page)

  if edit then
    if field==0 then
      limits.min = valueIncDec(event, limits.min, -1000, 0)
    elseif field==1 then
      limits.max = valueIncDec(event, limits.max, 0, 1000)
    elseif field==2 then
      limits.offset = valueIncDec(event, limits.offset, -1000, 1000)
    elseif field==3 then
      limits.revert = fieldIncDec(event, limits.revert, 1)
    end
    model.setOutput(servoPage, limits)
  elseif event == EVT_VIRTUAL_EXIT then
    servoPage = nil
    dirty = true
  end
end

-- Confirmation Menu
local function addMix(channel, input, name, weight, index)
  local mix = { source=input, name=name }
  if weight ~= nil then
    mix.weight = weight
  end
  if index == nil then
    index = 0
  end
  model.insertMix(channel, index, mix)
end

local function applySettings()
  model.defaultInputs()
  model.deleteMixes()
  if engineMode == 1 then
    addMix(thrCH1, MIXSRC_FIRST_INPUT+defaultChannel(2), "Engine")
  end
  addMix(elevCH1, MIXSRC_FIRST_INPUT+defaultChannel(1), "D-EleL", 50)
  addMix(elevCH1, MIXSRC_FIRST_INPUT+defaultChannel(3), "D-AilL", 50, 1)
  addMix(elevCH2, MIXSRC_FIRST_INPUT+defaultChannel(1), "D-EleR", 50)
  addMix(elevCH2, MIXSRC_FIRST_INPUT+defaultChannel(3), "D-AilR", -50, 1)
  if rudderMode == 1 then
    addMix(rudCH1, MIXSRC_FIRST_INPUT+defaultChannel(0), "Rudder")
  end
end

local function drawNextLine(x, y, label, channel)
  lcd.drawText(x, y, label, 0);
  lcd.drawText(x+48, y, ":", 0);
  lcd.drawSource(x+52, y, MIXSRC_CH1+channel, 0)
  y = y + 8
  if y > 50 then
    y = 12
    x = 120
  end
  return x, y
end

local function drawConfirmationMenu()
  local x = 22
  local y = 12
  lcd.clear()
  lcd.drawText(48, 1, "Ready to go?", 0);
  lcd.drawFilledRectangle(0, 0, LCD_W, 9, 0)
  if engineMode == 1 then
    x, y = drawNextLine(x, y, "Throttle", thrCH1)
  end
  x, y = drawNextLine(x, y, "Elevon L", elevCH1)
  x, y = drawNextLine(x, y, "Elevon R", elevCH2)
  if rudderMode == 1 then
    drawNextLine(x, y, "Rudder", rudCH1)
  end
  lcd.drawText(48, LCD_H-8, "[Enter Long] to confirm", 0);
  lcd.drawFilledRectangle(0, LCD_H-9, LCD_W, 9, 0)
  fieldsMax = 0
end

local function confirmationMenu(event)
  if dirty then
    dirty = false
    drawConfirmationMenu()
  end

  navigate(event, fieldsMax, RUDDER_PAGE, page)

  if event == EVT_VIRTUAL_EXIT then
    return 2
  elseif event == EVT_VIRTUAL_ENTER_LONG then
    killEvents(event)
    applySettings()
    return 2
  else
    return 0
  end
end

-- Main

local function run(event)
  if event == nil then
    error("Cannot be run as a model script!")
  end

  if servoPage ~= nil then
    servoMenu(event)
  elseif page == ENGINE_PAGE then
    engineMenu(event)
  elseif page == ELEVONS_PAGE then
    elevonsMenu(event)
  elseif page == RUDDER_PAGE then
    rudderMenu(event)
  elseif page == CONFIRMATION_PAGE then
    return confirmationMenu(event)
  end
  return 0
end

return { init=init, run=run }
