
return {
   read           = 135, -- MSP_GPS_RESCUE
   write          = 225, -- MSP_SET_GPS_RESCUE
   title          = "GPS / Rescue",
   reboot         = false,
   eepromWrite    = true,
   minBytes       = 16,
   requiredVersion = 1.041,
   text = {

      { t = "Min Sats.",      x =  3,  y = 10, to=SMLSIZE },
      { t = "Angle",          x =  3,  y = 20, to=SMLSIZE },
      { t = "Initial Alt",    x =  3,  y = 30, to=SMLSIZE },
      { t = "Descent Dst",    x =  3,  y = 40, to=SMLSIZE },
      { t = "Ground Spd",     x =  3,  y = 50, to=SMLSIZE },

      { t = "Snty.",          x =  80,   y = 10, to = SMLSIZE },
      { t = "Throttle",       x =  80,   y = 20, to = SMLSIZE },
      { t = "Min",            x =  85,   y = 30, to = SMLSIZE },
      { t = "Hover",          x =  80,   y = 40, to = SMLSIZE },
      { t = "Max",            x =  85,   y = 50, to = SMLSIZE },
   },
   fields = {
      { x = 58, y = 10, min =    0, max =  50, vals = { 16  }, to = SMLSIZE },
      { x = 58, y = 20, min =    0, max = 200, vals = { 1,2 }, to = SMLSIZE },
      { x = 58, y = 30, min =   20, max = 100, vals = { 3,4 }, to = SMLSIZE },
      { x = 58, y = 40, min =   30, max = 500, vals = { 5,6 }, to = SMLSIZE },
      { x = 58, y = 50, min =   30, max =3000, vals = { 7,8 }, to = SMLSIZE },

      { x = 105, y = 10, min =   0,  max = 2   , vals = { 15    }, to = SMLSIZE,table = { [0]="OFF","ON","FS_ONLY"}},	 	  
      { x = 105, y = 30, min = 1000, max = 2000, vals =   { 9,10 }, to = SMLSIZE },
      { x = 105, y = 40, min = 1000, max = 2000, vals =  { 13,14 }, to = SMLSIZE },
      { x = 105, y = 50, min = 1000, max = 2000, vals =  { 11,12 }, to = SMLSIZE },
   },
}
