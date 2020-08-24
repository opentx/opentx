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

local backgroundBitmap

local options = {
  { "RFMD", SOURCE, 0 },
  { "FRat", SOURCE, 0 },
  { "TPWR", SOURCE, 0 },
  -- rssi is taken directly
}

local function create(zone, options)
  local wgt = { zone=zone, options=options}

  backgroundBitmap = Bitmap.open("/WIDGETS/Ghost/img/background.png")

  if wgt.options.RFMD == 0 then
    wgt.options.Sensor = "RFMD"
  end
  if wgt.options.FRat == 0 then
    wgt.options.Sensor = "FRat"
  end
  if wgt.options.TPWR == 0 then
    wgt.options.Sensor = "TPWR"
  end

  return wgt
end

local function update(wgt, options)
  wgt.options = options
end

local function background(wgt)
end

function refresh(wgt)
  -- runs onty on large enough zone
  if wgt.zone.w < 180 or wgt.zone.h < 145 then
    return
  end

  if backgroundBitmap ~= nil then
    lcd.drawBitmap(backgroundBitmap, wgt.zone.x, wgt.zone.y)
  end

  if getRSSI() ~= 0 then
    -- Ghost mode
    value = string.format("%s", getValue(wgt.options.RFMD))
    lcd.drawText(wgt.zone.x + 75, wgt.zone.y + 2, value, CENTER + DBLSIZE)

    -- Frame rate
    value = string.format("%d Hz", getValue(wgt.options.FRat))
    lcd.drawText(wgt.zone.x + 85, wgt.zone.y + 35, value, CENTER + DBLSIZE)

    -- RSSI
    lcd.drawText(wgt.zone.x + 85, wgt.zone.y + 70, getRSSI() .." %", CENTER + DBLSIZE)

    -- Transmit power
    value = string.format("%d mW", getValue(wgt.options.TPWR))
    lcd.drawText(wgt.zone.x + 85, wgt.zone.y + 105, value, CENTER + DBLSIZE)
  end

end

return { name="Ghost", options=options, create=create, update=update, refresh=refresh, background=background }
