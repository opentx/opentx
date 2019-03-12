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
local VALUE = 0
local COMBO = 1

local edit = false
local page = 1
local current = 1
local pages = {}
local fields = {}

-- load common Bitmaps
local ImgMarkBg = Bitmap.open("img/mark_bg.png")
local BackgroundImg = Bitmap.open("img/background.png")
local ImgPlane = Bitmap.open("img/plane.png")
local ImgPageUp = Bitmap.open("img/pageup.png")
local ImgPageDn = Bitmap.open("img/pagedn.png")


-- Change display attribute to current field
local function addField(step)
  local field = fields[current]
  local min, max
  if field[3] == VALUE then
    min = field[6]
    max = field[7]
  elseif field[3] == COMBO then
    min = 0
    max = #(field[6]) - 1
  end
  if (step < 0 and field[5] > min) or (step > 0 and field[5] < max) then
    field[5] = field[5] + step
  end
end

-- Select the next or previous page
local function selectPage(step)
  page = 1 + ((page + step - 1 + #pages) % #pages)
  edit = false
  current = 1
end

-- Select the next or previous editable field
local function selectField(step)
  repeat
    current = 1 + ((current + step - 1 + #fields) % #fields)
  until fields[current][4]==1
end

-- Redraw the current page
local function redrawFieldsPage(event)

  for index = 1, 10, 1 do
    local field = fields[index]
    if field == nil then
      break
    end

    local attr = current == (index) and ((edit == true and BLINK or 0) + INVERS) or 0
    attr = attr + TEXT_COLOR

    if field[4] == 1 then
      if field[3] == VALUE then
        lcd.drawNumber(field[1], field[2], field[5], LEFT + attr)
      elseif field[3] == COMBO then
        if field[5] >= 0 and field[5] < #(field[6]) then
          lcd.drawText(field[1],field[2], field[6][1+field[5]], attr)
        end
      end
    end
  end
end

local function updateField(field)
  local value = field[5]
end

-- Main
local function runFieldsPage(event)
  if event == EVT_EXIT_BREAK then -- exit script
    return 2
  elseif event == EVT_ENTER_BREAK or event == EVT_ROT_BREAK then -- toggle editing/selecting current field
    if fields[current][5] ~= nil then
      edit = not edit
      if edit == false then
        updateField(fields[current])
      end
    end
  elseif edit then
    if event == EVT_PLUS_FIRST or event == EVT_ROT_RIGHT or event == EVT_PLUS_REPT then
      addField(1)
    elseif event == EVT_MINUS_FIRST or event == EVT_ROT_LEFT or event == EVT_MINUS_REPT then
      addField(-1)
    end
  else
    if event == EVT_MINUS_FIRST or event == EVT_ROT_RIGHT then
      selectField(1)
    elseif event == EVT_PLUS_FIRST or event == EVT_ROT_LEFT then
      selectField(-1)
    end
  end
  redrawFieldsPage(event)
  return 0
end

-- set visibility flags starting with SECOND field of fields
local function setFieldsVisible(...)
  local arg={...}
  local cnt = 2
  for i,v in ipairs(arg) do
    fields[cnt][4] = v
    cnt = cnt + 1
  end
end

-- draws one letter mark
local function drawMark(x, y, name)
  lcd.drawBitmap(ImgMarkBg, x, y)
  lcd.drawText(x+8, y+3, name, TEXT_COLOR)
end


local MotorFields = {
  {50, 50, COMBO, 1, 1, { "No", "Yes"} },
  {50, 127, COMBO, 1, 2, { "CH1", "CH2", "CH3", "CH4", "CH5", "CH6", "CH7", "CH8" } },
}

local ImgEngine

local function runMotorConfig(event)
  lcd.clear()
  if ImgEngine == nil then
   ImgEngine = Bitmap.open("img/prop.png")
  end
  lcd.drawBitmap(BackgroundImg, 0, 0)
  lcd.drawBitmap(ImgPageDn, 455, 95)
  lcd.drawBitmap(ImgEngine, 310, 50)
  lcd.setColor(CUSTOM_COLOR, lcd.RGB(255, 255, 255))
  fields = MotorFields
  lcd.drawText(40, 20, "Does your model have a motor ?", TEXT_COLOR)
  lcd.drawFilledRectangle(40, 45, 200, 30, CUSTOM_COLOR)
  fields[2][4]=0
  if fields[1][5] == 1 then
    lcd.drawText(40, 100, "What channel is it on ?", TEXT_COLOR)
    lcd.drawFilledRectangle(40, 122, 100, 30, CUSTOM_COLOR)
    fields[2][4]=1
  end
  local result = runFieldsPage(event)
  return result
end

-- fields format : {[1]x, [2]y, [3]COMBO, [4]visible, [5]default, [6]{values}}
-- fields format : {[1]x, [2]y, [3]VALUE, [4]visible, [5]default, [6]min, [7]max}
local AilFields = {
  {50, 50, COMBO, 1, 2, { "None", "One, or two with Y cable", "Two"} },
  {50, 127, COMBO, 1, 0, { "CH1", "CH2", "CH3", "CH4", "CH5", "CH6", "CH7", "CH8" } }, -- Ail1 chan
  {50, 167, COMBO, 1, 4, { "CH1", "CH2", "CH3", "CH4", "CH5", "CH6", "CH7", "CH8" } }, -- Ail2 chan
}

local ImgAilR
local ImgAilL

local function runAilConfig(event)
  lcd.clear()
  if ImgAilR == nil then
    ImgAilR = Bitmap.open("img/rail.png")
    ImgAilL = Bitmap.open("img/lail.png")
  end
  lcd.drawBitmap(BackgroundImg, 0, 0)
  lcd.drawBitmap(ImgPageUp, 0, 95)
  lcd.drawBitmap(ImgPageDn, 455, 95)
  lcd.drawBitmap(ImgPlane, 252, 100)
  fields = AilFields
  if fields[1][5] == 1 then
    lcd.drawBitmap(ImgAilR, 324, 123)
    lcd.drawBitmap(ImgAilL, 275, 210)
    drawMark(362, 132, "A")
    drawMark(302, 227, "A")
    lcd.drawFilledRectangle(40, 122, 100, 30, CUSTOM_COLOR)
    drawMark(152, 124, "A")
    setFieldsVisible(1, 0)
  elseif fields[1][5] == 2 then
    lcd.drawBitmap(ImgAilR, 324, 123)
    lcd.drawBitmap(ImgAilL, 275, 210)
    drawMark(362, 132, "A")
    drawMark(302, 227, "B")
    lcd.drawFilledRectangle(40, 122, 100, 30, CUSTOM_COLOR)
    drawMark(152, 124, "A")
    lcd.drawFilledRectangle(40, 162, 100, 30, CUSTOM_COLOR)
    drawMark(152, 164, "B")
    setFieldsVisible(1, 1)
  else
    setFieldsVisible(0, 0)
  end
  lcd.drawText(40, 20, "Number of ailerons on your model ?", TEXT_COLOR)
  lcd.drawFilledRectangle(40, 45, 400, 30, CUSTOM_COLOR)
  local result = runFieldsPage(event)
  return result
end

local FlapsFields = {
  {50, 50, COMBO, 1, 0, { "No", "Yes, on one channel", "Yes, on two channels"} },
  {50, 127, COMBO, 1, 6, { "CH1", "CH2", "CH3", "CH4", "CH5", "CH6", "CH7", "CH8" } },
  {50, 167, COMBO, 1, 7, { "CH1", "CH2", "CH3", "CH4", "CH5", "CH6", "CH7", "CH8" } },
}

local ImgFlp

local function runFlapsConfig(event)
  lcd.clear()
  if ImgFlp == nil then
    ImgFlp = Bitmap.open("img/flap.png")
  end
  lcd.drawBitmap(BackgroundImg, 0, 0)
  lcd.drawBitmap(ImgPageUp, 0, 95)
  lcd.drawBitmap(ImgPageDn, 455, 95)
  lcd.drawBitmap(ImgPlane, 252, 100)
  fields = FlapsFields
  if fields[1][5] == 1 then
    lcd.drawBitmap(ImgFlp, 315, 143)
    lcd.drawBitmap(ImgFlp, 285, 191)
    drawMark(362, 132, "A")
    drawMark(302, 227, "A")
    lcd.drawFilledRectangle(40, 122, 100, 30, CUSTOM_COLOR)
    drawMark(152, 124, "A")
    setFieldsVisible(1, 0)
  elseif fields[1][5] == 2 then
    lcd.drawBitmap(ImgFlp, 315, 143)
    lcd.drawBitmap(ImgFlp, 285, 191)
    drawMark(362, 132, "A")
    drawMark(302, 227, "B")
    lcd.drawFilledRectangle(40, 122, 100, 30, CUSTOM_COLOR)
    drawMark(152, 124, "A")
    lcd.drawFilledRectangle(40, 162, 100, 30, CUSTOM_COLOR)
    drawMark(152, 164, "B")
    setFieldsVisible(1, 1)
  else
    setFieldsVisible(0, 0)
  end
  lcd.drawText(40, 20, "Does your model have flaps ?", TEXT_COLOR)
  lcd.drawFilledRectangle(40, 45, 400, 30, CUSTOM_COLOR)
  local result = runFieldsPage(event)
  return result
end

local TailFields = {
  {50, 50, COMBO, 1, 1, { "1 channel for Elevator, no Rudder", "One chan for Elevator, one for Rudder", "Two chans for Elevator, one for Rudder", "V Tail"} },
  {50, 127, COMBO, 1, 1, { "CH1", "CH2", "CH3", "CH4", "CH5", "CH6", "CH7", "CH8" } }, --ele
  {50, 167, COMBO, 1, 3, { "CH1", "CH2", "CH3", "CH4", "CH5", "CH6", "CH7", "CH8" } }, --rud
  {50, 207, COMBO, 0, 5, { "CH1", "CH2", "CH3", "CH4", "CH5", "CH6", "CH7", "CH8" } }, --ele2
}

local ImgTail
local ImgVTail
local ImgTailRud

local function runTailConfig(event)
  lcd.clear()
  if ImgTail == nil then
    ImgTail = Bitmap.open("img/tail.png")
    ImgVTail = Bitmap.open("img/vtail.png")
    ImgTailRud = Bitmap.open("img/tailrud.png")
  end
  lcd.drawBitmap(BackgroundImg, 0, 0)
  lcd.drawBitmap(ImgPageUp, 0, 95)
  lcd.drawBitmap(ImgPageDn, 455, 95)
  fields = TailFields
  if fields[1][5] == 0 then
    lcd.drawBitmap(ImgTail, 252, 100)
    drawMark(275, 120, "A")
    drawMark(390, 155, "A")
    lcd.drawFilledRectangle(40, 122, 100, 30, CUSTOM_COLOR)
    drawMark(152, 124, "A")
    setFieldsVisible(1, 0, 0)
  end
  if fields[1][5] == 1 then
    lcd.drawBitmap(ImgTail, 252, 100)
    lcd.drawBitmap(ImgTailRud, 340, 100)
    drawMark(275, 120, "A")
    drawMark(390, 155, "A")
    drawMark(370, 112, "B")
    lcd.drawFilledRectangle(40, 122, 100, 30, CUSTOM_COLOR)
    drawMark(152, 124, "A")
    lcd.drawFilledRectangle(40, 162, 100, 30, CUSTOM_COLOR)
    drawMark(152, 164, "B")
    setFieldsVisible(1, 1, 0)
  end
  if fields[1][5] == 2 then
    lcd.drawBitmap(ImgTail, 252, 100)
    lcd.drawBitmap(ImgTailRud, 340, 100)
    drawMark(275, 120, "A")
    drawMark(390, 155, "C")
    drawMark(370, 112, "B")
    lcd.drawFilledRectangle(40, 122, 100, 30, CUSTOM_COLOR)
    drawMark(152, 124, "A")
    lcd.drawFilledRectangle(40, 162, 100, 30, CUSTOM_COLOR)
    drawMark(152, 164, "B")
    lcd.drawFilledRectangle(40, 202, 100, 30, CUSTOM_COLOR)
    drawMark(152, 204, "C")
    setFieldsVisible(1, 1, 1)
  end
  if fields[1][5] == 3 then
    lcd.drawBitmap(ImgVTail, 252, 100)
    drawMark(315, 110, "A")
    drawMark(382, 120, "B")
    lcd.drawFilledRectangle(40, 122, 100, 30, CUSTOM_COLOR)
    drawMark(152, 124, "A")
    lcd.drawFilledRectangle(40, 162, 100, 30, CUSTOM_COLOR)
    drawMark(152, 164, "B")
    setFieldsVisible(1, 1, 0)
  end
  lcd.drawText(40, 20, "Pick the tail config of your model", TEXT_COLOR)
  lcd.drawFilledRectangle(40, 45, 400, 30, CUSTOM_COLOR)
  local result = runFieldsPage(event)
  return result
end

local lineIndex
local function drawNextLine(text, text2)
  lcd.drawText(40, lineIndex, text, TEXT_COLOR)
  lcd.drawText(250, lineIndex, text2 + 1, TEXT_COLOR)
  lineIndex = lineIndex + 20
end

local ConfigSummaryFields = {
  {110, 250, COMBO, 1, 0, { "No, I need to change something", "Yes, all is well, create the plane !"} },
}

local ImgSummary

local function runConfigSummary(event)
  lcd.clear()
  if ImgSummary == nil then
    ImgSummary = Bitmap.open("img/summary.png")
  end
  fields = ConfigSummaryFields
  lcd.drawBitmap(BackgroundImg, 0, 0)
  lcd.drawBitmap(ImgPageUp, 0, 95)
  lcd.drawBitmap(ImgSummary, 300, 60)
  lineIndex = 40
  -- motors
  if(MotorFields[1][5] == 1) then
    drawNextLine("Motor chan :", MotorFields[2][5])
  elseif (MotorFields[2][5] == 2) then
    drawNextLine("Motor 1 chan :", MotorFields[2][5])
    drawNextLine("Motor 2 chan :", MotorFields[3][5])
  end
  -- ail
  if(AilFields[1][5] == 1) then
    drawNextLine("Aileron chan :",AilFields[2][5])
  elseif (AilFields[1][5] == 2) then
    drawNextLine("Aileron 1 chan :",AilFields[2][5])
    drawNextLine("Aileron 2 chan :",AilFields[3][5])
  end
  -- flaps
  if(FlapsFields[1][5] == 1) then
    drawNextLine("Flaps chan :",FlapsFields[2][5])
  elseif (FlapsFields[1][5] == 2) then
    drawNextLine("Flaps 1 chan :",FlapsFields[2][5])
    drawNextLine("Flaps 2 chan :",FlapsFields[3][5])
  end
  -- tail
  if(TailFields[1][5] == 0) then
    drawNextLine("Elevator chan :",TailFields[2][5])
  elseif (TailFields[1][5] == 1) then
    drawNextLine("Elevator chan :",TailFields[2][5])
    drawNextLine("Rudder chan :",TailFields[3][5])
  elseif (TailFields[1][5] == 2) then
    drawNextLine("Elevator 1 chan :",TailFields[2][5])
    drawNextLine("Rudder chan :",TailFields[3][5])
    drawNextLine("Elevator 2 chan :",TailFields[4][5])
  elseif (TailFields[1][5] == 3) then
    drawNextLine("V-Tail elevator :", TailFields[2][5])
    drawNextLine("V-Tail rudder :", TailFields[3][5])
  end
  local result = runFieldsPage(event)
  if(fields[1][5] == 1 and edit == false) then
    selectPage(1)
  end
  return result
end

local function addMix(channel, input, name, weight, index)
  local mix = { source=input, name=name }
  if weight ~= nil then
    mix.weight = weight
  end
  if index == nil then
    index = 0
  end
  model.insertMix(channel, index, mix)
end

local function createModel(event)
  lcd.clear()
  lcd.drawBitmap(BackgroundImg, 0, 0)
  lcd.drawBitmap(ImgSummary, 300, 60)
  model.defaultInputs()
  model.deleteMixes()
  -- motor
  if(MotorFields[1][5] == 1) then
    addMix(MotorFields[2][5], MIXSRC_FIRST_INPUT+defaultChannel(2), "Motor")
  elseif (MotorFields[2][5] == 2) then
    addMix(MotorFields[2][5], MIXSRC_FIRST_INPUT+defaultChannel(2), "Motor1")
    addMix(MotorFields[3][5], MIXSRC_FIRST_INPUT+defaultChannel(2), "Motor2")
  end
  -- Ailerons
  if(AilFields[1][5] == 1) then
    addMix(AilFields[2][5], MIXSRC_FIRST_INPUT+defaultChannel(3), "Ail")
  elseif (AilFields[1][5] == 2) then
    addMix(AilFields[2][5], MIXSRC_FIRST_INPUT+defaultChannel(3), "AilL")
    addMix(AilFields[3][5], MIXSRC_FIRST_INPUT+defaultChannel(3), "AilR", -100)
  end
  -- Flaps
  if(FlapsFields[1][5] == 1) then
    addMix(FlapsFields[2][5], MIXSRC_SA, "Flaps")
  elseif (FlapsFields[1][5] == 2) then
    addMix(FlapsFields[2][5], MIXSRC_SA, "FlapsL")
    addMix(FlapsFields[3][5], MIXSRC_SA, "FlapsR")
  end
  -- Tail
  if(TailFields[1][5] == 0) then
    addMix(TailFields[2][5], MIXSRC_FIRST_INPUT+defaultChannel(1), "Elev")
  elseif (TailFields[1][5] == 1) then
    addMix(TailFields[2][5], MIXSRC_FIRST_INPUT+defaultChannel(1), "Elev")
    addMix(TailFields[3][5], MIXSRC_FIRST_INPUT+defaultChannel(0), "Rudder")
  elseif (TailFields[1][5] == 2) then
    addMix(TailFields[2][5], MIXSRC_FIRST_INPUT+defaultChannel(1), "ElevL")
    addMix(TailFields[3][5], MIXSRC_FIRST_INPUT+defaultChannel(0), "Rudder")
    addMix(TailFields[4][5], MIXSRC_FIRST_INPUT+defaultChannel(1), "ElevR")
  elseif (TailFields[1][5] == 3) then
    addMix(TailFields[2][5], MIXSRC_FIRST_INPUT+defaultChannel(1), "V-EleL", 50)
    addMix(TailFields[2][5], MIXSRC_FIRST_INPUT+defaultChannel(0), "V-RudL", 50, 1)
    addMix(TailFields[3][5], MIXSRC_FIRST_INPUT+defaultChannel(1), "V-EleR", 50)
    addMix(TailFields[3][5], MIXSRC_FIRST_INPUT+defaultChannel(0), "V-RudR", -50, 1)
  end
  lcd.drawText(70, 90, "Model successfully created !", TEXT_COLOR)
  lcd.drawText(100, 130, "Press RTN to exit", TEXT_COLOR)
  return 2
end

-- Init
local function init()
  current, edit = 1, false
  pages = {
    runMotorConfig,
    runAilConfig,
    runFlapsConfig,
    runTailConfig,
    runConfigSummary,
    createModel,
  }
end

-- Main
local function run(event)
  if event == nil then
    error("Cannot be run as a model script!")
    return 2
  elseif (event == EVT_PAGE_BREAK or event == EVT_PAGEDN_FIRST) and page < #pages-1 then
    selectPage(1)
  elseif (event == EVT_PAGE_LONG or event == EVT_PAGEUP_FIRST) and page > 1 then
    killEvents(event);
    selectPage(-1)
  end

  local result = pages[page](event)
  return result
end

return { init=init, run=run }
