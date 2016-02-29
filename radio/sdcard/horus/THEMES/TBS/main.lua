local function load()
  lcd.setColor(TEXT_COLOR_INDEX, WHITE)
  lcd.setColor(TEXT_BGCOLOR_INDEX, BLACK)
  lcd.setColor(TEXT_INVERTED_COLOR_INDEX, BLACK)
  lcd.setColor(TEXT_INVERTED_BGCOLOR_INDEX, YELLOW)
  lcd.setColor(LINE_COLOR_INDEX, GREY)
  lcd.setColor(SCROLLBOX_COLOR_INDEX, BLUE)
  lcd.setColor(MENU_TITLE_BGCOLOR_INDEX, DARKGREY)
  lcd.setColor(MENU_TITLE_COLOR_INDEX, WHITE)
  lcd.setColor(MENU_TITLE_DISABLE_COLOR_INDEX, BLACK)
  lcd.setColor(HEADER_COLOR_INDEX, lcd.RGB(32, 34, 42))
  lcd.setColor(ALARM_COLOR_INDEX, lcd.RGB(32, 34, 42))
  lcd.setColor(WARNING_COLOR_INDEX, YELLOW)
  lcd.setColor(TEXT_DISABLE_COLOR_INDEX, GREY)
  lcd.setColor(CURVE_AXIS_COLOR_INDEX, LIGHTGREY)
  lcd.setColor(CURVE_COLOR_INDEX, lcd.RGB(32, 34, 42))
  lcd.setColor(CURVE_CURSOR_COLOR_INDEX, lcd.RGB(32, 34, 42))
  lcd.setColor(TITLE_BGCOLOR_INDEX, lcd.RGB(32, 34, 42))
  lcd.setColor(TRIM_BGCOLOR_INDEX, YELLOW)
  lcd.setColor(TRIM_SHADOW_COLOR_INDEX, lcd.RGB(100, 100, 100))
  lcd.setColor(MAINVIEW_PANES_COLOR_INDEX, GREY)
  lcd.setColor(MAINVIEW_GRAPHICS_COLOR_INDEX, WHITE)
  lcd.setColor(HEADER_BGCOLOR_INDEX, BLACK)
  lcd.setColor(HEADER_ICON_BGCOLOR_INDEX, BLACK)
  lcd.setColor(HEADER_CURRENT_BGCOLOR_INDEX, lcd.RGB(10, 78, 121))
  lcd.setColor(OVERLAY_COLOR_INDEX, BLACK)
  return 0
end

local function drawBackground()
  lcd.drawFilledRectangle(0, 0, LCD_W, LCD_H, TEXT_BGCOLOR);
end

local function drawTopbarBackground(icon)
  lcd.drawFilledRectangle(0, 0, LCD_W, MENU_HEADER_HEIGHT, HEADER_BGCOLOR);
  lcd.drawLine(0, MENU_HEADER_HEIGHT, LCD_W, MENU_HEADER_HEIGHT, SOLID, TEXT_COLOR)

  --if (icon) then
    --lcd.drawBitmapPattern(5, 7, icon, MENU_TITLE_COLOR);
  --else
    --lcd.drawBitmap(5, 7, LBM_TOPMENU_BMP_OPENTX);
  --end

  --lcd.drawTopbarDatetime();
end

return { name="TBS", load=load, drawBackground=drawBackground, drawTopbarBackground=drawTopbarBackground }
