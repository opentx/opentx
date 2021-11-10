return {
    read              = 90, -- MSP_ADVANCED_CONFIG
    write             = 91, -- MSP_SET_ADVANCED_CONFIG
    reboot            = true,
    eepromWrite       = true,
    title             = "PWM",
    minBytes             = 9,
    text= {
        { t = "32K", x = 10, y = 14, to = SMLSIZE },
        { t = "Gyro", x = 10, y = 24, to = SMLSIZE },
        { t = "PID", x = 10, y = 34, to = SMLSIZE },
        { t = "Prot", x = 58, y = 14, to = SMLSIZE },
        { t = "Unsync", x = 58, y = 24, to = SMLSIZE },
        { t = "PWM", x = 58, y = 34, to = SMLSIZE },
        { t = "Idle", x = 58, y = 44, to = SMLSIZE }
    },
    fields = {
        { x = 32, y = 14, vals = { 9 }, min = 0, max = 1, to = SMLSIZE, table = { [0] = "OFF", "ON" }, upd = function(self) self.updateRateTables(self) end },
        { x = 32, y = 24, vals = { 1 }, min = 1, max = 32, to = SMLSIZE, upd = function(self) self.updatePidRateTable(self) end },
        { x = 32, y = 34, vals = { 2 }, min = 1, max = 16, to = SMLSIZE, },
        { x = 90, y = 14, vals = { 4 }, min = 0, max = 9, to = SMLSIZE, table = { [0] = "OFF", "OS125", "OS42", "MSHOT","BRSH", "DS150", "DS300", "DS600","DS1200", "PS1000" } },
        { x = 90, y = 24, vals = { 3 }, min = 0, max = 1, to = SMLSIZE, table = { [0] = "OFF", "ON" } },
        { x = 90, y = 34, vals = { 5, 6 }, min = 200, max = 32000, to = SMLSIZE },
        { x = 90, y = 44, vals = { 7, 8 }, min = 0, max = 2000, to = SMLSIZE, scale = 100 },
    },
    calculateGyroRates = function(self, baseRate)
        self.gyroRates = {}
        self.fields[2].table = {}
        for i=1, 32 do
            self.gyroRates[i] = baseRate/i
            local fmt = nil
            self.fields[2].table[i] = string.format("%.2f",baseRate/i)
        end
    end,
    calculatePidRates = function(self, baseRate)
        self.fields[3].table = {}
        for i=1, 16 do
            self.fields[3].table[i] = string.format("%.2f",baseRate/i)
        end
    end,
    updateRateTables = function(self)
        if self.values[9] == 0 then
            self.calculateGyroRates(self, 8)
            self.calculatePidRates(self, 8)
        elseif self.values[9] == 1 then
            self.calculateGyroRates(self, 32)
            self.calculatePidRates(self, 32)
        end
    end,
    updatePidRateTable = function(self)
        local newRateIdx = self.values[1]
        local newRate = self.gyroRates[newRateIdx]
        self.calculatePidRates(self, newRate)
    end
}
