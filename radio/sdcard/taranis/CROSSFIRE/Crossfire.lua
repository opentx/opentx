local COLUMN_2 = 140

local edit = false
local pageIndex = 1
local fieldIndex = 1
local pageOffset = 0
local pages = { }

-- Change display attribute to current field
local function incrField(step)
  local fields = pages[pageIndex].fields
  local field = fields[fieldIndex]
  local min, max = 0, 0
  if field.type <= 5 then
    min = field.min
    max = field.max
  elseif field.type == 9 then
    min = 0
    max = #field.values - 1
  end
  if (step < 0 and field.value > min) or (step > 0 and field.value < max) then
    field.value = field.value + step
  end
end

-- Select the next or previous page
local function selectPage(step)
  pageIndex = 1 + ((pageIndex + step - 1 + #pages) % #pages)
end

-- Select the next or previous editable field
local function selectField(step)
  local fields = pages[pageIndex].fields
  local newFieldIndex = fieldIndex
  repeat
    newFieldIndex = 1 + ((newFieldIndex + step - 1 + #fields) % #fields)
  until newFieldIndex == fieldIndex or (pages[pageIndex].fields[newFieldIndex].type ~= 11 and pages[pageIndex].fields[newFieldIndex].name ~= nil)
  fieldIndex = newFieldIndex
  if fieldIndex > 7 + pageOffset then
    pageOffset = fieldIndex - 7
  elseif fieldIndex <= pageOffset then
    pageOffset = fieldIndex - 1
  end
end

local function drawDevicePage(page)
  for index = 1, 7, 1 do
    local field = page.fields[pageOffset+index]
    if field == nil then
      break
    end
    
    if field.name == nil then
      lcd.drawText(0, 1+8*index, "...")
    else
      local attr = fieldIndex == (pageOffset+index) and ((edit == true and BLINK or 0) + INVERS) or 0
      lcd.drawText(0, 1+8*index, field.name)
      if field.functions ~= nil then
        field.functions.display(field, 1+8*index, attr)
      end

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
  local newpage = {
    id = id,
    name = name,
    pagetimeout = 0,
    fields = {},
    fieldstimeout = 0,
  }
  for i=1, fields_count do
    newpage.fields[i] = { name=nil }
  end
  return newpage
end

local function getPage(name)
  for i=1, #pages do
    if pages[i].name == name then
      return pages[i]
    end
  end
  return nil
end

local function parseDeviceInfoMessage(data)
  local id = data[2]
  local name = ""
  local i = 3
  while data[i] ~= 0 do
    name = name .. string.char(data[i])
    i = i + 1
  end
  local fields_count = data[i+13]
  local pg = getPage(name)
  if pg == nil then
    pg = createPage(id, name, fields_count)
    pages[#pages + 1] = pg
  end
  local time = getTime()
  pg.pagetimeout = time + 3000 -- 30s
end

local function split(inputstr)
  local t={}
  local i=1
  for str in string.gmatch(inputstr, "([^;]+)") do
    t[i] = str
    i = i + 1
  end
  return t
end

local function fieldGetString(data, offset)
  local result = ""
  while data[offset] ~= 0 do
    result = result .. string.char(data[offset])
    offset = offset + 1
  end
  offset = offset + 1
  return result, offset
end

-- UINT8
local function fieldUint8SelectionLoad(field, data, offset)
  field.value = data[offset]
  field.min = data[offset+1]
  field.max = data[offset+2]
  field.unit, offset = fieldGetString(data, offset+4)
end

local function fieldUint8SelectionSave(fieldIndex)
  crossfireTelemetryPush(0x2D, { pages[pageIndex].id, 0xEA, fieldIndex, pages[pageIndex].fields[fieldIndex].value })
end

local function fieldIntSelectionDisplay(field, y, attr)
  lcd.drawNumber(COLUMN_2, y, field.value, LEFT + attr)
  lcd.drawText(lcd.getLastPos(), y, field.unit, attr)
end

-- UINT16
local function fieldUint16SelectionLoad(field, data, offset)
  field.value = bit32.lshift(data[offset], 8) + data[offset+1]
  field.min = bit32.lshift(data[offset+2], 8) + data[offset+3]
  field.max = bit32.lshift(data[offset+4], 8) + data[offset+5]
  field.unit, offset = fieldGetString(data, offset+6)
end

local function fieldUint16SelectionSave(fieldIndex)
  crossfireTelemetryPush(0x2D, { pages[pageIndex].id, 0xEA, fieldIndex, bit32.rshift(pages[pageIndex].fields[fieldIndex].value, 8), pages[pageIndex].fields[fieldIndex].value % 255 })
end

-- UINT32
local function fieldUint32SelectionLoad(field, data, offset)
  field.value = bit32.lshift(data[offset], 24) + bit32.lshift(data[offset+1], 16) + bit32.lshift(data[offset+2], 8) + data[offset+3]
  field.min = bit32.lshift(data[offset+4], 24) + bit32.lshift(data[offset+5], 16) + bit32.lshift(data[offset+6], 8) + data[offset+7]
  field.max = bit32.lshift(data[offset+8], 24) + bit32.lshift(data[offset+9], 16) + bit32.lshift(data[offset+10], 8) + data[offset+11]
  field.unit, offset = fieldGetString(data, offset+12)
end

local function fieldUint32SelectionSave(fieldIndex)
  crossfireTelemetryPush(0x2D, { pages[pageIndex].id, 0xEA, fieldIndex, bit32.rshift(pages[pageIndex].fields[fieldIndex].value, 24), bit32.rshift(pages[pageIndex].fields[fieldIndex].value, 16) % 255, bit32.rshift(pages[pageIndex].fields[fieldIndex].value, 8) % 255, pages[pageIndex].fields[fieldIndex].value % 255 })
end

-- TEXT SELECTION
local function fieldTextSelectionLoad(field, data, offset)
  local values
  values, offset = fieldGetString(data, offset)
  field.values = split(values)
  field.value = data[offset]
end

local function fieldTextSelectionSave(fieldIndex)
  crossfireTelemetryPush(0x2D, { pages[pageIndex].id, 0xEA, fieldIndex, pages[pageIndex].fields[fieldIndex].value })
end

local function fieldTextSelectionDisplay(field, y, attr)
  lcd.drawText(COLUMN_2, y, field.values[field.value+1], attr)
end

-- STRING
local function fieldStringSelectionLoad(field, data, offset)
  field.value, offset = fieldGetString(data, offset)
  if #data >= offset then
    field.maxlen = data[offset]
  end
end

local function fieldStringSelectionSave(fieldIndex)
  -- crossfireTelemetryPush(0x2D, { pages[pageIndex].id, 0xEA, fieldIndex,  })
end

local function fieldStringSelectionDisplay(field, y, attr)
  lcd.drawText(COLUMN_2, y, field.value, attr)
end

local types_functions = {
  { load=fieldUint8SelectionLoad, save=fieldUint8SelectionSave, display=fieldIntSelectionDisplay },
  { load=fieldUint8SelectionLoad, save=fieldUint8SelectionSave, display=fieldIntSelectionDisplay },
  { load=fieldUint16SelectionLoad, save=fieldUint16SelectionSave, display=fieldIntSelectionDisplay },
  { load=fieldUint16SelectionLoad, save=fieldUint16SelectionSave, display=fieldIntSelectionDisplay },
  { load=fieldUint32SelectionLoad, save=fieldUint32SelectionSave, display=fieldIntSelectionDisplay },
  { load=fieldUint32SelectionLoad, save=fieldUint32SelectionSave, display=fieldIntSelectionDisplay },
  nil,
  nil,
  nil,
  { load=fieldTextSelectionLoad, save=fieldTextSelectionSave, display=fieldTextSelectionDisplay },
  { load=fieldStringSelectionLoad, save=fieldStringSelectionSave, display=fieldStringSelectionDisplay },
  nil,
  { load=fieldStringSelectionLoad, save=fieldStringSelectionSave, display=fieldStringSelectionDisplay },
  
}

local function parseParameterInfoMessage(data)
  local page = pages[pageIndex]
  local index = data[3]
  local field = page.fields[index]
  field.parent = data[4]
  field.type = data[5]
  field.functions = types_functions[field.type+1]
  local parent = field.parent
  local name = ""
  while parent ~= 0 do
    name = name .. " "
    parent = page.fields[parent].parent
  end
  local i = 6
  while data[i] ~= 0 do
    name = name .. string.char(data[i])
    i = i + 1
  end
  i = i + 1
  field.name = name
  if field.functions ~= nil then
    field.functions.load(field, data, i)
  end
  if field.type ~= 11 and page.fields[fieldIndex].type == 11 then
    fieldIndex = index
  end
  page.fieldstimeout = 0
end

local devicesRefreshTimeout = 0
local function refreshNext()
  local command, data = crossfireTelemetryPop()
  if command == nil then
    local time = getTime()
    if time > devicesRefreshTimeout then
      if #pages == 0 then
        devicesRefreshTimeout = time + 100 -- 1s
      else
        devicesRefreshTimeout = time + 1000 -- 10s
      end
      crossfireTelemetryPush(0x28, { 0x00, 0xEA })
    elseif pageIndex <= #pages and time > pages[pageIndex].fieldstimeout then
      for i=1, #pages[pageIndex].fields do
        local field = pages[pageIndex].fields[i]
        if field.name == nil then
          crossfireTelemetryPush(0x2C, { pages[pageIndex].id, 0xEA, i })
          pages[pageIndex].fieldstimeout = time + 200 -- 2s
        end
      end
    end
  elseif command == 0x29 then
    parseDeviceInfoMessage(data)
  elseif command == 0x2B then
    parseParameterInfoMessage(data)
  end
end

-- Main
local function runNoDevicesPage(event)
  if event == EVT_EXIT_BREAK then
    return 2
  end
  lcd.clear()
  lcd.drawText(24, 28, "Waiting for Crossfire devices...")
  return 0
end

local function runDevicePage(index, event)
  local page = pages[index]
  if event == EVT_EXIT_BREAK then             -- exit script
    return 2
  elseif event == EVT_ENTER_BREAK then        -- toggle editing/selecting current field
    local field = page.fields[fieldIndex]
    if field.name ~= nil then
      edit = not edit
      if edit == false and field.functions.save ~= nil then
        field.functions.save(fieldIndex)
      end
    end
  elseif edit then
    if event == EVT_PLUS_FIRST or event == EVT_PLUS_REPT then
      incrField(1)
    elseif event == EVT_MINUS_FIRST or event == EVT_MINUS_REPT then
      incrField(-1)
    end
  else
    if event == EVT_MINUS_FIRST then
      selectField(1)
    elseif event == EVT_PLUS_FIRST then
      selectField(-1)
    end
  end

  lcd.clear()
  lcd.drawScreenTitle(page.name, index, #pages)
  drawDevicePage(page)
  return 0
end

-- Init
local function init()
  fieldIndex, edit = 1, false
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
  end

  local result
  if #pages == 0 then
    result = runNoDevicesPage(event)
  else
    result = runDevicePage(pageIndex, event)
  end

  refreshNext()

  return result
end

return { init=init, run=run }
