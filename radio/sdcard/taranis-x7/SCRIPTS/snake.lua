--------------------------------------------------------------
-- Classic snake game
--
-- 2009 Led Lab @PUC-Rio www.eluaproject.net
-- Dado Sutter
-- Ives Negreiros
-- To Benjamin
---------------------------------------------------------------

local xMax = math.floor( LCD_W / 6 ) - 1
local yMax = math.floor( LCD_H / 8 ) - 1
local game_map = {}

local Head = {}
local Tail = {}

local highscore = 0
local size = 3
Tail.x = 1
Tail.y = 1
Head.x = Tail.x + ( size - 1 )
Head.y = Tail.y

local Food = {}
Food.x = false
Food.y = false

Head.dx = 1
Head.dy = 0
Tail.dx = Head.dx
Tail.dy = Head.dy
local direction = "right"
local score = 0

local function create_food()
  Food.x, Food.y = math.random( xMax - 1), math.random( yMax - 1)
  while game_map[ Food.x ][ Food.y ] do
    Food.x, Food.y = math.random( xMax - 1 ), math.random( yMax - 1 )
  end
  game_map[ Food.x ][ Food.y ] = "food"
  lcd.drawText( Food.x * 6, Food.y * 8+2, "@", 0 )
end

local function eat_food()
  playFile("/SCRIPTS/snake.wav")
  lcd.drawText( Head.x * 6, Head.y * 8, " ", 0 )
  game_map[ Head.x ][ Head.y ] = nil
  create_food()
  score = score + 1
end

local function check_collision()
  if Head.x < 0 or Head.x > xMax then
    return true
  elseif Head.y < 0 or Head.y > yMax then
    return true
  elseif ( ( game_map[ Head.x ][ Head.y ] ) and ( game_map[ Head.x ][ Head.y ] ~= "food" ) ) then
    return true
  end
  return false
end

local function move()
  if game_map[ Tail.x ][ Tail.y ] == "right" then
    Tail.dx = 1
    Tail.dy = 0
  elseif game_map[ Tail.x ][ Tail.y ] == "left" then
    Tail.dx = -1
    Tail.dy = 0
  elseif game_map[ Tail.x ][ Tail.y ] == "up" then
    Tail.dx = 0
    Tail.dy = -1
  elseif game_map[ Tail.x ][ Tail.y ] == "down" then
    Tail.dx = 0
    Tail.dy = 1
  end
  
  game_map[ Head.x ][ Head.y ] = direction
  Head.x = Head.x + Head.dx
  Head.y = Head.y + Head.dy

  if Head.x < 0 or Head.x > xMax or Head.y < 0 or Head.y > yMax then
    return
  elseif game_map[ Head.x ][ Head.y ] == "food" then
    eat_food()
  else
    lcd.drawText(Tail.x * 6, Tail.y * 8, " ", 16)
    game_map[ Tail.x ][ Tail.y ] = nil
    Tail.x = Tail.x + Tail.dx
    Tail.y = Tail.y + Tail.dy
  end

  lcd.drawText(Head.x * 6, Head.y * 8, "*", 0)
end

local function init()
  food = false
  lcd.clear()
  size = 3
  score = 0
  Tail.x = 1
  Tail.y = 1
  Head.x = Tail.x + ( size - 1 )
  Head.y = Tail.y
  Head.dx = 1
  Head.dy = 0
  Tail.dx = Head.dx
  Tail.dy = Head.dy
  direction = "right"

  for i = 0, xMax, 1 do
    game_map[ i ] = {}
  end
  
  for i = 0, size - 1, 1 do
    game_map[ Tail.x + ( i * Tail.dx ) ][ Tail.y + ( i * Tail.dy ) ] = direction
    lcd.drawText( ( Tail.x + ( i * Tail.dx ) ) * 6, ( Tail.y + ( i * Tail.dy ) ) * 8, "*", 0 )
  end
  
  create_food()
end

local snakeCounter = 0

local function run(event)
  if event == nil then
    raise("Cannot be run as a model script!")
  end
  
  if event == EVT_EXIT_BREAK then
    return 2
  end
  
  snakeCounter = snakeCounter + 1
  if snakeCounter < 30 then
    return 0
  end
  
  snakeCounter = 0
 
  local dir = direction
  if getValue('rud') > 100 and direction ~= "left" then
    dir = "right"
    Head.dx = 1
    Head.dy = 0
  end
  if getValue('rud') < -100 and direction ~= "right" then
    dir = "left"
    Head.dx = -1
    Head.dy = 0
  end
  if getValue('ele') > 100 and direction ~= "down" then
    dir = "up"
    Head.dx = 0
    Head.dy = -1
  end
  if getValue('ele') < -100 and direction ~= "up" then
    dir = "down"
    Head.dx = 0
    Head.dy = 1
  end

  direction = dir
  move()

  lcd.refresh()

  if check_collision() then
    return 1
  end
  
  return 0
end

return { init=init, run=run }

