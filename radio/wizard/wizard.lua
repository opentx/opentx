-- Wizard pages
local MODELTYPE_MENU = 0
local PLANE_MENU    = MODELTYPE_MENU+1
local HELI_MENU     = PLANE_MENU+10
local DELTA_MENU    = HELI_MENU+10
local QUADRI_MENU   = DELTA_MENU+10
local CONFIRMATION_MENU = 100

local ENGINE_MENU = PLANE_MENU
local AILERONS_MENU = PLANE_MENU+1
local FLAPERONS_MENU = PLANE_MENU+2
local BRAKES_MENU = PLANE_MENU+3
local TAIL_MENU = PLANE_MENU+4

local ELEVONS_MENU = DELTA_MENU+1
local DRUDDER_MENU = DELTA_MENU+2

-- Navigation variables
local page = MODELTYPE_MENU
local lastPage = TAIL_MENU
local dirty = true
local edit = false
local field = 0
local fieldsMax = 0

-- Model settings
local modelType = 0
local engineMode = 0
local engineCH1 = 0
local aileronsMode = 0
local aileronsCH1 = 0
local aileronsCH2 = 4
local flapsMode = 0
local flapsCH1 = 5
local flapsCH2 = 6
local brakesMode = 0
local brakesCH1 = 8
local brakesCH2 = 9
local tailMode = 0
local eleCH1 = 0
local eleCH2 = 7
local rudCH1 = 0
local elevonsCH1 = 0
local elevonsCH2 = 4
local dRudderMode = 0
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
      rudCH1 = stick
    elseif index == 1 then
      eleCH1 = stick
    elseif index == 2 then
      engineCH1 = stick
    else
      aileronsCH1 = stick
    end
  end
end

-- Model Type Menu

local function modelTypeSurround(index)
  lcd.drawRectangle(12+47*index, 13, 48, 48)
  lcd.drawPixmap(17+47*index, 8, "/TEMPLATES/mark.bmp")
end

local function drawModelChoiceMenu()
  lcd.clear()
  lcd.drawScreenTitle("", 0, 0)
  -- lcd.drawText(58, 13, "Select model type", 0)
  lcd.drawPixmap( 16, 17, "/TEMPLATES/plane.bmp")
  lcd.drawPixmap( 63, 17, "/TEMPLATES/heli.bmp")
  lcd.drawPixmap(110, 17, "/TEMPLATES/delta.bmp")
  lcd.drawPixmap(157, 17, "/TEMPLATES/quadri.bmp")
  modelTypeSurround(modelType)
end

local function modelTypeMenu(event)
  if dirty == true then
    drawModelChoiceMenu()
    dirty = false
  end
  if event == EVT_ENTER_BREAK then
    page = PLANE_MENU+(10*modelType)
    dirty = true
  else
    modelType = fieldIncDec(event, modelType, 3, true)
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
    lcd.drawPixmap(132, 8, "/TEMPLATES/engine-0.bmp")
    fieldsMax = 0
  else
    -- 1 channel
    lcd.drawPixmap(132, 8, "/TEMPLATES/engine-1.bmp")
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

  navigate(event, fieldsMax, MODELTYPE_MENU, page+1)

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

-- Ailerons Menu

local aileronsModeItems = {"Yes...", "No", "Yes, 2 channels..."}

local function drawAileronsMenu()
  lcd.clear()
  lcd.drawText(1, 0, "Has your model got ailerons?", 0)
  lcd.drawFilledRectangle(0, 0, LCD_W, 8, GREY_DEFAULT+FILL_WHITE)
  lcd.drawCombobox(0, 8, LCD_W/2, aileronsModeItems, aileronsMode, getFieldFlags(0)) 
  lcd.drawLine(LCD_W/2-1, 18, LCD_W/2-1, LCD_H, DOTTED, 0)
  if aileronsMode == 2 then
    -- 2 channels
    lcd.drawPixmap(112, 8, "/TEMPLATES/ailerons-2.bmp")
    lcd.drawText(20, LCD_H-16, "Assign channels", 0);
    lcd.drawText(LCD_W/2-19, LCD_H-8, ">>>", 0);
    lcd.drawSource(116, LCD_H-8, SOURCE_FIRST_CH+aileronsCH1, getFieldFlags(1))
    lcd.drawSource(175, LCD_H-8, SOURCE_FIRST_CH+aileronsCH2, getFieldFlags(2))
    fieldsMax = 2
  elseif aileronsMode == 1 then
    -- No ailerons
    lcd.drawPixmap(112, 8, "/TEMPLATES/ailerons-0.bmp")
    fieldsMax = 0
  else
    -- 1 channel
    lcd.drawPixmap(112, 8, "/TEMPLATES/ailerons-1.bmp")
    lcd.drawText(25, LCD_H-16, "Assign channel", 0);
    lcd.drawText(LCD_W/2-19, LCD_H-8, ">>>", 0);
    lcd.drawSource(151, LCD_H-8, SOURCE_FIRST_CH+aileronsCH1, getFieldFlags(1))
    fieldsMax = 1
  end
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
    aileronsCH1 = channelIncDec(event, aileronsCH1)
  elseif field==2 then
    aileronsCH2 = channelIncDec(event, aileronsCH2)
  end
end

-- Flaps Menu

local flapsModeItems = {"No", "Yes...", "Yes, 2 channels..."}

local function drawFlapsMenu()
  lcd.clear()
  lcd.drawText(1, 0, "Has your model got flaps?", 0)
  lcd.drawFilledRectangle(0, 0, LCD_W, 8, GREY_DEFAULT+FILL_WHITE)
  lcd.drawCombobox(0, 8, LCD_W/2, flapsModeItems, flapsMode, getFieldFlags(0)) 
  lcd.drawLine(LCD_W/2-1, 18, LCD_W/2-1, LCD_H, DOTTED, 0)
  if flapsMode == 0 then
    -- no flaps
    lcd.drawPixmap(112, 8, "/TEMPLATES/ailerons-0.bmp")
    fieldsMax = 0
  elseif flapsMode == 1 then
    -- 1 channel
    lcd.drawPixmap(112, 8, "/TEMPLATES/flaps-1.bmp")
    lcd.drawText(25, LCD_H-16, "Assign channel", 0);
    lcd.drawText(LCD_W/2-19, LCD_H-8, ">>>", 0);
    lcd.drawSource(151, LCD_H-8, SOURCE_FIRST_CH+flapsCH1, getFieldFlags(1))
    fieldsMax = 1
  elseif flapsMode == 2 then
    -- 2 channels
    lcd.drawPixmap(112, 8, "/TEMPLATES/flaps-2.bmp")
    lcd.drawText(20, LCD_H-16, "Assign channels", 0);
    lcd.drawText(LCD_W/2-19, LCD_H-8, ">>>", 0);
    lcd.drawSource(116, LCD_H-8, SOURCE_FIRST_CH+flapsCH1, getFieldFlags(1))
    lcd.drawSource(175, LCD_H-8, SOURCE_FIRST_CH+flapsCH2, getFieldFlags(2))
    fieldsMax = 2
  end
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

local brakesModeItems = {"No", "Yes...", "Yes, 2 channels..."}

local function drawBrakesMenu()
  lcd.clear()
  lcd.drawText(1, 0, "Has your model got air brakes?", 0)
  lcd.drawFilledRectangle(0, 0, LCD_W, 8, GREY_DEFAULT+FILL_WHITE)
  lcd.drawCombobox(0, 8, LCD_W/2, brakesModeItems, brakesMode, getFieldFlags(0)) 
  lcd.drawLine(LCD_W/2-1, 18, LCD_W/2-1, LCD_H, DOTTED, 0)
  if brakesMode == 0 then
    -- no brakes
    lcd.drawPixmap(112, 8, "/TEMPLATES/ailerons-0.bmp")
    fieldsMax = 0
  elseif brakesMode == 1 then
    -- 1 channel
    lcd.drawPixmap(112, 8, "/TEMPLATES/brakes-1.bmp")
    lcd.drawText(25, LCD_H-16, "Assign channel", 0);
    lcd.drawText(LCD_W/2-19, LCD_H-8, ">>>", 0);
    lcd.drawSource(151, LCD_H-8, SOURCE_FIRST_CH+brakesCH1, getFieldFlags(1))
    fieldsMax = 1
  elseif brakesMode == 2 then
    -- 2 channels
    lcd.drawPixmap(112, 8, "/TEMPLATES/brakes-2.bmp")
    lcd.drawText(20, LCD_H-16, "Assign channels", 0);
    lcd.drawText(LCD_W/2-19, LCD_H-8, ">>>", 0);
    lcd.drawSource(116, LCD_H-8, SOURCE_FIRST_CH+brakesCH1, getFieldFlags(1))
    lcd.drawSource(175, LCD_H-8, SOURCE_FIRST_CH+brakesCH2, getFieldFlags(2))
    fieldsMax = 2
  end
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

local tailModeItems = {"Ele(1ch), no Rud...", "Ele(1ch) + Rud...", "Ele(2ch) + Rud...", "V-Tail..."}

local function drawTailMenu()
  lcd.clear()
  lcd.drawText(1, 0, "Which is the tail config on your model?", 0)
  lcd.drawFilledRectangle(0, 0, LCD_W, 8, GREY_DEFAULT+FILL_WHITE)
  lcd.drawCombobox(0, 8, LCD_W/2, tailModeItems, tailMode, getFieldFlags(0)) 
  lcd.drawLine(LCD_W/2-1, 18, LCD_W/2-1, LCD_H, DOTTED, 0)
  lcd.drawText(LCD_W/2-19, LCD_H-8, ">>>", 0);
  if tailMode == 0 then
    -- Elevator(1ch), no rudder...
    lcd.drawPixmap(112, 8, "/TEMPLATES/tail-e.bmp")
    lcd.drawText(25, LCD_H-16, "Assign channel", 0);
    lcd.drawSource(175, 30, SOURCE_FIRST_CH+eleCH1, getFieldFlags(1))
    fieldsMax = 1
  elseif tailMode == 1 then
    -- Elevator(1ch) + rudder...
    lcd.drawPixmap(112, 8, "/TEMPLATES/tail-er.bmp")
    lcd.drawText(20, LCD_H-16, "Assign channels", 0);
    lcd.drawSource(175, 30, SOURCE_FIRST_CH+eleCH1, getFieldFlags(1))
    lcd.drawSource(175, 10, SOURCE_FIRST_CH+rudCH1, getFieldFlags(2))
    fieldsMax = 2
  elseif tailMode == 2 then
    -- Elevator(2ch) + rudder...
    lcd.drawPixmap(112, 8, "/TEMPLATES/tail-eer.bmp")
    lcd.drawText(20, LCD_H-16, "Assign channels", 0);
    lcd.drawSource(175, 30, SOURCE_FIRST_CH+eleCH1, getFieldFlags(1))
    lcd.drawSource(175, 20, SOURCE_FIRST_CH+eleCH2, getFieldFlags(2))
    lcd.drawSource(175, 10, SOURCE_FIRST_CH+rudCH1, getFieldFlags(3))
    fieldsMax = 3
  else
    -- V-Tail...
    lcd.drawPixmap(112, 8, "/TEMPLATES/tail-v.bmp")
    lcd.drawText(20, LCD_H-16, "Assign channels", 0);
    lcd.drawSource(175, 20, SOURCE_FIRST_CH+eleCH1, getFieldFlags(1))
    lcd.drawSource(175, 10, SOURCE_FIRST_CH+eleCH2, getFieldFlags(2))
    fieldsMax = 2
  end
end

local function tailMenu(event)
  if dirty then
    dirty = false
    drawTailMenu()
  end

  lastPage = TAIL_MENU
  navigate(event, fieldsMax, page-1, CONFIRMATION_MENU)

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

-- Delta Engine Menu

local function dEngineMenu(event)
  if dirty then
    dirty = false
    drawEngineMenu()
  end

  navigate(event, fieldsMax, MODELTYPE_MENU, page+1)

  if field==0 then
    engineMode = fieldIncDec(event, engineMode, 1)
  elseif field==1 then
    engineCH1 = channelIncDec(event, engineCH1)
  end
end

-- Elevons Menu

local function drawElevonsMenu()
  lcd.clear()
  lcd.drawText(1, 0, "Select elevon channnels", 0)
  lcd.drawFilledRectangle(0, 0, LCD_W, 8, GREY_DEFAULT+FILL_WHITE)
  lcd.drawLine(LCD_W/2-1, 7, LCD_W/2-1, LCD_H, DOTTED, 0)
  lcd.drawPixmap(110, 9, "/TEMPLATES/elevons.bmp")
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

-- Delta Rudder menu

local dRudderModeItems = {"Yes...", "No"}

local function drawDRudderMenu()
  lcd.clear()
  lcd.drawText(1, 0, "Has your model got a rudder?", 0)
  lcd.drawFilledRectangle(0, 0, LCD_W, 8, GREY_DEFAULT+FILL_WHITE)
  lcd.drawCombobox(0, 8, LCD_W/2, dRudderModeItems, dRudderMode, getFieldFlags(0)) 
  lcd.drawLine(LCD_W/2-1, 18, LCD_W/2-1, LCD_H, DOTTED, 0)
  if dRudderMode == 1 then
    -- No rudder
    lcd.drawPixmap(109, 14, "/TEMPLATES/drudder-0.bmp")
    fieldsMax = 0
  else
    -- 1 channel
    lcd.drawPixmap(109, 14, "/TEMPLATES/drudder-1.bmp")
    lcd.drawText(25, LCD_H-16, "Assign channel", 0);
    lcd.drawText(LCD_W/2-19, LCD_H-8, ">>>", 0);
    lcd.drawSource(190, LCD_H-55, SOURCE_FIRST_CH+rudCH1, getFieldFlags(1))
    fieldsMax = 1
  end
end

local function dRudderMenu(event)
  if dirty then
    dirty = false
    drawDRudderMenu()
  end
  
  lastPage = DRUDDER_MENU
  navigate(event, fieldsMax, page-1, CONFIRMATION_MENU)

  if field==0 then
    dRudderMode = fieldIncDec(event, dRudderMode, 1)
  elseif field==1 then
    rudCH1 = channelIncDec(event, rudCH1)
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
  lcd.drawPixmap(122, 8, "/TEMPLATES/servo.bmp")
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
  if lastPage == DRUDDER_MENU then
    lcd.drawText(x, y, "Elevons:", 0)
    lcd.drawSource(x+52, y, SOURCE_FIRST_CH+elevonsCH1, 0)
    x, y = nextLine(x, y)
    lcd.drawText(x, y, "Elevons:", 0)
    lcd.drawSource(x+52, y, SOURCE_FIRST_CH+elevonsCH2, 0)
    x, y = nextLine(x, y)
    if dRudderMode == 0 then
      lcd.drawText(x, y, "Rudder:", 0)
      lcd.drawSource(x+52, y, SOURCE_FIRST_CH+rudCH1, 0)      
    end
  else
    if aileronsMode ~= 1 then
      lcd.drawText(x, y, "Ailerons:", 0)
      lcd.drawSource(x+52, y, SOURCE_FIRST_CH+aileronsCH1, 0)
      x, y = nextLine(x, y)
      if aileronsMode == 2 then
        lcd.drawText(x, y, "Ailerons:", 0)
        lcd.drawSource(x+52, y, SOURCE_FIRST_CH+aileronsCH2, 0)
        x, y = nextLine(x, y)
      end
    end
    if flapsMode ~= 0 then
      lcd.drawText(x, y, "Flaps:", 0)
      lcd.drawSource(x+52, y, SOURCE_FIRST_CH+flapsCH1, 0)
      x, y = nextLine(x, y)
      if flapsMode == 2 then
        lcd.drawText(x, y, "Flaps:", 0)
        lcd.drawSource(x+52, y, SOURCE_FIRST_CH+flapsCH2, 0)
        x, y = nextLine(x, y)
      end
    end
    if brakesMode == 1  then
      lcd.drawText(x, y, "Brakes:", 0)
      lcd.drawSource(x+52, y, SOURCE_FIRST_CH+brakesCH1, 0)
      x, y = nextLine(x, y)
      if brakesMode == 2 then
        lcd.drawText(x, y, "Brakes:", 0)
        lcd.drawSource(x+52, y, SOURCE_FIRST_CH+brakesCH2, 0)
        x, y = nextLine(x, y)
      end
    end
    if tailMode == 3 then
      lcd.drawText(x, y, "V-Tail:", 0)
      lcd.drawSource(x+52, y, SOURCE_FIRST_CH+eleCH1, 0)
      x, y = nextLine(x, y)
      lcd.drawText(x, y, "V-Tail:", 0)
      lcd.drawSource(x+52, y, SOURCE_FIRST_CH+eleCH2, 0)
    else
      lcd.drawText(x, y, "Elevator:", 0)
      lcd.drawSource(x+52, y, SOURCE_FIRST_CH+eleCH1, 0)
      x, y = nextLine(x, y)
      if tailMode == 1 then
        lcd.drawText(x, y, "Rudder:", 0)
        lcd.drawSource(x+52, y, SOURCE_FIRST_CH+rudCH1, 0)
      else
        lcd.drawText(x, y, "Elevator:", 0)
        lcd.drawSource(x+52, y, SOURCE_FIRST_CH+eleCH2, 0)
        x, y = nextLine(x, y)
        lcd.drawText(x, y, "Rudder:", 0)
        lcd.drawSource(x+52, y, SOURCE_FIRST_CH+rudCH1, 0)
      end        
    end
  end
  lcd.drawText(48, LCD_H-8, "[Enter Long] to confirm", 0);
  lcd.drawFilledRectangle(0, LCD_H-9, LCD_W, 9, 0)
  lcd.drawPixmap(LCD_W-18, 0, "/TEMPLATES/confirm-tick.bmp")
  lcd.drawPixmap(0, LCD_H-17, "/TEMPLATES/confirm-plane.bmp")
  fieldsMax = 0
end

local function confirmationMenu(event)
  if dirty then
    dirty = false
    drawConfirmationMenu()
  end

  navigate(event, fieldsMax, lastPage, page)

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
  if page == MODELTYPE_MENU then
    modelTypeMenu(event)
  elseif servoPage ~= nil then
    servoMenu(event) 
  elseif page == ENGINE_MENU then
    engineMenu(event)
  elseif page == AILERONS_MENU then
    aileronsMenu(event)
  elseif page == FLAPERONS_MENU then
    flapsMenu(event)
  elseif page == BRAKES_MENU then
    brakesMenu(event)
  elseif page == TAIL_MENU then
    tailMenu(event)
  elseif page == DELTA_MENU then
    dEngineMenu(event)
  elseif page == ELEVONS_MENU then
    elevonsMenu(event)
  elseif page == DRUDDER_MENU then
    dRudderMenu(event)
  elseif page == CONFIRMATION_MENU then
    return confirmationMenu(event)
  end
  return 0
end

return { init=init, run=run }
