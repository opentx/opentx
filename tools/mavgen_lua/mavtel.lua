--MAVLink demo telemetry script
--Init
local function init()
end

local known_hb = {}

--Background
local function background()
    if (last_HEARTBEAT) then 
        if nil == known_hb[last_HEARTBEAT.sysid] then 
            -- new system discovered
            known_hb[last_HEARTBEAT.sysid] = {}
        end
        if not known_hb[last_HEARTBEAT.sysid][last_HEARTBEAT.compid] then
            known_hb[last_HEARTBEAT.sysid][last_HEARTBEAT.compid] = true
        end
    end
end


local function mask()
    lcd.drawText(1,0, 'MAVLink Status', MIDSIZE)
    lcd.drawText(8, 15, "pkts received ")
    lcd.drawText(8, 25, "parse errors")
    lcd.drawText(8, 35, "buffer overrun")
    lcd.drawText(8, 45, "rx_success_cnt")
    lcd.drawText(8, 55, "rx_drop_count")
--
    lcd.drawText(120, 15, "HB from");

end

local function values()
    if (link_status) then
        lcd.drawText(95, 15, tostring(link_status.msg_received))
        lcd.drawText(95, 25, tostring(link_status.parse_errors))
        lcd.drawText(95, 35, tostring(link_status.buffer_overrun))
        lcd.drawText(95, 45, tostring(link_status.packet_rx_success_count))
        lcd.drawText(95, 55, tostring(link_status.packet_rx_drop_count))
    end

    compstr = ""
    for sysid,comps in pairs(known_hb) do
        for comp,junk in pairs(comps) do
            compstr = compstr..tostring(sysid).."."..tostring(comp)
            compstr = compstr.." "
        end
    end
    lcd.drawText(125, 25, compstr)
end

--Main
local function run(event)
    lcd.clear()
    mask()
    values()
    return 0
end

return {init=init, run=run, background=background}


--[[
    TRACE("provide_MavlinkStatus(%x)", L);
    assert(L);

    lua_newtable(L);
    push_value(L, "msg_received", status.msg_received);
    push_value(L, "buffer_overrun", status.buffer_overrun);
    push_value(L, "parse_error", status.parse_error);
    push_value(L, "parse_state", status.parse_state);
    push_value(L, "packet_idx", status.packet_idx);
    push_value(L, "current_rx_seq", status.current_rx_seq);
    push_value(L, "current_tx_seq", status.current_tx_seq);
    push_value(L, "packet_rx_success_count", status.packet_rx_success_count);
    push_value(L, "packet_rx_drop_count", status.packet_rx_drop_count);
    push_value(L, "flags", status.flags);
    push_value(L, "signature_wait", status.signature_wait);
    // not supported yet: struct __mavlink_signing *signing;
    // not supported yet: struct __mavlink_signing_streams *signing_streams;
    lua_setglobal(L, "link_status");
--]]
