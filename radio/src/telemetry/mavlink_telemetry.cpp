#include "opentx.h"

#if defined (LUA)
    #undef G // kill Lua convenience macro that is colliding with a MAVLink convenience macro

    #include "lua/mavlink2lua.h"
    #include "lua/lua_api.h"
#endif

#include <common/mavlink.h>
#include <mavlink_types.h>

#define CHECK_PAYLOAD_SIZE(id) if (payload_space < MAVLINK_MSG_ID_ ## id ## _LEN) return false
#define MAVLINK_CHANNEL MAVLINK_COMM_0


const uint8_t OUR_SYSTEM_ID = 250;
const uint8_t OUR_COMPONENT_ID = MAV_COMP_ID_UART_BRIDGE;

static mavlink_message_t msg;
static mavlink_status_t status;
static uint8_t send_buffer[MAVLINK_MAX_PACKET_LEN];

#if !defined(LUA)
    // MAVLink telemetry depends on Lua
    #define lsScripts nullptr
#endif


uint8_t prepare_heartbeat(mavlink_channel_t chan) {
	TRACE("prepare_heartbeat on %d", chan);
    mavlink_msg_heartbeat_pack(OUR_SYSTEM_ID, OUR_COMPONENT_ID, &msg, 
    		MAV_TYPE_GCS, 			// our type
    		MAV_AUTOPILOT_GENERIC, 	// type of supported autopilot
    		0, 						// base mode
    		0, 						// custom mode
    		MAV_STATE_ACTIVE		// our state
	);
	uint8_t len = mavlink_msg_to_send_buffer(send_buffer, &msg);
	return len;
}


void handle_message(mavlink_channel_t chan, const mavlink_message_t& msg) {
#if defined(LUA)
	TRACE("handle_message(%x)", msg.msgid);
	ParseFuncPtr parseFunc = funcmap[msg.msgid];
	if (lsScripts && parseFunc) {
		TRACE("calling parseFunc %x with L=%x", parseFunc, lsScripts);
		parseFunc(lsScripts, msg);
		TRACE("done");
	}
	else {
		TRACE("parseFunc or lsScripts was NULL");
	}
#endif
}


bool processMavlinkTelemetryData(uint8_t data)
{
    bool newmsg = mavlink_parse_char(MAVLINK_CHANNEL, data, &msg, &status);
    if (newmsg) {
    	// we recevied a complete and valid message
        handle_message(MAVLINK_CHANNEL, msg);
    } 
    return !newmsg;
}


void wakeupMavlinkTelemetry(bool recvInProgress) 
{
#if defined(LUA)
	static uint8_t loopCount10ms = 10;
	static uint8_t loopCount100ms = 10;
	static uint8_t loopCount1000ms = 10;
	static bool mavlinkHeartbeatDue = false;

	if (!recvInProgress) {
		// send now

	    if (mavlinkHeartbeatDue) {
		    // send heartbeat
			mavlinkHeartbeatDue = false;
			uint8_t len = prepare_heartbeat(MAVLINK_CHANNEL);
			bool success  __attribute__((unused));
		        success = serial2SendBuffer(send_buffer, len);
			TRACE("hb sent: %d", success);
		}
	}


	// prescale call frequency of 10ms
    if (!loopCount10ms--) {
    	// every 100ms
    	loopCount10ms = 10;
    	if (!loopCount100ms--) {
    		// every 1 sec
    		loopCount100ms = 10;
	    	provide_MavlinkStatus(lsScripts, status);
    		mavlinkHeartbeatDue = true;

	    	if (!loopCount1000ms--) {
	    		// every 10s
	    		loopCount1000ms = 10;
	    		TRACE("10s");
			}
    	}
    }
#endif
}
