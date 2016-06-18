local VALUE = 0
local COMBO = 1

local COLUMN_2 = 150

local edit = false
local page = 1
local current = 1
local refreshState = 0
local refreshIndex = 0
local calibrationState = 0
local pageOffset = 0
local calibrationStep = 0
local pages = { }

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

-- Select the next or previous page
local function selectPage(step)
  page = 1 + ((page + step - 1 + #pages) % #pages)
  refreshIndex = 0
  calibrationStep = 0
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
  local width = (140 * refreshIndex) / #fields
  lcd.drawRectangle(30, 1, 144, 6)
  lcd.drawFilledRectangle(32, 3, width, 2);
end

local function drawDevicePage(page)
  -- if refreshIndex < #fields then
  --   drawProgressBar()
  -- end

  for index = 1, 7, 1 do
    field = page["fields"][pageOffset+index]
    if field == nil then
      break
    end
    
    if field["name"] == nil then
      lcd.drawText(0, 1+8*index, "...")
    else
      attr = current == (pageOffset+index) and ((edit == true and BLINK or 0) + INVERS) or 0

      lcd.drawText(0, 1+8*index, field["name"])

      -- if field[4] == nil then
      --   lcd.drawText(COLUMN_2, 1+8*index, "---", attr)
      -- else
      --   if field[2] == VALUE then
      --     lcd.drawNumber(COLUMN_2, 1+8*index, field[4], LEFT + attr)
      --   elseif field[2] == COMBO then
      --     if field[4] >= 0 and field[4] < #(field[5]) then
      --       lcd.drawText(COLUMN_2, 1+8*index, field[5][1+field[4]], attr)
      --     end
      --   end
      -- end
    end
  end
end

local function createPage(id, name, fields_count)
  newpage = {
    id = id,
    name = name,
    state = 0,
    pagetimeout = 0,
    fields = {},
    fieldstimeout = 0,
  }
  for i=1, fields_count do
    newpage["fields"][i] = { name=nil }
  end
  return newpage
end

local function getPage(name)
  for i=1, #pages do
    if pages[i]["name"] == name then
      return pages[i]
    end
  end
  return nil
end

local function parseDeviceInfoMessage(data)
  id = data[2]
  name = ""
  i = 3
  while data[i] ~= 0 do
    name = name .. string.char(data[i])
    i = i + 1
  end
  fields_count = data[i+13]
  pg = getPage(name)
  if pg == nil then
    pg = createPage(id, name, fields_count-1) -- TODO fields_count should be different
    pages[#pages + 1] = pg
  end
  pg["pagetimeout"] = time + 3000 -- 30s
end

local function parseParameterInfoMessage(data)
  index = data[3]
  parent = data[4]
  pages[page].fields[index].parent = parent
  name = ""
  while parent ~= 0 do
    name = name .. " "
    parent = pages[page].fields[parent].parent
  end
  i = 6
  while data[i] ~= 0 do
    name = name .. string.char(data[i])
    i = i + 1
  end
  pages[page].fields[index].name = name
  pages[page].fieldstimeout = 0
end

local telemetryPopTimeout = 0
local devicesRefreshTimeout = 0
local function refreshNext()
  command, data = crossfireTelemetryPop()
  if command == nil then
    time = getTime()
    if time > devicesRefreshTimeout then
      if #pages == 0 then
        devicesRefreshTimeout = time + 100 -- 1s
      else
        devicesRefreshTimeout = time + 1000 -- 10s
      end
      crossfireTelemetryPush(0x28, { 0x00, 0xEA })
    elseif page <= #pages and pages[page]["state"] == 0 and time > pages[page]["fieldstimeout"] then
      for i=1, #pages[page]["fields"] do
        field = pages[page]["fields"][i]
        if field["name"] == nil then
          crossfireTelemetryPush(0x2C, { pages[page]["id"], 0xEA, i })
          pages[page]["fieldstimeout"] = time + 200 -- 2s
        end
      end
    end
  elseif command == 0x29 then
    parseDeviceInfoMessage(data)
  elseif command == 0x2B then
    parseParameterInfoMessage(data)
  end

--  if refreshState == 0 then
--    if calibrationState == 1 then
--      if telemetryWrite(0x9D, calibrationStep) == true then
--        refreshState = 1
--        calibrationState = 2
--        telemetryPopTimeout = getTime() + 80 -- normal delay is 500ms
--      end
--    elseif refreshIndex < #fields then
--      field = fields[refreshIndex + 1]
--      if telemetryRead(field[3]) == true then
--        refreshState = 1
--        telemetryPopTimeout = getTime() + 80 -- normal delay is 500ms
--      end
--    end
--  elseif refreshState == 1 then
--    physicalId, primId, dataId, value = telemetryPop()
--    if physicalId == 0x1A and primId == 0x32 and dataId == 0x0C30 then
--      fieldId = value % 256
--      if calibrationState == 2 then
--        if fieldId == 0x9D then
--          refreshState = 0
--          calibrationState = 0
--          calibrationStep = (calibrationStep + 1) % 6
--        end
--      else
--        field = fields[refreshIndex + 1]
--        if fieldId == field[3] then
--          value = math.floor(value / 256)
--          if field[3] >= 0x9E and field[3] <= 0xA0 then
--            b1 = value % 256
--            b2 = math.floor(value / 256)
--            value = b1*256 + b2
--            value = value - bit32.band(value, 0x8000) * 2
--          end
--          if field[2] == COMBO and #field == 6 then
--            for index = 1, #(field[6]), 1 do
--              if value == field[6][index] then
--                value = index - 1
--                break
--              end
--            end
--          elseif field[2] == VALUE and #field == 8 then
--            value = value - field[8] + field[5]
--          end
--          fields[refreshIndex + 1][4] = value
--          refreshIndex = refreshIndex + 1
--          refreshState = 0
--        end
--      end
--    elseif getTime() > telemetryPopTimeout then
--      refreshState = 0
--      calibrationState = 0
--    end
--  end
end

local function updateField(field)
  value = field[4]
  if field[2] == COMBO and #field == 6 then
    value = field[6][1+value]
  elseif field[2] == VALUE and #field == 8 then
    value = value + field[8] - field[5]
  end
  telemetryWrite(field[3], value)
end

-- Main
local function runFieldsPage(event)
  if event == EVT_EXIT_BREAK then             -- exit script
    return 2
  elseif event == EVT_ENTER_BREAK then        -- toggle editing/selecting current field
    if fields[current][4] ~= nil then
      edit = not edit
      if edit == false then
        updateField(fields[current])
      end
    end
  elseif edit then
    if event == EVT_PLUS_FIRST or event == EVT_PLUS_REPT then
      addField(1)
    elseif event == EVT_MINUS_FIRST or event == EVT_MINUS_REPT then
      addField(-1)
    end
  else
    if event == EVT_MINUS_FIRST then
      selectField(1)
    elseif event == EVT_PLUS_FIRST then
      selectField(-1)
    end
  end
  redrawFieldsPage()
  return 0
end

local function runNoDevicesPage(event)
  lcd.clear()
  lcd.drawText(24, 28, "Waiting for Crossfire devices...")
  return 0
end

local function runDevicePage(index, event)
  lcd.clear()
  lcd.drawScreenTitle(pages[index]["name"], index, #pages)
  drawDevicePage(pages[index])
  return 0
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
  elseif event == EVT_PAGE_BREAK then
    selectPage(1)
  elseif event == EVT_PAGE_LONG then
    killEvents(event);
    selectPage(-1)
  elseif event == EVT_EXIT_BREAK then
    return 2
  end

  if #pages == 0 then
    result = runNoDevicesPage(event)
  else
    result = runDevicePage(page, event)
  end

  refreshNext()

  return result
end

return { init=init, run=run }
