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
-- Plane Wizard pages
local ENGINE_PAGE = 0
local AILERONS_PAGE = 1
local FLAPERONS_PAGE = 2
local BRAKES_PAGE = 3
local TAIL_PAGE = 4
local CONFIRMATION_PAGE = 5

-- Navigation variables
local page = ENGINE_PAGE
local dirty = true
local edit = false
local field = 0
local fieldsMax = 0

-- Model settings
local engineMode = 1
local thrCH1 = 0
local aileronsMode = 1
local ailCH1 = 0
local ailCH2 = 5
local flapsMode = 0
local flapsCH1 = 6
local flapsCH2 = 7
local brakesMode = 0
local brakesCH1 = 8
local brakesCH2 = 9
local tailMode = 1
local eleCH1 = 0
local eleCH2 = 4
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
    if event == EVT_ROT_LEFT or event == EVT_UP_BREAK then
      value = (value + max)
      dirty = true
    elseif event == EVT_ROT_RIGHT or event == EVT_DOWN_BREAK then
      value = (value + max + 2)
      dirty = true
    end
    value = (value % (max+1))
  end
  return value
end

local function valueIncDec(event, value, min, max)
  if edit then
    if event == EVT_ROT_RIGHT or event == EVT_RIGHT_BREAK then
      if value < max then
        value = (value + 1)
        dirty = true
      end
    elseif event == EVT_ROT_LEFT or event == EVT_LEFT_BREAK then
      if value > min then
        value = (value - 1)
        dirty = true
      end
    end
  end
  return value
end

local function navigate(event, fieldMax, prevPage, nextPage)
  if event == EVT_ENTER_BREAK then
    edit = not edit
    dirty = true
  elseif edit then
    if event == EVT_EXIT_BREAK then
      edit = false
      dirty = true
    elseif not dirty then
      dirty = blinkChanged()
    end
  else
    if event == EVT_PAGE_BREAK or event==EVT_RIGHT_BREAK then
      page = nextPage
      field = 0
      dirty = true
    elseif event == EVT_PAGE_LONG or event==EVT_LEFT_BREAK then
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
  if not edit and event==EVT_MENU_BREAK then
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
  eleCH1 = defaultChannel(1)
  thrCH1 = defaultChannel(2)
  ailCH1 = defaultChannel(3)
end

-- Engine Menu
local engineModeItems = {"No", "Yes"}
local function drawEngineMenu()
  lcd.clear()
  if engineMode == 1 then
    -- 1 channel
    lcd.drawText(5, 30, "Assign channel", 0);
    lcd.drawText(5, 40, ">>>", 0);
    lcd.drawSource(25, 40, MIXSRC_CH1+thrCH1, getFieldFlags(1))
    fieldsMax = 1
  else
    -- No engine
    fieldsMax = 0
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

-- Ailerons Menu
local aileronsModeItems = {"No", "Yes, 1 channel", "Yes, 2 channels"}
local function drawAileronsMenu()
  lcd.clear()
  if aileronsMode == 2 then
    -- 2 channels
    lcd.drawText(5, 30, "Assign channels", 0);
    lcd.drawText(30, 40, "L", 0);
    lcd.drawText(65, 40, "R", 0);
    lcd.drawText(5, 50, ">>>", 0);
    lcd.drawSource(25, 50, MIXSRC_CH1+ailCH1, getFieldFlags(1))
    lcd.drawSource(60, 50, MIXSRC_CH1+ailCH2, getFieldFlags(2))
    fieldsMax = 2
  elseif aileronsMode == 1 then
    -- 1 channel
    lcd.drawText(5, 30, "Assign channel", 0);
    lcd.drawText(5, 40, ">>>", 0);
    lcd.drawSource(25, 40, MIXSRC_CH1+ailCH1, getFieldFlags(1))
    fieldsMax = 1
  else
    -- No ailerons
    fieldsMax = 0
  end
  lcd.drawText(1, 0, "Got ailerons?", 0)
  lcd.drawFilledRectangle(0, 0, LCD_W, 8, FILL_WHITE)
  lcd.drawCombobox(0, 8, LCD_W, aileronsModeItems, aileronsMode, getFieldFlags(0))
end

local function aileronsMenu(event)
  if dirty then
    dirty = false
    drawAileronsMenu()
  end

  navigate(event, fieldsMax, page-1, page+1)

  if field==0 then
    aileronsMode = fieldIncDec(event, aileronsMode, 2)
  elseif field==1 then
    ailCH1 = channelIncDec(event, ailCH1)
  elseif field==2 then
    ailCH2 = channelIncDec(event, ailCH2)
  end
end

-- Flaps Menu
local flapsModeItems = {"No", "Yes, 1 channel", "Yes, 2 channels"}
local function drawFlapsMenu()
  lcd.clear()
  if flapsMode == 0 then
    -- no flaps
    fieldsMax = 0
  elseif flapsMode == 1 then
    -- 1 channel
    lcd.drawText(5, 30, "Assign channel", 0);
    lcd.drawText(5, 40, ">>>", 0);
    lcd.drawSource(25, 40, MIXSRC_CH1+flapsCH1, getFieldFlags(1))
    fieldsMax = 1
  elseif flapsMode == 2 then
    -- 2 channels
    lcd.drawText(5, 30, "Assign channels", 0);
    lcd.drawText(30, 40, "L", 0);
    lcd.drawText(65, 40, "R", 0);
    lcd.drawText(5, 50, ">>>", 0);
    lcd.drawSource(25, 50, MIXSRC_CH1+flapsCH1, getFieldFlags(1))
    lcd.drawSource(60, 50, MIXSRC_CH1+flapsCH2, getFieldFlags(2))
    fieldsMax = 2
  end
  lcd.drawText(1, 0, "Got flaps?", 0)
  lcd.drawFilledRectangle(0, 0, LCD_W, 8, FILL_WHITE)
  lcd.drawCombobox(0, 8, LCD_W, flapsModeItems, flapsMode, getFieldFlags(0))
end

local function flapsMenu(event)
  if dirty then
    dirty = false
    drawFlapsMenu()
  end

  navigate(event, fieldsMax, page-1, page+1)

  if field==0 then
    flapsMode = fieldIncDec(event, flapsMode, 2)
  elseif field==1 then
    flapsCH1 = channelIncDec(event, flapsCH1)
  elseif field==2 then
    flapsCH2 = channelIncDec(event, flapsCH2)
  end
end

-- Airbrakes Menu
local brakesModeItems = {"No", "Yes, 1 channel", "Yes, 2 channels"}
local function drawBrakesMenu()
  lcd.clear()
  if brakesMode == 0 then
    -- no brakes
    fieldsMax = 0
  elseif brakesMode == 1 then
    -- 1 channel
    lcd.drawText(5, 30, "Assign channel", 0);
    lcd.drawText(5, 40, ">>>", 0);
    lcd.drawSource(25, 40, MIXSRC_CH1+brakesCH1, getFieldFlags(1))
    fieldsMax = 1
  elseif brakesMode == 2 then
    -- 2 channels
    lcd.drawText(5, 30, "Assign channels", 0);
    lcd.drawText(30, 40, "L", 0);
    lcd.drawText(65, 40, "R", 0);
    lcd.drawText(5, 50, ">>>", 0);
    lcd.drawSource(25, 50, MIXSRC_CH1+brakesCH1, getFieldFlags(1))
    lcd.drawSource(60, 50, MIXSRC_CH1+brakesCH2, getFieldFlags(2))
    fieldsMax = 2
  end
  lcd.drawText(1, 0, "Got air brakes?", 0)
  lcd.drawFilledRectangle(0, 0, LCD_W, 8, FILL_WHITE)
  lcd.drawCombobox(0, 8, LCD_W, brakesModeItems, brakesMode, getFieldFlags(0))
end

local function brakesMenu(event)
  if dirty then
    dirty = false
    drawBrakesMenu()
  end

  navigate(event, fieldsMax, page-1, page+1)

  if field==0 then
    brakesMode = fieldIncDec(event, brakesMode, 2)
  elseif field==1 then
    brakesCH1 = channelIncDec(event, brakesCH1)
  elseif field==2 then
    brakesCH2 = channelIncDec(event, brakesCH2)
  end
end

-- Tail Menu
local tailModeItems = {"Ele(1)", "Ele(1) + Ruder(1)", "Ele(2) + Ruder(1)", "V-Tail(2)"}
local function drawTailMenu()
  lcd.clear()
  if tailMode == 0 then
    -- Elevator(1ch), no rudder...
    lcd.drawText(5, 30, "Assign channel", 0);
    lcd.drawText(5, 40, ">>>", 0);
    lcd.drawSource(25, 40, MIXSRC_CH1+eleCH1, getFieldFlags(1))
    fieldsMax = 1
  elseif tailMode == 1 then
    -- Elevator(1ch) + rudder...
    lcd.drawText(5, 30, "Assign channels", 0);
    lcd.drawText(25, 40, "Ele", 0);
    lcd.drawText(60, 40, "Rud", 0);
    lcd.drawText(5, 50, ">>>", 0);
    lcd.drawSource(25, 50, MIXSRC_CH1+eleCH1, getFieldFlags(1))
    lcd.drawSource(60, 50, MIXSRC_CH1+rudCH1, getFieldFlags(2))
    fieldsMax = 2
  elseif tailMode == 2 then
    -- Elevator(2ch) + rudder...
    lcd.drawText(5, 30, "Assign channels", 0);
    lcd.drawText(25, 40, "EleL", 0);
    lcd.drawText(60, 40, "EleR", 0);
    lcd.drawText(95, 40, "Rud", 0);
    lcd.drawText(5, 50, ">>>", 0);
    lcd.drawSource(25, 50, MIXSRC_CH1+eleCH1, getFieldFlags(1))
    lcd.drawSource(60, 50, MIXSRC_CH1+eleCH2, getFieldFlags(2))
    lcd.drawSource(95, 50, MIXSRC_CH1+rudCH1, getFieldFlags(3))
    fieldsMax = 3
  else
    -- V-Tail...
    lcd.drawText(5, 30, "Assign channels", 0);
    lcd.drawText(25, 40, "VtaL", 0);
    lcd.drawText(60, 40, "VtaR", 0);
    lcd.drawText(5, 50, ">>>", 0);
    lcd.drawSource(25, 50, MIXSRC_CH1+eleCH1, getFieldFlags(1))
    lcd.drawSource(60, 50, MIXSRC_CH1+eleCH2, getFieldFlags(2))
    fieldsMax = 2
  end
  lcd.drawText(1, 0, "Tail config", 0)
  lcd.drawFilledRectangle(0, 0, LCD_W, 8, FILL_WHITE)
  lcd.drawCombobox(0, 8, LCD_W, tailModeItems, tailMode, getFieldFlags(0))
end

local function tailMenu(event)
  if dirty then
    dirty = false
    drawTailMenu()
  end

  navigate(event, fieldsMax, page-1, page+1)

  if field==0 then
    tailMode = fieldIncDec(event, tailMode, 3)
  elseif field==1 then
    eleCH1 = channelIncDec(event, eleCH1)
  elseif (field==2 and tailMode==1) or field==3 then
    rudCH1 = channelIncDec(event, rudCH1)
  elseif field==2 then
    eleCH2 = channelIncDec(event, eleCH2)
  end
end

-- Servo (limits) Menu
local function drawServoMenu(limits)
  lcd.clear()
  lcd.drawSource(1, 0, MIXSRC_CH1+servoPage, 0)
  lcd.drawText(25, 0, "servo min/max/center/direction?", 0)
  lcd.drawFilledRectangle(0, 0, LCD_W, 8, FILL_WHITE)
  lcd.drawLine(LCD_W/2-1, 8, LCD_W/2-1, LCD_H, DOTTED, 0)
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
  elseif event == EVT_EXIT_BREAK then
    servoPage = nil
    dirty = true
  end
end

-- Confirmation Menu
local function drawNextLine(x, y, label, channel)
  lcd.drawText(x, y, label, 0);
  lcd.drawSource(x+30, y, MIXSRC_CH1+channel, 0)
  y = y + 8
  if y > 50 then
    y = 12
    x = 70
  end
  return x, y
end

local function drawConfirmationMenu()
  local x = 5
  local y = 12
  lcd.clear()
  lcd.drawText(0, 1, "Ready to go?", 0);
  lcd.drawFilledRectangle(0, 0, LCD_W, 9, 0)
  if engineMode == 1 then
    x, y = drawNextLine(x, y, "Thr", thrCH1)
  end
  if aileronsMode == 1 then
    x, y = drawNextLine(x, y, "Ail", ailCH1)
  elseif aileronsMode == 2 then
    x, y = drawNextLine(x, y, "AilL", ailCH1)
    x, y = drawNextLine(x, y, "AilR", ailCH2)
  end
  if flapsMode == 1 then
    x, y = drawNextLine(x, y, "Flap", flapsCH1)
  elseif flapsMode == 2 then
    x, y = drawNextLine(x, y, "FlpL", flapsCH1)
    x, y = drawNextLine(x, y, "FlpR", flapsCH2)
  end
  if brakesMode == 1 then
    x, y = drawNextLine(x, y, "Brak", brakesCH1)
  elseif brakesMode == 2 then
    x, y = drawNextLine(x, y, "BrkL", brakesCH1)
    x, y = drawNextLine(x, y, "BrkR", brakesCH2)
  end
  if tailMode == 3 then
    x, y = drawNextLine(x, y, "VtaL", eleCH1)
    x, y = drawNextLine(x, y, "VtaR", eleCH2)
  else
    x, y = drawNextLine(x, y, "Rud", rudCH1)
    if tailMode == 1 then
      x, y = drawNextLine(x, y, "Elev", eleCH1)
    elseif tailMode == 2 then
      x, y = drawNextLine(x, y, "EleL", eleCH1)
      x, y = drawNextLine(x, y, "EleR", eleCH2)
    end
  end
  lcd.drawText(0, LCD_H-8, "[Enter Long] to confirm", 0);
  lcd.drawFilledRectangle(0, LCD_H-9, LCD_W, 9, 0)
  fieldsMax = 0
end

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
  if engineMode > 0 then
    addMix(thrCH1, MIXSRC_FIRST_INPUT+defaultChannel(2), "Engine")
  end
  if aileronsMode == 1 then
    addMix(ailCH1, MIXSRC_FIRST_INPUT+defaultChannel(3), "Ail")
  elseif aileronsMode == 2 then
    addMix(ailCH1, MIXSRC_FIRST_INPUT+defaultChannel(3), "AilL")
    addMix(ailCH2, MIXSRC_FIRST_INPUT+defaultChannel(3), "AilR", -100)
  end
  if flapsMode == 1 then
    addMix(flapsCH1, MIXSRC_SA, "Flap")
  elseif flapsMode == 2 then
    addMix(flapsCH1, MIXSRC_SA, "FlapL")
    addMix(flapsCH2, MIXSRC_SA, "FlapR")
  end
  if brakesMode == 1 then
    addMix(brakesCH1, MIXSRC_SD, "Brake")
  elseif brakesMode == 2 then
    addMix(brakesCH1, MIXSRC_SD, "BrakeL")
    addMix(brakesCH2, MIXSRC_SD, "BrakeR")
  end
  if tailMode == 3 then
    addMix(eleCH1, MIXSRC_FIRST_INPUT+defaultChannel(1), "V-EleL", 50)
    addMix(eleCH1, MIXSRC_FIRST_INPUT+defaultChannel(0), "V-RudL", 50, 1)
    addMix(eleCH2, MIXSRC_FIRST_INPUT+defaultChannel(1), "V-EleR", 50)
    addMix(eleCH2, MIXSRC_FIRST_INPUT+defaultChannel(0), "V-RudR", -50, 1)
  else
    if tailMode > 0 then
      addMix(rudCH1, MIXSRC_FIRST_INPUT+defaultChannel(0), "Rudder")
    end
    if tailMode == 1 then
      addMix(eleCH1, MIXSRC_FIRST_INPUT+defaultChannel(1), "Elev")
    elseif tailMode == 2 then
      addMix(eleCH1, MIXSRC_FIRST_INPUT+defaultChannel(1), "ElevG")
      addMix(eleCH2, MIXSRC_FIRST_INPUT+defaultChannel(1), "ElevD")
    end
  end
end

local function confirmationMenu(event)
  if dirty then
    dirty = false
    drawConfirmationMenu()
  end

  navigate(event, fieldsMax, TAIL_PAGE, page)

  if event == EVT_EXIT_BREAK then
    return 2
  elseif event == EVT_ENTER_LONG then
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
  elseif page == AILERONS_PAGE then
    aileronsMenu(event)
  elseif page == FLAPERONS_PAGE then
    flapsMenu(event)
  elseif page == BRAKES_PAGE then
    brakesMenu(event)
  elseif page == TAIL_PAGE then
    tailMenu(event)
  elseif page == CONFIRMATION_PAGE then
    return confirmationMenu(event)
  end
  return 0
end

return { init=init, run=run }
