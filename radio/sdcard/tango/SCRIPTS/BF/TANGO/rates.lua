return {
    read           = 111, -- MSP_RC_TUNING
    write          = 204, -- MSP_SET_RC_TUNING
    title          = "Rates",
    reboot         = false,
    eepromWrite    = true,
    minBytes       = 16,
    yMinLimit      = 11,
    yMaxLimit      = 84,
    text = {
        { t = "RC",         x = 43, y = 11,  to = SMLSIZE },
        { t = "Rate",       x = 38, y = 18,  to = SMLSIZE },
        { t = "Super",      x = 63, y = 11,  to = SMLSIZE },
        { t = "Rate",       x = 66, y = 18,  to = SMLSIZE },
        { t = "RC",         x = 99, y = 11,  to = SMLSIZE },
        { t = "Expo",       x = 94, y = 18,  to = SMLSIZE },
        { t = "ROLL",       x = 10, y = 26,  to = SMLSIZE },
        { t = "PITCH",      x = 10, y = 36,  to = SMLSIZE },
        { t = "YAW",        x = 10, y = 46,  to = SMLSIZE },

        { t = "Throttle",   x = 10, y = 60,  to = SMLSIZE },
        { t = "Mid",        x = 20, y = 68,  to = SMLSIZE },
        { t = "Expo",       x = 20, y = 76,  to = SMLSIZE },
        { t = "Limit Type", x = 20, y = 84,  to = SMLSIZE },
        { t = "Limit %",    x = 20, y = 92,  to = SMLSIZE },
        { t = "TPA",        x = 10, y = 100, to = SMLSIZE },
        { t = "Rate",       x = 20, y = 108, to = SMLSIZE },
        { t = "Breakpoint", x = 20, y = 116, to = SMLSIZE },
    },
    fields = {
        -- RC Rates
        { x = 39, y = 26,  vals = { 1 },     min = 0,    max = 255, scale = 100, to = SMLSIZE },
        { x = 39, y = 36,  vals = { 13 },    min = 0,    max = 255, scale = 100, to = SMLSIZE },
        { x = 39, y = 46,  vals = { 12 },    min = 0,    max = 255, scale = 100, to = SMLSIZE },
        -- Super Rates
        { x = 66, y = 26,  vals = { 3 },     min = 0,    max = 100, scale = 100, to = SMLSIZE },
        { x = 66, y = 36,  vals = { 4 },     min = 0,    max = 100, scale = 100, to = SMLSIZE },
        { x = 66, y = 46,  vals = { 5 },     min = 0,    max = 255, scale = 100, to = SMLSIZE },
        -- RC Expo
        { x = 94, y = 26,  vals = { 2 },     min = 0,    max = 100, scale = 100, to = SMLSIZE },
        { x = 94, y = 36,  vals = { 14 },    min = 0,    max = 100, scale = 100, to = SMLSIZE },
        { x = 94, y = 46,  vals = { 11 },    min = 0,    max = 100, scale = 100, to = SMLSIZE },
        -- Throttle
        { x = 70, y = 68,  vals = { 7 },     min = 0,    max = 100, scale = 100, to = SMLSIZE },
        { x = 70, y = 76,  vals = { 8 },     min = 0,    max = 100, scale = 100, to = SMLSIZE },
        { x = 70, y = 84,  vals = { 15 },    min = 0,    max = 2,   to = SMLSIZE, table = { [0] = "OFF", "SCALE", "CLIP" } },
        { x = 70, y = 92,  vals = { 16 },    min = 25,   max = 100, to = SMLSIZE },
        -- TPA
        { x = 70, y = 108, vals = { 6 } ,    min = 0,    max = 100, scale = 100, to = SMLSIZE },
        { x = 70, y = 116, vals = { 9, 10 }, min = 1000, max = 2000, to = SMLSIZE },
    },
}
