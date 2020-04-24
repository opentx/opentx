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

local done = 0
local Protocols = {
  {1,  0, "Flysky", "Flysky", { "CH5", "CH6", "CH7", "CH8" }, 0 },
  {1,  1, "Flysky", "V9x9",   { "Flip", "Light", "Pict", "Video" }, 1 },
  {1,  2, "Flysky", "V6x6",   { "Flip", "Light", "Pict", "Video", "HLess", "RTH", "XCAL", "YCAL" }, 1 },
  {1,  3, "Flysky", "V912",   { "BtmBtn", "TopBtn" }, 1 },
  {1,  4, "Flysky", "CX20",   { "CH5", "CH6", "CH7" }, 0 },
  {28, 0, "Flysky AFHDS2A", "PWM_IBUS", { "CH5", "CH6", "CH7", "CH8", "CH9", "CH10", "CH11", "CH12", "CH13", "CH14" }, 0 },
  {28, 1, "Flysky AFHDS2A", "PPM_IBUS", { "CH5", "CH6", "CH7", "CH8", "CH9", "CH10", "CH11", "CH12", "CH13", "CH14" }, 0 },
  {28, 2, "Flysky AFHDS2A", "PWM_SBUS", { "CH5", "CH6", "CH7", "CH8", "CH9", "CH10", "CH11", "CH12", "CH13", "CH14" }, 0 },
  {28, 3, "Flysky AFHDS2A", "PPM_SBUS", { "CH5", "CH6", "CH7", "CH8", "CH9", "CH10", "CH11", "CH12", "CH13", "CH14" }, 0 },
  {53, 0, "Flyzone", "FZ-410", { }, 0 },
  {2,  0, "Hubsan", "H107", { "Flip", "Light", "Pict", "Video", "HLess" }, 1 },
  {2,  1, "Hubsan", "H301", { "RTH", "Light", "Stab", "Video" }, 0 },
  {2,  2, "Hubsan", "H501", { "RTH", "Light", "Pict", "Video", "HLess", "GPS_H", "ALT_H", "Flip", "FModes" }, 0 },
  {41, 0, "Bugs", "3-6-8", { "Arm", "Angle", "Flip", "Pict", "Video", "LED" }, 0 },
  {60, 0, "Pelikan", "PRO_V4", { "CH5", "CH6", "CH7", "CH8" }, 0 },
  {37, 0, "Corona", "COR_V1", { "CH5", "CH6", "CH7", "CH8" }, 0 },
  {37, 1, "Corona", "COR_V2", { "CH5", "CH6", "CH7", "CH8" }, 0 },
  {37, 2, "Corona", "FD_V3", { "CH5", "CH6", "CH7", "CH8" }, 0 },
  {25, 0, "FrSkyV", "V8", { "CH5", "CH6", "CH7", "CH8" }, 0 },
  {3,  0, "FrSkyD", "D8", { "CH5", "CH6", "CH7", "CH8" }, 0 },
  {3,  0, "FrSkyD", "D8Cloned", { "CH5", "CH6", "CH7", "CH8" }, 0 },
  {67, 0, "FrSkyL", "LR12", { "CH5", "CH6", "CH7", "CH8", "CH9", "CH10", "CH11", "CH12" }, 0 },
  {67, 1, "FrSkyL", "LR12_6CH", { "CH5", "CH6" }, 0 },
  {15, 0, "FrSkyX", "D16 FCC", { "CH5", "CH6", "CH7", "CH8", "CH9", "CH10", "CH11", "CH12", "CH13", "CH14", "CH15", "CH16" }, 0 },
  {15, 1, "FrSkyX", "D16_8CH FCC", { "CH5", "CH6", "CH7", "CH8" }, 0 },
  {15, 2, "FrSkyX", "D16 LBT", { "CH5", "CH6", "CH7", "CH8", "CH9", "CH10", "CH11", "CH12", "CH13", "CH14", "CH15", "CH16" }, 0 },
  {15, 3, "FrSkyX", "D16_8CH LBT", { "CH5", "CH6", "CH7", "CH8" }, 0 },
  {15, 4, "FrSkyX", "D16Cloned", { "CH5", "CH6", "CH7", "CH8", "CH9", "CH10", "CH11", "CH12", "CH13", "CH14", "CH15", "CH16" }, 0 },
  {64, 0, "FrSkyX2", "D16 FCC", { "CH5", "CH6", "CH7", "CH8", "CH9", "CH10", "CH11", "CH12", "CH13", "CH14", "CH15", "CH16" }, 0 },
  {64, 1, "FrSkyX2", "D16_8CH FCC", { "CH5", "CH6", "CH7", "CH8" }, 0 },
  {64, 2, "FrSkyX2", "D16 LBT", { "CH5", "CH6", "CH7", "CH8", "CH9", "CH10", "CH11", "CH12", "CH13", "CH14", "CH15", "CH16" }, 0 },
  {64, 3, "FrSkyX2", "D16_8CH LBT", { "CH5", "CH6", "CH7", "CH8" }, 1 },
  {64, 4, "FrSkyX2", "D16Cloned", { "CH5", "CH6", "CH7", "CH8", "CH9", "CH10", "CH11", "CH12", "CH13", "CH14", "CH15", "CH16" }, 0 },
  {55, 0, "FrSkyRX", "RX", { "CH5", "CH6", "CH7", "CH8", "CH9", "CH10", "CH11", "CH12", "CH13", "CH14", "CH15", "CH16" }, 0 },
  {55, 1, "FrSkyRX", "CloneTX", { }, 0 },
  {39, 0, "Hitec", "Opt_Fw",  { "CH5", "CH6", "CH7", "CH8", "CH9" }, 0 },
  {39, 1, "Hitec", "Opt_Hub", { "CH5", "CH6", "CH7", "CH8", "CH9" }, 0 },
  {39, 2, "Hitec", "Minima",  { "CH5", "CH6", "CH7", "CH8", "CH9" }, 0 },
  {57, 0, "HoTT", "Std", { "CH5", "CH6", "CH7", "CH8", "CH9", "CH10", "CH11", "CH12" }, 0 },
  {21, 0, "SFHSS", "Std", { "CH5", "CH6", "CH7", "CH8" }, 0 },
  {68, 0, "Skyartec", "Std", { "CH5", "CH6", "CH7" }, 0 },
  {7 , 0, "Devo", "8CH",  { "CH5", "CH6", "CH7", "CH8" }, 0 },
  {7 , 1, "Devo", "10CH", { "CH5", "CH6", "CH7", "CH8", "CH9", "CH10" }, 0 },
  {7 , 2, "Devo", "12CH", { "CH5", "CH6", "CH7", "CH8", "CH9", "CH10", "CH11", "CH12" }, 0 },
  {7 , 3, "Devo", "6CH",  { "CH5", "CH6" }, 0 },
  {7 , 4, "Devo", "7CH",  { "CH5", "CH6", "CH7" }, 0 },
  {30, 0, "WK2x01", "WK2801",  { "CH5", "CH6", "CH7", "CH8" }, 0 },
  {30, 1, "WK2x01", "WK2401",  { }, 0 },
  {30, 2, "WK2x01", "W6_5_1",  { "Gear", "Dis", "Gyro" }, 0 },
  {30, 3, "WK2x01", "W6_6_1",  { "Gear", "Col", "Gyro" }, 0 },
  {30, 4, "WK2x01", "W6HEL",   { "Gear", "Col", "Gyro" }, 0 },
  {30, 5, "WK2x01", "W6HEL_I", { "Gear", "Col", "Gyro" }, 0 },
  {6 , 0, "DSM", "2_22", { "CH5", "CH6", "CH7", "CH8", "CH9", "CH10", "CH11", "CH12", "n-a", "ThKill" }, 0 },
  {6 , 1, "DSM", "2_11", { "CH5", "CH6", "CH7", "CH8", "CH9", "CH10", "CH11", "CH12", "n-a", "ThKill" }, 0 },
  {6 , 2, "DSM", "X_22", { "CH5", "CH6", "CH7", "CH8", "CH9", "CH10", "CH11", "CH12", "n-a", "ThKill" }, 0 },
  {6 , 3, "DSM", "X_11", { "CH5", "CH6", "CH7", "CH8", "CH9", "CH10", "CH11", "CH12", "n-a", "ThKill" }, 0 },
  {22, 0, "J6Pro", "Std", { "CH5", "CH6", "CH7", "CH8", "CH9", "CH10", "CH11", "CH12" }, 0 },
  {22, 0, "WFLY", "WFR0xS", { "CH5", "CH6", "CH7", "CH8", "CH9" }, 0 },
  {24, 0, "Assan", "Std", { "CH5", "CH6", "CH7", "CH8" }, 0 },
  {14, 0, "Bayang", "Std", { "Flip", "RTH", "Pict", "Video", "HLess", "Invert", "Rates", "n-a", "n-a", "AnAux1", "AnAux2" }, 1 },
  {14, 1, "Bayang", "H8S3D", { "Flip", "RTH", "Pict", "Video", "HLess", "Invert", "Rates" }, 1 },
  {14, 2, "Bayang", "X16_AH", { "Flip", "RTH", "Pict", "Video", "HLess", "Invert", "Rates", "TakeOf" }, 1 },
  {14, 3, "Bayang", "IRDRONE", { "Flip", "RTH", "Pict", "Video", "HLess", "Invert", "Rates", "TakeOf", "EmStop" }, 1 },
  {14, 4, "Bayang", "DHD_D4", { "Flip", "RTH", "Pict", "Video", "HLess", "Invert", "Rates", "TakeOf", "EmStop" }, 1 },
  {59, 0, "BayangRX", "RX", { "AnAux1", "AnAux2", "Flip", "RTH", "Pict", "Video" }, 1 },
  {42, 0, "BugsMini", "Mini", { "Arm", "Angle", "Flip", "Pict", "Video", "LED" }, 0 },
  {42, 1, "BugsMini", "3H", { "Arm", "Angle", "Flip", "Pict", "Video", "LED", "AltHol" }, 0 },
  {34, 0, "Cabell", "V3", { "CH5", "CH6", "CH7", "CH8", "CH9", "CH10", "CH11", "CH12", "CH13", "CH14", "CH15", "CH16" }, 0 },
  {34, 1, "Cabell", "V3Telem", { "CH5", "CH6", "CH7", "CH8", "CH9", "CH10", "CH11", "CH12", "CH13", "CH14", "CH15", "CH16" }, 0 },
  {13, 0, "CG023", "Std", { "Flip", "Light", "Pict", "Video", "HLess" }, 1 },
  {13, 1, "CG023", "YD829", { "Flip", "n-a", "Pict", "Video", "HLess" }, 1 },
  {12, 0, "CX10", "Green", { "Flip", "Rate" }, 1 },
  {12, 1, "CX10", "Blue", { "Flip", "Rate", "Pict", "Video" }, 1 },
  {12, 2, "CX10", "DM007", { "Flip", "Mode", "Pict", "Video", "HLess" }, 1 },
  {12, 4, "CX10", "JC3015_1", { "Flip", "Mode", "Pict", "Video" }, 1 },
  {12, 5, "CX10", "JC3015_2", { "Flip", "Mode", "LED", "DFlip" }, 1 },
  {12, 6, "CX10", "MK33041", { "Flip", "Mode", "Pict", "Video", "HLess", "RTH" }, 1 },
  {33, 0, "DM022", "Std", { "Flip", "LED", "Cam1", "Cam2", "HLess", "RTH", "RLow" }, 1 },
  {45, 0, "E01X", "E012", { "n-a", "Flip", "n-a", "HLess", "RTH" }, 1 },
  {45, 1, "E01X", "E015", { "Arm", "Flip", "LED", "HLess", "RTH" }, 1 },
  {45, 2, "E01X", "E016H", { "Stop", "Flip", "n-a", "HLess", "RTH" }, 1 },
  {16, 0, "ESKY", "Std", { "Gyro", "Pitch" }, 0 },
  {16, 1, "ESKY", "ET4", { "Gyro", "Pitch" }, 0 },
  {35, 0, "ESKY150", "4CH", { }, 0 },
  {35, 1, "ESKY150", "7CH", { "FMode", "Aux6", "Aux7" }, 0 },
  {20, 0, "FY326", "FY326", { "Flip", "RTH", "HLess", "Expert", "Calib" }, 1 },
  {20, 1, "FY326", "FY319", { "Flip", "RTH", "HLess", "Expert", "Calib" }, 1 },
  {23, 0, "FY326", "FY326", { "Flip", "RTH", "HLess", "Expert" }, 1 },
  {32, 0, "GW008", "FY326", { "Flip" }, 1 },
  {36, 0, "H8_3D", "Std", { "Flip", "Light", "Pict", "Video", "RTH", "FlMode", "Cal1" }, 1 },
  {36, 1, "H8_3D", "H20H", { "Flip", "Light", "Pict", "Video", "Opt1", "Opt2", "Cal1", "Cal2", "Gimbal" }, 1 },
  {36, 2, "H8_3D", "H20", { "Flip", "Light", "Pict", "Video", "Opt1", "Opt2", "Cal1", "Cal2", "Gimbal" }, 1 },
  {36, 3, "H8_3D", "H30", { "Flip", "Light", "Pict", "Video", "Opt1", "Opt2", "Cal1", "Cal2", "Gimbal" }, 1 },
  {4 , 0, "Hisky", "Std", { "Gear", "Pitch", "Gyro", "CH8" }, 0 },
  {9 , 0, "KN", "WLToys", { "DRate", "THold", "IdleUp", "Gyro", "Ttrim", "Atrim", "Etrim" }, 0 },
  {9 , 1, "KN", "Feilun", { "DRate", "THold", "IdleUp", "Gyro", "Ttrim", "Atrim", "Etrim" }, 0 },
  {26, 0, "Hontai", "Std", { "Flip", "LED", "Pict", "Video", "HLess", "RTH", "Calib" }, 1 },
  {26, 1, "Hontai", "JJRCX1", { "Flip", "Arm", "Pict", "Video", "HLess", "RTH", "Calib" }, 1 },
  {26, 2, "Hontai", "X5C1", { "Flip", "Arm", "Pict", "Video", "HLess", "RTH", "Calib" }, 1 },
  {26, 3, "Hontai", "FQ777_951", { "Flip", "Arm", "Pict", "Video", "HLess", "RTH", "Calib" }, 1 },
  {18, 0, "MJXQ", "WHL08", { "Flip", "LED", "Pict", "Video", "HLess", "RTH", "AuFlip", "Pan", "Tilt", "Rate" }, 1 },
  {18, 1, "MJXQ", "X600", { "Flip", "LED", "Pict", "Video", "HLess", "RTH", "AuFlip", "Pan", "Tilt", "Rate" }, 1 },
  {18, 2, "MJXQ", "X800", { "Flip", "LED", "Pict", "Video", "HLess", "RTH", "AuFlip", "Pan", "Tilt", "Rate" }, 1 },
  {18, 3, "MJXQ", "H26D", { "Flip", "LED", "Pict", "Video", "HLess", "RTH", "AuFlip", "Pan", "Tilt", "Rate" }, 1 },
  {18, 4, "MJXQ", "E010", { "Flip", "LED", "Pict", "Video", "HLess", "RTH", "AuFlip", "Pan", "Tilt", "Rate" }, 1 },
  {18, 5, "MJXQ", "H26WH", { "Flip", "Arm", "Pict", "Video", "HLess", "RTH", "AuFlip", "Pan", "Tilt", "Rate" }, 1 },
  {18, 6, "MJXQ", "Phoenix", { "Flip", "Arm", "Pict", "Video", "HLess", "RTH", "AuFlip", "Pan", "Tilt", "Rate" }, 1 },
  {17, 0, "MT99XX", "Std", { "Flip", "LED", "Pict", "Video", "HLess" }, 1 },
  {17, 1, "MT99XX", "H7", { "Flip", "LED", "Pict", "Video", "HLess" }, 1 },
  {17, 2, "MT99XX", "YZ", { "Flip", "LED", "Pict", "Video", "HLess" }, 1 },
  {17, 3, "MT99XX", "LS", { "Flip", "Invert", "Pict", "Video", "HLess" }, 1 },
  {17, 4, "MT99XX", "FY805", { "Flip", "n-a", "n-a", "n-a", "HLess" }, 1 },
  {44, 0, "NCC1701", "Std", { "Warp" }, 1 },
  {51, 0, "Potensic", "A20", { "TakLan", "Emerg", "Mode", "HLess" }, 1 },
  {56, 0, "Propel", "74-Z", { "LEDs", "RollCW", "RolCCW", "Fire", "Weapon", "Calib", "AltHol", "TakeOf", "Land", "Train" }, 1 },
  {29, 0, "Q2x2", "Q222", { "Flip", "LED", "Mod2", "Mod1", "HLess", "RTH", "XCal", "YCal" }, 1 },
  {29, 1, "Q2x2", "Q242", { "Flip", "LED", "Pict", "Video", "HLess", "RTH", "XCal", "YCal" }, 1 },
  {29, 2, "Q2x2", "Q282", { "Flip", "LED", "Pict", "Video", "HLess", "RTH", "XCal", "YCal" }, 1 },
  {31, 0, "Q303", "Q303", { "AltHol", "Flip", "Pict", "Video", "HLess", "RTH", "Gimbal" }, 1 },
  {31, 1, "Q303", "C35", { "Arm", "VTX", "Pict", "Video", "n-a", "RTH", "Gimbal" }, 1 },
  {31, 2, "Q303", "CX10D", { "Arm", "Flip" }, 1 },
  {31, 3, "Q303", "CX10WD", { "Arm", "Flip" }, 1 },
  {50, 0, "Redpine", "Fast", { "sCH5", "sCH6", "sCH7", "sCH8", "sCH9", "sCH10", "sCH11", "sCH12", "sCH13", "sCH14", "sCH15", "sCH16" }, 0 },
  {50, 1, "Redpine", "Slow", { "sCH5", "sCH6", "sCH7", "sCH8", "sCH9", "sCH10", "sCH11", "sCH12", "sCH13", "sCH14", "sCH15", "sCH16" }, 0 },
  {11, 0, "SLT", "V1", { "Gear", "Pitch" }, 0 },
  {11, 1, "SLT", "V2", { "CH5", "CH6", "CH7", "CH8" }, 0 },
  {11, 2, "SLT", "Q100", { "Rates", "n-a", "CH7", "CH8", "Mode", "Flip", "n-a", "n-a", "Calib" }, 0 },
  {11, 3, "SLT", "Q200", { "Rates", "n-a", "CH7", "CH8", "Mode", "VidOn", "VidOff", "Calib" }, 0 },
  {11, 4, "SLT", "MR100", { "Rates", "n-a", "CH7", "CH8", "Mode", "Flip", "Video", "Pict" }, 0 },
  {10, 0, "Symax", "Std", { "Flip", "Rates", "Pict", "Video", "HLess" }, 1 },
  {10, 1, "Symax", "X5C", { "Flip", "Rates", "Pict", "Video", "HLess" }, 1 },
  {5 , 0, "V2x2", "Std", { "Flip", "Light", "Pict", "Video", "HLess", "CalX", "CalY" }, 1 },
  {5 , 1, "V2x2", "JXD506", { "Flip", "Light", "Pict", "Video", "HLess", "StaSto", "Emerg", "Cam_UD" }, 1 },
  {61, 0, "Tiger", "Std", { "Flip", "Light" }, 1 },
  {46, 0, "V911s", "Std", { "Calib" }, 1 },
  {46, 1, "E119", "Std", { "Calib" }, 1 },
  {62, 0, "XK", "X450", { "FMode", "TakeOf", "Emerg", "3D_6G", "Pict", "Video" }, 1 },
  {62, 1, "XK", "X420", { "FMode", "TakeOf", "Emerg", "3D_6G", "Pict", "Video" }, 1 },
  {8 , 0, "YD717", "Std", { "Flip", "Light", "Pict", "Video", "HLess" }, 1 },
  {8 , 1, "YD717", "SkyWlkr", { "Flip", "Light", "Pict", "Video", "HLess" }, 1 },
  {8 , 2, "YD717", "Simax4", { "Flip", "Light", "Pict", "Video", "HLess" }, 1 },
  {8 , 3, "YD717", "XinXun", { "Flip", "Light", "Pict", "Video", "HLess" }, 1 },
  {8 , 4, "YD717", "NiHui", { "Flip", "Light", "Pict", "Video", "HLess" }, 1 },
  {65,0, "FrSkyR9", "R9_915", { "CH5", "CH6", "CH7", "CH8", "CH9", "CH10", "CH11", "CH12", "CH13", "CH14", "CH15", "CH16" }, 0 },
  {65,1, "FrSkyR9", "R9_868", { "CH5", "CH6", "CH7", "CH8", "CH9", "CH10", "CH11", "CH12", "CH13", "CH14", "CH15", "CH16" }, 0 },
  {65,2, "FrSkyR9", "R9_915_8CH", { "CH5", "CH6", "CH7", "CH8" }, 0 },
  {65,3, "FrSkyR9", "R9_968_8CH", { "CH5", "CH6", "CH7", "CH8" }, 0 },

-- Need first 4 channels changed...
  {19, 0, "Shenqi", "Cycle", { }, 1 },
  {4 , 1, "Hisky", "HK310", { "Aux" }, 0 },
  {43, 0, "Traxxas", "6519", { }, 0 },
  {52, 0, "ZSX", "280", { "Light" }, 1 },
  {48, 0, "V761", "Std", { "Gyro" }, 1 },
  {49, 0, "KF606", "Std", { "Trim" }, 1 },
  {47, 0, "GD00x", "V1", { "Trim", "LED", "Rate" }, 1 },
  {47, 1, "GD00x", "V2", { "Trim", "LED", "Rate" }, 1 },
  {58, 0, "FX816", "P38", { },1 }
}

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

local function Multi_Draw_LCD()
  local line = 0
  local module_conf = {}
  local module_pos = "Internal"

  lcd.clear()
  drawScreenTitle("Multi channels namer")
  module_conf = model.getModule(0)
  if module_conf["Type"] ~= 6 then
    module_pos = "External"
    module_conf = model.getModule(1)
    if module_conf["Type"] ~= 6 then
      if LCD_W == 480 then
        lcd.drawText(10,50,"No Multi module configured...", BLINK)
      else
        --Draw on LCD_W=128
        lcd.drawText(2,17,"No Multi module configured...",SMLSIZE)
      end
      return
    end
  end

  local protocol = module_conf["protocol"]
  local sub_protocol = module_conf["subProtocol"]

  --Display settings
  local lcd_opt
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

  --Exceptions on first 4 channels...
  local stick_names = { "Rud", "Ele", "Thr", "Ail" }
  if ( protocol == 4 and sub_protocol ==1 ) or protocol == 19  or protocol == 52 then -- Hisky/HK310, Shenqi, ZSX
    stick_names[2] = "n-a"
    stick_names[4] = "n-a"
  elseif protocol == 43 then -- Traxxas
    stick_names[2] = "Aux4"
    stick_names[4] = "Aux3"
  elseif protocol == 48 then -- V761
    stick_names[4] = "n-a"
  elseif protocol == 47 or  protocol == 49 or  protocol == 58 then -- GD00x, KF606, FX816
    stick_names[1] = "n-a"
    stick_names[2] = "n-a"
  end

  --Determine fist 4 channels order
  local channel_names={}
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

  --Specific channels for protocol
  local protocol_name, sub_protocol_name, bind_ch
  for i,v in ipairs(Protocols) do
    if v[1] == protocol and v[2] == sub_protocol then
      protocol_name = v[3]
      sub_protocol_name = v[4]
      for j,w in ipairs(v[5]) do
        channel_names[4+j]=w;
      end
      bind_ch = v[6]
    end
  end

  lcd.drawText(x_pos, y_pos+y_inc*line,module_pos .. " Multi detected.", lcd_opt)
  line = line + 1
  if (ch_order == -1) then
    lcd.drawText(x_pos, y_pos+y_inc*line,"Channels order can't be read from Multi...", lcd_opt)
    line = line + 1
  end
  if protocol_name == nil or sub_protocol_name == nil then
    lcd.drawText(x_pos, y_pos+y_inc*line,"Unknown protocol "..tostring(protocol).."/"..tostring(sub_protocol).." ...", lcd_opt)
  else if LCD_W > 128 then
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
    lcd.drawText(x_pos, y_pos+y_inc*line,"Setting channel names.", lcd_opt)
    line = line + 1
    local output, nbr
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
    lcd.drawText(x_pos, y_pos+y_inc*line,"Done!", lcd_opt)
    line = line + 1
    done = 1
  end
end

-- Init
local function Multi_Init()
end

-- Main
local function Multi_Run(event)
  if event == nil then
    error("Cannot be run as a model script!")
    return 2
  else
    if done == 0 then
      Multi_Draw_LCD()
    else
      if event == EVT_VIRTUAL_ENTER or event == EVT_VIRTUAL_EXIT then
        return 2
      end
    end
    return 0
  end
end

return { init=Multi_Init, run=Multi_Run }
