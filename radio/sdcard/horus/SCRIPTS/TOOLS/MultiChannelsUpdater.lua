
local toolName = "TNS|Multi chan namer|TNE"

---- #########################################################################
---- #                                                                       #
---- # Copyright (C) OpenTX                                                  #
-----#                                                                       #
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

local protocol_name = ""
local sub_protocol_name = ""
local bind_ch = 0
local module_conf = {}
local module_pos = "Internal"
local file_ok = 0
local done = 0
local protocol = 0
local sub_protocol = 0
local f_seek = 0
local channel_names={}
local num_search = "Searching"

local function drawScreenTitle(title)
    if LCD_W == 480 then
        lcd.drawFilledRectangle(0, 0, LCD_W, 30, TITLE_BGCOLOR)
        lcd.drawText(1, 5, title, MENU_TITLE_COLOR)
    else
        lcd.drawScreenTitle(title, 0, 0)
    end
end

function bitand(a, b)
    local result = 0
    local bitval = 1
    while a > 0 and b > 0 do
      if a % 2 == 1 and b % 2 == 1 then -- test the rightmost bits
          result = result + bitval      -- set the current bit
      end
      bitval = bitval * 2 -- shift left
      a = math.floor(a/2) -- shift right
      b = math.floor(b/2)
    end
    return result
end

local function Multi_Draw_LCD(event)
  local line = 0

  lcd.clear()
  drawScreenTitle("Multi channels namer")

  --Display settings
  local lcd_opt = 0
  if LCD_W == 480 then
    x_pos = 10
    y_pos = 32
    y_inc = 20
  else
    x_pos = 0
    y_pos = 9
    y_inc = 8
    lcd_opt = SMLSIZE
  end

  --Multi Module detection
  if module_conf["Type"] ~= 6 then
    if LCD_W == 480 then
      lcd.drawText(10,50,"No Multi module configured...", BLINK)
    else
      --Draw on LCD_W=128
      lcd.drawText(2,17,"No Multi module configured...",SMLSIZE)
    end
    return
  else
    lcd.drawText(x_pos, y_pos+y_inc*line,module_pos .. " Multi detected.", lcd_opt)
    line = line + 1
  end

  --Channel order
  if (ch_order == -1) then
    lcd.drawText(x_pos, y_pos+y_inc*line,"Channels order can't be read from Multi...", lcd_opt)
    line = line + 1
  end

  --Can't open file MultiChan.txt
  if file_ok == 0 then
    lcd.drawText(x_pos, y_pos+y_inc*line,"Can't read MultiChan.txt file...", lcd_opt)
    return
  end

  if ( protocol_name == "" or sub_protocol_name == "" ) and f_seek ~=-1 then
    local f = io.open("/SCRIPTS/TOOLS/MultiChan.txt", "r")
    if f == nil then return end
    lcd.drawText(x_pos, y_pos+y_inc*line,num_search, lcd_opt)
    num_search = num_search .. "."
    if #num_search > 15 then
      num_search = string.sub(num_search,1,9)
    end
    local proto = 0
    local sub_proto = 0
    local proto_name = ""
    local sub_proto_name = ""
    local channels = ""
    local nbr_try = 0
    local nbr_car = 0
    repeat
      io.seek(f, f_seek)
      local data = io.read(f, 100)      -- read 100 characters
      if #data ==0 then
          f_seek = -1                   -- end of file
          break
      end
      proto, sub_proto, proto_name, sub_proto_name, bind_ch, channels = string.match(data,'(%d+),(%d),([%w-_ ]+),([%w-_ ]+),(%d)(.+)')
      if proto ~= nil and sub_proto ~= nil and protocol_name ~= nil and sub_protocol_name ~= nil and bind_ch ~= nil then
        if tonumber(proto) == protocol and tonumber(sub_proto) == sub_protocol then
          protocol_name = proto_name
          sub_protocol_name = sub_proto_name
          bind_ch = tonumber(bind_ch)
          if channels ~= nil then
            --extract channel names
            nbr_car = string.find(channels, "\r")
            if nbr_car == nil then nbr_car = string.find(channels, "\n") end
            if nbr_car ~= nil then
              channels = string.sub(channels,1,nbr_car-1)
            end
            local i = 5
            for k in string.gmatch(channels, ",([%w-_ ]+)") do
              channel_names[i] = k
              i = i + 1
            end
          end
          f_seek = -1                   -- protocol found
          break
        end
      end
      if f_seek ~= -1 then
        nbr_car = string.find(data, "\n")
        if nbr_car == nil then nbr_car = string.find(data, "\r") end
        if nbr_car == nil then
          f_seek = -1                   -- end of file
          break
        end
        f_seek = f_seek + nbr_car       -- seek to next line
        nbr_try = nbr_try + 1
      end
    until nbr_try > 20 or f_seek == -1
    io.close(f)
  end
  
  if f_seek ~= -1 then
    return -- continue searching...
  end

  --Protocol & Sub_protocol
  if protocol_name == "" or sub_protocol_name == "" then
    lcd.drawText(x_pos, y_pos+y_inc*line,"Unknown protocol "..tostring(protocol).."/"..tostring(sub_protocol).." ...", lcd_opt)
    return
  elseif LCD_W > 128 then
    lcd.drawText(x_pos, y_pos+y_inc*line,"Protocol: " .. protocol_name .. " / SubProtocol: " .. sub_protocol_name, lcd_opt)
    line = line + 1
  else
    lcd.drawText(x_pos, y_pos+y_inc*line,"Protocol: " .. protocol_name, lcd_opt)
    line = line + 1
    lcd.drawText(x_pos, y_pos+y_inc*line,"SubProtocol: " .. sub_protocol_name, lcd_opt)
    line = line + 1
  end

  text1=""
  text2=""
  for i,v in ipairs(channel_names) do
    if i<=8 then
      if i==1 then
        text1 = v
      else
        text1=text1 .. "," .. v
      end
    else
      if i==9 then
        text2 = v
      else
        text2=text2 .. "," .. v
      end
    end
  end
  if LCD_W > 128 then
    lcd.drawText(x_pos, y_pos+y_inc*line,"Channels: " .. text1, lcd_opt)
    line = line + 1
    if text2 ~= "" then
      lcd.drawText(x_pos*9, y_pos+y_inc*line,text2, lcd_opt)
      line = line + 1
    end
  end

  if event ~= EVT_VIRTUAL_ENTER and done == 0 then
    lcd.drawText(x_pos, y_pos+y_inc*line,"<ENT> Save", lcd_opt + INVERS + BLINK)
    return
  end
  
  lcd.drawText(x_pos, y_pos+y_inc*line,"Setting channel names.", lcd_opt)
  line = line + 1
  local output, nbr
  if done == 0 then
    for i,v in ipairs(channel_names) do
      output = model.getOutput(i-1)
      output["name"] = v
      model.setOutput(i-1,output)
      nbr = i
    end
    for i = nbr, 15 do
      output = model.getOutput(i)
      output["name"] = "n-a"
      model.setOutput(i,output)
    end
    if bind_ch == 1 then
      output = model.getOutput(15)
      output["name"] = "BindCH"
      model.setOutput(15,output)
    end
    done = 1
  end
  lcd.drawText(x_pos, y_pos+y_inc*line,"Done!", lcd_opt)
  line = line + 1
end

-- Init
local function Multi_Init()
  module_conf = model.getModule(0)
  if module_conf["Type"] ~= 6 then
    module_pos = "External"
    module_conf = model.getModule(1)
    if module_conf["Type"] ~= 6 then
      return
    end
  end

  protocol = module_conf["protocol"]
  sub_protocol = module_conf["subProtocol"]

  --Exceptions on first 4 channels...
  local stick_names = { "Rud", "Ele", "Thr", "Ail" }
  if ( protocol == 4 and sub_protocol == 1 ) or protocol == 19  or protocol == 52 then -- Hisky/HK310, Shenqi, ZSX
    stick_names[2] = "n-a"
    stick_names[4] = "n-a"
  elseif protocol == 43 then -- Traxxas
    stick_names[2] = "Aux4"
    stick_names[4] = "Aux3"
  elseif ( protocol == 48 and sub_protocol == 0 ) then -- V761 3CH
    stick_names[4] = "n-a"
  elseif protocol == 47 or  protocol == 49 or  protocol == 58 then -- GD00x, KF606, FX816
    stick_names[1] = "n-a"
    stick_names[2] = "n-a"
  end

  --Determine fist 4 channels order
  local ch_order=module_conf["channelsOrder"]
  if (ch_order == -1) then
    channel_names[1] = stick_names[defaultChannel(0)+1]
    channel_names[2] = stick_names[defaultChannel(1)+1]
    channel_names[3] = stick_names[defaultChannel(2)+1]
    channel_names[4] = stick_names[defaultChannel(3)+1]
  else
    channel_names[bitand(ch_order,3)+1] = stick_names[4]
    ch_order = math.floor(ch_order/4)
    channel_names[bitand(ch_order,3)+1] = stick_names[2]
    ch_order = math.floor(ch_order/4)
    channel_names[bitand(ch_order,3)+1] = stick_names[3]
    ch_order = math.floor(ch_order/4)
    channel_names[bitand(ch_order,3)+1] = stick_names[1]
  end
 
  --Exceptions on first 4 channels...
  if ( protocol == 73 or (protocol == 74 and sub_protocol == 0) ) then -- Kyosho or RadioLink Surface
    channel_names[1] = "ST"
    channel_names[2] = "THR"
    channel_names[3] = "CH3"
    channel_names[4] = "CH4"
  end
 
  --Check MultiChan.txt
  local f = io.open("/SCRIPTS/TOOLS/MultiChan.txt", "r")
  if f == nil then return end
  file_ok = 1
  io.close(f)
end

-- Main
local function Multi_Run(event)
  if event == nil then
    error("Cannot be run as a model script!")
    return 2
  else
    Multi_Draw_LCD(event)
    if event == EVT_VIRTUAL_EXIT then
      return 2
    end
  end
  return 0
end

return { init=Multi_Init, run=Multi_Run }
