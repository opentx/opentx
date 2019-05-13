---- #########################################################################
---- #                                                                       #
---- # Telemetry Widget script for FrSky Horus                               #
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

local shadowed = 0
local _no_telem_blink = 0
local _isDataAvailable = 0

local _cellDataLive = {}
local _cellDataLowest = {}
local _cellMax = 0;
local _cellMin = 0;
local _cellAvg = 0;
local _cellPercent = 0;
local _cellCount = 0;
local _cellSum = 0;

local _options = {
  { "Sensor",     SOURCE, 0     },
  { "Color",      COLOR,  WHITE },
  { "Shadow",     BOOL,   0     },
  { "LowestCell", BOOL,   0     }
}

--- This function return the percentage remaining in a single Lipo cel
local function getCellPercent(cellValue)
  if cellValue == nil then
    return 0
  end
  --## Data gathered from commercial lipo sensors
  local myArrayPercentList = { { 3, 0 }, { 3.093, 1 }, { 3.196, 2 }, { 3.301, 3 }, { 3.401, 4 }, { 3.477, 5 }, { 3.544, 6 }, { 3.601, 7 }, { 3.637, 8 }, { 3.664, 9 }, { 3.679, 10 }, { 3.683, 11 }, { 3.689, 12 }, { 3.692, 13 }, { 3.705, 14 }, { 3.71, 15 }, { 3.713, 16 }, { 3.715, 17 }, { 3.72, 18 }, { 3.731, 19 }, { 3.735, 20 }, { 3.744, 21 }, { 3.753, 22 }, { 3.756, 23 }, { 3.758, 24 }, { 3.762, 25 }, { 3.767, 26 }, { 3.774, 27 }, { 3.78, 28 }, { 3.783, 29 }, { 3.786, 30 }, { 3.789, 31 }, { 3.794, 32 }, { 3.797, 33 }, { 3.8, 34 }, { 3.802, 35 }, { 3.805, 36 }, { 3.808, 37 }, { 3.811, 38 }, { 3.815, 39 }, { 3.818, 40 }, { 3.822, 41 }, { 3.825, 42 }, { 3.829, 43 }, { 3.833, 44 }, { 3.836, 45 }, { 3.84, 46 }, { 3.843, 47 }, { 3.847, 48 }, { 3.85, 49 }, { 3.854, 50 }, { 3.857, 51 }, { 3.86, 52 }, { 3.863, 53 }, { 3.866, 54 }, { 3.87, 55 }, { 3.874, 56 }, { 3.879, 57 }, { 3.888, 58 }, { 3.893, 59 }, { 3.897, 60 }, { 3.902, 61 }, { 3.906, 62 }, { 3.911, 63 }, { 3.918, 64 }, { 3.923, 65 }, { 3.928, 66 }, { 3.939, 67 }, { 3.943, 68 }, { 3.949, 69 }, { 3.955, 70 }, { 3.961, 71 }, { 3.968, 72 }, { 3.974, 73 }, { 3.981, 74 }, { 3.987, 75 }, { 3.994, 76 }, { 4.001, 77 }, { 4.007, 78 }, { 4.014, 79 }, { 4.021, 80 }, { 4.029, 81 }, { 4.036, 82 }, { 4.044, 83 }, { 4.052, 84 }, { 4.062, 85 }, { 4.074, 86 }, { 4.085, 87 }, { 4.095, 88 }, { 4.105, 89 }, { 4.111, 90 }, { 4.116, 91 }, { 4.12, 92 }, { 4.125, 93 }, { 4.129, 94 }, { 4.135, 95 }, { 4.145, 96 }, { 4.176, 97 }, { 4.179, 98 }, { 4.193, 99 }, { 4.2, 100 } }

  for i, v in ipairs(myArrayPercentList) do
    if v[1] >= cellValue then
      result = v[2]
      break
    end
  end
  return result
end


-- This function is runned once at the creation of the widget
local function create(zone, options)
  local wgt = { zone = zone, options = options, counter = 0 }
  _cellDataLowest = {}

  -- use default if user did not set, So widget is operational on "select widget"
  if (wgt ~= nil) then
    if wgt.options.Sensor == 0 then
      wgt.options.Sensor = "Cels"
    end
  end

  return wgt
end

-- This function allow updates when you change widgets settings
local function update(wgt, options)
  if (wgt == nil) then return end

  wgt.options = options

  -- use default if user did not set, So widget is operational on "select widget"
  if wgt.options.Sensor == 0 then
    wgt.options.Sensor = "Cels"
  end
end

-- A quick and dirty check for empty table
local function isEmpty(self)
  for _, _ in pairs(self) do
    return false
  end
  return true
end

--- This function returns a table with cels values
local function updateCellData(wgt)

  local newCellData = getValue(wgt.options.Sensor)

    -- initialize historical table if not done yet
  if type(newCellData) == "table" then
    if isEmpty(_cellDataLowest) then
      for k, v in pairs(newCellData) do
        _cellDataLowest[k] = v
      end
    end
    -- this is necessary for simu where cellcount can change
    if #_cellDataLowest ~= #newCellData then
      for k, v in pairs(newCellData) do
        _cellDataLowest[k] = v
      end
    end
    -- stores the lowest cell values in historical table
    for k, v in pairs(newCellData) do
      if v < _cellDataLowest[k]
      then
        _cellDataLowest[k] = v
      end
    end

    --- calc highest of all cels
    local cellMax = 0
    for k, v in pairs(newCellData) do
      if v > cellMax then
        cellMax = v
      end
    end
    _cellMax = cellMax

    --- calc lowest of all cels
    local cellMin = 5
    for k, v in pairs(newCellData) do
      if v < cellMin then
        cellMin = v
      end
    end
    _cellMin = cellMin

    _cellCount = #newCellData

    --- sum of all cels
    local cellSum = 0
    for k, v in pairs(newCellData) do
      cellSum = cellSum + v
    end
    _cellSum = cellSum

    --- the average of all cels
    _cellAvg = _cellSum / _cellCount
    _cellPercent = getCellPercent(_cellMin)

    _cellDataLive = newCellData

    _isDataAvailable = true

  else

    _isDataAvailable = false
    --_cellMax = 0
    --_cellMin = 0
    --_cellAvg = 0
    --_cellPercent = 0
    --_cellCount = 0
    --_cellSum = 0

    --_cellDataLive = newCellData

    -- erase low cell memory when you change lipo
    --cellDataLowest = {}

  end

  -- MainValue
  if wgt.options.LowestCell == 1 then
    _mainValue = _cellMin
  elseif wgt.options.LowestCell == 0 then
    _mainValue = _cellSum
  else
    _mainValue = "-1"
  end

  -- SecondaryValue
  if wgt.options.LowestCell == 1 then
    _secondaryValue = _cellSum
  elseif wgt.options.LowestCell == 0 then
    _secondaryValue = _cellMin
  else
    _secondaryValue = "-1"
  end



end


-- This function returns green at 100%, red bellow 30% and graduate in between
-- color for battery
local function getPercentColor(cpercent)
  if cpercent < 30 then
    return lcd.RGB(0xff, 0, 0)
  else
    g = math.floor(0xdf * cpercent / 100)
    r = 0xdf - g
    return lcd.RGB(r, g, 0)
  end
end

-- This function returns green at gvalue, red at rvalue and graduate in between
-- color for cell
local function getRangeColor(value, gvalue, rvalue)
  if gvalue > rvalue and not range == 0 then
    local range = gvalue - rvalue
    if value > gvalue then return lcd.RGB(0, 0xdf, 0) end
    if value < rvalue then return lcd.RGB(0xdf, 0, 0) end
    g = math.floor(0xdf * (value - rvalue) / range)
    r = 0xdf - g
    return lcd.RGB(r, g, 0)
  else
    local range = rvalue - gvalue
    if value > gvalue then return lcd.RGB(0, 0xdf, 0) end
    if value < rvalue then return lcd.RGB(0xdf, 0, 0) end
    r = math.floor(0xdf * (value - gvalue) / range)
    g = 0xdf - r
    return lcd.RGB(r, g, 0)
  end
end


-- This size is for top bar widgets
local function refreshZoneTiny(wgt)
  --if _isDataAvailable then
  local myString = string.format("%2.1fV", _mainValue)
  lcd.drawText(wgt.zone.x + wgt.zone.w, wgt.zone.y, _cellPercent .. "%", RIGHT + SMLSIZE + CUSTOM_COLOR + _no_telem_blink)
  lcd.drawText(wgt.zone.x + wgt.zone.w, wgt.zone.y + 15, myString, RIGHT + SMLSIZE + CUSTOM_COLOR + _no_telem_blink)
  -- draw batt
  lcd.drawRectangle(zone.zone.x, wgt.zone.y + 6, 16, 25, CUSTOM_COLOR, 2)
  lcd.drawFilledRectangle(wgt.zone.x + 4, wgt.zone.y + 4, 6, 3, CUSTOM_COLOR)
  local rect_h = math.floor(25 * _cellPercent / 100)
  lcd.drawFilledRectangle(wgt.zone.x, wgt.zone.y + 6 + 25 - rect_h, 16, rect_h, CUSTOM_COLOR + _no_telem_blink)
  --end
end

--- Size is 160x32 1/8th
local function refreshZoneSmall(wgt)
  local myBatt = { ["x"] = 0, ["y"] = 0, ["w"] = 155, ["h"] = 35, ["segments_w"] = 25, ["color"] = WHITE, ["cath_w"] = 6, ["cath_h"] = 20 }

  -- draws bat
  lcd.setColor(CUSTOM_COLOR, WHITE)
  lcd.drawRectangle(wgt.zone.x + myBatt.x, wgt.zone.y + myBatt.y, myBatt.w, myBatt.h, CUSTOM_COLOR, 2)

  -- fill batt
  lcd.setColor(CUSTOM_COLOR, getPercentColor(_cellPercent))
  lcd.drawGauge(wgt.zone.x + 2, wgt.zone.y + 2, myBatt.w - 4, wgt.zone.h, _cellPercent, 100, CUSTOM_COLOR)

  -- write text
  if _isDataAvailable then
    lcd.setColor(CUSTOM_COLOR, wgt.options.Color)
  else
    lcd.setColor(CUSTOM_COLOR, GREY)
  end
  local topLine = string.format("%2.1fV      %2.0f%%", _mainValue, _cellPercent)
  lcd.drawText(wgt.zone.x + 20, wgt.zone.y + 2, topLine, MIDSIZE + CUSTOM_COLOR + shadowed + _no_telem_blink)

  return
end

--- Size is 180x70 1/4th  (with sliders/trim)
--- Size is 225x98 1/4th  (no sliders/trim)
local function refreshZoneMedium(wgt)
  local myBatt = { ["x"] = 0, ["y"] = 0, ["w"] = 85, ["h"] = 35, ["segments_w"] = 15, ["color"] = WHITE, ["cath_w"] = 6, ["cath_h"] = 20 }

  if _isDataAvailable then
    lcd.setColor(CUSTOM_COLOR, wgt.options.Color)
  else
    lcd.setColor(CUSTOM_COLOR, GREY)
  end

  -- draw values
  --lcd.drawText(wgt.zone.x + wgt.zone.w, wgt.zone.y, string.format("%2.1fV", _mainValue), DBLSIZE + CUSTOM_COLOR + RIGHT + shadowed + _no_telem_blink)
  lcd.drawText(wgt.zone.x, wgt.zone.y + 35, string.format("%2.1fV", _mainValue), DBLSIZE + CUSTOM_COLOR + shadowed + _no_telem_blink)

  -- fill batt
  lcd.setColor(CUSTOM_COLOR, getPercentColor(_cellPercent))
  lcd.drawGauge(wgt.zone.x + myBatt.x, wgt.zone.y + myBatt.y, myBatt.w, myBatt.h, _cellPercent, 100, CUSTOM_COLOR)

  -- draw cells
  local pos = { { x = 118, y = 0 }, { x = 118, y = 16 }, { x = 118, y = 32 }, { x = 118, y = 48 }, { x = 118, y = 64 }, { x = 118, y = 80 } }
  for i = 1, _cellCount, 1 do
    lcd.setColor(CUSTOM_COLOR, getRangeColor(_cellDataLive[i], _cellMax, _cellMax - 0.2))
    lcd.drawFilledRectangle(wgt.zone.x + pos[i].x, wgt.zone.y + pos[i].y, 58, 15, CUSTOM_COLOR)
    --lcd.setColor(CUSTOM_COLOR, getPercentColor(cellDataLowest[i]))
    --lcd.drawFilledRectangle(wgt.zone.x + pos[i].x, wgt.zone.y + pos[i].y+10, 58*getCellPercent(cellDataLowest[i])/100, 10, CUSTOM_COLOR)
    lcd.setColor(CUSTOM_COLOR, WHITE)
    lcd.drawText(wgt.zone.x + pos[i].x + 10, wgt.zone.y + pos[i].y, string.format("%.2f", _cellDataLive[i]), SMLSIZE + CUSTOM_COLOR + shadowed + _no_telem_blink)
    lcd.drawRectangle(wgt.zone.x + pos[i].x, wgt.zone.y + pos[i].y, 59, 15, CUSTOM_COLOR, 1)
  end

  -- draws bat
  lcd.setColor(CUSTOM_COLOR, WHITE)
  lcd.drawRectangle(wgt.zone.x + myBatt.x, wgt.zone.y + myBatt.y, myBatt.w, myBatt.h, CUSTOM_COLOR, 2)
  lcd.drawFilledRectangle(wgt.zone.x + myBatt.x + myBatt.w, wgt.zone.y + myBatt.h / 2 - myBatt.cath_h / 2, myBatt.cath_w, myBatt.cath_h, CUSTOM_COLOR)
  lcd.drawText(wgt.zone.x + myBatt.x + 20, wgt.zone.y + myBatt.y + 5, string.format("%2.0f%%", _cellPercent), LEFT + MIDSIZE + CUSTOM_COLOR + shadowed)
  --for i=1, myBatt.w - myBatt.segments_w, myBatt.segments_w do
  --  lcd.drawRectangle(wgt.zone.x + myBatt.x + i, wgt.zone.y + myBatt.y, myBatt.segments_w, myBatt.h, CUSTOM_COLOR, 1)
  --end
  return
end

--- Size is 192x152 1/2
local function refreshZoneLarge(wgt)
  local myBatt = { ["x"] = 0, ["y"] = 18, ["w"] = 76, ["h"] = 121, ["segments_h"] = 30, ["color"] = WHITE, ["cath_w"] = 30, ["cath_h"] = 10 }

  if _isDataAvailable then
    lcd.setColor(CUSTOM_COLOR, wgt.options.Color)
  else
    lcd.setColor(CUSTOM_COLOR, GREY)
  end

  lcd.drawText(wgt.zone.x + wgt.zone.w, wgt.zone.y, _cellPercent .. "%", RIGHT + DBLSIZE + CUSTOM_COLOR + shadowed)
  lcd.drawText(wgt.zone.x + wgt.zone.w, wgt.zone.y + 30, string.format("%2.1fV", _mainValue), RIGHT + DBLSIZE + CUSTOM_COLOR + shadowed)
  lcd.drawText(wgt.zone.x + wgt.zone.w, wgt.zone.y + 70, string.format("%2.1fV %2.1fS", _secondaryValue, _cellCount), RIGHT + SMLSIZE + CUSTOM_COLOR + shadowed)
  -- fill batt
  lcd.setColor(CUSTOM_COLOR, getPercentColor(_cellPercent))
  lcd.drawFilledRectangle(wgt.zone.x + myBatt.x, wgt.zone.y + myBatt.y + myBatt.h + myBatt.cath_h - math.floor(_cellPercent / 100 * myBatt.h), myBatt.w, math.floor(_cellPercent / 100 * myBatt.h), CUSTOM_COLOR)
  -- draw cells
  local pos = { { x = 80, y = 90 }, { x = 138, y = 90 }, { x = 80, y = 109 }, { x = 138, y = 109 }, { x = 80, y = 128 }, { x = 138, y = 128 } }
  for i = 1, _cellCount, 1 do
    lcd.setColor(CUSTOM_COLOR, getRangeColor(_cellDataLive[i], _cellMax, _cellMax - 0.2))
    lcd.drawFilledRectangle(wgt.zone.x + pos[i].x, wgt.zone.y + pos[i].y, 58, 20, CUSTOM_COLOR)
    lcd.setColor(CUSTOM_COLOR, WHITE)
    lcd.drawText(wgt.zone.x + pos[i].x + 10, wgt.zone.y + pos[i].y, string.format("%.2f", _cellDataLive[i]), CUSTOM_COLOR + shadowed)
    lcd.drawRectangle(wgt.zone.x + pos[i].x, wgt.zone.y + pos[i].y, 59, 20, CUSTOM_COLOR, 1)
  end

  -- draws bat
  lcd.setColor(CUSTOM_COLOR, WHITE)
  lcd.drawRectangle(wgt.zone.x + myBatt.x, wgt.zone.y + myBatt.y + myBatt.cath_h, myBatt.w, myBatt.h, CUSTOM_COLOR, 2)
  lcd.drawFilledRectangle(wgt.zone.x + myBatt.x + myBatt.w / 2 - myBatt.cath_w / 2, wgt.zone.y + myBatt.y, myBatt.cath_w, myBatt.cath_h, CUSTOM_COLOR)
  for i = 1, myBatt.h - myBatt.segments_h, myBatt.segments_h do
    lcd.drawRectangle(wgt.zone.x + myBatt.x, wgt.zone.y + myBatt.y + myBatt.cath_h + i, myBatt.w, myBatt.segments_h, CUSTOM_COLOR, 1)
  end
  return
end

--- Size is 390x172 1/1
--- Size is 460x252 1/1 (no sliders/trim/topbar)
local function refreshZoneXLarge(wgt)
  local myBatt = { ["x"] = 10, ["y"] = 20, ["w"] = 80, ["h"] = 121, ["segments_h"] = 30, ["color"] = WHITE, ["cath_w"] = 30, ["cath_h"] = 10 }

  lcd.setColor(CUSTOM_COLOR, wgt.options.Color)

  -- fill batt
  lcd.setColor(CUSTOM_COLOR, getPercentColor(_cellPercent))
  lcd.drawFilledRectangle(wgt.zone.x + myBatt.x, wgt.zone.y + myBatt.y + myBatt.h + myBatt.cath_h - math.floor(_cellPercent / 100 * myBatt.h), myBatt.w, math.floor(_cellPercent / 100 * myBatt.h), CUSTOM_COLOR)

  -- draw right text section
  if _isDataAvailable then
    lcd.setColor(CUSTOM_COLOR, wgt.options.Color)
  else
    lcd.setColor(CUSTOM_COLOR, GREY)
  end
  lcd.drawText(wgt.zone.x + wgt.zone.w, wgt.zone.y + myBatt.y, _cellPercent .. "%", RIGHT + DBLSIZE + CUSTOM_COLOR + shadowed + _no_telem_blink)

  lcd.drawText(wgt.zone.x + wgt.zone.w, wgt.zone.y + myBatt.y + 30, string.format("%2.1fV", _mainValue), RIGHT + DBLSIZE + CUSTOM_COLOR + shadowed + _no_telem_blink)
  lcd.drawText(wgt.zone.x + wgt.zone.w, wgt.zone.y + myBatt.y + 105, string.format("%2.1fV %2.1fS", _secondaryValue, _cellCount), RIGHT + SMLSIZE + CUSTOM_COLOR + shadowed + _no_telem_blink)

  -- draw cells
  local pos = { { x = 111, y = 38 }, { x = 164, y = 38 }, { x = 217, y = 38 }, { x = 111, y = 57 }, { x = 164, y = 57 }, { x = 217, y = 57 } }
  for i = 1, _cellCount, 1 do
    lcd.setColor(CUSTOM_COLOR, getRangeColor(_cellDataLive[i], _cellMax, _cellMax - 0.2))
    lcd.drawFilledRectangle(wgt.zone.x + pos[i].x, wgt.zone.y + pos[i].y, 53, 20, CUSTOM_COLOR)
    lcd.setColor(CUSTOM_COLOR, WHITE)
    lcd.drawText(wgt.zone.x + pos[i].x + 10, wgt.zone.y + pos[i].y, string.format("%.2f", _cellDataLive[i]), CUSTOM_COLOR + shadowed + _no_telem_blink)
    lcd.drawRectangle(wgt.zone.x + pos[i].x, wgt.zone.y + pos[i].y, 54, 20, CUSTOM_COLOR, 1)
  end
  -- draw cells for lowest cells
  local pos = { { x = 111, y = 110 }, { x = 164, y = 110 }, { x = 217, y = 110 }, { x = 111, y = 129 }, { x = 164, y = 129 }, { x = 217, y = 129 } }
  for i = 1, _cellCount, 1 do
    lcd.setColor(CUSTOM_COLOR, getRangeColor(_cellDataLowest[i], _cellDataLive[i], _cellDataLive[i] - 0.3))
    lcd.drawFilledRectangle(wgt.zone.x + pos[i].x, wgt.zone.y + pos[i].y, 53, 20, CUSTOM_COLOR)
    lcd.setColor(CUSTOM_COLOR, WHITE)
    lcd.drawRectangle(wgt.zone.x + pos[i].x, wgt.zone.y + pos[i].y, 54, 20, CUSTOM_COLOR, 1)
    lcd.drawText(wgt.zone.x + pos[i].x + 10, wgt.zone.y + pos[i].y, string.format("%.2f", _cellDataLowest[i]), CUSTOM_COLOR + shadowed + _no_telem_blink)
  end

  -- draws bat
  lcd.setColor(CUSTOM_COLOR, WHITE)
  lcd.drawRectangle(wgt.zone.x + myBatt.x, wgt.zone.y + myBatt.y + myBatt.cath_h, myBatt.w, myBatt.h, CUSTOM_COLOR, 2)
  lcd.drawFilledRectangle(wgt.zone.x + myBatt.x + myBatt.w / 2 - myBatt.cath_w / 2, wgt.zone.y + myBatt.y, myBatt.cath_w, myBatt.cath_h, CUSTOM_COLOR)
  for i = 1, myBatt.h - myBatt.segments_h, myBatt.segments_h do
    lcd.drawRectangle(wgt.zone.x + myBatt.x, wgt.zone.y + myBatt.y + myBatt.cath_h + i, myBatt.w, myBatt.segments_h, CUSTOM_COLOR, 1)
  end
  -- draw middle rectangles
  lcd.drawRectangle(wgt.zone.x + 110, wgt.zone.y + 38, 161, 40, CUSTOM_COLOR, 1)
  lcd.drawText(wgt.zone.x + 220, wgt.zone.y + 21, "Live data", RIGHT + SMLSIZE + INVERS + CUSTOM_COLOR + shadowed)
  lcd.drawRectangle(wgt.zone.x + 110, wgt.zone.y + 110, 161, 40, CUSTOM_COLOR, 1)
  lcd.drawText(wgt.zone.x + 230, wgt.zone.y + 93, "Lowest data", RIGHT + SMLSIZE + INVERS + CUSTOM_COLOR + shadowed)
  return
end

-- This function allow recording of lowest cells when widget is not active
local function background(wgt)
  if (wgt == nil) then return end

  updateCellData(wgt)
  return
end

local function refresh(wgt)

  if (wgt == nil) then return end

  if (wgt.options == nil) then
    print("refresh(wgt.options=nil)")
    return
  end

  if (wgt.zone == nil) then
    print("refresh(wgt.zone=nil)")
    return
  end

  if (wgt.options.LowestCell == nil) then
    print("refresh(wgt.options.LowestCell=nil)")
    return
  end

  if wgt.options.Shadow == 1 then
    shadowed = SHADOWED
  else
    shadowed = 0
  end


  updateCellData(wgt)

  if _isDataAvailable then
    _no_telem_blink = 0
  else
    _no_telem_blink = INVERS + BLINK
  end

  if     wgt.zone.w  > 380 and wgt.zone.h > 165 then refreshZoneXLarge(wgt)
  elseif wgt.zone.w  > 180 and wgt.zone.h > 145 then refreshZoneLarge(wgt)
  elseif wgt.zone.w  > 170 and wgt.zone.h >  65 then refreshZoneMedium(wgt)
  elseif wgt.zone.w  > 150 and wgt.zone.h >  28 then refreshZoneSmall(wgt)
  elseif wgt.zone.w  >  65 and wgt.zone.h >  35 then refreshZoneTiny(wgt)
  end

end

return { name="BattCheck", options=_options, create=create, update=update, background=background, refresh=refresh }
