
return {
    read           = 112, -- MSP_PID
    write          = 202, -- MSP_SET_PID
    title          = "PIDs (1/2)",
    reboot         = false,
    eepromWrite    = true,
    minBytes       = 9,
    text = {
        { t = "P",     x = 45,  y = 14, to = SMLSIZE },
        { t = "I",     x = 73,  y = 14, to = SMLSIZE },
        { t = "D",     x = 101, y = 14, to = SMLSIZE },
        { t = "ROLL",  x = 10,  y = 26, to = SMLSIZE },
        { t = "PITCH", x = 10,  y = 36, to = SMLSIZE },
        { t = "YAW",   x = 10,  y = 46, to = SMLSIZE },
    },
    fields = {
        -- P
        { x = 41, y = 26, min = 0, max = 200, vals = { 1 }, to = SMLSIZE },
        { x = 41, y = 36, min = 0, max = 200, vals = { 4 }, to = SMLSIZE },
        { x = 41, y = 46, min = 0, max = 200, vals = { 7 }, to = SMLSIZE },
        -- I
        { x = 69, y = 26, min = 0, max = 200, vals = { 2 }, to = SMLSIZE },
        { x = 69, y = 36, min = 0, max = 200, vals = { 5 }, to = SMLSIZE },
        { x = 69, y = 46, min = 0, max = 200, vals = { 8 }, to = SMLSIZE },
        -- D
        { x = 97, y = 26, min = 0, max = 200, vals = { 3 }, to = SMLSIZE },
        { x = 97, y = 36, min = 0, max = 200, vals = { 6 }, to = SMLSIZE },
        { x = 97, y = 46, min = 0, max = 200, vals = { 9 }, to = SMLSIZE },
    },
}
