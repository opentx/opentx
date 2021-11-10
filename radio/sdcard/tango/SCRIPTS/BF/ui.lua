local pageStatus =
{
    display     = 2,
    editing     = 3,
    saving      = 4,
    displayMenu = 5,
}

local uiMsp =
{
    reboot = 68,
    eepromWrite = 250
}

local currentState = pageStatus.display
local requestTimeout = 80 -- 800ms request timeout
local currentPage = 1
local currentLine = 1
local saveTS = 0
local saveTimeout = 0
local saveRetries = 0
local saveMaxRetries = 0
local pageRequested = false
local telemetryScreenActive = false
local menuActive = false
local lastRunTS = 0
local killEnterBreak = 0
local scrollPixelsY = 0

local Page = nil

local backgroundFill = TEXT_BGCOLOR or ERASE
local foregroundColor = LINE_COLOR or SOLID

local globalTextOptions = TEXT_COLOR or 0

local function saveSettings(new)
    if Page.values then
        if Page.preSave then
            payload = Page.preSave(Page)
        else
            payload = {}
            for i=1,(Page.outputBytes or #Page.values) do
                payload[i] = Page.values[i]
            end
        end
        protocol.mspWrite(Page.write, payload)
        saveTS = getTime()
        if currentState == pageStatus.saving then
            saveRetries = saveRetries + 1
        else
            currentState = pageStatus.saving
            saveRetries = 0
            saveMaxRetries = protocol.saveMaxRetries or 2 -- default 2
            saveTimeout = protocol.saveTimeout or 150     -- default 1.5s
        end
    end
end

local function invalidatePages()
    Page = nil
    currentState = pageStatus.display
    saveTS = 0
end

local function rebootFc()
    protocol.mspRead(uiMsp.reboot)
    invalidatePages()
end

local function eepromWrite()
    protocol.mspRead(uiMsp.eepromWrite)
end

local menuList = {
    {
        t = "save page",
        f = saveSettings
    },
    {
        t = "reload",
        f = invalidatePages
    },
    {
        t = "reboot",
        f = rebootFc
    }
}

local function processMspReply(cmd,rx_buf)
    if cmd == nil or rx_buf == nil then
        return
    end
    if cmd == Page.write then
        if Page.eepromWrite then
            eepromWrite()
        else
            invalidatePages()
        end
        pageRequested = false
        return
    end
    if cmd == uiMsp.eepromWrite then
        if Page.reboot then
            rebootFc()
        end
        invalidatePages()
        return
    end
    if cmd ~= Page.read then
        return
    end
    if #(rx_buf) > 0 then
        Page.values = {}
        for i=1,#(rx_buf) do
            Page.values[i] = rx_buf[i]
        end

        for i=1,#(Page.fields) do
            if (#(Page.values) or 0) >= Page.minBytes then
                local f = Page.fields[i]
                if f.vals then
                    f.value = 0;
                    for idx=1, #(f.vals) do
                        local raw_val = (Page.values[f.vals[idx]] or 0)
                        raw_val = bit32.lshift(raw_val, (idx-1)*8)
                        f.value = bit32.bor(f.value, raw_val)
                    end
                    f.value = f.value/(f.scale or 1)
                end
            end
        end
        if Page.postLoad then
            Page.postLoad(Page)
        end
    end
end

local function incMax(val, inc, base)
    return ((val + inc + base - 1) % base) + 1
end

local function incPage(inc)
    currentPage = incMax(currentPage, inc, #(PageFiles))
    Page = nil
    currentLine = 1
    collectgarbage()
end

local function incLine(inc)
    currentLine = clipValue(currentLine + inc, 1, #(Page.fields))
end

local function incMenu(inc)
    menuActive = clipValue(menuActive + inc, 1, #(menuList))
end

local function requestPage()
    if Page.read and ((Page.reqTS == nil) or (Page.reqTS + requestTimeout <= getTime())) then
        Page.reqTS = getTime()
        protocol.mspRead(Page.read)
    end
end

function drawScreenTitle(screen_title)
    if radio.resolution == lcdResolution.low then
        lcd.drawFilledRectangle(0, 0, LCD_W, 10)
        lcd.drawText(1,1,screen_title,INVERS)
    else
        lcd.drawFilledRectangle(0, 0, LCD_W, 30, TITLE_BGCOLOR)
        lcd.drawText(5,5,screen_title, MENU_TITLE_COLOR)
    end
end

local function drawScreen()
    local yMinLim = Page.yMinLimit or 0
    local yMaxLim = Page.yMaxLimit or LCD_H
    local currentLineY = Page.fields[currentLine].y
    local screen_title = Page.title
    drawScreenTitle("Betaflight / "..screen_title)
    if currentLineY <= Page.fields[1].y then
        scrollPixelsY = 0
    elseif currentLineY - scrollPixelsY <= yMinLim then
        scrollPixelsY = currentLineY - yMinLim
    elseif currentLineY - scrollPixelsY >= yMaxLim then
        scrollPixelsY = currentLineY - yMaxLim
    end
    for i=1,#(Page.text) do
        local f = Page.text[i]
        local textOptions = (f.to or 0) + globalTextOptions
        if (f.y - scrollPixelsY) >= yMinLim and (f.y - scrollPixelsY) <= yMaxLim then
            lcd.drawText(f.x, f.y - scrollPixelsY, f.t, textOptions)
        end
    end
    local val = "---"
    for i=1,#(Page.fields) do
        local f = Page.fields[i]
        local text_options = (f.to or 0) + globalTextOptions
        local heading_options = text_options
        local value_options = text_options
        if i == currentLine then
            value_options = text_options + INVERS
            if currentState == pageStatus.editing then
                value_options = value_options + BLINK
            end
        end
        local spacing = 20
        if f.t ~= nil then
            if (f.y - scrollPixelsY) >= yMinLim and (f.y - scrollPixelsY) <= yMaxLim then
                lcd.drawText(f.x, f.y - scrollPixelsY, f.t, heading_options)
            end
            if f.sp ~= nil then
                spacing = f.sp
            end
        else
            spacing = 0
        end
        if f.value then
            if f.upd and Page.values then
                f.upd(Page)
            end
            val = f.value
            if f.table and f.table[f.value] then
                val = f.table[f.value]
            end
        end
        if (f.y - scrollPixelsY) >= yMinLim and (f.y - scrollPixelsY) <= yMaxLim then
            lcd.drawText(f.x + spacing, f.y - scrollPixelsY, val, value_options)
        end
    end
end

function clipValue(val,min,max)
    if val < min then
        val = min
    elseif val > max then
        val = max
    end
    return val
end

local function getCurrentField()
    return Page.fields[currentLine]
end

local function incValue(inc)
    local f = Page.fields[currentLine]
    local idx = f.i or currentLine
    local scale = (f.scale or 1)
    local mult = (f.mult or 1)
    f.value = clipValue(f.value + ((inc*mult)/scale), (f.min/scale) or 0, (f.max/scale) or 255)
    f.value = math.floor((f.value*scale)/mult + 0.5)/(scale/mult)
    for idx=1, #(f.vals) do
        Page.values[f.vals[idx]] = bit32.rshift(math.floor(f.value*scale + 0.5), (idx-1)*8)
    end
    if f.upd and Page.values then
        f.upd(Page)
    end
end

local function drawMenu()
    local x = MenuBox.x
    local y = MenuBox.y
    local w = MenuBox.w
    local h_line = MenuBox.h_line
    local h_offset = MenuBox.h_offset
    local h = #(menuList) * h_line + h_offset*2

    lcd.drawFilledRectangle(x,y,w,h,backgroundFill)
    lcd.drawRectangle(x,y,w-1,h-1,foregroundColor)
    lcd.drawText(x+h_line/2,y+h_offset,"Menu:",globalTextOptions)

    for i,e in ipairs(menuList) do
        local text_options = globalTextOptions
        if menuActive == i then
            text_options = text_options + INVERS
        end
        lcd.drawText(x+MenuBox.x_offset,y+(i-1)*h_line+h_offset,e.t,text_options)
    end
end

function run_ui(event)
    local now = getTime()
    -- if lastRunTS old than 500ms
    if lastRunTS + 50 < now then
        invalidatePages()
    end
    lastRunTS = now
    if (currentState == pageStatus.saving) then
        if (saveTS + saveTimeout < now) then
            if saveRetries < saveMaxRetries then
                saveSettings()
            else
                -- max retries reached
                currentState = pageStatus.display
                invalidatePages()
            end
        end
    end
    -- process send queue
    mspProcessTxQ()
    -- navigation
    if isTelemetryScript and event == EVT_VIRTUAL_MENU_LONG then -- telemetry script
        menuActive = 1
        currentState = pageStatus.displayMenu
    elseif (not isTelemetryScript) and event == EVT_VIRTUAL_ENTER_LONG then -- standalone
        menuActive = 1
        killEnterBreak = 1
        currentState = pageStatus.displayMenu
    -- menu is currently displayed
    elseif currentState == pageStatus.displayMenu then
        if event == EVT_VIRTUAL_EXIT then
            currentState = pageStatus.display
        elseif event == EVT_VIRTUAL_PREV then
            incMenu(-1)
        elseif event == EVT_VIRTUAL_NEXT then
            incMenu(1)
        elseif event == EVT_VIRTUAL_ENTER then
            if killEnterBreak == 1 then
                killEnterBreak = 0
            else
                currentState = pageStatus.display
                menuList[menuActive].f()
            end
        end
    -- normal page viewing
    elseif currentState <= pageStatus.display then
        if not isTelemetryScript and event == EVT_VIRTUAL_PREV_PAGE then
            incPage(-1)
            killEvents(event) -- X10/T16 issue: pageUp is a long press
        elseif (not isTelemetryScript and event == EVT_VIRTUAL_NEXT_PAGE) or (isTelemetryScript and event == EVT_VIRTUAL_MENU) then
            incPage(1)
        elseif event == EVT_VIRTUAL_PREV or event == EVT_VIRTUAL_PREV_REPT then
            incLine(-1)
        elseif event == EVT_VIRTUAL_NEXT or event == EVT_VIRTUAL_NEXT_REPT then
            incLine(1)
        elseif event == EVT_VIRTUAL_ENTER then
            local field = Page.fields[currentLine]
            local idx = field.i or currentLine
            if Page.values and Page.values[idx] and (field.ro ~= true) then
                currentState = pageStatus.editing
            end
        elseif event == EVT_VIRTUAL_EXIT then
            return protocol.exitFunc();
        end
    -- editing value
    elseif currentState == pageStatus.editing then
        if event == EVT_VIRTUAL_EXIT or event == EVT_VIRTUAL_ENTER then
            currentState = pageStatus.display
        elseif event == EVT_VIRTUAL_INC or event == EVT_VIRTUAL_INC_REPT then
            incValue(1)
        elseif event == EVT_VIRTUAL_DEC or event == EVT_VIRTUAL_DEC_REPT then
            incValue(-1)
            killEvents(event)
        end
    end
    local nextPage = currentPage
    while Page == nil do
        Page = assert(loadScript(radio.templateHome .. PageFiles[currentPage]))()
        if Page.requiredVersion and apiVersion > 0 and Page.requiredVersion > apiVersion then
            incPage(1)

            if currentPage == nextPage then
                lcd.clear()
                lcd.drawText(NoTelem[1], NoTelem[2], "No Pages! API: " .. apiVersion, NoTelem[4])

                return 1
            end
        end
    end
    if not Page.values and currentState == pageStatus.display then
        requestPage()
    end
    lcd.clear()
    if TEXT_BGCOLOR then
        lcd.drawFilledRectangle(0, 0, LCD_W, LCD_H, TEXT_BGCOLOR)
    end
    drawScreen()
    if protocol.rssi() == 0 then
        lcd.drawText(NoTelem[1],NoTelem[2],NoTelem[3],NoTelem[4])
    end
    if currentState == pageStatus.displayMenu then
        drawMenu()
    elseif currentState == pageStatus.saving then
        lcd.drawFilledRectangle(SaveBox.x,SaveBox.y,SaveBox.w,SaveBox.h,backgroundFill)
        lcd.drawRectangle(SaveBox.x,SaveBox.y,SaveBox.w,SaveBox.h,SOLID)
        if saveRetries <= 0 then
            lcd.drawText(SaveBox.x+SaveBox.x_offset,SaveBox.y+SaveBox.h_offset,"Saving...",DBLSIZE + BLINK + (globalTextOptions))
        else
            lcd.drawText(SaveBox.x+SaveBox.x_offset,SaveBox.y+SaveBox.h_offset,"Retrying",DBLSIZE + (globalTextOptions))
        end
    end
    processMspReply(mspPollReply())
    return 0
end

return run_ui
