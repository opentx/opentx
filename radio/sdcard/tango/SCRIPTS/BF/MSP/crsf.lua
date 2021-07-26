
-- CRSF Devices
CRSF_ADDRESS_BETAFLIGHT          = 0xC8
CRSF_ADDRESS_RADIO_TRANSMITTER   = 0xEA
-- CRSF Frame Types
CRSF_FRAMETYPE_MSP_REQ           = 0x7A      -- response request using msp sequence as command
CRSF_FRAMETYPE_MSP_RESP          = 0x7B      -- reply with 60 byte chunked binary
CRSF_FRAMETYPE_MSP_WRITE         = 0x7C      -- write with 60 byte chunked binary 

crsfMspCmd = 0
crsfMspHeader = {}

protocol.mspSend = function(payload)
    local payloadOut = { CRSF_ADDRESS_BETAFLIGHT, CRSF_ADDRESS_RADIO_TRANSMITTER }
    for i=1, #(payload) do
        payloadOut[i+2] = payload[i]
    end
    return crossfireTelemetryPush(crsfMspCmd, payloadOut)
end

protocol.mspRead = function(cmd)
    crsfMspCmd = CRSF_FRAMETYPE_MSP_REQ
    return mspSendRequest(cmd, {})
end

protocol.mspWrite = function(cmd, payload)
    crsfMspCmd = CRSF_FRAMETYPE_MSP_WRITE
    return mspSendRequest(cmd, payload)
end

protocol.mspPoll = function()
    local command, data = crossfireTelemetryPop()
    if command == CRSF_FRAMETYPE_MSP_RESP then
        if data[1] == CRSF_ADDRESS_RADIO_TRANSMITTER and data[2] == CRSF_ADDRESS_BETAFLIGHT then
            local mspData = {}
            for i=3, #(data) do
                mspData[i-2] = data[i]
            end
            return mspReceivedReply(mspData)
        end
    end
    return nil
end
