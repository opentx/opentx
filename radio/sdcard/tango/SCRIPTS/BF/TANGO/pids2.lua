
return {
    read           = 94, -- MSP_PID_ADVANCED
    write          = 95, -- MSP_SET_PID_ADVANCED
    title          = "PIDs (2/2)",
    reboot         = false,
    eepromWrite    = true,
    minBytes       = 44,
    yMinLimit      = 11,
    yMaxLimit      = 84,
    text = {
        { t = "Feed",        x = 45,  y = 11, to = SMLSIZE },
        { t = "forward",     x = 37,  y = 18, to = SMLSIZE },
        { t = "D",           x = 85,  y = 11, to = SMLSIZE },
        { t = "Min",         x = 80,  y = 18, to = SMLSIZE },
        { t = "ROLL",        x = 10,  y = 26, to = SMLSIZE },
        { t = "PITCH",       x = 10,  y = 36, to = SMLSIZE },
        { t = "YAW",         x = 10,  y = 46, to = SMLSIZE },
        
        { t = "Feedforward", x = 10,  y = 60, to = SMLSIZE },
        { t = "Transition",  x = 20,  y = 68, to = SMLSIZE },
        { t = "D Min",       x = 10,  y = 76, to = SMLSIZE },
        { t = "Gain",        x = 20,  y = 84, to = SMLSIZE },
        { t = "Advance",     x = 20,  y = 92, to = SMLSIZE },
    },
    fields = {
        -- ROLL FF
        { x = 48, y = 26, min = 0, max = 2000, vals = { 33, 34 }, to = SMLSIZE },
        -- PITCH FF
        { x = 48, y = 36, min = 0, max = 2000, vals = { 35, 36 }, to = SMLSIZE },
        -- YAW FF
        { x = 48, y = 46, min = 0, max = 2000, vals = { 37, 38 }, to = SMLSIZE },
        -- ROLL D MIN
        { x = 80, y = 26, min = 0, max = 100,  vals = { 40 },     to = SMLSIZE },
        -- PITCH D MIN
        { x = 80, y = 36, min = 0, max = 100,  vals = { 41 },     to = SMLSIZE },
        -- YAW D MIN
        { x = 80, y = 46, min = 0, max = 100,  vals = { 42 },     to = SMLSIZE },
        -- FF TRANSITION
        { x = 80, y = 68, min = 0, max = 100,  vals = { 9 },      to = SMLSIZE, scale = 100 },
        -- D MIN GAIN
        { x = 80, y = 84, min = 0, max = 100,  vals = { 43 },     to = SMLSIZE },
        -- D MIN ADVANCE
        { x = 80, y = 92, min = 0, max = 200,  vals = { 44 },     to = SMLSIZE },
    },
}
