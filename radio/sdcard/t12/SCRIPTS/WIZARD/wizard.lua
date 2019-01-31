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
  if event == EVT_ROT_LEFT or event == EVT_UP_BREAK then
    value = (value + max)
    dirty = true
  elseif event == EVT_ROT_RIGHT or event == EVT_DOWN_BREAK then
    value = (value + max + 2)
    dirty = true
  end
  value = (value % (max+1))
  return value
end

-- Model Type Menu
local function modelTypeSurround(index)
  if(index<=1) then
    lcd.drawFilledRectangle(59*(index%2)+12, 13, 43, 23)
  else
    lcd.drawFilledRectangle(59*(index%2)+12, 34, 40, 20)
  end
end

local function drawModelChoiceMenu()
  lcd.clear()
  lcd.drawScreenTitle("Select model type", 0, 0)
    lcd.drawText( 20, 20, "Plane")
    lcd.drawText( 78, 20, "Delta")
    lcd.drawText( 20, 40, "Multi")
  modelTypeSurround(modelType)
  fieldsMax = 0
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
