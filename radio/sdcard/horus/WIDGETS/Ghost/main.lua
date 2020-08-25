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
local sensors = {}

local options = {
  { "VTX", BOOL, 0 },
}

local function getValues(wgt)
  if wgt.options.VTX == 0 then
    sensors[1] = string.format("%s", getValue("RFMD"))
    sensors[2] = string.format("%d Hz", getValue("FRat"))
    sensors[3] = string.format("%d %%", getValue("RDly"))
    sensors[4] = string.format("%d mW", getValue("TPWR"))
  else
    sensors[1] = string.format("%s", getValue("VBan"))
    sensors[2] = string.format("%d MHz", getValue("VFrq"))
    sensors[3] = string.format("%d", getValue("VChn"))
    sensors[4] = string.format("%d mW", getValue("VPwr"))
  end
end

local function create(zone, options)
  local wgt = { zone=zone, options=options}
  backgroundBitmap = Bitmap.open("/WIDGETS/Ghost/img/background.png")
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
    getValues(wgt)

    -- RF Mode/Band
    lcd.drawText(wgt.zone.x + 75, wgt.zone.y + 2, sensors[1], CENTER + DBLSIZE)

    -- Frame rate / Frequency
    lcd.drawText(wgt.zone.x + 85, wgt.zone.y + 35, sensors[2], CENTER + DBLSIZE)

    -- RSSI / Channel
    lcd.drawText(wgt.zone.x + 85, wgt.zone.y + 70, sensors[3], CENTER + DBLSIZE)

    -- Transmit power
    lcd.drawText(wgt.zone.x + 85, wgt.zone.y + 105, sensors[4], CENTER + DBLSIZE)
  end

end

return { name="Ghost", options=options, create=create, update=update, refresh=refresh, background=background }
