
return {
   read           = 136, -- MSP_GPS_RESCUE_PIDS
   write          = 226, -- MSP_SET_GPS_RESCUE_PIDS
   title          = "GPS / PIDs",
   reboot         = false,
   eepromWrite    = true,
   minBytes       = 14,
   requiredVersion = 1.041,
   text = {
      { t = "P",        x =  55,  y = 14, to=SMLSIZE },
      { t = "I",        x =  83,  y = 14, to=SMLSIZE },
      { t = "D",        x = 111,  y = 14, to=SMLSIZE },
      { t = "Throttle", x =   5,  y = 26, to=SMLSIZE },
      { t = "Velocity", x =   5,  y = 36, to=SMLSIZE },
      { t = "Yaw"     , x =   5,  y = 46, to=SMLSIZE },
   },
   fields = {
      -- P
      { x =  51, y = 26, min = 0, max = 200, vals = { 1,2 }, to=SMLSIZE },
      { x =  51, y = 36, min = 0, max = 200, vals = { 7,8 }, to=SMLSIZE },
      { x =  51, y = 46, min = 0, max = 500, vals = {13,14}, to = SMLSIZE },
      -- I
      { x =  79, y = 26, min = 0, max = 200, vals = { 3,4 }, to=SMLSIZE },
      { x =  79, y = 36, min = 0, max = 200, vals = { 9,10 }, to=SMLSIZE },
      -- D
      { x = 107, y = 26, min = 0, max = 200, vals = { 5,6 }, to=SMLSIZE },
      { x = 107, y = 36, min = 0, max = 200, vals = { 11,12 }, to=SMLSIZE },
   },
}
