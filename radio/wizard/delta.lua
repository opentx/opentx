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
local engineMode = 0
local engineCH1 = 0
local elevonsCH1 = 0
local elevonsCH2 = 4
local elevonsMode = 0
local rudderMode = 1
local rudderCH1 = 0
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
    if event == EVT_PLUS_BREAK then
      value = (value + max)
      dirty = true
    elseif event == EVT_MINUS_BREAK then
      value = (value + max + 2)
      dirty = true
    end
    value = (value % (max+1))
  end
  return value
end

local function valueIncDec(event, value, min, max)
  if edit then
    if event == EVT_PLUS_FIRST or event == EVT_PLUS_REPT then
      if value < max then
        value = (value + 1)
        dirty = true
      end
    elseif event == EVT_MINUS_FIRST or event == EVT_MINUS_REPT then
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
      dirty = true
    elseif event == EVT_PAGE_LONG then
      page = prevPage
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
  for stick = 0, 3, 1 do
    local index = channelOrder(stick);
    if index == 0 then
      rudderCH1 = stick
    elseif index == 2 then
      engineCH1 = stick
    end
  end
end

-- Engine Menu
local engineModeItems = {"Yes...", "No"}
local function drawEngineMenu()
  lcd.clear()
  lcd.drawText(1, 0, "Has your model got an engine?", 0)
  lcd.drawFilledRectangle(0, 0, LCD_W, 8, GREY_DEFAULT+FILL_WHITE)
  lcd.drawCombobox(0, 8, LCD_W/2, engineModeItems, engineMode, getFieldFlags(0)) 
  lcd.drawLine(LCD_W/2-1, 18, LCD_W/2-1, LCD_H, DOTTED, 0)
  if engineMode == 1 then
    -- No engine
    lcd.drawPixmap(132, 8, "engine-0.bmp")
    fieldsMax = 0
  else
    -- 1 channel
    lcd.drawPixmap(132, 8, "engine-1.bmp")
    lcd.drawText(25, LCD_H-16, "Assign channel", 0);
    lcd.drawText(LCD_W/2-19, LCD_H-8, ">>>", 0);
    lcd.drawSource(151, LCD_H-8, SOURCE_FIRST_CH+engineCH1, getFieldFlags(1))
    fieldsMax = 1
  end
end

local function engineMenu(event)
  if dirty then
    dirty = false
    drawEngineMenu()
  end

  navigate(event, fieldsMax, MODELTYPE_PAGE, page+1)

  if field==0 then
    engineMode = fieldIncDec(event, engineMode, 1)
  elseif field==1 then
    engineCH1 = channelIncDec(event, engineCH1)
  end
end

local function applyEngineSettings()
  if engineMode == 0 then
    mix = {source=channelOrder(2)}
    model.insertMix(engineCH1, 0, mix)
  end
end

-- Elevons Menu
local elevonsModeItems = {"2 Channels..."}
local function drawElevonsMenu()
  lcd.clear()
  lcd.drawText(1, 0, "Select elevon channnels", 0)
  lcd.drawFilledRectangle(0, 0, LCD_W, 8, GREY_DEFAULT+FILL_WHITE)
  lcd.drawCombobox(0, 8, LCD_W/2, elevonsModeItems, elevonsMode, getFieldFlags(0)) 
  lcd.drawLine(LCD_W/2-1, 18, LCD_W/2-1, LCD_H, DOTTED, 0)  
  lcd.drawPixmap(110, 9, "elevons.bmp")
  lcd.drawText(20, LCD_H-16, "Assign channels", 0);
  lcd.drawText(LCD_W/2-19, LCD_H-8, ">>>", 0);
  lcd.drawSource(116, LCD_H-8, SOURCE_FIRST_CH+elevonsCH1, getFieldFlags(1))
  lcd.drawSource(175, LCD_H-8, SOURCE_FIRST_CH+elevonsCH2, getFieldFlags(2))
  fieldsMax = 2
end

local function elevonsMenu(event)
  if dirty then
    dirty = false
    drawElevonsMenu()
  end

  navigate(event, fieldsMax, page-1, page+1)

  if field==1 then
    elevonsCH1 = channelIncDec(event, elevonsCH1)
  elseif field==2 then
    elevonsCH2 = channelIncDec(event, elevonsCH2)
  end
end

-- Rudder menu
local rudderModeItems = {"Yes...", "No"}

local function drawRudderMenu()
  lcd.clear()
  lcd.drawText(1, 0, "Has your model got a rudder?", 0)
  lcd.drawFilledRectangle(0, 0, LCD_W, 8, GREY_DEFAULT+FILL_WHITE)
  lcd.drawCombobox(0, 8, LCD_W/2, rudderModeItems, rudderMode, getFieldFlags(0)) 
  lcd.drawLine(LCD_W/2-1, 18, LCD_W/2-1, LCD_H, DOTTED, 0)
  if rudderMode == 1 then
    -- No rudder
    lcd.drawPixmap(109, 14, "drudder-0.bmp")
    fieldsMax = 0
  else
    -- 1 channel
    lcd.drawPixmap(109, 14, "drudder-1.bmp")
    lcd.drawText(25, LCD_H-16, "Assign channel", 0);
    lcd.drawText(LCD_W/2-19, LCD_H-8, ">>>", 0);
    lcd.drawSource(190, LCD_H-55, SOURCE_FIRST_CH+rudderCH1, getFieldFlags(1))
    fieldsMax = 1
  end
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
    rudderCH1 = channelIncDec(event, rudderCH1)
  end
end

-- Servo (limits) Menu
local function drawServoMenu(limits)
  lcd.clear()
  lcd.drawSource(1, 0, SOURCE_FIRST_CH+servoPage, 0)
  lcd.drawText(25, 0, "servo min/max/center/direction?", 0)
  lcd.drawFilledRectangle(0, 0, LCD_W, 8, GREY_DEFAULT+FILL_WHITE)
  lcd.drawLine(LCD_W/2-1, 8, LCD_W/2-1, LCD_H, DOTTED, 0)
  lcd.drawText(LCD_W/2-19, LCD_H-8, ">>>", 0);
  lcd.drawPixmap(122, 8, "servo.bmp")
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
local function applySettings()
  model.defaultInputs()
  model.deleteMixes()      
  applyEngineSettings()
end

local function nextLine(x, y)
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
  if engineMode == 0 then
    lcd.drawText(x, y, "Throttle:", 0);
    lcd.drawSource(x+52, y, SOURCE_FIRST_CH+engineCH1, 0)
    x, y = nextLine(x, y)
  end
  lcd.drawText(x, y, "Elevons:", 0)
  lcd.drawSource(x+52, y, SOURCE_FIRST_CH+elevonsCH1, 0)
  x, y = nextLine(x, y)
  lcd.drawText(x, y, "Elevons:", 0)
  lcd.drawSource(x+52, y, SOURCE_FIRST_CH+elevonsCH2, 0)
  x, y = nextLine(x, y)
  if rudderMode == 0 then
    lcd.drawText(x, y, "Rudder:", 0)
    lcd.drawSource(x+52, y, SOURCE_FIRST_CH+rudderCH1, 0)      
  end
  lcd.drawText(48, LCD_H-8, "[Enter Long] to confirm", 0);
  lcd.drawFilledRectangle(0, LCD_H-9, LCD_W, 9, 0)
  lcd.drawPixmap(LCD_W-18, 0, "confirm-tick.bmp")
  lcd.drawPixmap(0, LCD_H-17, "confirm-plane.bmp")
  fieldsMax = 0
end

local function confirmationMenu(event)
  if dirty then
    dirty = false
    drawConfirmationMenu()
  end

  navigate(event, fieldsMax, RUDDER_PAGE, page)
  
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
  lcd.lock()
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
