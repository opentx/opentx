local COLUMN_2 = 140

local pages = { }
local pageIndex = 1
local lineIndex = 0
local pageOffset = 0
local edit = false
local charIndex = 1
local fieldPopup = nil
local f = io.open("traces.txt", "w")

local function getField(line)
  local page = pages[pageIndex]
  local counter = 1
  local index = 1
  while 1 do
    local field = page.fields[index]
    if field == nil then
      return nil
    elseif field.hidden == 1 then
      -- continue
    elseif counter < line then
      counter = counter + 1
    else
      return field
    end
    index = index + 1
  end
end

local function initLineIndex()
  local index = 0
  repeat
    index = index + 1
    local field = getField(index)
    if field ~= nil and field.type ~= 11 and field.type ~= 12 and field.name ~= nil then
      lineIndex = index
      return
    end
  until index >= #pages[pageIndex].fields
  lineIndex = 0
end

-- Change display attribute to current field
local function incrField(step)
  local field = getField(lineIndex)
  if field.type == 10 then
    local byte = string.byte(field.value, charIndex) + step
    if byte < 32 then
      byte = 32
    elseif byte > 122 then
      byte = 122
    end
    field.value = string.sub(field.value, 1, charIndex-1) .. string.char(byte) .. string.sub(field.value, charIndex+1)
  else
    local min, max = 0, 0
    io.write(f, "field type:" .. field.type .. "\n")
    if field.type <= 5 then
      io.write(f, "field min:" .. field.min .. ", max:" .. field.max .. ", step:" .. field.step .. "\n")
      min = field.min
      max = field.max
      step = field.step * step
    elseif field.type == 9 then
      min = 0
      max = #field.values - 1
    end
    if (step < 0 and field.value > min) or (step > 0 and field.value < max) then
      field.value = field.value + step
      io.write(f, "field new value:" .. field.value .. "\n")
    end
  end
end

-- Select the next or previous page
local function selectPage(step)
  pageIndex = 1 + ((pageIndex + step - 1 + #pages) % #pages)
  initLineIndex()
end

-- Select the next or previous editable field
local function selectField(step)
  local newLineIndex = lineIndex
  local field
  repeat
    newLineIndex = newLineIndex + step
    if newLineIndex == 0 then
      newLineIndex = #pages[pageIndex].fields
    elseif newLineIndex == 1 + #pages[pageIndex].fields then
      newLineIndex = 1
    end
    field = getField(newLineIndex)
  until newLineIndex == lineIndex or (field ~= nil and field.type ~= 11 and field.type ~= 12 and field.name ~= nil)
  lineIndex = newLineIndex
  if lineIndex > 7 + pageOffset then
    pageOffset = lineIndex - 7
  elseif lineIndex <= pageOffset then
    pageOffset = lineIndex - 1
  end
end

local function drawDevicePage(page)
  for y = 1, 7, 1 do
    local field = getField(pageOffset+y)
    if field == nil then
      return
    elseif field.name == nil then
      lcd.drawText(0, 1+8*y, "...")
    else
      local attr = lineIndex == (pageOffset+y) and ((edit == true and BLINK or 0) + INVERS) or 0
      lcd.drawText(0, 1+8*y, field.name)
      if field.functions ~= nil then
        field.functions.display(field, 1+8*y, attr)
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
  local t = {}
  local i = 1
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

local function fieldGetValue(data, offset, size)
  local result = 0
  for i=0, size-1 do
    result = bit32.lshift(result, 8) + data[offset + i]
  end
  return result
end

local function fieldUnsignedSelectionLoad(field, data, offset, size)
  field.value = fieldGetValue(data, offset, size)
  field.min = fieldGetValue(data, offset+size, size)
  field.max = fieldGetValue(data, offset+2*size, size)
  field.default = fieldGetValue(data, offset+3*size, size)
  field.unit, offset = fieldGetString(data, offset+4*size)
  field.step = 1
  io.write(f, "  unsigned value:" .. field.value .. ", min:" .. field.min .. ", max:" .. field.max, ", step:" .. field.step .. ", unit:" .. field.unit .. "\n")
end

local function fieldUnsignedToSigned(field, size)
  local bandval = bit32.lshift(0x80, (size-1)*8)
  field.value = field.value - bit32.band(field.value, bandval) * 2
  field.min = field.min - bit32.band(field.min, bandval) * 2
  field.max = field.max - bit32.band(field.max, bandval) * 2
  field.default = field.default - bit32.band(field.default, bandval) * 2
  io.write(f, "  => to signed value:" .. field.value .. ", min:" .. field.min .. ", max:" .. field.max .. ", default:" .. field.default .. "\n")
end

local function fieldSignedSelectionLoad(field, data, offset, size)
  fieldUnsignedSelectionLoad(field, data, offset, size)
  fieldUnsignedToSigned(field, size)
end

local function fieldIntSave(index, value, size)
  local frame = { pages[pageIndex].id, 0xEA, index }
  for i=size-1, 0, -1 do
    frame[#frame + 1] = (bit32.rshift(value, 8*i) % 256)
  end
  crossfireTelemetryPush(0x2D, frame)
end

local function fieldUnsignedSelectionSave(field, size)
  local value = field.value
  fieldIntSave(field.index, value, size)
end

local function fieldSignedSelectionSave(field, size)
  local value = field.value
  if value < 0 then
    value = bit32.lshift(0x100, (size-1)*8) + value
  end
  io.write(f, "save value:" .. value .. "\n")
  fieldIntSave(field.index, value, size)
end

local function fieldIntDisplay(field, y, attr)
  lcd.drawNumber(COLUMN_2, y, field.value, LEFT + attr)
  lcd.drawText(lcd.getLastPos(), y, field.unit, attr)
end

-- UINT8
local function fieldUint8Load(field, data, offset)
  fieldUnsignedSelectionLoad(field, data, offset, 1)
end

local function fieldUint8Save(field)
  fieldUnsignedSelectionSave(field, 1)
end

-- INT8
local function fieldInt8Load(field, data, offset)
  fieldSignedSelectionLoad(field, data, offset, 1)
end

local function fieldInt8Save(field)
  fieldSignedSelectionSave(field, 1)
end

-- UINT16
local function fieldUint16Load(field, data, offset)
  fieldUnsignedSelectionLoad(field, data, offset, 2)
end

local function fieldUint16Save(field)

  fieldUnsignedSelectionSave(field, 2)
end

-- INT16
local function fieldInt16Load(field, data, offset)
  fieldSignedSelectionLoad(field, data, offset, 2)
end

local function fieldInt16Save(field)
  fieldSignedSelectionSave(field, 2)
end

-- UINT32
local function fieldUint32Load(field, data, offset)
  fieldUnsignedSelectionLoad(field, data, offset, 4)
end

local function fieldUint32Save(field)
  fieldUnsignedSelectionSave(field, 4)
end

-- INT32
local function fieldInt32Load(field, data, offset)
  fieldSignedSelectionLoad(field, data, offset, 4)
end

local function fieldInt32Save(field)
  fieldSignedSelectionSave(field, 4)
end

-- FLOAT
local function fieldFloatSelectionLoad(field, data, offset)
  field.value = fieldGetValue(data, offset, 4)
  field.min = fieldGetValue(data, offset+4, 4)
  field.max = fieldGetValue(data, offset+8, 4)
  field.default = fieldGetValue(data, offset+12, 4)
  fieldUnsignedToSigned(field, 4)
  field.prec = data[offset+16]
  if field.prec > 2 then
    field.prec = 2
  end
  field.step = fieldGetValue(data, offset+17, 4)
  field.unit, offset = fieldGetString(data, offset+21)
end

local function fieldFloatSelectionDisplay(field, y, attr)
  local attrnum
  if field.prec == 1 then
    attrnum = LEFT + attr + PREC1
  elseif field.prec == 2 then
    attrnum = LEFT + attr + PREC2
  else
    attrnum = LEFT + attr
  end
  lcd.drawNumber(COLUMN_2, y, field.value, attrnum)
  lcd.drawText(lcd.getLastPos(), y, field.unit, attr)
end

-- TEXT SELECTION
local function fieldTextSelectionLoad(field, data, offset)
  local values
  values, offset = fieldGetString(data, offset)
  field.values = split(values)
  field.value = data[offset]
  field.min = data[offset+1]
  field.max = data[offset+2]
  field.default = data[offset+3]
  field.unit, offset = fieldGetString(data, offset+4)
  io.write(f, "  value:" .. field.value .. "(" .. field.values[field.value+1] .. ")\n")
end

local function fieldTextSelectionSave(field)
  crossfireTelemetryPush(0x2D, { pages[pageIndex].id, 0xEA, field.index, field.value })
end

local function fieldTextSelectionDisplay(field, y, attr)
  lcd.drawText(COLUMN_2, y, field.values[field.value+1], attr)
  lcd.drawText(lcd.getLastPos(), y, field.unit, attr)
end

-- STRING
local function fieldStringLoad(field, data, offset)
  field.value, offset = fieldGetString(data, offset)
  if #data >= offset then
    field.maxlen = data[offset]
  end
end

local function fieldStringSave(field)
  local frame = { pages[pageIndex].id, 0xEA, field.index }
  for i=1, string.len(field.value) do
    frame[#frame + 1] = string.byte(field.value, i)
  end
  frame[#frame + 1] = 0
  crossfireTelemetryPush(0x2D, frame)
end

local function fieldStringDisplay(field, y, attr)
  if edit == true and attr ~= 0 then
    lcd.drawText(COLUMN_2, y, field.value, FIXEDWIDTH)
    lcd.drawText(COLUMN_2+6*charIndex-6, y, string.sub(field.value, charIndex, charIndex), FIXEDWIDTH + attr)
  else
    lcd.drawText(COLUMN_2, y, field.value, FIXEDWIDTH + attr)
  end
end

local function fieldCommandLoad(field, data, offset)
  field.status = data[offset]
  field.timeout = data[offset+1]
  field.info, offset = fieldGetString(data, offset+2)
  io.write(f, "  status:" .. field.status .. ", timeout:" .. field.timeout .. ", info=", field.info .. "\n")
  if field.status < 2 or field.status > 3 then
    fieldPopup = nil
  end
end

local fieldPopupTimeout = 0
local function fieldCommandSave(field)
  io.write(f, "Command send...\n")
  if field.status == 0 then
    field.status = 1
    local frame = { pages[pageIndex].id, 0xEA, field.index, field.status }
    crossfireTelemetryPush(0x2D, frame)
    fieldPopup = field
    fieldPopupTimeout = getTime() + field.timeout
  end
end

local function fieldCommandDisplay(field, y, attr)
  lcd.drawText(0, y, field.name, attr)
  if field.info ~= "" then
    lcd.drawText(COLUMN_2, y, "[" .. field.info .. "]")
  end
end

local types_functions = {
  { load=fieldUint8Load, save=fieldUint8Save, display=fieldIntDisplay },
  { load=fieldInt8Load, save=fieldInt8Save, display=fieldIntDisplay },
  { load=fieldUint16Load, save=fieldUint16Save, display=fieldIntDisplay },
  { load=fieldInt16Load, save=fieldInt16Save, display=fieldIntDisplay },
  { load=fieldUint32Load, save=fieldUint32Save, display=fieldIntDisplay },
  { load=fieldInt32Load, save=fieldInt32Save, display=fieldIntDisplay },
  nil,
  nil,
  { load=fieldFloatSelectionLoad, save=fieldInt32Save, display=fieldFloatSelectionDisplay },
  { load=fieldTextSelectionLoad, save=fieldTextSelectionSave, display=fieldTextSelectionDisplay },
  { load=fieldStringLoad, save=fieldStringSave, display=fieldStringDisplay },
  nil,
  { load=fieldStringLoad, save=nil, display=fieldStringDisplay },
  { load=fieldCommandLoad, save=fieldCommandSave, display=fieldCommandDisplay },
}

local function parseParameterInfoMessage(data)
  local page = pages[pageIndex]
  io.write(f, "  device:" .. data[2])
  if data[2] ~= page.id then
    return
  end
  local index = data[3]
  local field = page.fields[index]
  local chunks = data[4]
  if field.data then
    for i=5, #data do
      field.data[#field.data + 1] = data[i]
    end
    data = field.data
    io.write(f, " (" .. #field.data .. " bytes)")
  end
  if chunks > 0 then
    field.data = data
    if field.next_chunk then
      field.next_chunk = field.next_chunk + 1
    else
      field.next_chunk = 1
    end
    io.write(f, ", continued\n")
    page.fieldstimeout = 0
  else
    field.index = index
    field.parent = data[5]
    field.type = data[6] % 128
    field.hidden = bit32.rshift(data[6], 7)
    field.functions = types_functions[field.type+1]
    io.write(f, ", id:" .. index .. ", parent:" .. field.parent .. ", type:" .. field.type .. ", hidden:" .. field.hidden)
    local parent = field.parent
    local name = ""
    while parent ~= 0 do
      name = name .. " "
      parent = page.fields[parent].parent
    end
    local i = 7
    while data[i] ~= 0 do
      name = name .. string.char(data[i])
      i = i + 1
    end
    i = i + 1
    if name ~= "" then
      field.name = name
    end
    io.write(f, ", name:" .. field.name .. "\n")
    if field.functions ~= nil then
      field.functions.load(field, data, i)
    end
    page.fieldstimeout = 0
    if lineIndex == 0 and field.hidden == 0 and field.type and field.type ~= 11 and field.type ~= 12 then
      initLineIndex()
    end
  end
end

local devicesRefreshTimeout = 0
local paramsReceiveTimeout = 0
local function refreshNext()
  local command, data = crossfireTelemetryPop()
  if command == nil then
    local time = getTime()
    if fieldPopup ~= nil then
      if time > fieldPopupTimeout then
        io.write(f, "Poll command status...\n")
        local frame = { pages[pageIndex].id, 0xEA, fieldPopup.index }
        crossfireTelemetryPush(0x2D, frame)
        fieldPopupTimeout = time + fieldPopup.timeout
      end
    elseif time > devicesRefreshTimeout then
      if #pages == 0 then
        devicesRefreshTimeout = time + 100 -- 1s
      else
        devicesRefreshTimeout = time + 1000 -- 10s
      end
      io.write(f, "Ping devices...\n")
      crossfireTelemetryPush(0x28, { 0x00, 0xEA })
    elseif time > paramsReceiveTimeout and pageIndex <= #pages and time > pages[pageIndex].fieldstimeout then
      for i=1, #pages[pageIndex].fields do
        local field = pages[pageIndex].fields[i]
        if field.name == nil then
          local chunk = 0
          if field.next_chunk then
            chunk = field.next_chunk
          end
          io.write(f, "Request parameter " .. i .. "(" .. chunk .. ")...\n")
          crossfireTelemetryPush(0x2C, { pages[pageIndex].id, 0xEA, i, chunk })
          paramsReceiveTimeout = time + 200 -- 2s
          pages[pageIndex].fieldstimeout = paramsReceiveTimeout
          break
        end
      end
    end
  elseif command == 0x29 then
    io.write(f, "Device info received...\n")
    parseDeviceInfoMessage(data)
  elseif command == 0x2B then
    io.write(f, "Parameter info received...\n")
    parseParameterInfoMessage(data)
    paramsReceiveTimeout = 0
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

local function runDevicePage(event)
  local page = pages[pageIndex]
  if event == EVT_EXIT_BREAK then             -- exit script
    if edit == true then
      edit = false
    else
      return 2
    end
  elseif event == EVT_ENTER_BREAK then        -- toggle editing/selecting current field
    local field = getField(lineIndex)
    if field.name ~= nil then
      if field.type == 10 then
        if edit == false then
          edit = true
          charIndex = 1
        else
          charIndex = charIndex + 1
        end
      elseif field.type ~= 13 then
        edit = not edit
      end
      if edit == false and field.functions.save ~= nil then
        field.functions.save(field)
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
  lcd.drawScreenTitle(page.name, pageIndex, #pages)
  drawDevicePage(page)
  return 0
end

local function runPopupPage(event)
  local result
  if fieldPopup.status == 3 then
    result = popupConfirmation(fieldPopup.info, event)
  else
    result = popupWarning(fieldPopup.info, event)
  end
  if result == "OK" then
    io.write(f, "Command confirm...\n")
    local frame = { pages[pageIndex].id, 0xEA, fieldPopup.index, 4 }
    crossfireTelemetryPush(0x2D, frame)
  elseif result == "CANCEL" then
    io.write(f, "Command cancel...\n")
    local frame = { pages[pageIndex].id, 0xEA, fieldPopup.index, 5 }
    crossfireTelemetryPush(0x2D, frame)
  end
  return 0
end

-- Init
local function init()
  lineIndex, edit = 0, false
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
  elseif fieldPopup ~= nil then
    result = runPopupPage(event)
  else
    result = runDevicePage(event)
  end

  refreshNext()

  return result
end

return { init=init, run=run }
