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

local version = "v1.01"

local VALUE = 0
local COMBO = 1

local edit = false
local page = 1
local current = 1
local refreshState = 0
local refreshIndex = 0
local pageOffset = 0
local pages = {}
local fields = {}
local modifications = {}
local margin = 1
local spacing = 8
local numberPerPage = 7

local configFields = {
    { "Self_stable mode", COMBO, 0x80, nil, { "disable", "enable"} },
    { "Sbus out", COMBO, 0x81, nil, { "disable", "enable"} },
    { "VOut1(V)", VALUE, 0xB2, nil, 50, 84},
    { "VOut2(V)", VALUE, 0xB3, nil, 50, 84},
}


local failsafeFields = {
    { "CH1 failsafe", VALUE, 0x82, 150, 80, 220},
    { "CH2 failsafe", VALUE, 0x82, 150, 80, 220},
    { "CH3 failsafe", VALUE, 0x83, 150, 80, 220},
    { "CH4 failsafe", VALUE, 0x83, 150, 80, 220},
    { "CH5 failsafe", VALUE, 0x84, 150, 80, 220},
    { "CH6 failsafe", VALUE, 0x84, 150, 80, 220},
    { "CH7 failsafe", VALUE, 0x85, 150, 80, 220},
    { "CH8 failsafe", VALUE, 0x85, 150, 80, 220},
    { "CH9 failsafe", VALUE, 0x86, 150, 80, 220},
    { "CH10 failsafe", VALUE, 0x86, 150, 80, 220},
    { "CH11 failsafe", VALUE, 0x87, 150, 80, 220},
    { "CH12 failsafe", VALUE, 0x87, 150, 80, 220},
    { "CH13 failsafe", VALUE, 0x88, 150, 80, 220},
    { "CH14 failsafe", VALUE, 0x88, 150, 80, 220},
    { "CH15 failsafe", VALUE, 0x89, 150, 80, 220},
    { "CH16 failsafe", VALUE, 0x89, 150, 80, 220},
    { "CH17 failsafe", VALUE, 0x8A, 150, 80, 220},
    { "CH18 failsafe", VALUE, 0x8A, 150, 80, 220},
    { "CH19 failsafe", VALUE, 0x8B, 150, 80, 220},
    { "CH20 failsafe", VALUE, 0x8B, 150, 80, 220},
    { "CH21 failsafe", VALUE, 0x8C, 150, 80, 220},
    { "CH22 failsafe", VALUE, 0x8C, 150, 80, 220},
    { "CH23 failsafe", VALUE, 0x8D, 150, 80, 220},
    { "CH24 failsafe", VALUE, 0x8D, 150, 80, 220},
}

local rx1PinMapFields = {
    { "CH1 RX1 map", VALUE, 0x8E, 1, 0, 16},
    { "CH2 RX1 map", VALUE, 0x8E, 2, 0, 16},
    { "CH3 RX1 map", VALUE, 0x8F, 3, 0, 16},
    { "CH4 RX1 map", VALUE, 0x8F, 4, 0, 16},
    { "CH5 RX1 map", VALUE, 0x90, 5, 0, 16},
    { "CH6 RX1 map", VALUE, 0x90, 6, 0, 16},
    { "CH7 RX1 map", VALUE, 0x91, 7, 0, 16},
    { "CH8 RX1 map", VALUE, 0x91, 8, 0, 16},
    { "CH9 RX1 map", VALUE, 0x92, 9, 0, 16},
    { "CH10 RX1 map", VALUE, 0x92, 10, 0, 16},
    { "CH11 RX1 map", VALUE, 0x93, 11, 0, 16},
    { "CH12 RX1 map", VALUE, 0x93, 12, 0, 16},
    { "CH13 RX1 map", VALUE, 0x94, 13, 0, 16},
    { "CH14 RX1 map", VALUE, 0x94, 14, 0, 16},
    { "CH15 RX1 map", VALUE, 0x95, 15, 0, 16},
    { "CH16 RX1 map", VALUE, 0x95, 16, 0, 16},
    { "CH17 RX1 map", VALUE, 0x96, 1, 0, 16},
    { "CH18 RX1 map", VALUE, 0x96, 2, 0, 16},
    { "CH19 RX1 map", VALUE, 0x97, 3, 0, 16},
    { "CH20 RX1 map", VALUE, 0x97, 4, 0, 16},
    { "CH21 RX1 map", VALUE, 0x98, 5, 0, 16},
    { "CH22 RX1 map", VALUE, 0x98, 6, 0, 16},
    { "CH23 RX1 map", VALUE, 0x99, 7, 0, 16},
    { "CH24 RX1 map", VALUE, 0x99, 8, 0, 16},
}

local rx2PinMapFields = {
    { "CH1 RX2 map", VALUE, 0x9A, 1, 0, 16},
    { "CH2 RX2 map", VALUE, 0x9A, 2, 0, 16},
    { "CH3 RX2 map", VALUE, 0x9B, 3, 0, 16},
    { "CH4 RX2 map", VALUE, 0x9B, 4, 0, 16},
    { "CH5 RX2 map", VALUE, 0x9C, 5, 0, 16},
    { "CH6 RX2 map", VALUE, 0x9C, 6, 0, 16},
    { "CH7 RX2 map", VALUE, 0x9D, 7, 0, 16},
    { "CH8 RX2 map", VALUE, 0x9D, 8, 0, 16},
    { "CH9 RX2 map", VALUE, 0x9E, 9, 0, 16},
    { "CH10 RX2 map", VALUE, 0x9E, 10, 0, 16},
    { "CH11 RX2 map", VALUE, 0x9F, 11, 0, 16},
    { "CH12 RX2 map", VALUE, 0x9F, 12, 0, 16},
    { "CH13 RX2 map", VALUE, 0xA0, 13, 0, 16},
    { "CH14 RX2 map", VALUE, 0xA0, 14, 0, 16},
    { "CH15 RX2 map", VALUE, 0xA1, 15, 0, 16},
    { "CH16 RX2 map", VALUE, 0xA0, 16, 0, 16},
    { "CH17 RX2 map", VALUE, 0xA2, 1, 0, 16},
    { "CH18 RX2 map", VALUE, 0xA2, 2, 0, 16},
    { "CH19 RX2 map", VALUE, 0xA3, 3, 0, 16},
    { "CH20 RX2 map", VALUE, 0xA3, 4, 0, 16},
    { "CH21 RX2 map", VALUE, 0xA4, 5, 0, 16},
    { "CH22 RX2 map", VALUE, 0xA4, 6, 0, 16},
    { "CH23 RX2 map", VALUE, 0xA5, 7, 0, 16},
    { "CH24 RX2 map", VALUE, 0xA5, 8, 0, 16},
}

local rx3PinMapFields = {
    { "CH1 RX3 map", VALUE, 0xA6, 1, 0, 16},
    { "CH2 RX3 map", VALUE, 0xA6, 2, 0, 16},
    { "CH3 RX3 map", VALUE, 0xA7, 3, 0, 16},
    { "CH4 RX3 map", VALUE, 0xA7, 4, 0, 16},
    { "CH5 RX3 map", VALUE, 0xA8, 5, 0, 16},
    { "CH6 RX3 map", VALUE, 0xA8, 6, 0, 16},
    { "CH7 RX3 map", VALUE, 0xA9, 7, 0, 16},
    { "CH8 RX3 map", VALUE, 0xA9, 8, 0, 16},
    { "CH9 RX3 map", VALUE, 0xAA, 9, 0, 16},
    { "CH10 RX3 map", VALUE, 0xAA, 10, 0, 16},
    { "CH11 RX3 map", VALUE, 0xAB, 11, 0, 16},
    { "CH12 RX3 map", VALUE, 0xAB, 12, 0, 16},
    { "CH13 RX3 map", VALUE, 0xAC, 13, 0, 16},
    { "CH14 RX3 map", VALUE, 0xAC, 14, 0, 16},
    { "CH15 RX3 map", VALUE, 0xAD, 15, 0, 16},
    { "CH16 RX3 map", VALUE, 0xAD, 16, 0, 16},
    { "CH17 RX3 map", VALUE, 0xAE, 1, 0, 16},
    { "CH18 RX3 map", VALUE, 0xAE, 2, 0, 16},
    { "CH19 RX3 map", VALUE, 0xAF, 3, 0, 16},
    { "CH20 RX3 map", VALUE, 0xAF, 4, 0, 16},
    { "CH21 RX3 map", VALUE, 0xB0, 5, 0, 16},
    { "CH22 RX3 map", VALUE, 0xB0, 6, 0, 16},
    { "CH23 RX3 map", VALUE, 0xB1, 7, 0, 16},
    { "CH24 RX3 map", VALUE, 0xB1, 8, 0, 16},
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
    if page == 2 and step < 0 and field[4] == min then
      field[4] = 0
    elseif page == 2 and step > 0 and field[4] == 0 then
      field[4] = min
    else
        if (step < 0 and field[4] > min) or (step > 0 and field[4] < max) then
            field[4] = field[4] + step
        end
    end
end

-- Select the next or previous page
local function selectPage(step)
    local pagesNum = #pages
    if  configFields[1][4] == 1 then
        pagesNum = 1
    elseif configFields[2][4] == 1 then
        pagesNum = pagesNum - 1
    end
    page = 1 + ((page + step - 1 + pagesNum) % pagesNum)
    refreshIndex = 0
    pageOffset = 0
end

-- Select the next or previous editable field
local function selectField(step)
    current = 1 + ((current + step - 1 + #fields) % #fields)
    if current > numberPerPage + pageOffset then
        pageOffset = current - numberPerPage
    elseif current <= pageOffset then
        pageOffset = current - 1
    end
end

local function drawProgressBar()
    if LCD_W == 480 then
        local width = (300 * refreshIndex) / #fields
        lcd.drawRectangle(100, 10, 300, 6)
        lcd.drawFilledRectangle(102, 12, width, 2)
    else
        local width = (60 * refreshIndex) / #fields
        lcd.drawRectangle(45, 1, 60, 6)
        lcd.drawFilledRectangle(47, 3, width, 2)
    end
end

-- Redraw the current page
local function redrawFieldPage()
    lcd.clear()

    local pagesNum = #pages
    if  configFields[1][4] == 1 then
        pagesNum = 1
    elseif configFields[2][4] == 1 then
        pagesNum = pagesNum - 1
    end
    drawScreenTitle("RB30/RB40", page, pagesNum)

    if refreshIndex < #fields then
        drawProgressBar()
    end

    for index = 1, numberPerPage, 1 do
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
                if page == 2 then
                    if field[4] == 0 then
                        lcd.drawText(LCD_W, margin + spacing * index, "No pulse", RIGHT + attr)
                    else
                        lcd.drawNumber(LCD_W, margin + spacing * index, field[4] * 10, RIGHT + attr)
                    end
                else
                    if field[4] == 0 then
                        lcd.drawText(LCD_W, margin + spacing * index, "No map", RIGHT + attr)
                    else
                        if field[3] == 0xB2 or field[3] == 0xB3 then
                            lcd.drawNumber(LCD_W, margin + spacing * index, field[4] , attr + RIGHT + PREC1)
                        else
                            lcd.drawNumber(LCD_W, margin + spacing * index, field[4], RIGHT + attr)
                        end
                    end
                end
            elseif field[2] == COMBO then
                if field[4] >= 0 and field[4] < #(field[5]) then
                    lcd.drawText(LCD_W, margin + spacing * index, field[5][field[4] + 1], RIGHT + attr)
                end
            end
        end
    end
end

local function telemetryIdle(field)
    return sportTelemetryPush(0x1C, 0x21, 0x0b80, field)
end

local function telemetryUnIdle(field)
    return sportTelemetryPush(0x1C, 0x20, 0x0b80, field)
end

local function telemetryRead(field)
    return sportTelemetryPush(0x1C, 0x30, 0x0b80, field)
end

local function telemetryWrite(field, value)
    return sportTelemetryPush(0x1C, 0x31, 0x0b80, field + value * 256)
end

local telemetryPopTimeout = 0
local function refreshNext()
    if refreshState == 0 then
        if #modifications > 0 then
            telemetryWrite(modifications[1][1], modifications[1][2])
            modifications[1] = nil
            refreshIndex = 0
            refreshState = 0
        elseif refreshIndex < #fields then
            local field = fields[refreshIndex + 1]
            if page > 1 then
                field = fields[refreshIndex + 2]
            end
            if telemetryRead(field[3]) == true then
                refreshState = 1
                telemetryPopTimeout = getTime() + 100 -- normal delay is 500ms
            end
        end
    elseif refreshState == 1 then
        local physicalId, primId, dataId, value = sportTelemetryPop()
        if primId == 0x32 and (dataId >= 0x0b80 or dataId <= 0x0b8f) then
            local fieldId = value % 256
            local field = fields[refreshIndex + 1]
            if fieldId == field[3] then
                value = math.floor(value / 256)
                if field[2] == COMBO then
                    fields[refreshIndex + 1][4] = value
                else
                    fields[refreshIndex + 1][4] = value % 256
                    if page > 1 then
                        value = math.floor(value / 256)
                        fields[refreshIndex + 2][4] =  value
                    end
                end
                if page > 1 then
                    refreshIndex = refreshIndex + 2
                else
                    refreshIndex = refreshIndex + 1
                end
                refreshState = 0
            end
        elseif getTime() > telemetryPopTimeout then
            fields[refreshIndex + 1][4] = nil
            if page > 1 then
                fields[refreshIndex + 2][4] = nil
                refreshIndex = refreshIndex + 2
            else
                refreshIndex = refreshIndex + 1
            end
            refreshState = 0
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

    if page > 1 then
        if current % 2 == 1 then
            value = value + fields[current + 1][4] * 256
        elseif current % 2 == 0 then
            value = fields[current - 1][4] + value * 256
        end
    end
    modifications[#modifications + 1] = { field[3], value }
end

-- Main
local function runFieldsPage(event)
    if event == EVT_VIRTUAL_EXIT then -- exit script
        telemetryUnIdle(0x80)
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
    redrawFieldPage()
    return 0
end

local function runConfigPage(event)
    fields = configFields
    return runFieldsPage(event)
end

local function runFailsafePage(event)
    fields = failsafeFields
    return runFieldsPage(event)
end

local function runRx1PinMapPage(event)
    fields = rx1PinMapFields
    return runFieldsPage(event)
end

local function runRx2PinMapPage(event)
    fields = rx2PinMapFields
    return runFieldsPage(event)
end

local function runRx3PinMapPage(event)
    fields = rx3PinMapFields
    return runFieldsPage(event)
end

-- Init
local function init()
    current, edit, refreshState, refreshIndex = 1, false, 0, 0
    if LCD_W == 480 then
        margin = 10
        spacing = 20
        numberPerPage = 12
    end
    pages = {
        runConfigPage,
        runFailsafePage,
        runRx1PinMapPage,
        runRx2PinMapPage,
        runRx3PinMapPage
    }    
    telemetryIdle(0x80)
end

-- Main
local function run(event)
    if event == nil then
        error("Cannot be run as a model script!")
        return 2
    elseif event == EVT_VIRTUAL_NEXT_PAGE then
        if edit == false then
            selectPage(1)
        end
    elseif event == EVT_VIRTUAL_PREV_PAGE then
        if edit == false then
            killEvents(event)
            selectPage(-1)
        end
    end

    local result = pages[page](event)
    refreshNext()

    return result
end

return { init = init, run = run }

