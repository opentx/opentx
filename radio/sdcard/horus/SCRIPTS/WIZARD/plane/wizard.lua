

-- Init
local function init()

end

-- Main
local function run(event)
  lcd.clear()
  lcd.drawFilledRectangle(0, 0, LCD_W, LCD_H, TEXT_BGCOLOR)
  lcd.drawText(50, 50, "You are running dummy plane wizard", TEXT_COLOR)

  return 1
end

return { init=init, run=run }
