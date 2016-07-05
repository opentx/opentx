local options = {
  { "Option1", SOURCE, 1 },
  { "Option2", VALUE, 1000 },
  { "Option3", COLOR, RED }
}

local function create(zone, options)
  local pie = { zone=zone, options=options, counter=0 }
  print(options.Option2)
  return pie
end

local function update(pie, options)
  pie.options = options
end

local function background(pie)
  pie.counter = pie.counter + 1
end

function refresh(pie)
  pie.counter = pie.counter + 1
  lcd.drawNumber(pie.zone.x, pie.zone.y, pie.counter, LEFT + DBLSIZE + TEXT_COLOR);
end

return { name="Counter", options=options, create=create, update=update, refresh=refresh, background=background }
