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
local ailCH2 = 4
local flapsMode = 0
local flapsCH1 = 5
local flapsCH2 = 6
local brakesMode = 0
local brakesCH1 = 8
local brakesCH2 = 9
local tailMode = 1
local eleCH1 = 0
local eleCH2 = 7
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
  rudCH1 = defaultChannel(0)
  eleCH1 = defaultChannel(1)
  thrCH1 = defaultChannel(2)
  ailCH1 = defaultChannel(3)
end

-- Engine Menu
local engineModeItems = {"No", "Yes..."}
local function drawEngineMenu()
  lcd.clear()
  lcd.drawText(1, 0, "Has your model got an engine?", 0)
  lcd.drawFilledRectangle(0, 0, LCD_W, 8, GREY_DEFAULT+FILL_WHITE)
  lcd.drawCombobox(0, 8, LCD_W/2, engineModeItems, engineMode, getFieldFlags(0))
  lcd.drawLine(LCD_W/2-1, 18, LCD_W/2-1, LCD_H-1, DOTTED, 0)
  if engineMode == 1 then
    -- 1 channel
    lcd.drawPixmap(132, 8, "engine-1.bmp")
    lcd.drawText(25, LCD_H-16, "Assign channel", 0);
    lcd.drawText(LCD_W/2-19, LCD_H-8, ">>>", 0);
    lcd.drawSource(151, LCD_H-8, MIXSRC_CH1+thrCH1, getFieldFlags(1))
    fieldsMax = 1
  else
    -- No engine
    lcd.drawPixmap(132, 8, "engine-0.bmp")
    fieldsMax = 0
  end
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
local aileronsModeItems = {"No", "Yes...", "Yes, 2 channels..."}
local function drawAileronsMenu()
  lcd.clear()
  lcd.drawText(1, 0, "Has your model got ailerons?", 0)
  lcd.drawFilledRectangle(0, 0, LCD_W, 8, GREY_DEFAULT+FILL_WHITE)
  lcd.drawCombobox(0, 8, LCD_W/2, aileronsModeItems, aileronsMode, getFieldFlags(0))
  lcd.drawLine(LCD_W/2-1, 18, LCD_W/2-1, LCD_H-1, DOTTED, 0)
  if aileronsMode == 2 then
    -- 2 channels
    lcd.drawPixmap(112, 8, "ailerons-2.bmp")
    lcd.drawText(20, LCD_H-16, "Assign channels", 0);
    lcd.drawText(LCD_W/2-19, LCD_H-8, ">>>", 0);
    lcd.drawSource(116, LCD_H-8, MIXSRC_CH1+ailCH1, getFieldFlags(1))
    lcd.drawSource(175, LCD_H-8, MIXSRC_CH1+ailCH2, getFieldFlags(2))
    fieldsMax = 2
  elseif aileronsMode == 1 then
    -- 1 channel
    lcd.drawPixmap(112, 8, "ailerons-1.bmp")
    lcd.drawText(25, LCD_H-16, "Assign channel", 0);
    lcd.drawText(LCD_W/2-19, LCD_H-8, ">>>", 0);
    lcd.drawSource(151, LCD_H-8, MIXSRC_CH1+ailCH1, getFieldFlags(1))
    fieldsMax = 1
  else
    -- No ailerons
    lcd.drawPixmap(112, 8, "ailerons-0.bmp")
    fieldsMax = 0
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
    ailCH1 = channelIncDec(event, ailCH1)
  elseif field==2 then
    ailCH2 = channelIncDec(event, ailCH2)
  end
end

-- Flaps Menu
local flapsModeItems = {"No", "Yes...", "Yes, 2 channels..."}
local function drawFlapsMenu()
  lcd.clear()
  lcd.drawText(1, 0, "Has your model got flaps?", 0)
  lcd.drawFilledRectangle(0, 0, LCD_W, 8, GREY_DEFAULT+FILL_WHITE)
  lcd.drawCombobox(0, 8, LCD_W/2, flapsModeItems, flapsMode, getFieldFlags(0))
  lcd.drawLine(LCD_W/2-1, 18, LCD_W/2-1, LCD_H-1, DOTTED, 0)
  if flapsMode == 0 then
    -- no flaps
    lcd.drawPixmap(112, 8, "ailerons-0.bmp")
    fieldsMax = 0
  elseif flapsMode == 1 then
    -- 1 channel
    lcd.drawPixmap(112, 8, "flaps-1.bmp")
    lcd.drawText(25, LCD_H-16, "Assign channel", 0);
    lcd.drawText(LCD_W/2-19, LCD_H-8, ">>>", 0);
    lcd.drawSource(151, LCD_H-8, MIXSRC_CH1+flapsCH1, getFieldFlags(1))
    fieldsMax = 1
  elseif flapsMode == 2 then
    -- 2 channels
    lcd.drawPixmap(112, 8, "flaps-2.bmp")
    lcd.drawText(20, LCD_H-16, "Assign channels", 0);
    lcd.drawText(LCD_W/2-19, LCD_H-8, ">>>", 0);
    lcd.drawSource(116, LCD_H-8, MIXSRC_CH1+flapsCH1, getFieldFlags(1))
    lcd.drawSource(175, LCD_H-8, MIXSRC_CH1+flapsCH2, getFieldFlags(2))
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
  lcd.drawLine(LCD_W/2-1, 18, LCD_W/2-1, LCD_H-1, DOTTED, 0)
  if brakesMode == 0 then
    -- no brakes
    lcd.drawPixmap(112, 8, "ailerons-0.bmp")
    fieldsMax = 0
  elseif brakesMode == 1 then
    -- 1 channel
    lcd.drawPixmap(112, 8, "brakes-1.bmp")
    lcd.drawText(25, LCD_H-16, "Assign channel", 0);
    lcd.drawText(LCD_W/2-19, LCD_H-8, ">>>", 0);
    lcd.drawSource(151, LCD_H-8, MIXSRC_CH1+brakesCH1, getFieldFlags(1))
    fieldsMax = 1
  elseif brakesMode == 2 then
    -- 2 channels
    lcd.drawPixmap(112, 8, "brakes-2.bmp")
    lcd.drawText(20, LCD_H-16, "Assign channels", 0);
    lcd.drawText(LCD_W/2-19, LCD_H-8, ">>>", 0);
    lcd.drawSource(116, LCD_H-8, MIXSRC_CH1+brakesCH1, getFieldFlags(1))
    lcd.drawSource(175, LCD_H-8, MIXSRC_CH1+brakesCH2, getFieldFlags(2))
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
  lcd.drawLine(LCD_W/2-1, 18, LCD_W/2-1, LCD_H-1, DOTTED, 0)
  lcd.drawText(LCD_W/2-19, LCD_H-8, ">>>", 0);
  if tailMode == 0 then
    -- Elevator(1ch), no rudder...
    lcd.drawPixmap(112, 8, "tail-e.bmp")
    lcd.drawText(25, LCD_H-16, "Assign channel", 0);
    lcd.drawSource(175, 30, MIXSRC_CH1+eleCH1, getFieldFlags(1))
    fieldsMax = 1
  elseif tailMode == 1 then
    -- Elevator(1ch) + rudder...
    lcd.drawPixmap(112, 8, "tail-er.bmp")
    lcd.drawText(20, LCD_H-16, "Assign channels", 0);
    lcd.drawSource(175, 30, MIXSRC_CH1+eleCH1, getFieldFlags(1))
    lcd.drawSource(175, 10, MIXSRC_CH1+rudCH1, getFieldFlags(2))
    fieldsMax = 2
  elseif tailMode == 2 then
    -- Elevator(2ch) + rudder...
    lcd.drawPixmap(112, 8, "tail-eer.bmp")
    lcd.drawText(20, LCD_H-16, "Assign channels", 0);
    lcd.drawSource(175, 30, MIXSRC_CH1+eleCH1, getFieldFlags(1))
    lcd.drawSource(175, 20, MIXSRC_CH1+eleCH2, getFieldFlags(2))
    lcd.drawSource(175, 10, MIXSRC_CH1+rudCH1, getFieldFlags(3))
    fieldsMax = 3
  else
    -- V-Tail...
    lcd.drawPixmap(112, 8, "tail-v.bmp")
    lcd.drawText(20, LCD_H-16, "Assign channels", 0);
    lcd.drawSource(175, 20, MIXSRC_CH1+eleCH1, getFieldFlags(1))
    lcd.drawSource(175, 10, MIXSRC_CH1+eleCH2, getFieldFlags(2))
    fieldsMax = 2
  end
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
  lcd.drawFilledRectangle(0, 0, LCD_W, 8, GREY_DEFAULT+FILL_WHITE)
  lcd.drawLine(LCD_W/2-1, 8, LCD_W/2-1, LCD_H-1, DOTTED, 0)
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
local function drawNextLine(x, y, label, channel)
  lcd.drawText(x, y, label, 0);
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
    x, y = drawNextLine(x, y, "Throttle:", thrCH1)
  end
  if aileronsMode > 0 then
    x, y = drawNextLine(x, y, "Ailerons:", ailCH1)
    if aileronsMode == 2 then
      x, y = drawNextLine(x, y, "Ailerons:", ailCH2)
    end
  end
  if flapsMode > 0 then
    x, y = drawNextLine(x, y, "Flaps:", flapsCH1)
    if flapsMode == 2 then
      x, y = drawNextLine(x, y, "Flaps:", flapsCH2)
    end
  end
  if brakesMode > 0 then
    x, y = drawNextLine(x, y, "Brakes:", brakesCH1)
    if brakesMode == 2 then
      x, y = drawNextLine(x, y, "Brakes:", brakesCH2)
    end
  end
  if tailMode == 3 then
    x, y = drawNextLine(x, y, "V-Tail:", eleCH1)
    x, y = drawNextLine(x, y, "V-Tail:", eleCH2)
  else
    x, y = drawNextLine(x, y, "Elevator:", eleCH1)
    if tailMode == 2 then
      x, y = drawNextLine(x, y, "Elevator:", eleCH2)
    end
    drawNextLine(x, y, "Rudder:", rudCH1)
  end
  lcd.drawText(48, LCD_H-8, "[Enter Long] to confirm", 0);
  lcd.drawFilledRectangle(0, LCD_H-9, LCD_W, 9, 0)
  lcd.drawPixmap(LCD_W-18, 0, "confirm-tick.bmp")
  lcd.drawPixmap(0, LCD_H-17, "confirm-plane.bmp")
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
  if aileronsMode > 0 then
    addMix(ailCH1, MIXSRC_FIRST_INPUT+defaultChannel(3), "Aileron")
    if aileronsMode == 2 then
      addMix(ailCH2, MIXSRC_FIRST_INPUT+defaultChannel(3), "Aileron", -100)
    end
  end
  if flapsMode > 0 then
    addMix(flapsCH1, MIXSRC_SA, "Flap")
    if flapsMode == 2 then
      addMix(flapsCH2, MIXSRC_SA, "Flap")
    end
  end
  if brakesMode > 0 then
    addMix(brakesCH1, MIXSRC_SE, "Brake")
    if brakesMode == 2 then
      addMix(brakesCH2, MIXSRC_SE, "Brake")
    end
  end
  if tailMode == 3 then
    addMix(eleCH1, MIXSRC_FIRST_INPUT+defaultChannel(1), "V-Tail-E", 50)
    addMix(eleCH1, MIXSRC_FIRST_INPUT+defaultChannel(0), "V-Tail-R", 50, 1)
    addMix(eleCH2, MIXSRC_FIRST_INPUT+defaultChannel(1), "V-Tail-E", 50)
    addMix(eleCH2, MIXSRC_FIRST_INPUT+defaultChannel(0), "V-Tail-R", -50, 1)
  else
    addMix(eleCH1, MIXSRC_FIRST_INPUT+defaultChannel(1), "Elevator")
    if tailMode > 0 then
      addMix(rudCH1, MIXSRC_FIRST_INPUT+defaultChannel(0), "Rudder")
    end
    if tailMode == 2 then
      addMix(eleCH2, MIXSRC_FIRST_INPUT+defaultChannel(1), "Elevator")
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
