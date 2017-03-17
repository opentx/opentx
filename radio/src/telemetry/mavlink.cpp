/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x 
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*!	\file mavlink.cpp
 *	Mavlink telemetry decoder for Arducopter/Arduplane telemetry information.
 */


#include "telemetry/mavlink.h"

// this might need to move to the flight software
//static
mavlink_system_t mavlink_system = { 7, MAV_COMP_ID_MISSIONPLANNER };

// Mavlink message decoded Status Text
#define PARAM_NB_REPEAT 10
char mav_statustext[LEN_STATUSTEXT];
int8_t mav_heartbeat = 0;
int8_t mav_heartbeat_recv = 0;
uint8_t data_stream_start_stop = 0;


// Telemetry data hold
Telemetry_Data_t telemetry_data;

// *****************************************************
static void MAVLINK_parse_char(uint8_t c);
uint8_t telemetryRxBufferCount = 0;

void processTelemetryData(uint8_t byte) {
	MAVLINK_parse_char(byte);
}

/*!	\brief Reset basic Mavlink varables
 *	\todo Figure out exact function
 *
 */
void MAVLINK_reset(uint8_t warm_reset) {
	if (warm_reset && telemetry_data.status) {
		mav_statustext[0] = 0;
	}

	mavlink_status_t* p_status = mavlink_get_channel_status(MAVLINK_COMM_0);
	p_status->current_rx_seq = 0;
	p_status->current_tx_seq = 0;
	memset(&telemetry_data, 0, sizeof(telemetry_data));
	telemetry_data.rcv_control_mode = ERROR_NUM_MODES;
	telemetry_data.req_mode = ERROR_NUM_MODES;

	telemetry_data.type = MAV_TYPE_ENUM_END;
	telemetry_data.autopilot = MAV_AUTOPILOT_ENUM_END;
	telemetry_data.type_autopilot = MAVLINK_INVALID_TYPE;

	mav_heartbeat = 0;
	mav_heartbeat_recv = 0;
	data_stream_start_stop = 0;
}

//! \brief initalize mavlink extension
void MAVLINK_Init(void)
{
  mav_statustext[0] = 0;
  MAVLINK_reset(0);
  telemetryPortInit(g_eeGeneral.mavbaud);
}

/*!	\brief Status log message
 *	\details Processes the mavlink status messages. This message contains a
 *	severity and a message. The severity is an enum difined by MAV_SEVERITY also
 *	see RFC-5424 for the severity levels.
 *	The original message is maximum 50 characters and is without termination
 *	character. For readablity on the 9x the only the first 15 (LEN_STATUSTEXT)
 *	characters are used. To get the full message you can use the commented
 *	funtion below.
 */

static inline void REC_MAVLINK_MSG_ID_STATUSTEXT(const mavlink_message_t* msg) {
	_MAV_RETURN_char_array(msg, mav_statustext, LEN_STATUSTEXT,  1);
}

/*!	\brief System status including cpu load, battery status and communication status.
 *	\details From this message we use use only the batery infomation. The rest
 *	is not realy of use while flying. The complete message can be decoded in to
 *	a struct with the first two commented lines.
 *  The batery votage is in mV. We devide by 100 to display tenths of volts.'
 *	\todo Add battery remaining variable in telemetry_data struct for estimated
 *	remaining battery. Decoding funtion allready in place.
 */

static inline void REC_MAVLINK_MSG_ID_SYS_STATUS(const mavlink_message_t* msg) {
	telemetry_data.vbat = mavlink_msg_sys_status_get_voltage_battery(msg) / 100; // Voltage * 10
	telemetry_data.ibat = mavlink_msg_sys_status_get_current_battery(msg) / 10;
	telemetry_data.rem_bat = mavlink_msg_sys_status_get_battery_remaining(msg);

#ifdef MAVLINK_PARAMS
	telemetry_data.vbat_low = (getMavlinParamsValue(BATT_MONITOR) > 0)
					&& (((float) telemetry_data.vbat / 10.0) < getMavlinParamsValue(LOW_VOLT)) && (telemetry_data.vbat > 50);
#else
	telemetry_data.vbat_low = (telemetry_data.rem_bat < 10);
#endif
}

/*!	\brief Receive rc channels
 *
 */
static inline void REC_MAVLINK_MSG_ID_RC_CHANNELS(const mavlink_message_t* msg) {
	uint8_t temp_scale = 5 + g_model.mavlink.rc_rssi_scale;
	telemetry_data.rc_rssi =  mavlink_msg_rc_channels_get_rssi(msg) * 20 / temp_scale;
}

/*!	\brief Receive raw rc channels
 *
 */
static inline void REC_MAVLINK_MSG_ID_RC_CHANNELS_RAW(const mavlink_message_t* msg) {
	uint8_t temp_rssi =(mavlink_msg_rc_channels_raw_get_rssi(msg) * 100) / 255;
	uint8_t temp_scale = 25 + g_model.mavlink.rc_rssi_scale * 5;
	telemetry_data.rc_rssi =  (temp_rssi * 100) / temp_scale;
}

/*!	\brief Arducopter specific radio message
 *
 */
static inline void REC_MAVLINK_MSG_ID_RADIO(const mavlink_message_t* msg) {
	if (msg->sysid != 51)
		return;
	telemetry_data.pc_rssi =  (mavlink_msg_radio_get_rssi(msg) * 100) / 255;
	telemetry_data.packet_drop = mavlink_msg_radio_get_rxerrors(msg);
	telemetry_data.packet_fixed = mavlink_msg_radio_get_fixed(msg);
	telemetry_data.radio_sysid = msg->sysid;
	telemetry_data.radio_compid = msg->compid;
}
static inline void REC_MAVLINK_MSG_ID_RADIO_STATUS(const mavlink_message_t* msg) {
	REC_MAVLINK_MSG_ID_RADIO(msg);
}

//! \brief Navigaion output message
static inline void REC_MAVLINK_MSG_ID_NAV_CONTROLLER_OUTPUT(const mavlink_message_t* msg) {
	telemetry_data.bearing = mavlink_msg_nav_controller_output_get_target_bearing(msg);
}

//! \brief Hud navigation message
static inline void REC_MAVLINK_MSG_ID_VFR_HUD(const mavlink_message_t* msg) {
	telemetry_data.heading = mavlink_msg_vfr_hud_get_heading(msg);
	telemetry_data.loc_current.rel_alt = mavlink_msg_vfr_hud_get_alt(msg);
}

/*!	\brief Heartbeat message
 *	\details Heartbeat message is used for the following information:
 *	type and autopilot is used to determine if the UAV is an ArduPlane or Arducopter
 */
static inline void REC_MAVLINK_MSG_ID_HEARTBEAT(const mavlink_message_t* msg) {
	telemetry_data.mode  = mavlink_msg_heartbeat_get_base_mode(msg);
	telemetry_data.custom_mode  = mavlink_msg_heartbeat_get_custom_mode(msg);
	telemetry_data.status = mavlink_msg_heartbeat_get_system_status(msg);
	telemetry_data.mav_sysid = msg->sysid;
	telemetry_data.mav_compid = msg->compid;
	uint8_t type = mavlink_msg_heartbeat_get_type(msg);
	uint8_t autopilot = mavlink_msg_heartbeat_get_autopilot(msg);
	if (type != telemetry_data.type || autopilot != telemetry_data.autopilot) {
		telemetry_data.type = mavlink_msg_heartbeat_get_type(msg);
		telemetry_data.autopilot = mavlink_msg_heartbeat_get_autopilot(msg);
		if (autopilot == MAV_AUTOPILOT_ARDUPILOTMEGA) {
			if (type == MAV_TYPE_QUADROTOR ||
					type == MAV_TYPE_COAXIAL ||
					type == MAV_TYPE_HELICOPTER ||
					type == MAV_TYPE_HEXAROTOR ||
					type == MAV_TYPE_OCTOROTOR ||
					type == MAV_TYPE_TRICOPTER) {
				telemetry_data.type_autopilot = MAVLINK_ARDUCOPTER;
			}
			else if (type == MAV_TYPE_FIXED_WING) {
				telemetry_data.type_autopilot = MAVLINK_ARDUPLANE;
			}
			else {
				telemetry_data.type_autopilot = MAVLINK_INVALID_TYPE;
			}
		}
		else {
			telemetry_data.type_autopilot = MAVLINK_INVALID_TYPE;
		}
	}
	telemetry_data.active = (telemetry_data.mode & MAV_MODE_FLAG_SAFETY_ARMED) ? true : false;
	mav_heartbeat = 3; // 450ms display '*'
	mav_heartbeat_recv = 1;
}

static inline void REC_MAVLINK_MSG_ID_HIL_CONTROLS(const mavlink_message_t* msg) {
	telemetry_data.nav_mode = mavlink_msg_hil_controls_get_mode(msg);
}

/*!	\brief Process GPS raw intger message
 *	\details This message contains the following data:
 *		- fix type: 0-1: no fix, 2: 2D fix, 3: 3D fix.
 *		- Latitude, longitude in 1E7 * degrees
 *		- Altutude 1E3 * meters above MSL.
 *		- Course over ground in 100 * degrees
 *		- GPS HDOP horizontal dilution of precision in cm (m*100).
 *		- Ground speed in m/s * 100
 */
static inline void REC_MAVLINK_MSG_ID_GPS_RAW_INT(const mavlink_message_t* msg) {
	telemetry_data.fix_type = mavlink_msg_gps_raw_int_get_fix_type(msg);
	telemetry_data.loc_current.lat = mavlink_msg_gps_raw_int_get_lat(msg) / 1E7;
	telemetry_data.loc_current.lon = mavlink_msg_gps_raw_int_get_lon(msg) / 1E7;
	telemetry_data.loc_current.gps_alt = mavlink_msg_gps_raw_int_get_alt(msg) / 1E3;
	telemetry_data.eph = mavlink_msg_gps_raw_int_get_eph(msg) / 100.0;
	telemetry_data.course = mavlink_msg_gps_raw_int_get_cog(msg) / 100.0;
	telemetry_data.v = mavlink_msg_gps_raw_int_get_vel(msg) / 100.0 ;
	telemetry_data.satellites_visible = mavlink_msg_gps_raw_int_get_satellites_visible(msg);
}

#ifdef MAVLINK_PARAMS
const pm_char *getParamId(uint8_t idx) {
	const pm_char *mav_params_id [((NB_PID_PARAMS / 2) + 4)]  = {
		PSTR("RATE_YAW"), // Rate Yaw
		PSTR("STB_YAW"), // Stabilize Yaw
		PSTR("RATE_PIT"), // Rate Pitch
		PSTR("STB_PIT"), // Stabilize Pitch
		PSTR("RATE_RLL"), // Rate Roll
		PSTR("STB_RLL"), // Stabilize Roll
		PSTR("THR_ALT"), // PSTR("THR_BAR"), // Altitude Hold
		PSTR("HLD_LON"), // Loiter
		PSTR("HLD_LAT"), // Loiter
		PSTR("NAV_LON"), // Nav WP
		PSTR("NAV_LAT"), // Nav WP
		PSTR("LOW_VOLT"), // Battery low voltage
		PSTR("VOLT_DIVIDER"), //
		PSTR("BATT_MONITOR"), //
		PSTR("BATT_CAPACITY")
	};
	uint8_t i;
	if (idx < NB_PID_PARAMS) {
		i = idx / 2;
	} else {
		i = idx - (NB_PID_PARAMS / 2);
	}
	return mav_params_id[i];
}

void setMavlinParamsValue(uint8_t idx, float val) {
	MavlinkParam_t *param = getParam(idx);
	if (idx < NB_PARAMS && val != param->value) {
		param->value = val;
		param->repeat = PARAM_NB_REPEAT;
		uint8_t link_idx = NB_PID_PARAMS;
		switch (idx) {
		case RATE_PIT_P:
		case RATE_PIT_I:
		case STB_PIT_P:
		case STB_PIT_I:
			link_idx = idx + 4;
			break;
		case RATE_RLL_P:
		case RATE_RLL_I:
		case STB_RLL_P:
		case STB_RLL_I:
			link_idx = idx - 4;
			break;
		case HLD_LON_P:
		case HLD_LON_I:
		case NAV_LON_P:
		case NAV_LON_I:
			link_idx = idx + 2;
			break;
		case HLD_LAT_P:
		case HLD_LAT_I:
		case NAV_LAT_P:
		case NAV_LAT_I:
			link_idx = idx - 2;
			break;
		default:
			break;
		}
		if (link_idx < NB_PID_PARAMS) {
			MavlinkParam_t *p = getParam(link_idx);
			p->value = val;
			p->repeat = PARAM_NB_REPEAT;
		}
	}
}

void putsMavlinParams(uint8_t x, uint8_t y, uint8_t idx, uint8_t att) {
	if (idx < NB_PARAMS) {
		const pm_char * s = getParamId(idx);
		char c;
		while ((c = pgm_read_byte(s++))) {
			lcdDrawChar(x, y, (c == '_' ? ' ' : c), 0);
			x += FW;
		}
		if (idx < NB_PID_PARAMS) {
			x = 11 * FW;
			lcdDrawChar(x, y, "PI"[idx & 0x01], att);
		}
	}
}

static inline void setParamValue(int8_t *id, float value) {
	int8_t *p_id;
	for (int8_t idx = 0; idx < NB_PARAMS; idx++) {
		const pm_char * s = getParamId(idx);
		p_id = id;
		while (1) {
			char c1 = pgm_read_byte(s++);
			if (!c1) {
				// Founded !
				uint8_t founded = !*p_id;
				if (idx < NB_PID_PARAMS) {
					p_id++;
					switch (*p_id++) {
					case 'P':
						founded = !*p_id;
						break;
					case 'I':
						founded = !*p_id;
						idx++;
						break;
					default:
						founded = 0;
						break;
					}
				}
				// test end of string char == 0 and a valid PI
				if (founded) {
					MavlinkParam_t *param = getParam(idx);
					param->repeat = 0;
					param->valid = 1;
					param->value = value;
					mav_req_params_nb_recv++;
				}
				return;
			} else if (c1 != *p_id++) {
				break;
			}
		}
		if (idx < NB_PID_PARAMS) {
			// Skip I Parameter from PID
			idx++;
		}
	}
}

static inline void REC_MAVLINK_MSG_ID_PARAM_VALUE(const mavlink_message_t* msg) {
	mavlink_param_value_t param_value;
	mavlink_msg_param_value_decode(msg, &param_value);
	char *id = param_value.param_id;
	setParamValue((int8_t*)id, param_value.param_value);
	data_stream_start_stop = 0; // stop data stream while getting params list
	watch_mav_req_params_list = mav_req_params_nb_recv < (NB_PARAMS - 5) ? 20 : 0; // stop timeout
}
#endif

static inline void handleMessage(mavlink_message_t* p_rxmsg) {
	switch (p_rxmsg->msgid) {
	case MAVLINK_MSG_ID_HEARTBEAT:
		REC_MAVLINK_MSG_ID_HEARTBEAT(p_rxmsg);
		break;
	case MAVLINK_MSG_ID_STATUSTEXT:
		REC_MAVLINK_MSG_ID_STATUSTEXT(p_rxmsg);
		AUDIO_WARNING1();
		break;
	case MAVLINK_MSG_ID_SYS_STATUS:
		REC_MAVLINK_MSG_ID_SYS_STATUS(p_rxmsg);
		break;
	case MAVLINK_MSG_ID_RC_CHANNELS:
		REC_MAVLINK_MSG_ID_RC_CHANNELS(p_rxmsg);
		break;
	case MAVLINK_MSG_ID_RC_CHANNELS_RAW:
		REC_MAVLINK_MSG_ID_RC_CHANNELS_RAW(p_rxmsg);
		break;
	case MAVLINK_MSG_ID_RADIO:
		REC_MAVLINK_MSG_ID_RADIO(p_rxmsg);
		break;
	case MAVLINK_MSG_ID_RADIO_STATUS:
		REC_MAVLINK_MSG_ID_RADIO_STATUS(p_rxmsg);
		break;
	case MAVLINK_MSG_ID_NAV_CONTROLLER_OUTPUT:
		REC_MAVLINK_MSG_ID_NAV_CONTROLLER_OUTPUT(p_rxmsg);
		break;
	case MAVLINK_MSG_ID_VFR_HUD:
		REC_MAVLINK_MSG_ID_VFR_HUD(p_rxmsg);
		break;
	case MAVLINK_MSG_ID_HIL_CONTROLS:
		REC_MAVLINK_MSG_ID_HIL_CONTROLS(p_rxmsg);
		break;
	case MAVLINK_MSG_ID_GPS_RAW_INT:
		REC_MAVLINK_MSG_ID_GPS_RAW_INT(p_rxmsg);
		break;
#ifdef MAVLINK_PARAMS
	case MAVLINK_MSG_ID_PARAM_VALUE:
		REC_MAVLINK_MSG_ID_PARAM_VALUE(p_rxmsg);
		break;
#endif

	}

}


/*!	\brief Mavlink message parser
 *	\details Parses the characters in to a mavlink message.
 *	Case statement parses each character as it is recieved.
 *	\attention One big change form the 0.9 to 1.0 version is the
 *	MAVLINK_CRC_EXTRA. This requires the mavlink_message_crcs array of 256 bytes.
 *	\todo create dot for the statemachine
 */
static void MAVLINK_parse_char(uint8_t c) {

	static mavlink_message_t m_mavlink_message;
	//! The currently decoded message
	static mavlink_message_t* p_rxmsg = &m_mavlink_message;
	//! The current decode status
	mavlink_status_t* p_status = mavlink_get_channel_status(MAVLINK_COMM_0);


#if MAVLINK_CRC_EXTRA
	static const uint8_t mavlink_message_crcs[256] PROGMEM = MAVLINK_MESSAGE_CRCS;
#endif

	// Initializes only once, values keep unchanged after first initialization
	//mavlink_parse_state_initialize(p_status);

	//p_status->msg_received = 0;

	switch (p_status->parse_state) {
	case MAVLINK_PARSE_STATE_UNINIT:
	case MAVLINK_PARSE_STATE_IDLE:
		if (c == MAVLINK_STX) {
			p_status->parse_state = MAVLINK_PARSE_STATE_GOT_STX;
			mavlink_start_checksum(p_rxmsg);
		}
		break;

	case MAVLINK_PARSE_STATE_GOT_STX:
		// NOT counting STX, LENGTH, SEQ, SYSID, COMPID, MSGID, CRC1 and CRC2
		p_rxmsg->len = c;
		p_status->packet_idx = 0;
		mavlink_update_checksum(p_rxmsg, c);
		p_status->parse_state = MAVLINK_PARSE_STATE_GOT_LENGTH;
		break;

	case MAVLINK_PARSE_STATE_GOT_LENGTH:
		p_rxmsg->seq = c;
		mavlink_update_checksum(p_rxmsg, c);
		p_status->parse_state = MAVLINK_PARSE_STATE_GOT_SEQ;
		break;

	case MAVLINK_PARSE_STATE_GOT_SEQ:
		p_rxmsg->sysid = c;
		mavlink_update_checksum(p_rxmsg, c);
		p_status->parse_state = MAVLINK_PARSE_STATE_GOT_SYSID;
		break;

	case MAVLINK_PARSE_STATE_GOT_SYSID:
		p_rxmsg->compid = c;
		mavlink_update_checksum(p_rxmsg, c);
		p_status->parse_state = MAVLINK_PARSE_STATE_GOT_COMPID;
		break;

	case MAVLINK_PARSE_STATE_GOT_COMPID:
		p_rxmsg->msgid = c;
		mavlink_update_checksum(p_rxmsg, c);
		if (p_rxmsg->len == 0) {
			p_status->parse_state = MAVLINK_PARSE_STATE_GOT_PAYLOAD;
		} else {
			p_status->parse_state = MAVLINK_PARSE_STATE_GOT_MSGID;
		}
		break;

	case MAVLINK_PARSE_STATE_GOT_MSGID:
		_MAV_PAYLOAD_NON_CONST(p_rxmsg)[p_status->packet_idx++] = (char) c;
		mavlink_update_checksum(p_rxmsg, c);
		if (p_status->packet_idx == p_rxmsg->len) {
			p_status->parse_state = MAVLINK_PARSE_STATE_GOT_PAYLOAD;
		}
		break;

	case MAVLINK_PARSE_STATE_GOT_PAYLOAD:

#if MAVLINK_CRC_EXTRA
		mavlink_update_checksum(p_rxmsg, pgm_read_byte(&(mavlink_message_crcs[p_rxmsg->msgid])));
#endif
		if (c != (p_rxmsg->checksum & 0xFF)) {
			// Check first checksum byte
			p_status->parse_error = 3;
		} else {
			p_status->parse_state = MAVLINK_PARSE_STATE_GOT_CRC1;
		}
		break;

	case MAVLINK_PARSE_STATE_GOT_CRC1:
		if (c != (p_rxmsg->checksum >> 8)) {
			// Check second checksum byte
			p_status->parse_error = 4;
		} else {
			// Successfully got message
			if (mav_heartbeat < 0)
				mav_heartbeat = 0;
			p_status->current_rx_seq = p_rxmsg->seq;
			p_status->parse_state = MAVLINK_PARSE_STATE_IDLE;
			handleMessage(p_rxmsg);
		}
		break;
	}
	// Error occur
	if (p_status->parse_error) {
		p_status->parse_state = MAVLINK_PARSE_STATE_IDLE;
		if (c == MAVLINK_STX) {
			p_status->parse_state = MAVLINK_PARSE_STATE_GOT_STX;
			mavlink_start_checksum(p_rxmsg);
		}
		p_status->parse_error = 0;
	}
}

#ifdef MAVLINK_PARAMS
static inline void MAVLINK_msg_param_request_list_send() {
	mavlink_channel_t chan = MAVLINK_COMM_0;
	mavlink_msg_param_request_list_send(chan, mavlink_system.sysid, mavlink_system.compid);
}

static inline void MAVLINK_msg_param_set(uint8_t idx) {
	const pm_char* s = getParamId(idx);
	int8_t buf[15];
	int8_t *p = buf;
	while (1) {
		char c = pgm_read_byte(s++);
		if (!c) {
			if (idx < NB_PID_PARAMS) {
				*p++ = '_';
				uint8_t colIdx = idx & 0x01;
				*p++ = "PI"[colIdx];
			}
			*p++ = 0;
			break;
		}
		*p++ = c;
	}
	float param_value = getParam(idx)->value;

	mavlink_channel_t chan = MAVLINK_COMM_0;
	uint8_t param_type = (uint8_t)MAV_PARAM_TYPE_REAL32;
	const char* param_id = (const char*)buf;
	mavlink_msg_param_set_send(chan, mavlink_system.sysid, mavlink_system.compid, param_id, param_value, param_type);

}
#endif

__attribute__((unused))
static inline void MAVLINK_msg_request_data_stream_pack_send(uint8_t req_stream_id, uint16_t req_message_rate,
				uint8_t start_stop) {
	mavlink_channel_t chan = MAVLINK_COMM_0;
	mavlink_msg_request_data_stream_send(chan, mavlink_system.sysid, mavlink_system.compid, req_stream_id, req_message_rate,
					start_stop);
}


__attribute__((unused))
//! \brief old mode switch funtion
static inline void MAVLINK_msg_set_mode_send(uint8_t mode) {
	mavlink_channel_t chan = MAVLINK_COMM_0;
	mavlink_msg_set_mode_send(chan, mavlink_system.sysid, mode, 0);
}


/*!	\brief Telemetry monitoring, calls \link MAVLINK10mspoll.
 *	\todo Reimplemnt \link MAVLINK10mspoll
 *
 */
void telemetryWakeup() {
	uint16_t tmr10ms = get_tmr10ms();
	static uint16_t last_time = 0;
	if (tmr10ms - last_time > 15) {
		if (mav_heartbeat > -30) {
			mav_heartbeat--;
	
			if (mav_heartbeat == -30) {
				MAVLINK_reset(1);
				telemetryPortInit(g_eeGeneral.mavbaud);
			}
		}
		last_time = tmr10ms;
	}
}

