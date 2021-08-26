
-- Protocol version
MSP_VERSION = bit32.lshift(1,5)
MSP_STARTFLAG = bit32.lshift(1,4)

-- Sequence number for next MSP packet
local mspSeq = 0
local mspRemoteSeq = 0
local mspRxBuf = {}
local mspRxIdx = 1
local mspRxCRC = 0
local mspRxReq = 0
local mspStarted = false
local mspLastReq = 0
local mspTxBuf = {}
local mspTxIdx = 1
local mspTxCRC = 0
local mspTxPk = 0

mspPendingRequest = false

function mspProcessTxQ()
    if (#(mspTxBuf) == 0) then
        return false
    end
    if not protocol.push() then
        return true
    end
    local payload = {}
    payload[1] = mspSeq + MSP_VERSION
    mspSeq = bit32.band(mspSeq + 1, 0x0F)
    if mspTxIdx == 1 then
        -- start flag
        payload[1] = payload[1] + MSP_STARTFLAG
    end
    local i = 2
    while (i <= protocol.maxTxBufferSize) do
        if mspTxIdx > #(mspTxBuf) then
            break
        end
        payload[i] = mspTxBuf[mspTxIdx]
        mspTxIdx = mspTxIdx + 1
        mspTxCRC = bit32.bxor(mspTxCRC,payload[i])  
        i = i + 1
    end
    if i <= protocol.maxTxBufferSize then
        payload[i] = mspTxCRC
        i = i + 1
      -- zero fill
        while i <= protocol.maxTxBufferSize do
            payload[i] = 0
            i = i + 1
        end
        if protocol.mspSend(payload) then
            mspTxPk = mspTxPk + 1
        end
        mspTxBuf = {}
        mspTxIdx = 1
        mspTxCRC = 0     
        return false
    end
    if protocol.mspSend(payload) then
        mspTxPk = mspTxPk + 1
    end
    return true
end

function mspSendRequest(cmd, payload)
    -- busy
    if #(mspTxBuf) ~= 0 or not cmd then
        return nil
    end
    mspTxBuf[1] = #(payload)
    mspTxBuf[2] = bit32.band(cmd,0xFF)  -- MSP command
    for i=1,#(payload) do
        mspTxBuf[i+2] = bit32.band(payload[i],0xFF)
    end
    mspLastReq = cmd
    return mspProcessTxQ()
end

function mspReceivedReply(payload)
    local idx      = 1
    local head     = payload[idx]
    local err_flag = (bit32.band(head,0x20) ~= 0)
    idx = idx + 1
    if err_flag then
        -- error flag set
        mspStarted = false
        return nil
    end
    local start = (bit32.band(head,0x10) ~= 0)
    local seq   = bit32.band(head,0x0F)
    if start then
        -- start flag set
        mspRxIdx = 1
        mspRxBuf = {}
        mspRxSize = payload[idx]
        mspRxCRC  = bit32.bxor(mspRxSize,mspLastReq)
        mspRxReq  = mspLastReq
        idx = idx + 1
        mspStarted = true
    elseif not mspStarted then
        return nil
    elseif bit32.band(mspRemoteSeq + 1, 0x0F) ~= seq then
        mspStarted = false
        return nil
    end
    while (idx <= protocol.maxRxBufferSize) and (mspRxIdx <= mspRxSize) do
        mspRxBuf[mspRxIdx] = payload[idx]
        mspRxCRC = bit32.bxor(mspRxCRC,payload[idx])
        mspRxIdx = mspRxIdx + 1
        idx = idx + 1
    end
    if idx > protocol.maxRxBufferSize then
        mspRemoteSeq = seq
        return true
    end
    -- check CRC
    if mspRxCRC ~= payload[idx] then
        mspStarted = false
        return nil
    end
    mspStarted = false
    return mspRxBuf
end

function mspPollReply()
    while true do
        ret = protocol.mspPoll()
        if type(ret) == "table" then
            mspLastReq = 0
            return mspRxReq, ret
        else
            break
        end
    end
    return nil
end
