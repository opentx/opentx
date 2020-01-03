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
-- Navigation variables
local dirty = true

-- Model types
local modelType = 0
local MODELTYPE_PLANE = 0
local MODELTYPE_DELTA = 1
local MODELTYPE_QUAD = 2

-- Common functions
local function fieldIncDec(event, value, max)
  if event == EVT_PLUS_BREAK or event == EVT_ROT_LEFT or event == EVT_PLUS_REPT then
    value = (value + max)
    dirty = true
  elseif event == EVT_MINUS_BREAK or event == EVT_ROT_RIGHT or event == EVT_MINUS_REPT then
    value = (value + max + 2)
    dirty = true
  end
  value = (value % (max+1))
  return value
end

-- Model Type Menu
local function modelTypeSurround(index)
  lcd.drawRectangle(17+65*index, 14, 48, 48)
  lcd.drawPixmap(22+65*index, 9, "mark.bmp")
end

local function drawModelChoiceMenu()
  lcd.clear()
  lcd.drawScreenTitle("Select model type", 0, 0)
  lcd.drawPixmap(21, 18, "plane.bmp")
  lcd.drawPixmap(86, 18, "delta.bmp")
  lcd.drawPixmap(151, 18, "quadri.bmp")
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
    elseif modelType == MODELTYPE_DELTA then
      return "delta.lua"
    elseif modelType == MODELTYPE_QUAD then
      return "multi.lua"
    end
    dirty = true
  else
    modelType = fieldIncDec(event, modelType, 2)
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
