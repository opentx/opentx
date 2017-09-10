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
local options = {
  { "Option1", SOURCE, 1 },
  { "Option2", VALUE, 1000 },
  { "Option3", COLOR, RED },
  { "Shadow", BOOL, 0 }
}

local function create(zone, options)
  local pie = { zone=zone, options=options, counter=0 }
  print(options.Option2)
  return pie
end

local function update(pie, options)
  pie.options = options
end

local function background(pie)
  pie.counter = pie.counter + 1
end

function refresh(pie)
  pie.counter = pie.counter + 1
  if pie.options.Shadow == 1 then
    lcd.drawNumber(pie.zone.x, pie.zone.y, pie.counter, LEFT + DBLSIZE + TEXT_COLOR);
  else
    lcd.drawNumber(pie.zone.x, pie.zone.y, pie.counter, LEFT + DBLSIZE + TEXT_COLOR + SHADOWED);
  end
end

return { name="Counter", options=options, create=create, update=update, refresh=refresh, background=background }
