return {
    read              = 92, -- MSP_FILTER_CONFIG
    write             = 93, -- MSP_SET_FILTER_CONFIG
    eepromWrite       = true,
    reboot            = false,
    title             = "Filters",
    minBytes          = 37,
    outputBytes       = 37,
    yMinLimit         = 12,
    yMaxLimit         = 84,
    text= {
        { t = "Gyro Lowpass 1 Dynamic",   x = 2,  y = 12,  to = SMLSIZE },
        { t = "Min Cutoff",               x = 12, y = 20,  to = SMLSIZE },
        { t = "Max Cutoff",               x = 12, y = 28,  to = SMLSIZE },
        { t = "Filter Type",              x = 12, y = 36,  to = SMLSIZE },

        { t = "Gyro Lowpass 1",           x = 2,  y = 44,  to = SMLSIZE },
        { t = "Cutoff",                   x = 12, y = 52,  to = SMLSIZE },
        { t = "Filter Type",              x = 12, y = 60,  to = SMLSIZE },

        { t = "Gyro Lowpass 2",           x = 2,  y = 68,  to = SMLSIZE },
        { t = "Cutoff",                   x = 12, y = 76,  to = SMLSIZE },
        { t = "Filter Type",              x = 12, y = 84,  to = SMLSIZE },

        { t = "Gyro Notch 1",             x = 2,  y = 92,  to = SMLSIZE },
        { t = "Center",                   x = 12, y = 100, to = SMLSIZE },
        { t = "Cutoff",                   x = 12, y = 108, to = SMLSIZE },

        { t = "Gyro Notch 2",             x = 2,  y = 116, to = SMLSIZE },
        { t = "Center",                   x = 12, y = 124, to = SMLSIZE },
        { t = "Cutoff",                   x = 12, y = 132, to = SMLSIZE },

        { t = "D Term Lowpass 1 Dynamic", x = 2,  y = 140, to = SMLSIZE },
        { t = "Min Cutoff",               x = 12, y = 148, to = SMLSIZE },
        { t = "Max Cutoff",               x = 12, y = 156, to = SMLSIZE },
        { t = "Filter Type",              x = 12, y = 164, to = SMLSIZE },

        { t = "D Term Lowpass 1",         x = 2,  y = 172, to = SMLSIZE },
        { t = "Cutoff",                   x = 12, y = 180, to = SMLSIZE },
        { t = "Filter Type",              x = 12, y = 188, to = SMLSIZE },

        { t = "D Term Lowpass 2",         x = 2,  y = 196, to = SMLSIZE },
        { t = "Cutoff",                   x = 12, y = 204, to = SMLSIZE },
        { t = "Filter Type",              x = 12, y = 212, to = SMLSIZE },

        { t = "D Term Notch",             x = 2,  y = 220, to = SMLSIZE },
        { t = "Center",                   x = 12, y = 228, to = SMLSIZE },
        { t = "Cutoff",                   x = 12, y = 236, to = SMLSIZE },

        { t = "Yaw Lowpass",              x = 2,  y = 244, to = SMLSIZE },
        { t = "Cutoff",                   x = 12, y = 252, to = SMLSIZE },
    },
    fields = {
        -- Gyro Lowpass 1 Dynamic
        { x = 87, y = 20, min = 0, max = 1000,   to = SMLSIZE, vals = { 30, 31 } },
        { x = 87, y = 28, min = 0, max = 1000,   to = SMLSIZE, vals = { 32, 33 } },
        { x = 87, y = 36, min = 0, max = 1,      to = SMLSIZE, vals = { 25 }, table = { [0] = "PT1", [1] = "BIQUAD" } },

        -- Gyro Lowpass 1
        { x = 87, y = 52, min = 0, max = 16000,  to = SMLSIZE, vals = { 21, 22 } },
        { x = 87, y = 60, min = 0, max = 1,      to = SMLSIZE, vals = { 25 }, table = { [0] = "PT1", [1] = "BIQUAD" } },

        -- Gyro Lowpass 2
        { x = 87, y = 76, min = 0, max = 16000,  to = SMLSIZE, vals = { 23, 24 } },
        { x = 87, y = 84, min = 0, max = 1,      to = SMLSIZE, vals = { 26 }, table = { [0] = "PT1", [1] = "BIQUAD" } },

        -- Gyro Notch 1
        { x = 87, y = 100, min = 0, max = 16000, to = SMLSIZE, vals = { 6, 7 } },
        { x = 87, y = 108, min = 0, max = 16000, to = SMLSIZE, vals = { 8, 9 } },

        -- Gyro Notch 2
        { x = 87, y = 124, min = 0, max = 16000, to = SMLSIZE, vals = { 14, 15 } },
        { x = 87, y = 132, min = 0, max = 16000, to = SMLSIZE, vals = { 16, 17 } },

        -- D Term Lowpass 1 Dynamic
        { x = 87, y = 148, min = 0, max = 1000,  to = SMLSIZE, vals = { 34, 35 } },
        { x = 87, y = 156, min = 0, max = 1000,  to = SMLSIZE, vals = { 36, 37 } },
        { x = 87, y = 164, min = 0, max = 1,     to = SMLSIZE, vals = { 18 }, table = { [0] = "PT1", [1] = "BIQUAD" } },

        -- D Term Lowpass 1
        { x = 87, y = 180, min = 0, max = 16000, to = SMLSIZE, vals = { 2, 3 } },
        { x = 87, y = 188, min = 0, max = 1,     to = SMLSIZE, vals = { 18 }, table = { [0] = "PT1", [1] = "BIQUAD" } },

        -- D Term Lowpass 2
        { x = 87, y = 204, min = 0, max = 16000, to = SMLSIZE, vals = { 27, 28 } },
        { x = 87, y = 212, min = 0, max = 1,     to = SMLSIZE, vals = { 29 }, table = { [0] = "PT1", [1] = "BIQUAD" } },

        -- D Term Notch
        { x = 87, y = 228, min = 0, max = 16000, to = SMLSIZE, vals = { 10, 11 } },
        { x = 87, y = 236, min = 0, max = 16000, to = SMLSIZE, vals = { 12, 13 } },

        -- Yaw Lowpass
        { x = 87, y = 252, min = 0, max = 500,   to = SMLSIZE, vals = { 4, 5 } },
    }
}
