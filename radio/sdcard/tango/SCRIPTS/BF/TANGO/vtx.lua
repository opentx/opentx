
return {
    read           = 88, -- MSP_VTX_CONFIG
    write          = 89, -- MSP_VTX_SET_CONFIG
    eepromWrite    = true,
    reboot         = false,
    title          = "VTX",
    minBytes       = 5,
    prevBandVal    = 0,
    prevChanVal    = 0,
    prevFreqVal    = 0,
    lastFreqUpdTS  = 0,
    freqModCounter = 0,
    text = {},
    fields = {
        { t = "Band",    x = 10,  y = 14, sp = 30, min=0, max=5, vals = { 2 }, to = SMLSIZE, table = { [0]="U", "A", "B", "E", "F", "R" }, upd = function(self) self.handleBandChanUpdate(self) end },
        { t = "Chan",    x = 10,  y = 24, sp = 30, min=1, max=8, vals = { 3 }, to = SMLSIZE, upd = function(self) self.handleBandChanUpdate(self) end },
        { t = "Power",   x = 10,  y = 34, sp = 30, min=1, vals = { 4 }, to = SMLSIZE, upd = function(self) self.updatePowerTable(self) end },
        { t = "Pit",     x = 10,  y = 44, sp = 30, min=0, max=1, vals = { 5 }, to = SMLSIZE, table = { [0]="OFF", "ON" } },
        { t = "Dev",     x = 70,  y = 14, sp = 25, write = false, ro = true, vals = { 1 }, to = SMLSIZE , table = { [1]="6705",[3]="SA",[4]="Tramp",[255]="None"} },
        { t = "Freq",    x = 70,  y = 24, sp = 25, min = 5000, max = 5999, vals = { 6 }, to = SMLSIZE, upd = function(self) self.handleFreqValUpdate(self) end },
    },
    freqLookup = {
        { 5865, 5845, 5825, 5805, 5785, 5765, 5745, 5725 }, -- Boscam A
        { 5733, 5752, 5771, 5790, 5809, 5828, 5847, 5866 }, -- Boscam B
        { 5705, 5685, 5665, 5645, 5885, 5905, 5925, 5945 }, -- Boscam E
        { 5740, 5760, 5780, 5800, 5820, 5840, 5860, 5880 }, -- FatShark
        { 5658, 5695, 5732, 5769, 5806, 5843, 5880, 5917 }, -- RaceBand
    },
    postLoad = function (self)
        if (self.values[2] or 0) < 0 or (self.values[3] or 0) == 0 or (self.values[4] or 0) == 0 then
            self.values = {}
        else
            self.prevBandVal = 0                      -- reset value trackers
            self.prevChanVal = 0
            self.prevFreqVal = 0
            local rFreq
            if (self.values[7] or 0) > 0 then
                rFreq = math.floor(self.values[6] + (self.values[7] * 256))
            else
                rFreq = 0
            end
            if (self.values[2] or 0) > 0 then    -- band != 0
                if rFreq > 0 then             
                    self.prevFreqVal = rFreq
                    self.prevBandVal = self.values[2]
                    self.prevChanVal = self.values[3]
                    self.fields[1].min = 0   -- make sure 'U' band allowed
                    self.eepromWrite = true
                    self.fields[6].value = rFreq
                    self.values[6] = rFreq
                else                         -- if user freq not supported then
                    self.fields[1].min = 1   -- don't allow 'U' band
                    self.eepromWrite = false -- don't write EEPROM on older Betaflight versions
                end
            else   -- band == 0
                if rFreq > 0 then
                    self.prevFreqVal = rFreq
                    self.fields[1].min = 0         -- make sure 'U' band allowed
                    self.eepromWrite = true
                    self.fields[6].value = rFreq
                    self.values[6] = rFreq
                                                   -- set chan via freq / 100
                    self.prevChanVal = clipValue(math.floor((rFreq - 5100) / 100),
                                                 self.fields[2].min, self.fields[2].max)
                    self.fields[2].value = self.prevChanVal
                    self.values[3] = self.prevChanVal
                else
                    self.values = {}
                end
            end
        end
    end,
    preSave = function(self)
        local valsTemp = {}
        if self.values then
            local channel
            if self.values[2] > 0 then           -- band != 0
                channel = (self.values[2]-1)*8 + self.values[3]-1
            elseif self.fields[6].value then     -- band == 0
                channel = self.fields[6].value
            else
                channel = 24
            end
            valsTemp[1] = bit32.band(channel,0xFF)
            valsTemp[2] = bit32.rshift(channel,8)
            valsTemp[3] = self.values[4]
            valsTemp[4] = self.values[5]
        end
        return valsTemp
    end,
    -- find closest value in freq table that is above/below given freq
    findNextInFreqTable = function(self, newFreq)
        local startBand
        local endBand
        local incFlag       -- freq increasing or decreasing
        if newFreq > self.prevFreqVal then
            incFlag = 1
            startBand = 1
            endBand = self.fields[1].max
        else
            incFlag = -1
            startBand = self.fields[1].max
            endBand = 1
        end
        local curBand = self.values[2]
        local curChan = self.values[3]
        local selBand = 0
        local selChan = 0
        local selFreq = 0
        local diffVal = 9999
        local fVal
        local minChan = self.fields[2].min
        local maxChan = self.fields[2].max
             -- need to scan bands in same "direction" as 'incFlag'
             --  so same-freq selections will be handled properly (F8 & R7)
        for band=startBand,endBand,incFlag do
            for chan=minChan,maxChan do
                if band ~= curBand or chan ~= curChan then  -- don't reselect same band/chan
                    fVal = self.freqLookup[band][chan]
                    if incFlag > 0 then
                        if fVal >= self.prevFreqVal and fVal - self.prevFreqVal < diffVal then
                                  -- if same freq then only select if "next" band:
                            if fVal ~= self.prevFreqVal or band > curBand then
                                selBand = band
                                selChan = chan
                                selFreq = fVal
                                diffVal = fVal - self.prevFreqVal
                            end
                        end
                    else
                        if fVal <= self.prevFreqVal and self.prevFreqVal - fVal < diffVal then
                                  -- if same freq then only select if "previous" band:
                            if fVal ~= self.prevFreqVal or band < curBand then
                                selBand = band
                                selChan = chan
                                selFreq = fVal
                                diffVal = self.prevFreqVal - fVal
                            end
                        end
                    end
                end
            end
        end
        return selFreq, selBand, selChan
    end,
    -- returns the next user-frequency value in MHz; implements an
    --  "exponential" modification rate so dialing in values is faster
    getNextUserFreqValue = function(self, newFreq)
        local now = getTime()     -- track rate of change for possible mod speedup
        if now < self.lastFreqUpdTS + 15 then
            self.freqModCounter = self.freqModCounter + (15-(self.lastFreqUpdTS-now))  -- increase counter for mod speedup
        else
            self.freqModCounter = 0   -- no mod speedup
        end
        local uFreq
        if self.freqModCounter > 65 then  -- rate is fast enough; do mod speedup
            if newFreq > self.prevFreqVal then
                uFreq = clipValue(newFreq + math.floor(self.freqModCounter / 65),
                                  self.fields[6].min, self.fields[6].max)
            else
                uFreq = clipValue(newFreq - math.floor(self.freqModCounter / 65),
                                  self.fields[6].min, self.fields[6].max)
            end
        else
            uFreq = newFreq
        end
        self.lastFreqUpdTS = now
        return uFreq
    end,
    updatePowerTable = function(self)
        if self.values and not self.fields[3].table then
            if self.values[1] == 1 then          -- RTC6705
                self.fields[3].table = { 25, 200 }
                self.fields[3].max = 2
                self.fields[4].t = nil       -- don't display Pit field
                self.fields[4].table = { [0]="", "" }
            elseif self.values[1] == 3 then      -- SmartAudio
                self.fields[3].table = { 25, 200, 500, 800 }
                self.fields[3].max = 4
            elseif self.values[1] == 4 then      -- Tramp
                self.fields[3].table = { 25, 100, 200, 400, 600 }
                self.fields[3].max = 5
            elseif self.values[1] == 255 then    -- None/Unknown
                self.fields[3].t = nil       -- don't display Power field
                self.fields[3].max = 1
                self.fields[3].table = { [1]="" }
                self.fields[4].t = nil       -- don't display Pit field
                self.fields[4].table = { [0]="", "" }
            end
        end
    end,
    handleBandChanUpdate = function(self)
        if (#(self.values) or 0) >= self.minBytes then
            if (self.values[3] or 0) > 0 then
                if self.values[2] ~= self.prevBandVal or self.values[3] ~= self.prevChanVal then
                    if self.values[2] > 0 then   -- band != 0
                        self.prevFreqVal = self.freqLookup[self.values[2]][self.values[3]]
                    else   -- band == 0; set freq via channel*100
                        self.prevFreqVal = math.floor(5100 + (self.values[3] * 100))
                    end
                    self.fields[6].value = self.prevFreqVal
                    self.values[6] = self.prevFreqVal
                    self.prevBandVal = self.values[2]
                    self.prevChanVal = self.values[3]
                end
            end
        end
    end,
    handleFreqValUpdate = function(self)
        if (#(self.values) or 0) >= self.minBytes and (self.fields[6].value or 0) > 0 then
            local newFreq = self.fields[6].value
            if newFreq ~= self.prevFreqVal then
                if self.values[2] == 0 then
                        -- band == 0
                    local uFreq = self.getNextUserFreqValue(self, newFreq)
                    self.prevFreqVal = uFreq
                    if uFreq ~= newFreq then
                        self.fields[6].value = uFreq
                        self.values[6] = uFreq
                    end
                        -- set channel value via freq/100
                    self.prevChanVal = clipValue(math.floor((self.prevFreqVal - 5100) / 100),
                                                 self.fields[2].min, self.fields[2].max)
                    self.fields[2].value = self.prevChanVal
                    self.values[3] = self.prevChanVal
                else
                        -- band != 0; find closest freq in table that is above/below dialed freq
                    local selFreq, selBand, selChan = self.findNextInFreqTable(self, newFreq)
                    if selFreq > 0 then
                        self.prevFreqVal = selFreq
                        self.prevBandVal = selBand
                        self.prevChanVal = selChan
                        self.fields[6].value = selFreq          -- using new freq from table
                        self.values[6] = selFreq
                        self.fields[1].value = self.prevBandVal -- set band value for freq
                        self.values[2] = self.prevBandVal
                        self.fields[2].value = self.prevChanVal -- set channel value for freq
                        self.values[3] = self.prevChanVal
                    else
                        self.fields[6].value = self.prevFreqVal -- if no match then revert freq
                        self.values[6] = self.prevFreqVal
                    end
                end
            end
        end
    end
}