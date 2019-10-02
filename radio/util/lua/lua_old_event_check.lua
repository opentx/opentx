local toolName = "TNS|LUA event tests|TNE"
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

local virtual_next = 0
local virtual_previous = 0
local virtual_next_rept = 0
local virtual_previous_rept = 0
local virtual_page_next = 0
local virtual_page_previous = 0
local virtual_enter = 0
local virtual_enter_long = 0
local virtual_menu = 0
local virtual_menu_long = 0

-- Init
local function init()

end

-- Main
local function run(event)
  if event == nil then
    error("Cannot be run as a model script!")
    return 2
  elseif event == EVT_VIRTUAL_EXIT then
    return 2
  elseif event == EVT_PLUS_FIRST then
    virtual_next = virtual_next + 1
  elseif event == EVT_MINUS_FIRST then
    virtual_previous = virtual_previous + 1
  elseif event == EVT_PLUS_REPT then
    virtual_next_rept = virtual_next_rept + 1
  elseif event == EVT_MINUS_REPT then
    virtual_previous_rept = virtual_previous_rept + 1
  elseif event == EVT_PAGE_BREAK then
    virtual_page_next = virtual_page_next +1
  elseif event == EVT_PAGE_LONG then
    virtual_page_previous = virtual_page_previous + 1
  elseif event == EVT_ENTER_BREAK  then
    virtual_enter = virtual_enter + 1
  elseif event == EVT_MENU_BREAK  then
    virtual_menu = virtual_menu + 1
  end

  lcd.clear()
  lcd.drawScreenTitle("LUA EVENT TEST", 0, 0)
  lcd.drawText(1, 10, "PLUS: "..virtual_next)
  lcd.drawText(1, 20, "MINUS: "..virtual_previous)
  lcd.drawText(1, 40, "PLUS_REPT: "..virtual_next_rept)
  lcd.drawText(1, 50, "MINUS_REPT: "..virtual_previous_rept)
  lcd.drawText(LCD_W/2, 10, "ENTER: "..virtual_enter)
  lcd.drawText(LCD_W/2, 20, "MENU: "..virtual_menu)
  lcd.drawText(LCD_W/2, 40, "NEXT_PAGE: "..virtual_page_next)
  lcd.drawText(LCD_W/2, 50, "PREVIOUS_PAGE: "..virtual_page_previous)
  return 0
end

return { init=init, run=run }
