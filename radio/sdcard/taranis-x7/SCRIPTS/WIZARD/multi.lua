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
local ARM_PAGE = 4
local MODE_PAGE = 5
local BEEPER_PAGE = 6
local CONFIRMATION_PAGE = 7

-- Navigation variables
local page = THROTTLE_PAGE
local dirty = true
local edit = false
local field = 0
local fieldsMax = 0
local comboBoxMode = 0 -- Scrap variable
local validSwitch = {}

-- Model settings
local thrCH1 = 0
local rollCH1 = 0
local yawCH1 = 0
local pitchCH1 = 0
local armSW1 = 1
local beeperSW1 = 1
local modeSW1 = 1
local switches = {}

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
    if event == EVT_VIRTUAL_INC then
      value = (value + max)
      dirty = true
    elseif event == EVT_VIRTUAL_DEC then
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
    if event == EVT_PAGE_BREAK or event == EVT_RIGHT_FIRST then
      page = nextPage
      field = 0
      dirty = true
    elseif event == EVT_PAGE_LONG or event == EVT_LEFT_FIRST then
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

local function switchValueIncDec(event, value, min, max)
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

local function switchIncDec(event, value)
  if not edit and event== EVT_VIRTUAL_MENU then
    servoPage = value
    dirty = true
  else
    value = switchValueIncDec(event, value, 1, #switches)
  end
  return value
end

-- Init function
local function init()
  thrCH1 = defaultChannel(2)
  rollCH1 = defaultChannel(0)
  yawCH1 = defaultChannel(3)
  pitchCH1 = defaultChannel(1)
  local ver, radio, maj, minor, rev = getVersion()
  if string.match(radio, "x7") then
    switches = {"SA", "SB", "SC", "SD", "SF", "SH"}
  else
    switches = {"SA", "SB", "SC", "SD"}
  end
end

-- Throttle Menu
local function drawThrottleMenu()
  lcd.clear()
  lcd.drawText(1, 0, "Multicopter", 0)
  lcd.drawFilledRectangle(0, 0, LCD_W, 8, FILL_WHITE)
  lcd.drawCombobox(0, 8, LCD_W, {"Throttle"}, comboBoxMode, getFieldFlags(1))
  lcd.drawText(5, 30, "Assign channel", 0);
  lcd.drawText(5, 40, ">>>", 0);
  lcd.drawSource(25, 40, MIXSRC_CH1+thrCH1, getFieldFlags(0))
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
  lcd.drawText(1, 0, "Multicopter", 0)
  lcd.drawFilledRectangle(0, 0, LCD_W, 8, FILL_WHITE)
  lcd.drawCombobox(0, 8, LCD_W, {"Roll"}, comboBoxMode, getFieldFlags(1))
  lcd.drawText(5, 30, "Assign channel", 0);
  lcd.drawText(5, 40, ">>>", 0);
  lcd.drawSource(25, 40, MIXSRC_CH1+rollCH1, getFieldFlags(0))
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
  lcd.drawText(1, 0, "Multicopter", 0)
  lcd.drawFilledRectangle(0, 0, LCD_W, 8, FILL_WHITE)
  lcd.drawCombobox(0, 8, LCD_W, {"Pitch"}, comboBoxMode, getFieldFlags(1))
  lcd.drawText(5, 30, "Assign channel", 0);
  lcd.drawText(5, 40, ">>>", 0);
  lcd.drawSource(25, 40, MIXSRC_CH1+pitchCH1, getFieldFlags(0))
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
  lcd.drawText(1, 0, "Multicopter", 0)
  lcd.drawFilledRectangle(0, 0, LCD_W, 8, FILL_WHITE)
  lcd.drawCombobox(0, 8, LCD_W, {"Yaw"}, comboBoxMode, getFieldFlags(1))
  lcd.drawText(5, 30, "Assign channel", 0);
  lcd.drawText(5, 40, ">>>", 0);
  lcd.drawSource(25, 40, MIXSRC_CH1+yawCH1, getFieldFlags(0))
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

-- Arm Menu
local function drawArmMenu()
  lcd.clear()
  lcd.drawText(1, 0, "Multicopter", 0)
  lcd.drawFilledRectangle(0, 0, LCD_W, 8, FILL_WHITE)
  lcd.drawCombobox(0, 8, LCD_W, {"Arm"}, comboBoxMode, getFieldFlags(1))
  lcd.drawText(5, 30, "Assign AUX1", 0);
  lcd.drawText(5, 40, ">>>", 0);
  lcd.drawText(25, 40, switches[armSW1], getFieldFlags(0))
  fieldsMax = 0
end

local function armMenu(event)
  if dirty then
    dirty = false
    drawArmMenu()
  end
  navigate(event, fieldsMax, page-1, page+1)
  armSW1 = switchIncDec(event, armSW1)
end

-- Beeper Menu
local function drawbeeperMenu()
  lcd.clear()
  lcd.drawText(1, 0, "Multicopter", 0)
  lcd.drawFilledRectangle(0, 0, LCD_W, 8, FILL_WHITE)
  lcd.drawCombobox(0, 8, LCD_W, {"Beeper"}, comboBoxMode, getFieldFlags(1))
  lcd.drawText(5, 30, "Assign AUX2", 0);
  lcd.drawText(5, 40, ">>>", 0);
  lcd.drawText(25, 40, switches[beeperSW1], getFieldFlags(0))
  fieldsMax = 0
end

local function beeperMenu(event)
  if dirty then
    dirty = false
    drawbeeperMenu()
  end
  navigate(event, fieldsMax, page-1, page+1)
  beeperSW1 = switchIncDec(event, beeperSW1)
end

-- Mode Menu
local function drawmodeMenu()
  lcd.clear()
  lcd.drawText(1, 0, "Multicopter", 0)
  lcd.drawFilledRectangle(0, 0, LCD_W, 8, FILL_WHITE)
  lcd.drawCombobox(0, 8, LCD_W, {"Mode"}, comboBoxMode, getFieldFlags(1))
  lcd.drawText(5, 30, "Assign AUX3", 0);
  lcd.drawText(5, 40, ">>>", 0);
  lcd.drawText(25, 40, switches[modeSW1], getFieldFlags(0))
  fieldsMax = 0
end

local function modeMenu(event)
  if dirty then
    dirty = false
    drawmodeMenu()
  end
  navigate(event, fieldsMax, page-1, page+1)
  modeSW1 = switchIncDec(event, modeSW1)
end

-- Confirmation Menu
local function drawNextLine(x, y, label, channel)
  lcd.drawText(x, y, label, 0);
  lcd.drawText(x+46, y, ":", 0);
  lcd.drawSource(x+50, y, MIXSRC_CH1+channel, 0)
  y = y + 8
  if y > 50 then
    y = 12
    x = 120
  end
  return x, y
end

local function drawNextSWLine(x, y, label, switch)
  lcd.drawText(x, y, label, 0);
  lcd.drawText(x+38, y, ":", 0);
  lcd.drawText(x+42, y, switches[switch], 0)
  y = y + 8
  if y > 50 then
    y = 12
    x = 120
  end
  return x, y
end

local function drawConfirmationMenu()
  local x = 1
  local y = 12
  lcd.clear()
  lcd.drawText(0, 1, "Ready to go?", 0);
  lcd.drawFilledRectangle(0, 0, LCD_W, 9, 0)
  x, y = drawNextLine(x, y, "Throttle", thrCH1)
  x, y = drawNextLine(x, y, "Roll", rollCH1)
  x, y = drawNextLine(x, y, "Pitch", pitchCH1)
  x, y = drawNextLine(x, y, "Yaw", yawCH1)
  local x = 72
  local y = 12
  x, y = drawNextSWLine(x, y, "Arm", armSW1)
  x, y = drawNextSWLine(x, y, "Mode", modeSW1)
  x, y = drawNextSWLine(x, y, "Beeper", beeperSW1)
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
  addMix(thrCH1,   MIXSRC_FIRST_INPUT+defaultChannel(2), "Throttle")
  addMix(rollCH1,  MIXSRC_FIRST_INPUT+defaultChannel(3), "Roll")
  addMix(yawCH1,   MIXSRC_FIRST_INPUT+defaultChannel(0), "Yaw")
  addMix(pitchCH1, MIXSRC_FIRST_INPUT+defaultChannel(1), "Pitch")
  addMix(4, MIXSRC_SA + armSW1 - 1, "Arm")
  addMix(5, MIXSRC_SA + beeperSW1 - 1, "Beeper")
  addMix(6, MIXSRC_SA + modeSW1 - 1, "Mode")
end

local function confirmationMenu(event)
  if dirty then
    dirty = false
    drawConfirmationMenu()
  end

  navigate(event, fieldsMax, BEEPER_PAGE, page)

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
  if page == THROTTLE_PAGE then
    throttleMenu(event)
  elseif page == ROLL_PAGE then
    rollMenu(event)
  elseif page == YAW_PAGE then
    yawMenu(event)
  elseif page == PITCH_PAGE then
    pitchMenu(event)
  elseif page == ARM_PAGE then
    armMenu(event)
  elseif page == BEEPER_PAGE then
    beeperMenu(event)
  elseif page == MODE_PAGE then
    modeMenu(event)
  elseif page == CONFIRMATION_PAGE then
    return confirmationMenu(event)
  end
  return 0
end

return { init=init, run=run }
