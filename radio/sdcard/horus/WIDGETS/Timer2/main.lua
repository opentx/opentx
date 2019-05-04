-- A Timer version that fill better the widget area
-- Offer Shmuely
-- Date: 2018
-- ver: 0.2

local options = {
  { "TextColor", COLOR, YELLOW },
  { "Timer", VALUE, 1, 1, 3}
}

local function create(zone, options)
  local wgt  = { zone=zone, options=options}
  return wgt
end

local function update(wgt, options)
  if (wgt==nil) then
    print("update(nil)")
    return
  end
  wgt.options = options
  print("TimerNumB:" .. options.Timer)
end

local function background(wgt)
  return
end

------------------------------------------------------------

-- This size is for top bar wgts
local function refreshZoneTiny(wgt)
end

--- Size is 160x32 1/8th
local function refreshZoneSmall(wgt)
  local t1 = model.getTimer(wgt.options.Timer-1)
  local timerInfo = string.format("T%s: ", wgt.options.Timer)
  lcd.drawText(wgt.zone.x, wgt.zone.y, timerInfo, SMLSIZE + CUSTOM_COLOR)
  lcd.drawTimer(wgt.zone.x+35, wgt.zone.y, t1.value, DBLSIZE + CUSTOM_COLOR)
  return
end

--- Size is 225x98 1/4th  (no sliders/trim)
local function refreshZoneMedium(wgt)
  local t1 = model.getTimer(wgt.options.Timer-1)
  --print(t1.value)
  local timerInfo = string.format("Timer:%s", wgt.options.Timer)
  lcd.drawText (wgt.zone.x, wgt.zone.y, timerInfo, SMLSIZE + CUSTOM_COLOR)
  lcd.drawTimer(wgt.zone.x, wgt.zone.y+3, t1.value, XXLSIZE + CUSTOM_COLOR)
end

--- Size is 192x152 1/2
local function refreshZoneLarge(wgt)
  local t1 = model.getTimer(wgt.options.Timer-1)
  --print(t1.value)
  local timerInfo = string.format("Timer:%s", wgt.options.Timer)
  lcd.drawText(wgt.zone.x, wgt.zone.y, timerInfo, SMLSIZE + CUSTOM_COLOR)
  lcd.drawTimer(wgt.zone.x, wgt.zone.y+15, t1.value, XXLSIZE + CUSTOM_COLOR)
end

--- Size is 390x172 1/1
--- Size is 460x252 1/1 (no sliders/trim/topbar)
local function refreshZoneXLarge(wgt)
  local t1 = model.getTimer(wgt.options.Timer-1)
  --print(t1.value)
  local timerInfo = string.format("Timer:%s", wgt.options.Timer)
  lcd.drawText(wgt.zone.x, wgt.zone.y, timerInfo, SMLSIZE + CUSTOM_COLOR)
  lcd.drawTimer(wgt.zone.x, wgt.zone.y+15, t1.value, XXLSIZE + CUSTOM_COLOR)
end


function refresh(wgt)

  if (wgt==nil) then
    print("refresh(nil)")
    return
  end

  if (wgt.options==nil) then
    print("refresh(wgt.options=nil)")
    return
  end

  if (wgt.options.Timer==nil) then
    print("refresh(wgt.options.Timer=nil)")
    return
  end

  lcd.setColor(CUSTOM_COLOR, wgt.options.TextColor)

  if     wgt.zone.w  > 380 and wgt.zone.h > 165 then refreshZoneXLarge(wgt)
  elseif wgt.zone.w  > 180 and wgt.zone.h > 145 then refreshZoneLarge(wgt)
  elseif wgt.zone.w  > 170 and wgt.zone.h >  65 then refreshZoneMedium(wgt)
  elseif wgt.zone.w  > 150 and wgt.zone.h >  28 then refreshZoneSmall(wgt)
  elseif wgt.zone.w  >  65 and wgt.zone.h >  35 then refreshZoneTiny(wgt)
  end
end

return { name="Timer2", options=options, create=create, update=update, background=background, refresh=refresh }
