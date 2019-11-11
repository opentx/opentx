--- - #########################################################################
---- #                                                                       #
---- # Copyright (C) OpenTX                                                  #
----- #                                                                       #
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

local version = "v2.00"

local VALUE = 0
local COMBO = 1

local COLUMN_2 = 300

local edit = false
local page = 1
local current = 1
local refreshState = 0
local refreshIndex = 0
local calibrationState = 0
local pageOffset = 0
local calibrationStep = 0
local pages = {}
local fields = {}
local modifications = {}
local wingBitmaps = {}
local mountBitmaps = {}
local margin = 1
local spacing = 8
local counter = 0

local configFields = {
    { "Wing type", COMBO, 0x80, nil, { "Normal", "Delta", "VTail" } },
    { "Mounting type", COMBO, 0x81, nil, { "Horz", "Horz rev.", "Vert", "Vert rev." } },
}

local wingBitmapsFile = { "/SCRIPTS/TOOLS/bmp/plane.bmp", "/SCRIPTS/TOOLS/bmp/delta.bmp", "/SCRIPTS/TOOLS/bmp/vtail.bmp" }
local mountBitmapsFile = { "/SCRIPTS/TOOLS/bmp/horz.bmp", "/SCRIPTS/TOOLS/bmp/horz-r.bmp", "/SCRIPTS/TOOLS/bmp/vert.bmp", "/SCRIPTS/TOOLS/bmp/vert-r.bmp" }

local settingsFields = {
    {"SxR functions", COMBO, 0x9C, nil, { "Disable", "Enable" } },
    {"Quick Mode:", COMBO, 0xAA, nil, { "Disable", "Enable" } },
    {"CH5 mode", COMBO, 0xA8, nil, { "AIL2", "AUX1" } },
    {"CH6 mode", COMBO, 0xA9, nil, { "ELE2", "AUX2" } },
    {"AIL direction", COMBO, 0x82, nil, { "Normal", "Invers" }, { 255, 0 } },
    {"ELE direction", COMBO, 0x83, nil, { "Normal", "Invers" }, { 255, 0 } },
    {"RUD direction", COMBO, 0x84, nil, { "Normal", "Invers" }, { 255, 0 } },
    {"AIL2 direction", COMBO, 0x9A, nil, { "Normal", "Invers" }, { 255, 0 } },
    {"ELE2 direction", COMBO, 0x9B, nil, { "Normal", "Invers" }, { 255, 0 } },
    {"AIL stab gain", VALUE, 0x85, nil, 0, 200, "%"},
    {"ELE stab gain", VALUE, 0x86, nil, 0, 200, "%"},
    {"RUD stab gain", VALUE, 0x87, nil, 0, 200, "%"},
    {"AIL autolvl gain", VALUE, 0x88, nil, 0, 200, "%"},
    {"ELE autolvl gain", VALUE, 0x89, nil, 0, 200, "%"},
    {"ELE hover gain", VALUE, 0x8C, nil, 0, 200, "%"},
    {"RUD hover gain", VALUE, 0x8D, nil, 0, 200, "%"},
    {"AIL knife gain", VALUE, 0x8E, nil, 0, 200, "%"},
    {"RUD knife gain", VALUE, 0x90, nil, 0, 200, "%"},
    {"AIL autolvl offset", VALUE, 0x91, nil, -20, 20, "%", 0x6C},
    {"ELE autolvl offset", VALUE, 0x92, nil, -20, 20, "%", 0x6C},
    {"ELE hover offset", VALUE, 0x95, nil, -20, 20, "%", 0x6C},
    {"RUD hover offset", VALUE, 0x96, nil, -20, 20, "%", 0x6C},
    {"AIL knife offset", VALUE, 0x97, nil, -20, 20, "%", 0x6C},
    {"RUD knife offset", VALUE, 0x99, nil, -20, 20, "%", 0x6C},
}

local calibrationFields = {
    { "X:", VALUE, 0x9E, 0, -100, 100, "%" },
    { "Y:", VALUE, 0x9F, 0, -100, 100, "%" },
    { "Z:", VALUE, 0xA0, 0, -100, 100, "%" }
}

local function drawScreenTitle(title, page, pages)
    if math.fmod(math.floor(getTime() / 100), 10) == 0 then
        title = version
    end
    if LCD_W == 480 then
        lcd.drawFilledRectangle(0, 0, LCD_W, 30, TITLE_BGCOLOR)
        lcd.drawText(1, 5, title, MENU_TITLE_COLOR)
        lcd.drawText(LCD_W - 40, 5, page .. "/" .. pages, MENU_TITLE_COLOR)
    else
        lcd.drawScreenTitle(title, page, pages)
    end
end

-- Change display attribute to current field
local function addField(step)
    local field = fields[current]
    local min, max
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
    pageOffset = 0
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
    if LCD_W == 480 then
        local width = (300 * refreshIndex) / #fields
        lcd.drawRectangle(100, 10, 300, 6)
        lcd.drawFilledRectangle(102, 13, width, 2);
    else
        local width = (60 * refreshIndex) / #fields
        lcd.drawRectangle(45, 1, 60, 6)
        lcd.drawFilledRectangle(47, 3, width, 2);
    end
end

-- Redraw the current page
local function redrawFieldsPage(event)
    lcd.clear()
    drawScreenTitle("SxR", page, #pages)

    if refreshIndex < #fields then
        drawProgressBar()
    end

    for index = 1, 10, 1 do
        local field = fields[pageOffset + index]
        if field == nil then
            break
        end

        local attr = current == (pageOffset + index) and ((edit == true and BLINK or 0) + INVERS) or 0

        lcd.drawText(1, margin + spacing * index, field[1], attr)

        if field[4] == nil then
            lcd.drawText(LCD_W, margin + spacing * index, "---", RIGHT + attr)
        else
            if field[2] == VALUE then
                lcd.drawNumber(LCD_W, margin + spacing * index, field[4], RIGHT + attr)
            elseif field[2] == COMBO then
                if field[4] >= 0 and field[4] < #(field[5]) then
                    lcd.drawText(LCD_W, margin + spacing * index, field[5][1 + field[4]], RIGHT + attr)
                end
            end
        end
    end
end

local function telemetryRead(field)
    return sportTelemetryPush(0x17, 0x30, 0x0C30, field)
end

local function telemetryWrite(field, value)
    return sportTelemetryPush(0x17, 0x31, 0x0C30, field + value * 256)
end

local telemetryPopTimeout = 0
local function refreshNext()
    if refreshState == 0 then
        if calibrationState == 1 then
            if telemetryWrite(0x9D, calibrationStep) == true then
                refreshState = 1
                calibrationState = 2
                telemetryPopTimeout = getTime() + 120 -- normal delay is 500ms
            end
        elseif #modifications > 0 then
            telemetryWrite(modifications[1][1], modifications[1][2])
            modifications[1] = nil
        elseif refreshIndex < #fields then
            local field = fields[refreshIndex + 1]
            if telemetryRead(field[3]) == true then
                refreshState = 1
                telemetryPopTimeout = getTime() + 80 -- normal delay is 500ms
            end
        end
    elseif refreshState == 1 then
        local physicalId, primId, dataId, value = sportTelemetryPop()
        if physicalId == 0x1A and primId == 0x32 and dataId == 0x0C30 then
            local fieldId = value % 256
            if calibrationState == 2 then
                if fieldId == 0x9D then
                    refreshState = 0
                    calibrationState = 0
                    calibrationStep = (calibrationStep + 1) % 7
                end
            else
                local field = fields[refreshIndex + 1]
                if fieldId == field[3] then
                    local value = math.floor(value / 256)
                    if field[3] == 0xAA then
                        value = bit32.band(value, 0x0001)
                    end
                    if field[3] >= 0x9E and field[3] <= 0xA0 then
                        local b1 = value % 256
                        local b2 = math.floor(value / 256)
                        value = b1 * 256 + b2
                        value = value - bit32.band(value, 0x8000) * 2
                    end
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
                end
            end
        elseif getTime() > telemetryPopTimeout then
            fields[refreshIndex + 1][4] = nil
            refreshIndex = refreshIndex + 1
            refreshState = 0
            calibrationState = 0
        end
    end
end

local function updateField(field)
    local value = field[4]
    if field[2] == COMBO and #field == 6 then
        value = field[6][1 + value]
    elseif field[2] == VALUE and #field == 8 then
        value = value + field[8] - field[5]
    end
    modifications[#modifications + 1] = { field[3], value }
end

-- Main
local function runFieldsPage(event)
    if event == EVT_VIRTUAL_EXIT then -- exit script
        return 2
    elseif event == EVT_VIRTUAL_ENTER then -- toggle editing/selecting current field
        if fields[current][4] ~= nil then
            edit = not edit
            if edit == false then
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

local function runConfigPage(event)
    fields = configFields
    local result = runFieldsPage(event)
    if LCD_W == 128 then
        local mountText = { "Label is facing the sky", "Label is facing ground", "Label is left when", "Label is right when" }
        if fields[2][4] ~= nil then
            lcd.drawText(1, 30, "Pins toward tail")
            lcd.drawText(1, 40, mountText[1 + fields[2][4]])
            if fields[2][4] > 1 then
                lcd.drawText(1, 50, "looking from the tail")
            end
        end
    else
        if fields[1][4] ~= nil then
            if LCD_W == 480 then
                if wingBitmaps[1 + fields[1][4]] == nil then
                    wingBitmaps[1 + fields[1][4]] = Bitmap.open(wingBitmapsFile[1 + fields[1][4]])
                end
                lcd.drawBitmap(wingBitmaps[1 + fields[1][4]], 10, 90)
            else
                lcd.drawPixmap(20, 28, wingBitmapsFile[1 + fields[1][4]])
            end
        end
        if fields[2][4] ~= nil then
            if LCD_W == 480 then
                if mountBitmaps[1 + fields[2][4]] == nil then
                    mountBitmaps[1 + fields[2][4]] = Bitmap.open(mountBitmapsFile[1 + fields[2][4]])
                end
                lcd.drawBitmap(mountBitmaps[1 + fields[2][4]], 190, 110)
            else
                lcd.drawPixmap(128, 28, mountBitmapsFile[1 + fields[2][4]])
            end
        end
    end
    return result
end

local function runSettingsPage(event)
    fields = settingsFields
    return runFieldsPage(event)
end

-- Init
local function init()
    current, edit, refreshState, refreshIndex = 1, false, 0, 0
    if LCD_W == 480 then
        margin = 10
        spacing = 20
        wingBitmapsFile = { "/SCRIPTS/TOOLS/img/plane_b.png", "/SCRIPTS/TOOLS/img/delta_b.png", "/SCRIPTS/TOOLS/img/planev_b.png" }
        mountBitmapsFile = { "/SCRIPTS/TOOLS/img/up.png", "/SCRIPTS/TOOLS/img/down.png", "/SCRIPTS/TOOLS/img/vert.png", "/SCRIPTS/TOOLS/img/vert-r.png" }
    end
    pages = {
        runConfigPage,
        runSettingsPage,
    }
end

-- Main
local function run(event)
    if event == nil then
        error("Cannot be run as a model script!")
        return 2
    elseif event == EVT_VIRTUAL_NEXT_PAGE then
        selectPage(1)
    elseif event == EVT_VIRTUAL_PREV_PAGE then
        killEvents(event);
        selectPage(-1)
    end

    local result = pages[page](event)
    refreshNext()

    return result
end

return { init = init, run = run }

