local VALUE = 0
local COMBO = 1

local edit = false
local page = 1
local current = 1
local pages = {}
local fields = {}
local AilCfgBg = {}
local FlapsCfgBg = {}
local TailCfgBg = {}
local SummaryBackground

-- Release memory
local function clean()
  print("FREE MEMORY CALLED")
  MotorConfigBackground = nil
  AilCfgBg = nil
  FlapsCfgBg = nil
  TailCfgBg = nil
  SummaryBackground = nil
end

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

-- on demand load of bitmaps
local  function loadIndexedBitmap(bitmap, index, name)
  if bitmap[index] == nil then
    bitmap[index] = Bitmap.open(name..index..".gif")
  end
end

local MotorConfigBackground = Bitmap.open("img/bg_engine.gif")
local MotorFields = {
  {50, 50, COMBO, 1, 1, { "No", "Yes"} },
  {50, 127, COMBO, 1, 2, { "CH1", "CH2", "CH3", "CH4", "CH5", "CH6", "CH7", "CH8" } },
}

local function runMotorConfig(event)
  lcd.clear()
  lcd.drawBitmap(MotorConfigBackground, 0, 0)
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
  loadIndexedBitmap(AilCfgBg, 2, "img/bg_ail")
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

local function runAilConfig(event)
  lcd.clear()
  fields = AilFields
  if fields[1][5] == 1 then
    loadIndexedBitmap(AilCfgBg, 1, "img/bg_ail")
    lcd.drawBitmap(AilCfgBg[1], 0, 0)
    lcd.drawFilledRectangle(40, 122, 100, 30, CUSTOM_COLOR)
    setFieldsVisible(1, 0)
  elseif fields[1][5] == 2 then
    loadIndexedBitmap(AilCfgBg, 2, "img/bg_ail")
    lcd.drawBitmap(AilCfgBg[2], 0, 0)
    lcd.drawFilledRectangle(40, 122, 100, 30, CUSTOM_COLOR)
    lcd.drawFilledRectangle(40, 162, 100, 30, CUSTOM_COLOR)
    setFieldsVisible(1, 1)
  else
    loadIndexedBitmap(AilCfgBg, 0, "img/bg_ail")
    lcd.drawBitmap(AilCfgBg[0], 0, 0)
    setFieldsVisible(0, 0)
  end
  lcd.drawText(40, 20, "Number of ailerons on your model ?", TEXT_COLOR)
  lcd.drawFilledRectangle(40, 45, 400, 30, CUSTOM_COLOR)
  loadIndexedBitmap(FlapsCfgBg, 1, "img/bg_flaps")
  local result = runFieldsPage(event)
  return result
end

local FlapsFields = {
  {50, 50, COMBO, 1, 0, { "No", "Yes, on one channel", "Yes, on two channels"} },
  {50, 127, COMBO, 1, 6, { "CH1", "CH2", "CH3", "CH4", "CH5", "CH6", "CH7", "CH8" } },
  {50, 167, COMBO, 1, 7, { "CH1", "CH2", "CH3", "CH4", "CH5", "CH6", "CH7", "CH8" } },
}

local function runFlapsConfig(event)
  lcd.clear()
  fields = FlapsFields
  if fields[1][5] == 1 then
    loadIndexedBitmap(FlapsCfgBg, 1, "img/bg_flaps")
    lcd.drawBitmap(FlapsCfgBg[1], 0, 0)
    lcd.drawFilledRectangle(40, 122, 100, 30, CUSTOM_COLOR)
    setFieldsVisible(1, 0)
  elseif fields[1][5] == 2 then
    loadIndexedBitmap(FlapsCfgBg, 2, "img/bg_flaps")
    lcd.drawBitmap(FlapsCfgBg[2], 0, 0)
    lcd.drawFilledRectangle(40, 122, 100, 30, CUSTOM_COLOR)
    lcd.drawFilledRectangle(40, 162, 100, 30, CUSTOM_COLOR)
    setFieldsVisible(1, 1)
  else
    loadIndexedBitmap(FlapsCfgBg, 0, "img/bg_flaps")
    lcd.drawBitmap(FlapsCfgBg[0], 0, 0)
    setFieldsVisible(0, 0)
  end
  lcd.drawText(40, 20, "Does your model have flaps ?", TEXT_COLOR)
  lcd.drawFilledRectangle(40, 45, 400, 30, CUSTOM_COLOR)
  loadIndexedBitmap(TailCfgBg, 1, "img/bg_tail")
  local result = runFieldsPage(event)
  return result
end

local TailFields = {
  {50, 50, COMBO, 1, 1, { "1 channel for Elevator, no Rudder", "One chan for Elevator, one for Rudder", "Two chans for Elevator, one for Rudder", "V Tail"} },
  {50, 127, COMBO, 1, 1, { "CH1", "CH2", "CH3", "CH4", "CH5", "CH6", "CH7", "CH8" } }, --ele
  {50, 167, COMBO, 1, 3, { "CH1", "CH2", "CH3", "CH4", "CH5", "CH6", "CH7", "CH8" } }, --rud
  {50, 207, COMBO, 0, 5, { "CH1", "CH2", "CH3", "CH4", "CH5", "CH6", "CH7", "CH8" } }, --ele2
}

local function runTailConfig(event)
  lcd.clear()
  fields = TailFields
  if fields[1][5] == 0 then
    loadIndexedBitmap(TailCfgBg, 0, "img/bg_tail")
    lcd.drawBitmap(TailCfgBg[0], 0, 0)
    lcd.drawFilledRectangle(40, 122, 100, 30, CUSTOM_COLOR)
    setFieldsVisible(1, 0, 0)
  end
  if fields[1][5] == 1 then
    loadIndexedBitmap(TailCfgBg, 1, "img/bg_tail")
    lcd.drawBitmap(TailCfgBg[1], 0, 0)
    lcd.drawFilledRectangle(40, 122, 100, 30, CUSTOM_COLOR)
    lcd.drawFilledRectangle(40, 162, 100, 30, CUSTOM_COLOR)
    setFieldsVisible(1, 1, 0)
  end
  if fields[1][5] == 2 then
    loadIndexedBitmap(TailCfgBg, 2, "img/bg_tail")
    lcd.drawBitmap(TailCfgBg[2], 0, 0)
    lcd.drawFilledRectangle(40, 122, 100, 30, CUSTOM_COLOR)
    lcd.drawFilledRectangle(40, 162, 100, 30, CUSTOM_COLOR)
    lcd.drawFilledRectangle(40, 202, 100, 30, CUSTOM_COLOR)
    setFieldsVisible(1, 1, 1)
  end
  if fields[1][5] == 3 then
    loadIndexedBitmap(TailCfgBg, 3, "img/bg_tail")
    lcd.drawBitmap(TailCfgBg[3], 0, 0)
    lcd.drawFilledRectangle(40, 122, 100, 30, CUSTOM_COLOR)
    lcd.drawFilledRectangle(40, 162, 100, 30, CUSTOM_COLOR)
    setFieldsVisible(1, 1, 0)
  end
  lcd.drawText(40, 20, "Pick the tail config of your model", TEXT_COLOR)
  lcd.drawFilledRectangle(40, 45, 400, 30, CUSTOM_COLOR)
  if SummaryBackground == nil then
    SummaryBackground = Bitmap.open("img/bg_sum.gif")
  end
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

local function runConfigSummary(event)
  lcd.clear()
  fields = ConfigSummaryFields
  lcd.drawBitmap(SummaryBackground, 0, 0)
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
    drawNextLine("V-Tail rudder :", TailFields[2][5])
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
  lcd.drawBitmap(SummaryBackground, 0, 0)
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
    addMix(AilFields[2][5], MIXSRC_FIRST_INPUT+defaultChannel(3), "Ail1")
    addMix(AilFields[3][5], MIXSRC_FIRST_INPUT+defaultChannel(3), "Ail2", -100)
  end
  -- Flaps
  if(FlapsFields[1][5] == 1) then
    addMix(FlapsFields[2][5], MIXSRC_SA, "Flaps")
  elseif (FlapsFields[1][5] == 2) then
    addMix(FlapsFields[2][5], MIXSRC_SA, "Flaps1")
    addMix(FlapsFields[3][5], MIXSRC_SA, "Flaps2")
  end
  -- Tail
  if(TailFields[1][5] == 0) then
    addMix(TailFields[2][5], MIXSRC_FIRST_INPUT+defaultChannel(1), "Elev")
  elseif (TailFields[1][5] == 1) then
    addMix(TailFields[2][5], MIXSRC_FIRST_INPUT+defaultChannel(1), "Elev")
    addMix(TailFields[3][5], MIXSRC_FIRST_INPUT+defaultChannel(0), "Rudder")
  elseif (TailFields[1][5] == 2) then
    addMix(TailFields[2][5], MIXSRC_FIRST_INPUT+defaultChannel(1), "Elev1")
    addMix(TailFields[3][5], MIXSRC_FIRST_INPUT+defaultChannel(0), "Rudder")
    addMix(TailFields[4][5], MIXSRC_FIRST_INPUT+defaultChannel(1), "Elev2")
  elseif (TailFields[1][5] == 3) then
    addMix(TailFields[2][5], MIXSRC_FIRST_INPUT+defaultChannel(1), "VTailE", 50)
    addMix(TailFields[2][5], MIXSRC_FIRST_INPUT+defaultChannel(0), "VTailR", 50, 1)
    addMix(TailFields[3][5], MIXSRC_FIRST_INPUT+defaultChannel(1), "VTailE", 50)
    addMix(TailFields[3][5], MIXSRC_FIRST_INPUT+defaultChannel(0), "VTailR", -50, 1)
  end
  lcd.drawText(70, 140, "Model successfully created !", TEXT_COLOR)
  lcd.drawText(100, 180, "Press RTN to exit", TEXT_COLOR)
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
