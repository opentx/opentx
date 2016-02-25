local options = {
  { "Source", SOURCE, 1 },
  { "Min", VALUE, -1024 },
  { "Max", VALUE, 1024 },
  { "Color", COLOR, RED }
}

local function create(zone, options)
  local pie = { zone=zone, options=options, counter=0 }
  print(options.Source)
  return pie
end

function refresh(pie)
  pie.counter = pie.counter + 1
  lcd.drawNumber(pie.zone.x, pie.zone.y, pie.counter, LEFT + DBLSIZE + TEXT_COLOR);
end

return { name="Counter", options=options, create=create, refresh=refresh }
