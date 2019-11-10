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
local function HoTT_Release()
  hottBuffer( 0, 1 )
end

local function HoTT_Draw_LCD()
  local i
  local value
  local line
  local result
  local offset=0
  
  lcd.clear()

  --Draw title
  if LCD_W == 480 then
      lcd.drawFilledRectangle(0, 0, LCD_W, 30, TITLE_BGCOLOR)
      lcd.drawText(1, 5, "Graupner HoTT RX configuration", MENU_TITLE_COLOR)
      offset=32
  end
  
  --Draw RX Menu
  for line = 0, 6, 1 do
    for i = 0, 21-1, 1 do
      value=hottBuffer( line*21+24+i )
      if value > 0x80 then
        value = value - 0x80
        lcd.drawText(10+i*16,offset+16*line,string.char(value).."   ",INVERS)
      else
        lcd.drawText(10+i*16,offset+16*line,string.char(value))
      end
    end
  end
end

-- Init
local function HoTT_Init()
  --Init RX buffer
  hottBuffer( 4, 0xFF )

  --Check if the HoTT protocol is running
  local result = ''
  for i = 0, 3, 1 do
    result = result .. string.char(hottBuffer( i ))
  end
  if result ~= "HoTT" then
    error("Protocol HoTT not running!")
    return 2
  end
  
  --Request RX to send the menu
  hottBuffer( 199, 0xDF )
end

-- Main
local function HoTT_Run(event)
  if event == nil then
    error("Cannot be run as a model script!")
    return 2
  elseif event == EVT_VIRTUAL_EXIT then
    HoTT_Release()
    return 2
  else
    if event == EVT_VIRTUAL_PREV_PAGE then
      killEvents(event);
      hottBuffer( 199, 0xD7 )
    elseif event == EVT_VIRTUAL_NEXT_PAGE then
      hottBuffer( 199, 0xDE )
    elseif event == EVT_VIRTUAL_ENTER then
      hottBuffer( 199, 0xD9 )
    elseif event == EVT_VIRTUAL_NEXT then
      hottBuffer( 199, 0xDD )
    elseif event == EVT_VIRTUAL_PREV then
      hottBuffer( 199, 0xDB )
    end
    HoTT_Draw_LCD()
	return 0
  end
end

return { init=HoTT_Init, run=HoTT_Run }
