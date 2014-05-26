
-- Wizard pages
local MODELTYPE_MENU = 0
local PLANE_MENU    = MODELTYPE_MENU+1
local HELI_MENU     = PLANE_MENU+10
local DELTA_MENU    = HELI_MENU+10
local QUADRI_MENU   = DELTA_MENU+10

local ENGINE_MENU = PLANE_MENU
local AILERONS_MENU = PLANE_MENU+1
local FLAPERONS_MENU = PLANE_MENU+2
local BRAKES_MENU = PLANE_MENU+3

local page = MODELTYPE_MENU
local dirty = true
local choice = 0

local edit = false
local field = 0
local fieldsMax = 0
local engineMode = 0
local engineCH1 = 0
local aileronsMode = 0
local aileronsCH1 = 0
local aileronsCH2 = 4
local flaperonsMode = 0
local flaperonsCH1 = 5
local flaperonsCH2 = 6
local brakesMode = 0
local brakesCH1 = 7
local brakesCH2 = 8

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

local function keyIncDec(event, value, max, isvalue)
  if isvalue then
    if event == EVT_PLUS_BREAK then
      if value < max then
        value = (value + 1)
        dirty = true
      end
    elseif event == EVT_MINUS_BREAK then
      if value > 0 then
        value = (value - 1)
        dirty = true
      end
    end
  else
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

local function navigate(event, fieldMax, prevPage, nextPage)
  if event == EVT_ENTER_BREAK then
    edit = not edit
    dirty = true
  end
  if edit then
    if not dirty then
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
      field = keyIncDec(event, field, fieldMax)
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

-- Init function

local function init()
  for stick = 0, 4, 1 do
    local index = channelOrder(stick);
    if index == 0 then
    elseif index == 1 then
    elseif index == 2 then
      engineCH1 = stick
    else
      aileronsCH1 = stick
    end
  end
end

-- Model Type Menu

local function choiceSurround(index)
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
  choiceSurround(choice)
end

local function modelTypeMenu(event)
  if dirty == true then
    drawModelChoiceMenu()
    dirty = false
  end
  if event == EVT_ENTER_BREAK then
    page = PLANE_MENU+(10*choice)
    dirty = true
  else
    choice = keyIncDec(event, choice, 3)
  end
end

-- Engine Menu

local engineModeItems = {"Yes...", "No"}

local function drawEngineMenu()
  lcd.clear()
  lcd.drawText(1, 0, "Has your model got an engine?", 0)
  lcd.drawRect(0, 0, LCD_W, 8, GREY_DEFAULT+FILL_WHITE)
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

  if edit then
    if field==0 then
      engineMode = keyIncDec(event, engineMode, 1)
    elseif field==1 then
      engineCH1 = keyIncDec(event, engineCH1, 7, true)
    end
  end    
end

-- Ailerons Menu

local aileronsModeItems = {"Yes...", "No", "Yes, 2 channels..."}

local function drawAileronsMenu()
  lcd.clear()
  lcd.drawText(1, 0, "Has your model got ailerons?", 0)
  lcd.drawRect(0, 0, LCD_W, 8, GREY_DEFAULT+FILL_WHITE)
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

  if edit then
    if field==0 then
      aileronsMode = keyIncDec(event, aileronsMode, 2)
    elseif field==1 then
      aileronsCH1 = keyIncDec(event, aileronsCH1, 7, true)
    elseif field==2 then
      aileronsCH2 = keyIncDec(event, aileronsCH2, 7, true)
    end
  end    
end

-- Flaperons Menu

local flaperonsModeItems = {"No", "Yes...", "Yes, 2 channels..."}

local function drawFlaperonsMenu()
  lcd.clear()
  lcd.drawText(1, 0, "Has your model got flaperons?", 0)
  lcd.drawRect(0, 0, LCD_W, 8, GREY_DEFAULT+FILL_WHITE)
  lcd.drawCombobox(0, 8, LCD_W/2, flaperonsModeItems, flaperonsMode, getFieldFlags(0)) 
  lcd.drawLine(LCD_W/2-1, 18, LCD_W/2-1, LCD_H, DOTTED, 0)
  if flaperonsMode == 0 then
    -- no flaperons
    -- lcd.drawPixmap(112, 8, "/TEMPLATES/ailerons-0.bmp")
    fieldsMax = 0
  elseif flaperonsMode == 1 then
    -- 1 channel
    lcd.drawPixmap(112, 8, "/TEMPLATES/flaps-1.bmp")
    lcd.drawText(25, LCD_H-16, "Assign channel", 0);
    lcd.drawText(LCD_W/2-19, LCD_H-8, ">>>", 0);
    lcd.drawSource(151, LCD_H-8, SOURCE_FIRST_CH+flaperonsCH1, getFieldFlags(1))
    fieldsMax = 1
  elseif flaperonsMode == 2 then
    -- 2 channels
    lcd.drawPixmap(112, 8, "/TEMPLATES/flaps-2.bmp")
    lcd.drawText(20, LCD_H-16, "Assign channels", 0);
    lcd.drawText(LCD_W/2-19, LCD_H-8, ">>>", 0);
    lcd.drawSource(116, LCD_H-8, SOURCE_FIRST_CH+flaperonsCH1, getFieldFlags(1))
    lcd.drawSource(175, LCD_H-8, SOURCE_FIRST_CH+flaperonsCH2, getFieldFlags(2))
    fieldsMax = 2
  end
end

local function flaperonsMenu(event)
  if dirty then
    dirty = false
    drawFlaperonsMenu()
  end

  navigate(event, fieldsMax, page-1, page+1)

  if edit then
    if field==0 then
      flaperonsMode = keyIncDec(event, flaperonsMode, 2)
    elseif field==1 then
      flaperonsCH1 = keyIncDec(event, flaperonsCH1, 7, true)
    elseif field==2 then
      flaperonsCH2 = keyIncDec(event, flaperonsCH2, 7, true)
    end
  end    
end

-- Airbrakes Menu

local brakesModeItems = {"No", "Yes...", "Yes, 2 channels..."}

local function drawBrakesMenu()
  lcd.clear()
  lcd.drawText(1, 0, "Has your model got air brakes?", 0)
  lcd.drawRect(0, 0, LCD_W, 8, GREY_DEFAULT+FILL_WHITE)
  lcd.drawCombobox(0, 8, LCD_W/2, brakesModeItems, brakesMode, getFieldFlags(0)) 
  lcd.drawLine(LCD_W/2-1, 18, LCD_W/2-1, LCD_H, DOTTED, 0)
  if brakesMode == 0 then
    -- no brakes
    -- lcd.drawPixmap(112, 8, "/TEMPLATES/ailerons-0.bmp")
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

  if edit then
    if field==0 then
      brakesMode = keyIncDec(event, brakesMode, 2)
    elseif field==1 then
      brakesCH1 = keyIncDec(event, brakesCH1, 7, true)
    elseif field==2 then
      brakesCH2 = keyIncDec(event, brakesCH2, 7, true)
    end
  end    
end

-- Main

local function run(event)
  if event == EVT_EXIT_BREAK then
    return 2
  end
  lcd.lock()
  if page == MODELTYPE_MENU then
    modelTypeMenu(event)
  elseif page == ENGINE_MENU then
    engineMenu(event)
  elseif page == AILERONS_MENU then
    aileronsMenu(event)
  elseif page == FLAPERONS_MENU then
    flaperonsMenu(event)
  elseif page == BRAKES_MENU then
    brakesMenu(event)
  end
  return 0
end

return { init=init, run=run }
