
--
-- Copyright (C) OpenTX
--
-- Based on code named
--   th9x - http://code.google.com/p/th9x
--   er9x - http://code.google.com/p/er9x
--   gruvin9x - http://code.google.com/p/gruvin9x
--
-- License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
--
-- This program is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Public License version 2 as
-- published by the Free Software Foundation.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--

-- LUA Mix Script

local outputs = { "RFS" }

local function bit(p)
  return 2 ^ (p - 1)  -- 1-based indexing
end

-- Typical call:  if hasbit(x, bit(3)) then ...
local function hasbit(x, p)
  return ((x % (p + p) >= p) and 1 or 0)
end

--  "Rx1 Ovl", "Rx2 Ovl", "SBUS Ovl", "Rx1 FS", "Rx1 LF", "Rx2 FS", "Rx2 LF", "Rx1 Lost", "Rx2 Lost", "Rx1 NS", "Rx2 NS"

local function run()
  local status = getValue("RBS")

  --  UNCOMMENT DESIRED SENSOR (many are defaulted commented to prevent hitting to fast the 32 sensor limit)
  -- setTelemetryValue(0xb20, 1, 2, hasbit(status, bit(1)), 0, 0, "R1OL")
  -- setTelemetryValue(0xb20, 2, 2, hasbit(status, bit(2)), 0, 0, "R2OL")
  -- setTelemetryValue(0xb20, 3, 2, hasbit(status, bit(3)), 0, 0, "SBOL")
  setTelemetryValue(0xb20, 1, 4, hasbit(status, bit(4)), 0, 0, "R1FS")
  setTelemetryValue(0xb20, 2, 5, hasbit(status, bit(6)), 0, 0, "R2FS")
  -- setTelemetryValue(0xb20, 6, 2, hasbit(status, bit(5)), 0, 0, "R1FL")
  -- setTelemetryValue(0xb20, 7, 2, hasbit(status, bit(7)), 0, 0, "R2FL")
  -- setTelemetryValue(0xb20, 8, 2, hasbit(status, bit(8)), 0, 0, "RX1L")
  -- setTelemetryValue(0xb20, 9, 2, hasbit(status, bit(9)), 0, 0, "RX2L")
  -- setTelemetryValue(0xb20, 10, 2, hasbit(status, bit(10)), 0, 0, "R1NS")
  -- setTelemetryValue(0xb20, 11, 2, hasbit(status, bit(11)), 0, 0, "R2NS")
  if status ~= 0 then
    return 10
  else
    return  0
  end
end

return { run=run, output=outputs }
