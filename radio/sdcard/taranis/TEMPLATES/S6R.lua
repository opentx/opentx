local VALUE = 0
local COMBO = 1

local edit = false
local current = 1
local refreshState = 0
local refreshIndex = 0
local pageOffset = 0

local fields = {
  {"S6R functions:", COMBO, 0x9C, nil, { "Disable", "Enable" } },
  {"Wing type:", COMBO, 0x80, nil, { "REA", "DELTA", "VTAIL" } },
  {"", COMBO, 0x81, nil, { "Reverse", "Upright", "Upright reverse" } },
  {"AIL direction:", COMBO, 0x82, nil, { "Normal", "Invers" }, { 0, 255 } },
  {"ELE direction:", COMBO, 0x83, nil, { "Normal", "Invers" }, { 0, 255 } },
  {"RUD direction:", COMBO, 0x84, nil, { "Normal", "Invers" }, { 0, 255 } },
  {"AIL2 direction:", COMBO, 0x9A, nil, { "Normal", "Invers" }, { 0, 255 } },
  {"ELE2 direction:", COMBO, 0x9B, nil, { "Normal", "Invers" }, { 0, 255 } },
  {"AIL stabilize gain:", VALUE, 0x85, nil, 0, 200, "%"},
  {"ELE stabilize gain:", VALUE, 0x86, nil, 0, 200, "%"},
  {"RUD stabilize gain:", VALUE, 0x87, nil, 0, 200, "%"},
  {"AIL auto level gain:", VALUE, 0x88, nil, 0, 200, "%"},
  {"ELE auto level gain:", VALUE, 0x89, nil, 0, 200, "%"},
  {"ELE upright gain:", VALUE, 0x8C, nil, 0, 200, "%"},
  {"RUD upright gain:", VALUE, 0x8D, nil, 0, 200, "%"},
  {"AIL crab gain:", VALUE, 0x8E, nil, 0, 200, "%"},
  {"RUD crab gain:", VALUE, 0x90, nil, 0, 200, "%"},
  {"AIL auto angle offset:", VALUE, 0x91, nil, -20, 20, "%", 0x6C},
  {"ELE auto angle offset:", VALUE, 0x92, nil, -20, 20, "%", 0x6C},
  {"ELE upright angle offset:", VALUE, 0x95, nil, -20, 20, "%", 0x6C},
  {"RUD upright angle offset:", VALUE, 0x96, nil, -20, 20, "%", 0x6C},
  {"AIL crab angle offset:", VALUE, 0x97, nil, -20, 20, "%", 0x6C},
  {"RUD crab angle offset:", VALUE, 0x99, nil, -20, 20, "%", 0x6C},
  -- {"Calib Accel. position:", COMBO, 0x9D, nil, { "Up", "Down", "Left", "Right", "Forward", "Back" } },
}

-- Change display attribute to current field
local function addField(step)
  field = fields[current]
  if field[2] == VALUE then
    min = field[5]
    max = field[6]
  elseif field[2] == COMBO then
    min = 0
    max = #(field[5]) - 1
  end
  if (step < 0 and field[4] > min) or (step > 0 and field[4] < max) then
    field[4] = field[4] + step
  end
end

-- Select the next or previous editable field
local function selectField(step)
  current = 1 + ((current + step - 1 + #fields) % #fields)
  if current > 7 + pageOffset then
    pageOffset = current - 7
  elseif current <= pageOffset then
    pageOffset = current - 1
  end
end

local function drawProgressBar()
  local width = (160 * refreshIndex) / #fields
  lcd.drawRectangle(48, 0, 164, 8)
  lcd.drawFilledRectangle(50, 2, width, 4);
end

-- Redraw the current page
local function redrawPage()
  lcd.clear()
  lcd.drawScreenTitle("S6R", 0, 0)

  if refreshIndex < #fields then
    drawProgressBar()
  end

  for index = 1, 7, 1 do
    field = fields[pageOffset+index]
    attr = current == (pageOffset+index) and ((edit == true and BLINK or 0) + INVERS) or 0

    -- display label
    lcd.drawText(0, 1+8*index, field[1])

    if field[4] == nil then
      lcd.drawText(150, 1+8*index, "---", attr)
    else
      if field[2] == VALUE then
        lcd.drawNumber(150, 1+8*index, field[4], LEFT + attr)
      elseif field[2] == COMBO then
        if field[4] >= 0 and field[4] < #(field[5]) then
          lcd.drawText(150, 1+8*index, field[5][1+field[4]], attr)
        end
      end
    end
  end
end

local telemetryPopTimeout = 0
local function refreshNext()
  if refreshState == 0 then
    if refreshIndex < #fields then
      field = fields[refreshIndex + 1]
      if telemetryPush(0x1A, 0x30, 0x0C30, field[3]) == true then
        refreshState = 1
        telemetryPopTimeout = getTime() + 80 -- normal delay is 500ms
      end
    end
  else
    field = fields[refreshIndex + 1]
    physicalId, primId, dataId, value = telemetryPop()
    if physicalId == 0x1A and primId == 0x32 and dataId == 0x0C30 and value % 256 == field[3] then
      value = math.floor(value / 256)
      if field[2] == COMBO and #field == 6 then
        for index = 1, #(field[6]), 1 do
          if value == field[6][index] then
            value = index - 1
            break
          end
        end
      elseif field[2] == VALUE and #field == 8 then
        value = value - field[8] + field[5]
      end
      fields[refreshIndex + 1][4] = value
      refreshIndex = refreshIndex + 1
      refreshState = 0
    elseif getTime() > telemetryPopTimeout then
      refreshState = 0
    end
  end
end

local function updateField(field)
  value = field[4]
  if field[2] == COMBO and #field == 6 then
    value = field[6][1+value]
  elseif field[2] == VALUE and #field == 8 then
    value = value + field[8] - field[5]
  end
  telemetryPush(0x1A, 0x31, 0x0C30, field[3] + value*256)
end

-- Init
local function init()
  current, edit, refreshState, refreshIndex = 1, false, 0, 0
end

-- Main
local function run(event)
  if event == nil then
    error("Cannot be run as a model script!")
    return 2
  elseif event == EVT_EXIT_BREAK then                   -- exit script
    return 2
  elseif event == EVT_ENTER_BREAK then                  -- toggle editing/selecting current field
    if fields[current][4] ~= nil then
      edit = not edit
      if edit == false then
        updateField(fields[current])
      end
    end  
    redrawPage()
  elseif edit then
    if event == EVT_PLUS_FIRST or event == EVT_PLUS_REPT then
      addField(1)
    elseif event == EVT_MINUS_FIRST or event == EVT_MINUS_REPT then
      addField(-1)
    end
    redrawPage()
  else
    if event == EVT_MINUS_FIRST then
      selectField(1)
    elseif event == EVT_PLUS_FIRST then
      selectField(-1) 
    end
    redrawPage()
  end
  refreshNext()
  return 0
end

return { init=init, run=run }
