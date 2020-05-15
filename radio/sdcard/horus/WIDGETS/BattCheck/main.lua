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

-- Horus Widget to display the levels of lipo battery with per cell indication
-- 3djc & Offer Shmuely
-- Date: 2020
-- ver: 0.5
local version = "v0.5"

local _options = {
  { "Sensor", SOURCE, 0 }, -- default to 'Cels'
  { "Color", COLOR, WHITE },
  { "Shadow", BOOL, 0 },
  { "LowestCell", BOOL, 1 }   -- 0=main voltage display shows all-cell-voltage, 1=main voltage display shows lowest-cell
}

-- Data gathered from commercial lipo sensors

local _lipoPercentListSplit = {
  { { 3, 0 }, { 3.093, 1 }, { 3.196, 2 }, { 3.301, 3 }, { 3.401, 4 }, { 3.477, 5 }, { 3.544, 6 }, { 3.601, 7 }, { 3.637, 8 }, { 3.664, 9 }, { 3.679, 10 }, { 3.683, 11 }, { 3.689, 12 }, { 3.692, 13 } },
  { { 3.705, 14 }, { 3.71, 15 }, { 3.713, 16 }, { 3.715, 17 }, { 3.72, 18 }, { 3.731, 19 }, { 3.735, 20 }, { 3.744, 21 }, { 3.753, 22 }, { 3.756, 23 }, { 3.758, 24 }, { 3.762, 25 }, { 3.767, 26 } },
  { { 3.774, 27 }, { 3.78, 28 }, { 3.783, 29 }, { 3.786, 30 }, { 3.789, 31 }, { 3.794, 32 }, { 3.797, 33 }, { 3.8, 34 }, { 3.802, 35 }, { 3.805, 36 }, { 3.808, 37 }, { 3.811, 38 }, { 3.815, 39 } },
  { { 3.818, 40 }, { 3.822, 41 }, { 3.825, 42 }, { 3.829, 43 }, { 3.833, 44 }, { 3.836, 45 }, { 3.84, 46 }, { 3.843, 47 }, { 3.847, 48 }, { 3.85, 49 }, { 3.854, 50 }, { 3.857, 51 }, { 3.86, 52 } },
  { { 3.863, 53 }, { 3.866, 54 }, { 3.87, 55 }, { 3.874, 56 }, { 3.879, 57 }, { 3.888, 58 }, { 3.893, 59 }, { 3.897, 60 }, { 3.902, 61 }, { 3.906, 62 }, { 3.911, 63 }, { 3.918, 64 } },
  { { 3.923, 65 }, { 3.928, 66 }, { 3.939, 67 }, { 3.943, 68 }, { 3.949, 69 }, { 3.955, 70 }, { 3.961, 71 }, { 3.968, 72 }, { 3.974, 73 }, { 3.981, 74 }, { 3.987, 75 }, { 3.994, 76 } },
  { { 4.001, 77 }, { 4.007, 78 }, { 4.014, 79 }, { 4.021, 80 }, { 4.029, 81 }, { 4.036, 82 }, { 4.044, 83 }, { 4.052, 84 }, { 4.062, 85 }, { 4.074, 86 }, { 4.085, 87 }, { 4.095, 88 } },
  { { 4.105, 89 }, { 4.111, 90 }, { 4.116, 91 }, { 4.12, 92 }, { 4.125, 93 }, { 4.129, 94 }, { 4.135, 95 }, { 4.145, 96 }, { 4.176, 97 }, { 4.179, 98 }, { 4.193, 99 }, { 4.2, 100 } },
}

local function periodicInit(t, durationMili)
  t.startTime = getTime();
  t.durationMili = durationMili;
end
local function periodicReset(t)
  t.startTime = getTime();
end
local function periodicHasPassed(t)
  local elapsed = getTime() - t.startTime;
  local elapsedMili = elapsed * 10;
  if (elapsedMili < t.durationMili) then
    return false;
  end
  return true;
end
local function periodicGetElapsedTime(t)
  local elapsed = getTime() - t.startTime;
  local elapsedMili = elapsed * 10;
  return elapsedMili;
end

-- This function is run once at the creation of the widget
local function create(zone, options)
  local wgt = {
    zone = zone,
    options = options,
    counter = 0,
    shadowed = 0,

    telemResetCount = 0,
    telemResetLowestMinRSSI = 101,
    no_telem_blink = 0,
    isDataAvailable = 0,
    cellDataLive = { 0, 0, 0, 0, 0, 0 },
    cellDataHistoryLowest = { 5, 5, 5, 5, 5, 5 },
    cellDataHistoryCellLowest = 5,
    cellMax = 0,
    cellMin = 0,
    cellAvg = 0,
    cellPercent = 0,
    cellCount = 0,
    cellSum = 0,
    mainValue = 0,
    secondaryValue = 0,
    periodic1 = { startTime = getTime(), durationMili = 1000 },
    periodicProfiler = { startTime = getTime(), durationMili = 5000 },
    profTimes = {},
  }

  -- use default if user did not set, So widget is operational on "select widget"
  if wgt.options.Sensor == 0 then
    wgt.options.Sensor = "Cels"
  end

  wgt.options.LowestCell = wgt.options.LowestCell % 2 -- modulo due to bug that cause the value to be other than 0|1

  return wgt
end

-- This function allow updates when you change widgets settings
local function update(wgt, options)
  if (wgt == nil) then
    return
  end

  wgt.options = options

  -- use default if user did not set, So widget is operational on "select widget"
  if wgt.options.Sensor == 0 then
    wgt.options.Sensor = "Cels"
  end

  wgt.options.LowestCell = wgt.options.LowestCell % 2 -- modulo due to bug that cause the value to be other than 0|1

end


-- clear old telemetry data upon reset event
local function onTelemetryResetEvent(wgt)
  wgt.telemResetCount = wgt.telemResetCount + 1

  wgt.cellDataLive = { 0, 0, 0, 0, 0, 0 }
  wgt.cellDataHistoryLowest = { 5, 5, 5, 5, 5, 5 }
  wgt.cellDataHistoryCellLowest = 5
end


-- workaround to detect telemetry-reset event, until a proper implementation on the lua interface will be created
-- this workaround assume that:
--   RSSI- is always going down
--   RSSI- is reset on the C++ side when a telemetry-reset is pressed by user
--   widget is calling this func on each refresh/background
-- on event detection, the function onTelemetryResetEvent() will be trigger
--
local function detectResetEvent(wgt)

  local currMinRSSI = getValue('RSSI-')
  if (currMinRSSI == nil) then
    return
  end
  if (currMinRSSI == wgt.telemResetLowestMinRSSI) then
    return
  end

  if (currMinRSSI < wgt.telemResetLowestMinRSSI) then
    -- rssi just got lower, record it
    wgt.telemResetLowestMinRSSI = currMinRSSI
    return
  end


  -- reset telemetry detected
  wgt.telemResetLowestMinRSSI = 101

  -- notify event
  onTelemetryResetEvent(wgt)

end

--- This function return the percentage remaining in a single Lipo cel
--- since running on long array found to be very intensive to hrous cpu, we are splitting the list to small lists
local function getCellPercent(wgt, cellValue)
  if cellValue == nil then
    return 0
  end
  local result = 0;

  for i1, v1 in ipairs(_lipoPercentListSplit) do
    --is the cellVal < last-value-on-sub-list? (first-val:v1[1], last-val:v1[#v1])
    if (cellValue <= v1[#v1][1]) then
      -- cellVal is in this sub-list, find the exact value
      for i2, v2 in ipairs(v1) do
        if v2[1] >= cellValue then
          result = v2[2]
          return result
        end
      end
    end
  end
  -- in case somehow voltage is too high (>4.2), don't return nil
  return 100
end

--- This function returns a table with cels values
local function calculateBatteryData(wgt)

  local newCellData = getValue(wgt.options.Sensor)

  if type(newCellData) ~= "table" then
    wgt.isDataAvailable = false
    return
  end

  local cellMax = 0
  local cellMin = 5
  local cellSum = 0
  for k, v in pairs(newCellData) do
    -- stores the lowest cell values in historical table
    if v > 1 and v < wgt.cellDataHistoryLowest[k] then
      -- min 1v to consider a valid reading
      wgt.cellDataHistoryLowest[k] = v

      --- calc history lowest of all cells
      if v < wgt.cellDataHistoryCellLowest then
        wgt.cellDataHistoryCellLowest = v
      end

    end

    -- calc highest of all cells
    if v > cellMax then
      cellMax = v
    end

    --- calc lowest of all cells
    if v < cellMin and v > 1 then
      -- min 1v to consider a valid reading
      cellMin = v
    end
    --- sum of all cells
    cellSum = cellSum + v


  end

  wgt.cellMin = cellMin
  wgt.cellMax = cellMax
  wgt.cellCount = #newCellData
  wgt.cellSum = cellSum

  --- average of all cells
  wgt.cellAvg = wgt.cellSum / wgt.cellCount

  wgt.cellDataLive = newCellData

  -- mainValue
  if wgt.options.LowestCell == 1 then
    wgt.mainValue = wgt.cellMin
  elseif wgt.options.LowestCell == 0 then
    wgt.mainValue = wgt.cellSum
  else
    wgt.mainValue = "-1"
  end

  -- secondaryValue
  if wgt.options.LowestCell == 1 then
    wgt.secondaryValue = wgt.cellSum
  elseif wgt.options.LowestCell == 0 then
    wgt.secondaryValue = wgt.cellMin
  else
    wgt.secondaryValue = "-2"
  end

  wgt.isDataAvailable = true

  -- calculate intensive CPU data
  if (periodicHasPassed(wgt.periodic1) or wgt.cellPercent == 0) then

    --wgt.cellPercent = getCellPercent(wgt, wgt.cellMin) -- use batt percentage by lowest cell voltage
    wgt.cellPercent = getCellPercent(wgt, wgt.cellAvg) -- use batt percentage by average cell voltage

    periodicReset(wgt.periodic1)
  end

end

-- color for battery
-- This function returns green at 100%, red bellow 30% and graduate in between
local function getPercentColor(percent)
  if percent < 30 then
    return lcd.RGB(0xff, 0, 0)
  else
    g = math.floor(0xdf * percent / 100)
    r = 0xdf - g
    return lcd.RGB(r, g, 0)
  end
end

-- color for cell
-- This function returns green at gvalue, red at rvalue and graduate in between
local function getRangeColor(value, green_value, red_value)
  local range = math.abs(green_value - red_value)
  if range == 0 then
    return lcd.RGB(0, 0xdf, 0)
  end
  if value == nil then
    return lcd.RGB(0, 0xdf, 0)
  end

  if green_value > red_value then
    if value > green_value then
      return lcd.RGB(0, 0xdf, 0)
    end
    if value < red_value then
      return lcd.RGB(0xdf, 0, 0)
    end
    g = math.floor(0xdf * (value - red_value) / range)
    r = 0xdf - g
    return lcd.RGB(r, g, 0)
  else
    if value > green_value then
      return lcd.RGB(0, 0xdf, 0)
    end
    if value < red_value then
      return lcd.RGB(0xdf, 0, 0)
    end
    r = math.floor(0xdf * (value - green_value) / range)
    g = 0xdf - r
    return lcd.RGB(r, g, 0)
  end
end

--- Zone size: 70x39 1/8th top bar
local function refreshZoneTiny(wgt)
  local myString = string.format("%2.1fV", wgt.mainValue)
  lcd.drawText(wgt.zone.x + wgt.zone.w - 25, wgt.zone.y + 5, wgt.cellPercent .. "%", RIGHT + SMLSIZE + CUSTOM_COLOR + wgt.no_telem_blink)
  lcd.drawText(wgt.zone.x + wgt.zone.w - 25, wgt.zone.y + 20, myString, RIGHT + SMLSIZE + CUSTOM_COLOR + wgt.no_telem_blink)
  -- draw batt
  lcd.drawRectangle(wgt.zone.x + 50, wgt.zone.y + 9, 16, 25, CUSTOM_COLOR, 2)
  lcd.drawFilledRectangle(wgt.zone.x + 50 + 4, wgt.zone.y + 7, 6, 3, CUSTOM_COLOR)
  local rect_h = math.floor(25 * wgt.cellPercent / 100)
  lcd.drawFilledRectangle(wgt.zone.x + 50, wgt.zone.y + 9 + 25 - rect_h, 16, rect_h, CUSTOM_COLOR + wgt.no_telem_blink)
end

--- Zone size: 160x32 1/8th
local function refreshZoneSmall(wgt)
  local myBatt = { ["x"] = 0, ["y"] = 0, ["w"] = 155, ["h"] = 35, ["segments_w"] = 25, ["color"] = WHITE, ["cath_w"] = 6, ["cath_h"] = 20 }

  -- draws bat
  lcd.setColor(CUSTOM_COLOR, WHITE)
  lcd.drawRectangle(wgt.zone.x + myBatt.x, wgt.zone.y + myBatt.y, myBatt.w, myBatt.h, CUSTOM_COLOR, 2)

  -- fill batt
  lcd.setColor(CUSTOM_COLOR, getPercentColor(wgt.cellPercent))
  lcd.drawGauge(wgt.zone.x + 2, wgt.zone.y + 2, myBatt.w - 4, wgt.zone.h, wgt.cellPercent, 100, CUSTOM_COLOR)

  -- write text
  if wgt.isDataAvailable then
    lcd.setColor(CUSTOM_COLOR, wgt.options.Color)
  else
    lcd.setColor(CUSTOM_COLOR, GREY)
  end
  local topLine = string.format("%2.1fV      %2.0f%%", wgt.mainValue, wgt.cellPercent)
  lcd.drawText(wgt.zone.x + 20, wgt.zone.y + 2, topLine, MIDSIZE + CUSTOM_COLOR + wgt.shadowed + wgt.no_telem_blink)

end

--- Zone size: 180x70 1/4th  (with sliders/trim)
--- Zone size: 225x98 1/4th  (no sliders/trim)
local function refreshZoneMedium(wgt)
  local myBatt = { ["x"] = 0, ["y"] = 0, ["w"] = 85, ["h"] = 35, ["segments_w"] = 15, ["color"] = WHITE, ["cath_w"] = 6, ["cath_h"] = 20 }

  if wgt.isDataAvailable then
    lcd.setColor(CUSTOM_COLOR, wgt.options.Color)
  else
    lcd.setColor(CUSTOM_COLOR, GREY)
  end

  -- draw values
  lcd.drawText(wgt.zone.x + wgt.zone.w, wgt.zone.y, string.format("%2.1fV", wgt.mainValue), DBLSIZE + CUSTOM_COLOR + RIGHT + wgt.shadowed + wgt.no_telem_blink)

  -- more info if 1/4 is high enough (without trim & slider)
  if wgt.zone.h > 80 then
    --lcd.drawText(wgt.zone.x + 50     , wgt.zone.y + 70, string.format("%2.2fV"   , wgt.secondaryValue), SMLSIZE + CUSTOM_COLOR + wgt.no_telem_blink)
    lcd.drawText(wgt.zone.x, wgt.zone.y + 70, string.format("dV %2.2fV", wgt.cellMax - wgt.cellMin), SMLSIZE + CUSTOM_COLOR + wgt.no_telem_blink)
    lcd.drawText(wgt.zone.x, wgt.zone.y + 84, string.format("Min %2.2fV", wgt.cellDataHistoryCellLowest), SMLSIZE + CUSTOM_COLOR + wgt.no_telem_blink)
  end

  -- fill batt
  lcd.setColor(CUSTOM_COLOR, getPercentColor(wgt.cellPercent))
  lcd.drawGauge(wgt.zone.x + myBatt.x, wgt.zone.y + myBatt.y, myBatt.w, myBatt.h, wgt.cellPercent, 100, CUSTOM_COLOR)

  -- draw cells
  local cellH = 19
  local cellX = 118
  local cellW = wgt.zone.w / 3

  local pos = { { x = 0, y = 38 }, { x = cellW, y = 38 }, { x = 2 * cellW, y = 38 }, { x = 0, y = 57 }, { x = cellW, y = 57 }, { x = 2 * cellW, y = 57 } }
  for i = 1, wgt.cellCount, 1 do
    local cellY = wgt.zone.y + (i - 1) * (cellH - 1)

    -- fill current cell
    --lcd.drawFilledRectangle(wgt.zone.x + cellX     , cellY, 58, cellH, CUSTOM_COLOR)
    lcd.setColor(CUSTOM_COLOR, getRangeColor(wgt.cellDataLive[i], wgt.cellMax, wgt.cellMax - 0.2))
    lcd.drawFilledRectangle(wgt.zone.x + pos[i].x, wgt.zone.y + pos[i].y, cellW - 1, 20, CUSTOM_COLOR)
    lcd.setColor(CUSTOM_COLOR, WHITE)
    lcd.drawText(wgt.zone.x + pos[i].x + 10, wgt.zone.y + pos[i].y, string.format("%.2f", wgt.cellDataLive[i]), CUSTOM_COLOR + wgt.shadowed)
    lcd.drawRectangle(wgt.zone.x + pos[i].x, wgt.zone.y + pos[i].y, cellW, 20, CUSTOM_COLOR, 1)
  end

  -- draws bat
  lcd.setColor(CUSTOM_COLOR, WHITE)
  lcd.drawRectangle(wgt.zone.x + myBatt.x, wgt.zone.y + myBatt.y, myBatt.w, myBatt.h, CUSTOM_COLOR, 2)
  lcd.drawFilledRectangle(wgt.zone.x + myBatt.x + myBatt.w, wgt.zone.y + myBatt.h / 2 - myBatt.cath_h / 2, myBatt.cath_w, myBatt.cath_h, CUSTOM_COLOR)
  lcd.drawText(wgt.zone.x + myBatt.x + 20, wgt.zone.y + myBatt.y + 5, string.format("%2.0f%%", wgt.cellPercent), LEFT + MIDSIZE + CUSTOM_COLOR + wgt.shadowed)

end

--- Zone size: 192x152 1/2
local function refreshZoneLarge(wgt)
  local myBatt = { ["x"] = 0, ["y"] = 18, ["w"] = 76, ["h"] = 121, ["segments_h"] = 30, ["color"] = WHITE, ["cath_w"] = 30, ["cath_h"] = 10 }

  if wgt.isDataAvailable then
    lcd.setColor(CUSTOM_COLOR, wgt.options.Color)
  else
    lcd.setColor(CUSTOM_COLOR, GREY)
  end

  lcd.drawText(wgt.zone.x + wgt.zone.w, wgt.zone.y, wgt.cellPercent .. "%", RIGHT + DBLSIZE + CUSTOM_COLOR + wgt.shadowed)
  lcd.drawText(wgt.zone.x + wgt.zone.w, wgt.zone.y + 30, string.format("%2.1fV", wgt.mainValue), RIGHT + DBLSIZE + CUSTOM_COLOR + wgt.shadowed)
  lcd.drawText(wgt.zone.x + wgt.zone.w, wgt.zone.y + 70, string.format("%2.1fV %dS", wgt.secondaryValue, wgt.cellCount), RIGHT + SMLSIZE + CUSTOM_COLOR + wgt.shadowed)
  -- fill batt
  lcd.setColor(CUSTOM_COLOR, getPercentColor(wgt.cellPercent))
  lcd.drawFilledRectangle(wgt.zone.x + myBatt.x, wgt.zone.y + myBatt.y + myBatt.h + myBatt.cath_h - math.floor(wgt.cellPercent / 100 * myBatt.h), myBatt.w, math.floor(wgt.cellPercent / 100 * myBatt.h), CUSTOM_COLOR)
  -- draw cells
  local pos = { { x = 80, y = 90 }, { x = 138, y = 90 }, { x = 80, y = 109 }, { x = 138, y = 109 }, { x = 80, y = 128 }, { x = 138, y = 128 } }
  for i = 1, wgt.cellCount, 1 do
    lcd.setColor(CUSTOM_COLOR, getRangeColor(wgt.cellDataLive[i], wgt.cellMax, wgt.cellMax - 0.2))
    lcd.drawFilledRectangle(wgt.zone.x + pos[i].x, wgt.zone.y + pos[i].y, 58, 20, CUSTOM_COLOR)
    lcd.setColor(CUSTOM_COLOR, WHITE)
    lcd.drawText(wgt.zone.x + pos[i].x + 10, wgt.zone.y + pos[i].y, string.format("%.2f", wgt.cellDataLive[i]), CUSTOM_COLOR + wgt.shadowed)
    lcd.drawRectangle(wgt.zone.x + pos[i].x, wgt.zone.y + pos[i].y, 59, 20, CUSTOM_COLOR, 1)
  end

  -- draw bat
  lcd.setColor(CUSTOM_COLOR, WHITE)
  lcd.drawRectangle(wgt.zone.x + myBatt.x, wgt.zone.y + myBatt.y + myBatt.cath_h, myBatt.w, myBatt.h, CUSTOM_COLOR, 2)
  lcd.drawFilledRectangle(wgt.zone.x + myBatt.x + myBatt.w / 2 - myBatt.cath_w / 2, wgt.zone.y + myBatt.y, myBatt.cath_w, myBatt.cath_h, CUSTOM_COLOR)
  for i = 1, myBatt.h - myBatt.segments_h, myBatt.segments_h do
    lcd.drawRectangle(wgt.zone.x + myBatt.x, wgt.zone.y + myBatt.y + myBatt.cath_h + i, myBatt.w, myBatt.segments_h, CUSTOM_COLOR, 1)
  end

end

--- Zone size: 390x172 1/1
--- Zone size: 460x252 1/1 (no sliders/trim/topbar)
local function refreshZoneXLarge(wgt)
  local myBatt = { ["x"] = 10, ["y"] = 20, ["w"] = 80, ["h"] = 121, ["segments_h"] = 30, ["color"] = WHITE, ["cath_w"] = 30, ["cath_h"] = 10 }

  lcd.setColor(CUSTOM_COLOR, wgt.options.Color)

  -- fill batt
  lcd.setColor(CUSTOM_COLOR, getPercentColor(wgt.cellPercent))
  lcd.drawFilledRectangle(wgt.zone.x + myBatt.x, wgt.zone.y + myBatt.y + myBatt.h + myBatt.cath_h - math.floor(wgt.cellPercent / 100 * myBatt.h), myBatt.w, math.floor(wgt.cellPercent / 100 * myBatt.h), CUSTOM_COLOR)

  -- draw right text section
  if wgt.isDataAvailable then
    lcd.setColor(CUSTOM_COLOR, wgt.options.Color)
  else
    lcd.setColor(CUSTOM_COLOR, GREY)
  end
  lcd.drawText(wgt.zone.x + wgt.zone.w, wgt.zone.y + myBatt.y, wgt.cellPercent .. "%", RIGHT + DBLSIZE + CUSTOM_COLOR + wgt.shadowed + wgt.no_telem_blink)

  lcd.drawText(wgt.zone.x + wgt.zone.w, wgt.zone.y + myBatt.y + 30, string.format("%2.1fV", wgt.mainValue), RIGHT + DBLSIZE + CUSTOM_COLOR + wgt.shadowed + wgt.no_telem_blink)
  lcd.drawText(wgt.zone.x + wgt.zone.w, wgt.zone.y + myBatt.y + 105, string.format("%2.1fV %dS", wgt.secondaryValue, wgt.cellCount), RIGHT + SMLSIZE + CUSTOM_COLOR + wgt.shadowed + wgt.no_telem_blink)

  -- draw cells
  local pos = { { x = 111, y = 38 }, { x = 164, y = 38 }, { x = 217, y = 38 }, { x = 111, y = 57 }, { x = 164, y = 57 }, { x = 217, y = 57 } }
  for i = 1, wgt.cellCount, 1 do
    lcd.setColor(CUSTOM_COLOR, getRangeColor(wgt.cellDataLive[i], wgt.cellMax, wgt.cellMax - 0.2))
    lcd.drawFilledRectangle(wgt.zone.x + pos[i].x, wgt.zone.y + pos[i].y, 53, 20, CUSTOM_COLOR)
    lcd.setColor(CUSTOM_COLOR, WHITE)
    lcd.drawText(wgt.zone.x + pos[i].x + 10, wgt.zone.y + pos[i].y, string.format("%.2f", wgt.cellDataLive[i]), CUSTOM_COLOR + wgt.shadowed + wgt.no_telem_blink)
    lcd.drawRectangle(wgt.zone.x + pos[i].x, wgt.zone.y + pos[i].y, 54, 20, CUSTOM_COLOR, 1)
  end
  -- draw cells for lowest cells
  local pos = { { x = 111, y = 110 }, { x = 164, y = 110 }, { x = 217, y = 110 }, { x = 111, y = 129 }, { x = 164, y = 129 }, { x = 217, y = 129 } }
  for i = 1, wgt.cellCount, 1 do
    lcd.setColor(CUSTOM_COLOR, getRangeColor(wgt.cellDataHistoryLowest[i], wgt.cellDataLive[i], wgt.cellDataLive[i] - 0.3))
    lcd.drawFilledRectangle(wgt.zone.x + pos[i].x, wgt.zone.y + pos[i].y, 53, 20, CUSTOM_COLOR)
    lcd.setColor(CUSTOM_COLOR, WHITE)
    lcd.drawRectangle(wgt.zone.x + pos[i].x, wgt.zone.y + pos[i].y, 54, 20, CUSTOM_COLOR, 1)
    lcd.drawText(wgt.zone.x + pos[i].x + 10, wgt.zone.y + pos[i].y, string.format("%.2f", wgt.cellDataHistoryLowest[i]), CUSTOM_COLOR + wgt.shadowed + wgt.no_telem_blink)
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
  lcd.drawText(wgt.zone.x + 220, wgt.zone.y + 21, "Live data", RIGHT + SMLSIZE + INVERS + CUSTOM_COLOR + wgt.shadowed)
  lcd.drawRectangle(wgt.zone.x + 110, wgt.zone.y + 110, 161, 40, CUSTOM_COLOR, 1)
  lcd.drawText(wgt.zone.x + 230, wgt.zone.y + 93, "Lowest data", RIGHT + SMLSIZE + INVERS + CUSTOM_COLOR + wgt.shadowed)
  return
end

-- This function allow recording of lowest cells when widget is in background
local function background(wgt)
  if (wgt == nil) then
    return
  end

  detectResetEvent(wgt)

  calculateBatteryData(wgt)

end

local function refresh(wgt)
  if (wgt == nil) then
    return
  end
  if type(wgt) ~= "table" then
    return
  end
  if (wgt.options == nil) then
    return
  end
  if (wgt.zone == nil) then
    return
  end
  if (wgt.options.LowestCell == nil) then
    return
  end

  if wgt.options.Shadow == 1 then
    wgt.shadowed = SHADOWED
  else
    wgt.shadowed = 0
  end

  detectResetEvent(wgt)

  calculateBatteryData(wgt)

  if wgt.isDataAvailable then
    wgt.no_telem_blink = 0
  else
    wgt.no_telem_blink = INVERS + BLINK
  end

  if wgt.zone.w > 380 and wgt.zone.h > 165 then
    refreshZoneXLarge(wgt)
  elseif wgt.zone.w > 180 and wgt.zone.h > 145 then
    refreshZoneLarge(wgt)
  elseif wgt.zone.w > 170 and wgt.zone.h > 65 then
    refreshZoneMedium(wgt)
  elseif wgt.zone.w > 150 and wgt.zone.h > 28 then
    refreshZoneSmall(wgt)
  elseif wgt.zone.w > 65 and wgt.zone.h > 35 then
    refreshZoneTiny(wgt)
  end
end

return { name = "BattCheck", options = _options, create = create, update = update, background = background, refresh = refresh }