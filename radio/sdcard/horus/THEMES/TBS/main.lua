local bitmap = 0

local function load()
  lcd.setColor(TEXT_COLOR, WHITE)
  lcd.setColor(TEXT_BGCOLOR, BLACK)
  lcd.setColor(TEXT_INVERTED_COLOR, BLACK)
  lcd.setColor(TEXT_INVERTED_BGCOLOR, YELLOW)
  lcd.setColor(LINE_COLOR, GREY)
  lcd.setColor(SCROLLBOX_COLOR, BLUE)
  lcd.setColor(MENU_TITLE_BGCOLOR, DARKGREY)
  lcd.setColor(MENU_TITLE_COLOR, WHITE)
  lcd.setColor(MENU_TITLE_DISABLE_COLOR, BLACK)
  lcd.setColor(HEADER_COLOR, lcd.RGB(32, 34, 42))
  lcd.setColor(ALARM_COLOR, lcd.RGB(32, 34, 42))
  lcd.setColor(WARNING_COLOR, YELLOW)
  lcd.setColor(TEXT_DISABLE_COLOR, GREY)
  lcd.setColor(CURVE_AXIS_COLOR, LIGHTGREY)
  lcd.setColor(CURVE_COLOR, lcd.RGB(32, 34, 42))
  lcd.setColor(CURVE_CURSOR_COLOR, lcd.RGB(32, 34, 42))
  lcd.setColor(TITLE_BGCOLOR, lcd.RGB(32, 34, 42))
  lcd.setColor(TRIM_BGCOLOR, YELLOW)
  lcd.setColor(TRIM_SHADOW_COLOR, lcd.RGB(100, 100, 100))
  lcd.setColor(MAINVIEW_PANES_COLOR, GREY)
  lcd.setColor(MAINVIEW_GRAPHICS_COLOR, WHITE)
  lcd.setColor(HEADER_BGCOLOR, BLACK)
  lcd.setColor(HEADER_ICON_BGCOLOR, BLACK)
  lcd.setColor(HEADER_CURRENT_BGCOLOR, lcd.RGB(10, 78, 121))
  lcd.setColor(OVERLAY_COLOR, BLACK)
  bitmap = Bitmap.open("THEMES/TBS/thumb.bmp")
  return 0
end

local function drawBackground()
  lcd.drawFilledRectangle(0, 0, LCD_W, LCD_H, TEXT_BGCOLOR);

  lcd.drawBitmap(bitmap, 50, 50)
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
