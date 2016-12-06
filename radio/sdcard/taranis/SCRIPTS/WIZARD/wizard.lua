-- Navigation variables
local dirty = true

-- Model types
local modelType = 0
local MODELTYPE_PLANE = 0
local MODELTYPE_HELI = 1
local MODELTYPE_DELTA = 2
local MODELTYPE_QUAD = 3

-- Common functions
local function fieldIncDec(event, value, max)
  if event == EVT_PLUS_BREAK or event == EVT_ROT_LEFT then
    value = (value + max)
    dirty = true
  elseif event == EVT_MINUS_BREAK or event == EVT_ROT_RIGHT then
    value = (value + max + 2)
    dirty = true
  end
  value = (value % (max+1))
  return value
end

-- Model Type Menu
local function modelTypeSurround(index)
  lcd.drawRectangle(12+47*index, 13, 48, 48)
  lcd.drawPixmap(17+47*index, 8, "mark.bmp")
end

local function drawModelChoiceMenu()
  lcd.clear()
  lcd.drawScreenTitle("", 0, 0)
  -- lcd.drawText(58, 13, "Select model type", 0)
  lcd.drawPixmap( 16, 17, "plane.bmp")
  lcd.drawPixmap( 63, 17, "heli.bmp")
  lcd.drawPixmap(110, 17, "delta.bmp")
  lcd.drawPixmap(157, 17, "quadri.bmp")
  modelTypeSurround(modelType)
end

local function modelTypeMenu(event)
  if dirty == true then
    drawModelChoiceMenu()
    dirty = false
  end
  if event == EVT_ENTER_BREAK then
    if modelType == MODELTYPE_PLANE then
      return "plane.lua"
    elseif modelType == MODELTYPE_HELI then
    elseif modelType == MODELTYPE_DELTA then
      return "delta.lua"
    elseif modelType == MODELTYPE_QUAD then
      return "multi.lua"
    end
    dirty = true
  else
    modelType = fieldIncDec(event, modelType, 3)
  end
  return 0
end

-- Main
local function run(event)
  if event == nil then
    error("Cannot be run as a model script!")
  end

  if event == EVT_EXIT_BREAK then
    return 2
  end

  return modelTypeMenu(event)
end

return { run=run }
