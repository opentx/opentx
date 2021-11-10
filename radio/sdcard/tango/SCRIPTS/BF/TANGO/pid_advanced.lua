return {
    read           = 94, -- MSP_PID_ADVANCED
    write          = 95, -- MSP_SET_PID_ADVANCED
    title          = "PID Adv.",
    reboot         = false,
    eepromWrite    = true,
    minBytes       = 46,
    outputBytes    = 46,
    yMinLimit      = 12,
    yMaxLimit      = 84,
    text = {
        { t = "Acro Trainer",      x = 2,  y = 12,  to = SMLSIZE },
        { t = "Angle Limit",       x = 12, y = 20,  to = SMLSIZE },

        { t = "Throttle Boost",    x = 2,  y = 28,  to = SMLSIZE },
        { t = "Absolute Control",  x = 2,  y = 36,  to = SMLSIZE },
        { t = "I Term Rotation",   x = 2,  y = 44,  to = SMLSIZE },
        { t = "VBAT Compensation", x = 2,  y = 52,  to = SMLSIZE },

        { t = "I Term Relax",      x = 2,  y = 60,  to = SMLSIZE },
        { t = "Axes",              x = 12, y = 68,  to = SMLSIZE },
        { t = "Type",              x = 12, y = 76,  to = SMLSIZE },

        { t = "Integrated Yaw",    x = 2,  y = 84,  to = SMLSIZE },

        { t = "Anti Gravity",      x = 2,  y = 92,  to = SMLSIZE },
        { t = "Mode",              x = 12, y = 100, to = SMLSIZE },
        { t = "Gain",              x = 12, y = 108, to = SMLSIZE },
        { t = "Threshold",         x = 12, y = 116, to = SMLSIZE },
    },
    fields = {
        { x = 87, y = 20,  to=SMLSIZE, min = 20,   max = 80,    vals = { 32 } },

        { x = 87, y = 28,  to=SMLSIZE, min = 0,    max = 100,   vals = { 31 } },
        { x = 87, y = 36,  to=SMLSIZE, min = 0,    max = 20,    vals = { 30 } },
        { x = 87, y = 44,  to=SMLSIZE, min = 0,    max = 1,     vals = { 26 }, table = { [0]="OFF", "ON" } },
        { x = 87, y = 52,  to=SMLSIZE, min = 0,    max = 1,     vals = { 8 },  table = { [0]="OFF", "ON" } },

        { x = 87, y = 68,  to=SMLSIZE, min = 0,    max = 4,     vals = { 28 }, table = { [0]="NONE", "RP", "RPY", "RP (inc)", "RPY (inc)" } },
        { x = 87, y = 76,  to=SMLSIZE, min = 0,    max = 1,     vals = { 29 }, table = { [0]="Gyro", "Setpoint" } },

        { x = 87, y = 84,  to=SMLSIZE, min = 0,    max = 1,     vals = { 45 }, table = { [0]="OFF", "ON" } },

        { x = 87, y = 100, to=SMLSIZE, min = 0,    max = 1,     vals = { 39 }, table = { [0]="Smooth", "Step" } },
        { x = 87, y = 108, to=SMLSIZE, min = 1000, max = 30000, vals = { 22, 23 }, scale = 1000, mult = 100 },
        { x = 87, y = 116, to=SMLSIZE, min = 20,   max = 1000,  vals = { 20, 21 } },
    }
}
