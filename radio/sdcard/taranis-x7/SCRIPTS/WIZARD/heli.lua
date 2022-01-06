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
local switches = {"SA", "SB", "SC", "SD", "SF"}
local switchValues = {[0]=2, 5, 8, 11, 14}

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
  {0, 12, COMBO, 1, 0, {"Flybarless (FBL)", "Flybarred (FB)" } },
  {65, 25, COMBO, 1, 0, {"120", "120X", "140", "90" } },
}

local function runTypeConfig(event)
  lcd.clear()
  fields = TypeFields
  lcd.drawScreenTitle("Helicopter Type", 1,9)
  fields[2][4] = 0
  if fields[1][5] == 1 then
    lcd.drawText(0, 25, "Swash Type")
    fields[2][4] = 1
  end
  local result = runFieldsPage(event)
  return result
end

local StyleFields = {
  {0, 12, COMBO, 1, 0, { "Sport", "Light 3D","Full 3D" } },
}

local function runStyleConfig(event)
  lcd.clear()
  fields = StyleFields
  lcd.drawScreenTitle("Your Flying Style",2,9)
  fields[1][4]=1
  local result = runFieldsPage(event)
  return result
end

local SwitchFields = {
  {75, 12, COMBO, 1, 0, { "SA", "SB", "SC", "SD","SF" } },
  {75, 33, COMBO, 1, 4, { "SA", "SB", "SC", "SD","SF" } },
  {75, 54, COMBO, 1, 2, { "SA", "SB", "SC", "SD", "SF" } },
}

local function runSwitchConfig(event)
  lcd.clear()
  lcd.drawScreenTitle("Assign Switches",3,9)
  fields = SwitchFields
  lcd.drawText(0, 12, "FM (Idle Up)")
  fields[1][4]=1
  lcd.drawText(0, 33, "Throttle Hold")
  fields[2][4]=1
  fields[3][4]=0
  if TypeFields[1][5]==1 then
    lcd.drawText(0, 54, "Gyro Rate")
    fields[3][4]=1
  end
  local result = runFieldsPage(event)
  return result
end

local ThrFields = {
  {0, 12, COMBO, 1, 2, { "CH1", "CH2", "CH3", "CH4", "CH5", "CH6", "CH7", "CH8" } },
}

local function runThrConfig(event)
  lcd.clear()
  fields = ThrFields
  lcd.drawScreenTitle("Throttle Channel",3,9)
  fields[1][4]=1
  local result = runFieldsPage(event)
  return result
end

local CurveFields = {
  {75, 12, COMBO, 1, 0, { "Thr Up", "V Curve","Flat" } },
  {75, 32, COMBO, 1, 0, { "V Curve","Flat" } },
  {75, 54, COMBO, 1, 0, { "V Curve","Flat" } },
}

local function runCurveConfig(event)
  lcd.clear()
  fields = CurveFields
  lcd.drawScreenTitle("FM Throttle Curves",4,9)
  lcd.drawText(0, 12, "FM0 Curve")
  fields[1][4]=1
  lcd.drawText(0, 32, "FM1 Curve")
  fields[2][4]=1
  lcd.drawText(0, 54, "FM2 Curve")
  fields[3][4]=1
  local result = runFieldsPage(event)
  return result
end

local AilerFields = {
  {0, 12, COMBO, 1, 0, { "CH1", "CH2", "CH3", "CH4", "CH5", "CH6", "CH7", "CH8" } },
}

local function runAilerConfig(event)
  lcd.clear()
  fields = AilerFields
  lcd.drawScreenTitle("Aileron Channel",5,9)
  fields[1][4]=1
  local result = runFieldsPage(event)
  return result
end

local EleFields = {
  {0, 12, COMBO, 1, 1, { "CH1", "CH2", "CH3", "CH4", "CH5", "CH6", "CH7", "CH8" } },
}

local function runEleConfig(event)
  lcd.clear()
  fields = EleFields
  lcd.drawScreenTitle("Elevator Channel",6,9)
  fields[1][4]=1
  local result = runFieldsPage(event)
  return result
end

local RudFields = {
  {0, 12, COMBO, 1, 3, { "CH1", "CH2", "CH3", "CH4", "CH5", "CH6", "CH7", "CH8" } },
}

local function runRudConfig(event)
  lcd.clear()
  fields = RudFields
  lcd.drawScreenTitle("Rudder (Tail) Channel",7,9)
  fields[1][4]=1
  local result = runFieldsPage(event)
  return result
end

local lineIndex

local function drawNextChanelLine(text, text2)
  lcd.drawText(0, lineIndex, text)
  lcd.drawText(55, lineIndex, ": CH" .. text2 + 1)
  lineIndex = lineIndex + 9
end

local function drawNextSwitchLine(text, text2)
  lcd.drawText(0, lineIndex, text)
  lcd.drawText(55, lineIndex, ": " ..switches[text2 + 1])
  lineIndex = lineIndex + 9
end

local function drawNextTextLine(text, text2)
  lcd.drawText(0, lineIndex, text)
  lcd.drawText(55, lineIndex, ": " ..text2)
  lineIndex = lineIndex + 9
end

local function switchLine(text)
  text=SwitchFields[2][5]
  getFieldInfo(text)
  swnum=text.id
end

local SummaryFields = {
  {0, 56, {1}},
}

local function runSummary(event)
  lcd.clear()
  lcd.drawScreenTitle("Summary", 8,9)
  fields = SummaryFields
  lineIndex = 9

  -- Type
  if TypeFields[1][5]==0 then
    drawNextTextLine("TYPE","FBL")
  else
    drawNextTextLine("TYPE","FB")
    if TypeFields[2][5]==0 then
      lcd.drawText(75,9,"120")
    elseif TypeFields[2][5]==1 then
      lcd.drawText(75,9,"120X")
    elseif TypeFields[2][5]==2 then
      lcd.drawText(75,9,"140")
    else
      lcd.drawText(75,9,"90")
    end
  end

  -- Style
  if StyleFields[1][5]==0 then
    drawNextTextLine("Style","Sport")
  elseif StyleFields[1][5]==1 then
    drawNextTextLine("Style","Light 3D")
  else
    drawNextTextLine("Style","Full 3D")
  end

  -- Switch
  drawNextSwitchLine("FM SW",SwitchFields[1][5])
  drawNextSwitchLine("Th Hold SW",SwitchFields[2][5])
  if TypeFields[1][5]==1 then
    drawNextSwitchLine("Gyro SW",SwitchFields[3][5])
  end

  -- thr
  drawNextChanelLine("Throttle",ThrFields[1][5])
  fields[1][4]=1
  local result = runFieldsPage(event)
  return result
end

local ConfigSummaryFields = {
  {10, 50,{1}},
}

local function runConfigSummary(event)
  lcd.clear()
  lcd.drawScreenTitle("Long Enter -> Create", 9,9)
  fields = ConfigSummaryFields
  lineIndex = 8

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
  drawNextChanelLine("Aileron",AilerFields[1][5])

  -- Elev
  drawNextChanelLine("Elevator",EleFields[1][5])

  -- Rudder
  drawNextChanelLine("Rudder",RudFields[1][5])
  fields[1][4]=1
  local result = runFieldsPage(event)
  return result
end

local function runCreateModel(event)
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
    model.setCurve(0,{name="TC0",y={-100, 0, 0, 40, 40}})
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
    model.setCurve(2,{name="TC2",y={70, 60, 70}})
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
    model.insertMix(AilerFields[1][5], 0,{source=83,name="Ail",weight=100})
  end

  -- Elev
  if TypeFields[1][5] == 0 then
    model.insertMix(EleFields[1][5], 0,{name="Ele",weight=100})
  else
    model.insertMix(EleFields[1][5], 0,{source=82,name="Ele",weight=100})
  end

  -- Rudder
  model.insertMix(RudFields[1][5], 0,{name="Rud",weight=100})

  -- Gyro
  if TypeFields[1][5] == 0 then
    model.insertMix(4, 0,{source=81,name="Gyr",weight=25})
  else
    model.insertMix( 4, 0,{source=81,name="HH",weight=25})
    model.insertMix( 4, 1,{source=81,name="Rat",weight=-25,switch=gyRate,multiplex=2})
  end

  -- Pitch
  if TypeFields[1][5] == 0 then
    model.insertMix(5, 0,{source=77,name="Pch",weight=100})
  else
    model.insertMix(5, 0,{source=84,name="Pch",weight=100})
  end

  --Set Swash Parameters
  if TypeFields[1][5]==1 and TypeFields[2][5]==0 then
    model.swashRingData({type="1",collectiveSource=77,aileronSource=78,elevatorSource=76,collectiveWeight=60,aileronWeight=60,elevatorWeight=60})
  elseif TypeFields[2][5]==1 then
    model.swashRingData({type="2",collectiveSource=77,aileronSource=78,elevatorSource=76,collectiveWeight=60,aileronWeight=60,elevatorWeight=60})
  elseif TypeFields[2][5]==2 then
    model.swashRingData({type="3",collectiveSource=77,aileronSource=78,elevatorSource=76,collectiveWeight=40,aileronWeight=40,elevatorWeight=60})
  elseif TypeFields[2][5]==3 then
    model.swashRingData({type="4",collectiveSource=77,aileronSource=78,elevatorSource=76,collectiveWeight=35,aileronWeight=35,elevatorWeight=60})
  end
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
    runSummary,
    runConfigSummary,
  }
end

-- Main
local function run(event)
  if event == nil then
    error("Cannot be run as a model script!")
    return 2
  elseif event == EVT_VIRTUAL_NEXT_PAGE and page < #pages then
    selectPage(1)
  elseif event == EVT_VIRTUAL_ENTER_LONG and page == #pages then
    runCreateModel(event)
    lcd.drawText(0,15,"Model Sucessfully created !")
    return 2
  elseif event == EVT_VIRTUAL_PREV_PAGE and page > 1 then
    killEvents(event);
    selectPage(-1)
  end
  local result = pages[page](event)
  return result
end

return { init=init, run=run }
