---- #########################################################################
---- #                                                                       #
---- # Copyright (C) OpenTX                                                  #
-----#                                                                       #
-----# Credits: graphics by https://github.com/jrwieland                     #
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
local switches = {"SA", "SB", "SC", "SD", "SE", "SF", "SG"}
local switchValues = {[0]=2, 5, 8, 11, 14, 17, 19}
local Text_Color= lcd.setColor(TEXT_COLOR, BLACK)
-- load common Bitmaps
local BackgroundImg = Bitmap.open("img/background.png")
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
    attr = attr

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
  if event == EVT_VIRTUAL_EXIT then -- exit script
    return 2
  elseif event == EVT_VIRTUAL_ENTER then -- toggle editing/selecting current field
    if fields[current][5] ~= nil then
      edit = not edit
      if edit == false then
        lcd.clear()
        updateField(fields[current])
      end
    end
  elseif edit then
    if event == EVT_VIRTUAL_INC or event == EVT_VIRTUAL_INC_REPT then
      addField(1)
    elseif event == EVT_VIRTUAL_DEC or event == EVT_VIRTUAL_DEC_REPT then
      addField(-1)
    end
  else
    if event == EVT_VIRTUAL_NEXT then
      selectField(1)
    elseif event == EVT_VIRTUAL_PREV then
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

local TypeFields = {
  {50, 50, COMBO, 1, 0, {"FBL", "FB" } },
  {43, 126, COMBO, 1, 0, {"120", "120X", "140", "90" } },
}

local TypeBackground

local function runTypeConfig(event)
  lcd.clear()
  if TypeBackground == nil then
    TypeBackground = Bitmap.open("img/type.png")
  end
  fields = TypeFields
  lcd.drawBitmap(TypeBackground, 0, 0)
  lcd.drawBitmap(ImgPageDn, 455, 95)
  lcd.drawText(40, 20, "What Type of Helicopter ?")
  lcd.drawFilledRectangle(40, 45, 200, 30, TEXT_BGCOLOR)
  fields[2][4] = 0
  if fields[1][5] == 1 then
    lcd.drawText(30, 100, "Specify your Swash Type")
    lcd.drawFilledRectangle(40, 122, 100, 30, TEXT_BGCOLOR)
    fields[2][4] = 1
  end
  local result = runFieldsPage(event)
  return result
end

local StyleFields = {
  {50, 50, COMBO, 1, 0, { "Sport", "Light 3D","Full 3D" } },
}

local StyleBackground

local function runStyleConfig(event)
  lcd.clear()
  if StyleBackground == nil then
    StyleBackground = Bitmap.open("img/style.png")
  end
  lcd.drawBitmap(StyleBackground, 0, 0)
  lcd.drawBitmap(ImgPageUp, 0, 95)
  lcd.drawBitmap(ImgPageDn, 455, 95)
  fields = StyleFields
  lcd.drawText(40, 20, "Your Flying Style")
  lcd.drawFilledRectangle(40, 45, 100, 30, TEXT_BGCOLOR)
  fields[1][4]=1
  local result = runFieldsPage(event)
  return result
end

local SwitchFields = {
  {50, 50, COMBO, 1, 1, { "SA", "SB", "SC", "SD", "SE", "SF" } },
  {50, 110, COMBO, 1, 5, { "SA", "SB", "SC", "SD", "SE", "SF","SG" } },
  {50, 190, COMBO, 1, 0, { "SA", "SB", "SC", "SD", "SE", "SF","SG" } },
}

local SwitchBackground

local function runSwitchConfig(event)
  lcd.clear()
  if SwitchBackground == nil then
    SwitchBackground = Bitmap.open("img/fm.png")
  end
  lcd.drawBitmap(SwitchBackground, 0, 0)
  lcd.drawBitmap(ImgPageUp, 0, 95)
  lcd.drawBitmap(ImgPageDn, 455, 95)
  fields = SwitchFields
  lcd.drawText(40, 20, "FM (Idle Up)")
  lcd.drawFilledRectangle(40, 45, 100, 30, TEXT_BGCOLOR)
  fields[1][4]=1
  lcd.drawText(40, 85, "Throttle Hold")
  lcd.drawFilledRectangle(40, 105, 100, 30, TEXT_BGCOLOR)
  fields[2][4]=1
  fields[3][4]=0
  if TypeFields[1][5]==1 then
    lcd.drawText(40, 160, "Gyro Rate")
    lcd.drawFilledRectangle(40, 185, 100, 30, TEXT_BGCOLOR)
    fields[3][4]=1
  end
  local result = runFieldsPage(event)
  return result
end

local ThrFields = {
  {50, 50, COMBO, 1, 2, { "CH1", "CH2", "CH3", "CH4", "CH5", "CH6", "CH7", "CH8" } },
}

local ThrBackground

local function runThrConfig(event)
  lcd.clear()
  if ThrBackground == nil then
    ThrBackground = Bitmap.open("img/thr.png")
  end
  lcd.drawBitmap(ThrBackground, 0, 0)
  lcd.drawBitmap(ImgPageUp, 0, 95)
  lcd.drawBitmap(ImgPageDn, 455, 95)
  fields = ThrFields
  lcd.drawText(40, 20, "Throttle Channel")
  lcd.drawFilledRectangle(40, 45, 100, 30, TEXT_BGCOLOR)
  fields[1][4]=1
  local result = runFieldsPage(event)
  return result
end

local CurveFields = {
  {50, 50, COMBO, 1, 0, { "Thr Up", "V Curve","Flat" } },
  {50, 120, COMBO, 1, 0, { "V Curve","Flat" } },
  {50, 190, COMBO, 1, 1, { "V Curve","Flat" } },
}

local CurveBackground

local function runCurveConfig(event)
  lcd.clear()
  if CurveBackground == nil then
    CurveBackground = Bitmap.open("img/thcurve.png")
  end
  lcd.drawBitmap(CurveBackground, 0, 0)
  lcd.drawBitmap(ImgPageUp, 0, 95)
  lcd.drawBitmap(ImgPageDn, 455, 95)
  fields = CurveFields
  lcd.drawText(40, 20, "Throttle Curve FM0")
  lcd.drawFilledRectangle(40, 45, 100, 30, TEXT_BGCOLOR)
  fields[1][4]=1
  lcd.drawText(40, 90, "Throttle Curve FM1")
  lcd.drawFilledRectangle(40, 115, 100, 30, TEXT_BGCOLOR)
  fields[2][4]=1
  lcd.drawText(40, 160, "Throttle Curve FM2")
  lcd.drawFilledRectangle(40, 185, 100, 30, TEXT_BGCOLOR)
  fields[3][4]=1
  local result = runFieldsPage(event)
  return result
end

local AilerFields = {
  {50, 50, COMBO, 1, 0, { "CH1", "CH2", "CH3", "CH4", "CH5", "CH6", "CH7", "CH8" } },
}

local AilerBackground

local function runAilerConfig(event)
  lcd.clear()
  if AilerBackground == nil then
    AilerBackground = Bitmap.open("img/ail.png")
  end
  lcd.drawBitmap(AilerBackground, 0, 0)
  lcd.drawBitmap(ImgPageUp, 0, 95)
  lcd.drawBitmap(ImgPageDn, 455, 95)
  fields = AilerFields
  lcd.drawText(40, 20, "Aileron Channel")
  lcd.drawFilledRectangle(40, 45, 100, 30, TEXT_BGCOLOR)
  fields[1][4]=1
  local result = runFieldsPage(event)
  return result
end

local EleFields = {
  {50, 50, COMBO, 1, 1, { "CH1", "CH2", "CH3", "CH4", "CH5", "CH6", "CH7", "CH8" } },
}

local EleBackground

local function runEleConfig(event)
  lcd.clear()
  if EleBackground == nil then
    EleBackground = Bitmap.open("img/ele.png")
  end
  lcd.drawBitmap(EleBackground, 0, 0)
  lcd.drawBitmap(ImgPageUp, 0, 95)
  lcd.drawBitmap(ImgPageDn, 455, 95)
  fields = EleFields
  lcd.drawText(40, 20, "Elevator Channel")
  lcd.drawFilledRectangle(40, 45, 100, 30, TEXT_BGCOLOR)
  fields[1][4]=1
  local result = runFieldsPage(event)
  return result
end

local RudFields = {
  {50, 50, COMBO, 1, 3, { "CH1", "CH2", "CH3", "CH4", "CH5", "CH6", "CH7", "CH8" } },
}

local RudBackground

local function runRudConfig(event)
  lcd.clear()
  if RudBackground == nil then
    RudBackground = Bitmap.open("img/rudder.png")
  end
  lcd.drawBitmap(RudBackground, 0, 0)
  lcd.drawBitmap(ImgPageUp, 0, 95)
  lcd.drawBitmap(ImgPageDn, 455, 95)
  fields = RudFields
  lcd.drawText(40, 20, "Rudder (Tail) Channel")
  lcd.drawFilledRectangle(40, 45, 100, 30, TEXT_BGCOLOR)
  fields[1][4]=1
  local result = runFieldsPage(event)
  return result
end

local lineIndex

local function drawNextChanelLine(text, text2)
  lcd.drawText(40, lineIndex, text)
  lcd.drawText(242, lineIndex, ": CH" .. text2 + 1)
  lineIndex = lineIndex + 20
end

local function drawNextSwitchLine(text, text2)
  lcd.drawText(40, lineIndex, text)
  lcd.drawText(242, lineIndex, ": " ..switches[text2 + 1])
  lineIndex = lineIndex + 20
end

local function drawNextTextLine(text, text2)
  lcd.drawText(40, lineIndex, text)
  lcd.drawText(242, lineIndex, ": " ..text2)
  lineIndex = lineIndex + 20
end

local function switchLine(text)
  text=SwitchFields[2][5]
  getFieldInfo(text)
  swnum=text.id
end

local ConfigSummaryFields = {
  {110, 250, COMBO, 1, 0, { "No, I need to change something", "Yes, all is well, create the model !"} },
}

local function runConfigSummary(event)
  lcd.clear()
  fields = ConfigSummaryFields
  lcd.drawBitmap(BackgroundImg, 0, 0)
  lcd.drawBitmap(ImgPageUp, 0, 95)
  lineIndex = 10

  -- Type
  if TypeFields[1][5]==0 then
    drawNextTextLine("Type","FBL")
  else
    drawNextTextLine("Type","FB")
    if TypeFields[2][5]==0 then
      lcd.drawText(290,10,"Swash 120")
    elseif TypeFields[2][5]==1 then
      lcd.drawText(290,10,"Swash 120X")
    elseif TypeFields[2][5]==2 then
      lcd.drawText(290,10,"Swash 140")
    else
      lcd.drawText(290,10,"Swash 90")
    end
  end

  -- Style
  if StyleFields[1][5]==0 then
    drawNextTextLine("Primary Style","Sport")
  elseif StyleFields[1][5]==1 then
    drawNextTextLine("Primary Style","Light 3D")
  else
    drawNextTextLine("Primary Style","Full 3D")
  end

  -- Switch
  drawNextSwitchLine("FM Switch",SwitchFields[1][5])
  drawNextSwitchLine("Th Hold Switch",SwitchFields[2][5])
  if TypeFields[1][5]==1 then
    drawNextSwitchLine("Gyro Rate Switch",SwitchFields[3][5])
  end

  -- thr
  drawNextChanelLine("Throttle Channel",ThrFields[1][5])

  -- FM0 Curve
  if CurveFields[1][5]==0 then
    drawNextTextLine("FM0 Curve","Throttle Up")
  elseif CurveFields[1][5]==1 then
    drawNextTextLine("FM0 Curve","V Style")
  else
    drawNextTextLine("FM0 Curve","Flat Style")
  end

  -- FM1 Curve
  if CurveFields[2][5]==0 then
    drawNextTextLine("FM1 Curve","V Style")
  else
    drawNextTextLine("FM1 Curve","Flat Style")
  end

  -- FM3 Curve
  if CurveFields[3][5]==0 then
    drawNextTextLine("FM2 Curve","V Style")
  else
    drawNextTextLine("FM2 Curve","Flat Style")
  end

  -- Ail
  drawNextChanelLine("Aileron Channel",AilerFields[1][5])

  -- Elev
  drawNextChanelLine("Elevator Channel",EleFields[1][5])

  -- Rudder
  drawNextChanelLine("Rudder Channel",RudFields[1][5])

  local result = runFieldsPage(event)
  if(fields[1][5] == 1 and edit == false) then
    selectPage(1)
  end
  return result
end

local function createModel(event)
  lcd.clear()
  local b = SwitchFields[1][5]
  local tUp = switchValues[b]
  local i = SwitchFields[2][5]
  local hold = switchValues[i]
  local f = SwitchFields[3][5]
  local gyRate = switchValues[f]
  model.defaultInputs()
  model.deleteMixes()

  -- Curve Fm0
  if StyleFields[1][5]==0 and CurveFields[1][5]==0 then
    model.setCurve(0,{name="TC0",y={-100, 0, 20, 40, 40}})
  elseif StyleFields[1][5]==1 and CurveFields[1][5]==0 then
    model.setCurve(0,{name="TC0",y={-100, 0, 35, 50, 50}})
  elseif StyleFields[1][5]==2 and CurveFields[1][5]==0 then
    model.setCurve(0,{name="TC0",y={-100, 0, 40, 80, 80}})
  elseif StyleFields[1][5]==0 and CurveFields[1][5]==1 then
    model.setCurve(0,{name="TC0",y={50, 40, 50}})
  elseif StyleFields[1][5]==1 and CurveFields[1][5]==1 then
    model.setCurve(0,{name="TC0",y={65, 55, 65}})
  elseif StyleFields[1][5]==2 and CurveFields[1][5]==1 then
    model.setCurve(0,{name="TC0",y={70, 60, 70}})
  elseif  StyleFields[1][5]==0 and CurveFields[1][5]==2 then
    model.setCurve(0,{name="TC0",y={60,60,60}})
  elseif  StyleFields[1][5]==1 and CurveFields[1][5]==2 then
    model.setCurve(0,{name="TC0",y={65,65,65}})
  else
    model.setCurve(0,{name="TC0",y={70,70,70}})
  end

  --Curve FM1
  if StyleFields[1][5]==0 and CurveFields[2][5]==0 then
    model.setCurve(1,{name="TC1",y={60, 50, 60}})
  elseif  StyleFields[1][5]==1 and CurveFields[2][5]==0 then
    model.setCurve(1,{name="TC1",y={70, 60, 70}})
  elseif StyleFields[1][5]==2 and CurveFields[2][5]==0 then
    model.setCurve(1,{name="TC1",y={85, 75, 85}})
  elseif StyleFields[1][5]==0 and CurveFields[2][5]==1 then
    model.setCurve(1,{name="TC1",y={65,65,65}})
  elseif StyleFields[1][5]==1 and CurveFields[2][5]==1 then
    model.setCurve(1,{name="TC1",y={70,70,70}})
  else
    model.setCurve(1,{name="TC1",y={85 ,85,85}})
  end

  --Curve FM2
  if StyleFields[1][5]>=0 and CurveFields[3][5]==0 then
    model.setCurve(2,{name="Tc2",y={70, 60, 70}})
  elseif StyleFields[1][5]==1 and CurveFields[3][5]==0 then
    model.setCurve(2,{name="TC2",y={85, 70, 85}})
  elseif StyleFields[1][5]==2 and CurveFields[3][5]==0 then
    model.setCurve(2,{name="TC2",y={100, 90, 100}})
  elseif StyleFields[1][5]==0 and CurveFields[3][5]==1 then
    model.setCurve(2,{name="TC2",y={75 ,75,75}})
  elseif StyleFields[1][5]==1 and CurveFields[3][5]==1 then
    model.setCurve(2,{name="TC2",y={85 ,85, 85}})
  else
    model.setCurve(2,{name="TC2",y={95 ,95, 95}})
  end

  --Curve TH Hold
  model.setCurve(3,{name="THD",y={-100,-100,-100}})

  -- Throttle
  model.insertMix(ThrFields[1][5], 0,{name="Th0",weight=100,curveType=3,curveValue=1})
  model.insertMix(ThrFields[1][5], 1,{name="Th1",weight=100,switch=tUp,multiplex=2,curveType=3,curveValue=2})
  model.insertMix(ThrFields[1][5], 2,{name="Th2",weight=100,switch=tUp-1,multiplex=2,curveType=3,curveValue=3})
  model.insertMix(ThrFields[1][5], 3,{name="Hld",weight=100,offset=-15,switch=hold+1,multiplex=2,curveType=3,curveValue=4})

  -- Ail
  if TypeFields[1][5] == 0 then
    model.insertMix(AilerFields[1][5], 0,{name="Ail",weight=100})
  else
    model.insertMix(AilerFields[1][5], 0,{source=102,name="Ail",weight=100})
  end

  -- Elev
  if TypeFields[1][5] == 0 then
    model.insertMix(EleFields[1][5], 0,{name="Ele",weight=100})
  else
    model.insertMix(EleFields[1][5], 0,{source=101,name="Ele",weight=100})
  end

  -- Rudder
  model.insertMix(RudFields[1][5], 0,{name="Rud",weight=100})

  -- Gyro
  if TypeFields[1][5] == 0 then
    model.insertMix(4, 0,{source=100,name="Gyr",weight=25})
  else
    model.insertMix( 4, 0,{source=100,name="HH",weight=25})
    model.insertMix( 4, 1,{source=100,name="Rat",weight=-25,switch=gyRate+1,multiplex=2})
  end

  -- Pitch
  if TypeFields[1][5] == 0 then
    model.insertMix(5, 0,{source=89,name="Pch",weight=100})
  else
    model.insertMix(5, 0,{source=103,name="Pch",weight=100})
  end

  --Set Swash Parameters
    if TypeFields[1][5]==1 and TypeFields[2][5]==0 then
      model.swashRingData({type=1,collectiveSource=89,aileronSource=90,elevatorSource=88,collectiveWeight=60,aileronWeight=60,elevatorWeight=60})
    elseif TypeFields[2][5]==1 then
      model.swashRingData({type=2,collectiveSource=89,aileronSource=90,elevatorSource=88,collectiveWeight=60,aileronWeight=60,elevatorWeight=60})
    elseif TypeFields[2][5]==2 then
      model.swashRingData({type=3,collectiveSource=89,aileronSource=90,elevatorSource=88,collectiveWeight=40,aileronWeight=40,elevatorWeight=60})
    elseif TypeFields[2][5]==3 then
      model.swashRingData({type=4,collectiveSource=89,aileronSource=90,elevatorSource=88,collectiveWeight=35,aileronWeight=35,elevatorWeight=60})
    end
    lcd.drawText(70, 90, "Model successfully created !")
    lcd.drawText(100, 130, "Press RTN to exit", TEXT_COLOR)
    return 2
  end

-- Init
local function init()
  current, edit = 1, false
  pages = {
    runTypeConfig,
    runStyleConfig,
    runSwitchConfig,
    runThrConfig,
    runCurveConfig,
    runAilerConfig,
    runEleConfig,
    runRudConfig,
    runConfigSummary,
    createModel,
  }
end

-- Main
local function run(event)
  if event == nil then
    error("Cannot be run as a model script!")
    return 2
  elseif event == EVT_VIRTUAL_NEXT_PAGE and page < #pages-1 then
    selectPage(1)
  elseif event == EVT_VIRTUAL_PREV_PAGE and page > 1 then
    killEvents(event);
    selectPage(-1)
  end
  local result = pages[page](event)
  return result
end

return { init=init, run=run }
