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


--###############################################################################
-- Multi buffer for HoTT description
-- To start operation:
--   Write "HoTT" at address 0..3
--   Write 0xFF at address 4 will request the buffers to be cleared
--   Write 0xDF at address 199
-- Read buffer at address [0123] will return the string "HoTT"
-- Read buffer from address 24 access the RX text for 162 bytes, 21 caracters
--    by 7 lines
-- Write at address 199 sends an order to the RX: 0xDF=start, 0xD7=prev page,
--    0xDE=next page, 0xD9=enter, 0xDD=next or 0xDB=prev
-- Write at address 4 the value 0xFF will request the buffer to be cleared
-- !! Before exiting the script must write 0 at address 0 for normal operation !!
--###############################################################################

local function HoTT_Release()
  multiBuffer( 0, 0 )
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
      value=multiBuffer( line*21+24+i )
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
  --Set protocol to talk to
  multiBuffer( 0, string.byte('H') )
  multiBuffer( 1, string.byte('o') )
  multiBuffer( 2, string.byte('T') )
  multiBuffer( 3, string.byte('T') )
  --Request init of the RX buffer
  multiBuffer( 4, 0xFF )
  --Request RX to send the config menu
  multiBuffer( 199, 0xDF )
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
      multiBuffer( 199, 0xD7 )
    elseif event == EVT_VIRTUAL_NEXT_PAGE then
      multiBuffer( 199, 0xDE )
    elseif event == EVT_VIRTUAL_ENTER then
      multiBuffer( 199, 0xD9 )
    elseif event == EVT_VIRTUAL_NEXT then
      multiBuffer( 199, 0xDD )
    elseif event == EVT_VIRTUAL_PREV then
      multiBuffer( 199, 0xDB )
    end
    HoTT_Draw_LCD()
	return 0
  end
end

return { init=HoTT_Init, run=HoTT_Run }
