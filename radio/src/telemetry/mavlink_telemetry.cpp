#include "lua/mavlink2lua.h"
#include "opentx.h"

#include <common/mavlink.h>
#include <mavlink_types.h>

#define CHECK_PAYLOAD_SIZE(id) if (payload_space < MAVLINK_MSG_ID_ ## id ## _LEN) return false
#define MAVLINK_CHANNEL MAVLINK_COMM_0


const uint8_t OUR_SYSTEM_ID = 250;
const uint8_t OUR_COMPONENT_ID = MAV_COMP_ID_UART_BRIDGE;

static mavlink_message_t msg;
static mavlink_status_t status;
static uint8_t send_buffer[MAVLINK_MAX_PACKET_LEN];

bool heartbeat_due = false;

uint8_t prepare_heartbeat(mavlink_channel_t chan) {
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
	TRACE("handle_message(%x)", msg.msgid);
	ParseFuncPtr parseFunc = funcmap[msg.msgid];

	if (parseFunc) {
		TRACE("calling parseFunc %x with L=%x", parseFunc, lsScripts);
		parseFunc(lsScripts, msg);
		TRACE("done");
	}
	else {
		TRACE("parseFunc was NULL");
	}
}


void processMavlinkTelemetryData(uint8_t data)
{
    bool newmsg = mavlink_parse_char(MAVLINK_CHANNEL, data, &msg, &status);
    if (newmsg) {
    	// we recevied a complete and valid message
        handle_message(MAVLINK_CHANNEL, msg);

        if (serial2TxFifo.isEmpty()) {
	        // after msg rcv is complete, this should be a good time to send
	        if (false) {
	            // handle outgoing messages perpared from Lua
		    	// TODO
	        }
	        else if (heartbeat_due) {
		        // or send heartbeat if due
	        	uint8_t len = prepare_heartbeat(MAVLINK_CHANNEL);
	        	for (uint8_t i=0; i<len; i++) {
	    			serial2TxFifo.push(send_buffer[i]);
	        	}
	    	}
	    }
    } 
}


