
-- Wizard states
local TEMPLATE_CHOICE_INIT = 0
local TEMPLATE_CHOICE_RUN = 1
local TEMPLATE_NEXT = 2

local state = TEMPLATE_CHOICE_INIT
local choice = 0

local function choiceSurround(index)
  lcd.drawRectangle(12+47*index, 13, 48, 48)
  lcd.drawPixmap(17+47*index, 8, "/TEMPLATES/mark.bmp")
end

local function drawTemplateChoice()
  lcd.clear()
  lcd.drawScreenTitle("", 0, 0)
  -- lcd.drawText(58, 13, "Select model type", 0)
  lcd.drawPixmap( 16, 17, "/TEMPLATES/plane.bmp")
  lcd.drawPixmap( 63, 17, "/TEMPLATES/heli.bmp")
  lcd.drawPixmap(110, 17, "/TEMPLATES/delta.bmp")
  lcd.drawPixmap(157, 17, "/TEMPLATES/quadri.bmp")
  choiceSurround(choice)
end

local function templateChoiceRun(event)
  nextchoice = choice
  if event == EVT_ENTER_BREAK then
    state = 2
  elseif event == EVT_PLUS_BREAK then
    nextchoice = (nextchoice + 1)
  elseif event == EVT_MINUS_BREAK then
    nextchoice = (nextchoice + 3)
  end
  nextchoice = nextchoice % 4
  if nextchoice ~= choice then
    choice = nextchoice
    drawTemplateChoice()
  end
end

local function drawWing(x, y)
  lcd.drawLine(x+1, y+1, x+54, y)
  lcd.drawLine(x+1, y, x+1, y+16)
  lcd.drawLine(x, y+17, x+54, y+27)
  lcd.drawLine(x+1, y+11, x+33, y+17)
  lcd.drawLine(x+34, y+16, x+34, y+22)
  MIRROR = 128
  lcd.drawRectangle(x+55, y-8, 15, 39)
  lcd.drawLine(x+MIRROR-5, y+1, x+MIRROR-58, y)
  lcd.drawLine(x+MIRROR-5, y, x+MIRROR-5, y+16)
  lcd.drawLine(x+MIRROR-4, y+17, x+MIRROR-58, y+27)
  lcd.drawLine(x+MIRROR-5, y+11, x+MIRROR-37, y+17)
  lcd.drawLine(x+MIRROR-38, y+16, x+MIRROR-38, y+22)
end


local function run(event)
  if event == EVT_EXIT_BREAK then
    return 2
  end
  lcd.lock()
  if state == TEMPLATE_CHOICE_INIT then
    drawTemplateChoice()
    state = TEMPLATE_CHOICE_RUN
  elseif state == TEMPLATE_CHOICE_RUN then
    templateChoiceRun(event)
  elseif state == 2 then
    state = 3
    lcd.clear()
    lcd.drawText(68, 13, "Wing config", 0)
    lcd.drawScreenTitle("MODEL SETUP WIZARD", 2, 3)
    drawWing(2, 33)
    lcd.drawText(140, 30, "Ail", 0)
    lcd.drawText(175, 30, "50%", INVERS)
    lcd.drawText(140, 42, "Flp", 0)
    lcd.drawText(175, 42, "30%", 0)
  end
  return 0
end

return { run=run }
