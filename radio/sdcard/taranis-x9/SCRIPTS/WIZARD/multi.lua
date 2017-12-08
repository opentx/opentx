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
-- Multicopter Wizard pages
local THROTTLE_PAGE = 0
local ROLL_PAGE = 1
local PITCH_PAGE = 2
local YAW_PAGE = 3
local CONFIRMATION_PAGE = 4

-- Navigation variables
local page = THROTTLE_PAGE
local dirty = true
local edit = false
local field = 0
local fieldsMax = 0

-- Model settings
local thrCH1 = 0
local rollCH1 = 0
local yawCH1 = 0
local pitchCH1 = 0

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
    if event == EVT_PLUS_BREAK or event == EVT_ROT_LEFT then
      value = (value + max)
      dirty = true
    elseif event == EVT_MINUS_BREAK or event == EVT_ROT_RIGHT then
      value = (value + max + 2)
      dirty = true
    end
    value = (value % (max+1))
  end
  return value
end

local function valueIncDec(event, value, min, max)
  if edit then
    if event == EVT_PLUS_FIRST or event == EVT_PLUS_REPT or event == EVT_ROT_RIGHT then
      if value < max then
        value = (value + 1)
        dirty = true
      end
    elseif event == EVT_MINUS_FIRST or event == EVT_MINUS_REPT or event == EVT_ROT_LEFT then
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
    if event == EVT_PAGE_BREAK then
      page = nextPage
      field = 0
      dirty = true
    elseif event == EVT_PAGE_LONG then
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
  thrCH1 = defaultChannel(2)
  rollCH1 = defaultChannel(3)
  yawCH1 = defaultChannel(0)
  pitchCH1 = defaultChannel(1)
end

-- Throttle Menu
local function drawThrottleMenu()
  lcd.clear()
  lcd.drawText(1, 0, "Select multicopter throttle channel", 0)
  lcd.drawFilledRectangle(0, 0, LCD_W, 8, GREY_DEFAULT+FILL_WHITE)
  lcd.drawLine(LCD_W/2-1, 18, LCD_W/2-1, LCD_H-1, DOTTED, 0)
  lcd.drawPixmap(120, 8, "multi-thr.bmp")
  lcd.drawText(25, LCD_H-16, "Assign channel", 0);
  lcd.drawText(LCD_W/2-19, LCD_H-8, ">>>", 0);
  lcd.drawSource(113, LCD_H-8, MIXSRC_CH1+thrCH1, getFieldFlags(0))
  fieldsMax = 0
end

local function throttleMenu(event)
  if dirty then
    dirty = false
    drawThrottleMenu()
  end
  navigate(event, fieldsMax, page, page+1)
  thrCH1 = channelIncDec(event, thrCH1)
end

-- Roll Menu
local function drawRollMenu()
  lcd.clear()
  lcd.drawText(1, 0, "Select multicopter roll channel", 0)
  lcd.drawFilledRectangle(0, 0, LCD_W, 8, GREY_DEFAULT+FILL_WHITE)
  lcd.drawLine(LCD_W/2-1, 18, LCD_W/2-1, LCD_H-1, DOTTED, 0)
  lcd.drawPixmap(120, 8, "multi-roll.bmp")
  lcd.drawText(25, LCD_H-16, "Assign channel", 0);
  lcd.drawText(LCD_W/2-19, LCD_H-8, ">>>", 0);
  lcd.drawSource(113, LCD_H-8, MIXSRC_CH1+rollCH1, getFieldFlags(0))
  fieldsMax = 0
end

local function rollMenu(event)
  if dirty then
    dirty = false
    drawRollMenu()
  end
  navigate(event, fieldsMax, page-1, page+1)
  rollCH1 = channelIncDec(event, rollCH1)
end

-- Pitch Menu
local function drawPitchMenu()
  lcd.clear()
  lcd.drawText(1, 0, "Select multicopter pitch channel", 0)
  lcd.drawFilledRectangle(0, 0, LCD_W, 8, GREY_DEFAULT+FILL_WHITE)
  lcd.drawLine(LCD_W/2-1, 18, LCD_W/2-1, LCD_H-1, DOTTED, 0)
  lcd.drawPixmap(120, 8, "multi-pitch.bmp")
  lcd.drawText(25, LCD_H-16, "Assign channel", 0);
  lcd.drawText(LCD_W/2-19, LCD_H-8, ">>>", 0);
  lcd.drawSource(113, LCD_H-8, MIXSRC_CH1+pitchCH1, getFieldFlags(0))
  fieldsMax = 0
end

local function pitchMenu(event)
  if dirty then
    dirty = false
    drawPitchMenu()
  end
  navigate(event, fieldsMax, page-1, page+1)
  pitchCH1 = channelIncDec(event, pitchCH1)
end

-- Yaw Menu
local function drawYawMenu()
  lcd.clear()
  lcd.drawText(1, 0, "Select multicopter yaw channel", 0)
  lcd.drawFilledRectangle(0, 0, LCD_W, 8, GREY_DEFAULT+FILL_WHITE)
  lcd.drawLine(LCD_W/2-1, 18, LCD_W/2-1, LCD_H-1, DOTTED, 0)
  lcd.drawPixmap(120, 8, "multi-yaw.bmp")
  lcd.drawText(25, LCD_H-16, "Assign channel", 0);
  lcd.drawText(LCD_W/2-19, LCD_H-8, ">>>", 0);
  lcd.drawSource(113, LCD_H-8, MIXSRC_CH1+yawCH1, getFieldFlags(0))
  fieldsMax = 0
end

local function yawMenu(event)
  if dirty then
    dirty = false
    drawYawMenu()
  end
  navigate(event, fieldsMax, page-1, page+1)
  yawCH1 = channelIncDec(event, yawCH1)
end

-- Confirmation Menu
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
  x, y = drawNextLine(x, y, "Throttle", thrCH1)
  x, y = drawNextLine(x, y, "Roll", rollCH1)
  x, y = drawNextLine(x, y, "Pitch", pitchCH1)
  x, y = drawNextLine(x, y, "Yaw", yawCH1)
  lcd.drawText(48, LCD_H-8, "Long [ENT] to confirm", 0);
  lcd.drawFilledRectangle(0, LCD_H-9, LCD_W, 9, 0)
  lcd.drawPixmap(LCD_W-18, LCD_H-17, "confirm-tick.bmp")
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
  addMix(thrCH1,   MIXSRC_FIRST_INPUT+defaultChannel(2), "Engine")
  addMix(rollCH1,  MIXSRC_FIRST_INPUT+defaultChannel(3), "Roll")
  addMix(yawCH1,   MIXSRC_FIRST_INPUT+defaultChannel(0), "Yaw")
  addMix(pitchCH1, MIXSRC_FIRST_INPUT+defaultChannel(1), "Pitch")
end

local function confirmationMenu(event)
  if dirty then
    dirty = false
    drawConfirmationMenu()
  end

  navigate(event, fieldsMax, YAW_PAGE, page)

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
  if page == THROTTLE_PAGE then
    throttleMenu(event)
  elseif page == ROLL_PAGE then
    rollMenu(event)
  elseif page == YAW_PAGE then
    yawMenu(event)
  elseif page == PITCH_PAGE then
    pitchMenu(event)
  elseif page == CONFIRMATION_PAGE then
    return confirmationMenu(event)
  end
  return 0
end

return { init=init, run=run }
