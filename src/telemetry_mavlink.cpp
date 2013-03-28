/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
 * - Andreas Weitl
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Gabriel Birkus
 * - Gerard Valade <gerard.valade@gmail.com>
 * - Jean-Pierre Parisy
 * - Karl Szmutny
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * opentx is based on code named
 * gruvin9x by Bryan J. Rentoul: http://code.google.com/p/gruvin9x/,
 * er9x by Erez Raviv: http://code.google.com/p/er9x/,
 * and the original (and ongoing) project by
 * Thomas Husterer, th9x: http://code.google.com/p/th9x/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "opentx.h"
#include "menus.h"
#include "serial.h"
#include "mavlink.h"

#define DUMP_RX_TX

// this might need to move to the flight software
//static
mavlink_system_t mavlink_system = { 7, 1, 0, 0 };

//static uint8_t system_id = 7;
//static uint8_t component_id = 1;
//static uint8_t target_system = 7;
//static uint8_t target_component = 1;

//static mavlink_message_t mavlink_message_in;

// Mavlink message decoded Status Text
#define PARAM_NB_REPEAT 10
#define LEN_STATUSTEXT 15
static uint8_t mav_statustext[LEN_STATUSTEXT];
static int8_t mav_heartbeat = 0;
static int8_t mav_heartbeat_recv = 0;
static int8_t watch_mav_req_id_action = 0;
static int8_t watch_mav_req_start_data_stream = 15;
static uint8_t data_stream_start_stop = 0;
int8_t watch_mav_req_params_list = 5;
static uint8_t mav_req_params_nb_recv = 0;
static int8_t watch_mav_req_params_set = 0;

// Telemetry data hold
Telemetry_Data_t telemetry_data;

// *****************************************************
static void MAVLINK_parse_char(uint8_t c);

#ifdef DUMP_RX_TX
#define MAX_RX_BUFFER 16
uint8_t mavlinkRxBufferCount = 0;
uint8_t mavlinkRxBuffer[MAX_RX_BUFFER];
uint8_t mav_dump_rx = 0;
void MAVLINK_rxhandler(uint8_t byte) {
	if (mav_dump_rx) {
		if (byte == MAVLINK_STX) {
			mavlinkRxBufferCount = 0;
		}
		if (mavlinkRxBufferCount < MAX_RX_BUFFER) {
			mavlinkRxBuffer[mavlinkRxBufferCount++] = byte;
		}
	}
	MAVLINK_parse_char(byte);

}
#else
void MAVLINK_rxhandler(uint8_t byte) {
	MAVLINK_parse_char(byte);
}
#endif

SerialFuncP RXHandler = MAVLINK_rxhandler;

void MAVLINK_reset(uint8_t warm_reset) {
	if (warm_reset && telemetry_data.status) {
		mav_statustext[0] = 0;
	}
#ifdef DUMP_RX_TX
	mavlinkRxBufferCount = 0;
	mav_dump_rx = 0;
#endif

	mavlink_status_t* p_status = mavlink_get_channel_status(MAVLINK_COMM_0);
	p_status->current_rx_seq = 0;
	p_status->current_tx_seq = 0;
	memset(&telemetry_data, 0, sizeof(telemetry_data));
	telemetry_data.rcv_control_mode = ERROR_NUM_MODES;
	telemetry_data.req_mode = ERROR_NUM_MODES;

	mav_heartbeat = 0;
	mav_heartbeat_recv = 0;
	watch_mav_req_id_action = 0;
	watch_mav_req_start_data_stream = 15;
	watch_mav_req_params_list = 5;
	watch_mav_req_params_set = 0;
	data_stream_start_stop = 0;
}

void MAVLINK_Init(void) {
	mav_statustext[0] = 0;
	MAVLINK_reset(0);
	SERIAL_Init();
}

static inline void REC_MAVLINK_MSG_ID_STATUSTEXT(const mavlink_message_t* msg) {
	//memcpy(mav_statustext, msg->payload + sizeof(uint8_t), sizeof(int8_t) * LEN_STATUSTEXT);
	//	_MAV_RETURN_int8_t_array(msg, mav_statustext, LEN_STATUSTEXT,  1);
	memcpy(mav_statustext, &_MAV_PAYLOAD(msg)[1], LEN_STATUSTEXT);
}

static inline void REC_MAVLINK_MSG_ID_SYS_STATUS(const mavlink_message_t* msg) {
	mavlink_sys_status_t sys_status;
	mavlink_msg_sys_status_decode(msg, &sys_status);

	telemetry_data.packet_drop = mavlink_msg_sys_status_get_packet_drop(msg);

	uint8_t mode = mavlink_msg_sys_status_get_mode(msg);
	uint8_t nav_mode = mavlink_msg_sys_status_get_nav_mode(msg);
	telemetry_data.rcv_control_mode = MAVLINK_NavMode2CtrlMode(mode, nav_mode);
	//telemetry_data.mode = mode;
	//telemetry_data.nav_mode = nav_mode;

	telemetry_data.status = mavlink_msg_sys_status_get_status(msg);
	//telemetry_data.load = mavlink_msg_sys_status_get_load(msg);
	telemetry_data.vbat = mavlink_msg_sys_status_get_vbat(msg) / 100; // Voltage * 10

	telemetry_data.vbat_low = (getMavlinParamsValue(BATT_MONITOR) > 0)
					&& (((float) telemetry_data.vbat / 10.0) < getMavlinParamsValue(LOW_VOLT)) && (telemetry_data.vbat > 50);
}

static inline void REC_MAVLINK_MSG_ID_GPS_RAW(const mavlink_message_t* msg) {
	telemetry_data.fix_type = mavlink_msg_gps_raw_get_fix_type(msg);
	telemetry_data.loc_current.lat = mavlink_msg_gps_raw_get_lat(msg);
	telemetry_data.loc_current.lon = mavlink_msg_gps_raw_get_lon(msg);
	telemetry_data.loc_current.alt = mavlink_msg_gps_raw_get_alt(msg);
	telemetry_data.eph = mavlink_msg_gps_raw_get_eph(msg);
	telemetry_data.hdg = mavlink_msg_gps_raw_get_hdg(msg);
	telemetry_data.v = mavlink_msg_gps_raw_get_v(msg);
}

static inline void REC_MAVLINK_MSG_ID_GPS_STATUS(const mavlink_message_t* msg) {
	telemetry_data.satellites_visible = mavlink_msg_gps_status_get_satellites_visible(msg);
}

static inline void REC_MAVLINK_MSG_ID_ACTION_ACK(const mavlink_message_t* msg) {
	uint8_t ack_action = mavlink_msg_action_ack_get_action(msg);
	telemetry_data.ack_result = mavlink_msg_action_ack_get_result(msg);
	uint8_t *ptr = mav_statustext;
	if (!telemetry_data.ack_result)
		*ptr++ = 'N';
	*ptr++ = 'A';
	*ptr++ = 'C';
	*ptr++ = 'K';
	*ptr++ = ' ';
	*ptr++ = ack_action / 10 + '0';
	*ptr++ = ack_action % 10 + '0';
	*ptr++ = 0;
	watch_mav_req_id_action = 0;
	AUDIO_WARNING1();
}

#ifdef MAVLINK_PARAMS
const pm_char * getParamId(uint8_t idx) {
	const pm_char *mav_params_id [((NB_PID_PARAMS / 2) + 4)]  = { //
					//
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
									PSTR("BATT_CAPACITY") };
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
		watch_mav_req_params_set = 4; // 1;
	}
}

void putsMavlinParams(uint8_t x, uint8_t y, uint8_t idx, uint8_t att) {
	if (idx < NB_PARAMS) {
		const pm_char * s = getParamId(idx);
		char c;
		while ((c = pgm_read_byte(s++))) {
			lcd_putcAtt(x, y, (c == '_' ? ' ' : c), 0);
			x += FW;
		}
		if (idx < NB_PID_PARAMS) {
			x = 11 * FW;
			lcd_putcAtt(x, y, "PI"[idx & 0x01], att);
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
	int8_t *id = param_value.param_id;
	setParamValue(id, param_value.param_value);
	data_stream_start_stop = 0; // stop data stream while getting params list
	watch_mav_req_params_list = mav_req_params_nb_recv < (NB_PARAMS - 5) ? 20 : 0; // stop timeout
}
#endif

static inline void handleMessage(mavlink_message_t* p_rxmsg) {
	switch (p_rxmsg->msgid) {
	case MAVLINK_MSG_ID_HEARTBEAT:
		mav_heartbeat = 3; // 450ms display '*'
		mav_heartbeat_recv = 1;
		break;
	case MAVLINK_MSG_ID_STATUSTEXT:
		REC_MAVLINK_MSG_ID_STATUSTEXT(p_rxmsg);
		AUDIO_WARNING1();
		break;
	case MAVLINK_MSG_ID_SYS_STATUS:
		REC_MAVLINK_MSG_ID_SYS_STATUS(p_rxmsg);
		watch_mav_req_start_data_stream = 20;
		break;
	case MAVLINK_MSG_ID_GPS_RAW:
		REC_MAVLINK_MSG_ID_GPS_RAW(p_rxmsg);
		break;
	case MAVLINK_MSG_ID_GPS_STATUS:
		REC_MAVLINK_MSG_ID_GPS_STATUS(p_rxmsg);
		break;
	case MAVLINK_MSG_ID_ACTION_ACK:
		REC_MAVLINK_MSG_ID_ACTION_ACK(p_rxmsg);
		break;
#ifdef MAVLINK_PARAMS
	case MAVLINK_MSG_ID_PARAM_VALUE:
		REC_MAVLINK_MSG_ID_PARAM_VALUE(p_rxmsg);
		break;
#endif

	}

}

#if 0
static void MAVLINK_parse_char(uint8_t c) {
//	mavlink_message_t msg;
//	mavlink_status_t status;

	static mavlink_message_t m_mavlink_message;
	static mavlink_status_t m_mavlink_status;
	mavlink_message_t *p_rxmsg = &m_mavlink_message;///< The currently decoded message
	mavlink_status_t *p_status = &m_mavlink_status;///< The current decode status

	if (mavlink_parse_char(MAVLINK_COMM_0, c, p_rxmsg, p_status)) {
		handleMessage(p_rxmsg);
	}
}
#endif

static void MAVLINK_parse_char(uint8_t c) {

	static mavlink_message_t m_mavlink_message;
	static mavlink_message_t* p_rxmsg = &m_mavlink_message; ///< The currently decoded message
	mavlink_status_t* p_status = mavlink_get_channel_status(MAVLINK_COMM_0); ///< The current decode status

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
		_MAV_PAYLOAD(p_rxmsg)[p_status->packet_idx++] = (char) c;
		mavlink_update_checksum(p_rxmsg, c);
		if (p_status->packet_idx == p_rxmsg->len) {
			p_status->parse_state = MAVLINK_PARSE_STATE_GOT_PAYLOAD;
		}
		break;

	case MAVLINK_PARSE_STATE_GOT_PAYLOAD:
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
			//p_status->msg_received = 1;
			p_status->parse_state = MAVLINK_PARSE_STATE_IDLE;
			//memcpy(r_message, p_rxmsg, sizeof(mavlink_message_t));

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
	// If a message has been sucessfully decoded, check index
	/*
	 if (p_status->msg_received == 1) {
	 p_status->current_rx_seq = p_rxmsg->seq;
	 p_status->packet_rx_success_count++;
	 }
	 */

	//r_mavlink_status->current_rx_seq = p_status->current_rx_seq + 1;
	//r_mavlink_status->packet_rx_success_count = p_status->packet_rx_success_count;
	//r_mavlink_status->packet_rx_drop_count = p_status->parse_error;
	//p_status->parse_error = 0;
	//return p_status->msg_received;
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
	//float param_value = ((float) telemetry_data.params[idx].pi_param[subIdx].pi_value / 100.00 + 0.005);
	float param_value = getParam(idx)->value;

	mavlink_channel_t chan = MAVLINK_COMM_0;
	mavlink_msg_param_set_send(chan, mavlink_system.sysid, mavlink_system.compid, buf, param_value);
}
#endif

static inline void MAVLINK_msg_request_data_stream_pack_send(uint8_t req_stream_id, uint16_t req_message_rate,
				uint8_t start_stop) {
	mavlink_channel_t chan = MAVLINK_COMM_0;
	mavlink_msg_request_data_stream_send(chan, mavlink_system.sysid, mavlink_system.compid, req_stream_id, req_message_rate,
					start_stop);
}

static inline void MAVLINK_msg_action_pack_send(uint8_t action) {
	mavlink_channel_t chan = MAVLINK_COMM_0;
	mavlink_msg_action_send(chan, mavlink_system.sysid, mavlink_system.compid, action);
}

static inline void MAVLINK_msg_set_mode_send(uint8_t mode) {
	mavlink_channel_t chan = MAVLINK_COMM_0;
	mavlink_msg_set_mode_send(chan, mavlink_system.sysid, mode);
}

void MAVLINK10mspoll(uint8_t count) {
	switch (count) {
	case 2: // MAVLINK_MSG_ID_ACTION
		if (watch_mav_req_id_action > 0) {
			watch_mav_req_id_action--;
			// Repeat  is not ack : 150ms*0x07
			if ((watch_mav_req_id_action & 0x07) == 0 && telemetry_data.req_mode < NUM_MODES) {
				uint8_t action = MAVLINK_CtrlMode2Action(telemetry_data.req_mode);
				MAVLINK_msg_action_pack_send(action);
				uint8_t *ptr = mav_statustext;
				*ptr++ = 'R';
				*ptr++ = 'Q';
				*ptr++ = ' ';
				*ptr++ = action / 10 + '0';
				*ptr++ = action % 10 + '0';
				*ptr++ = 0;
			}
		}
		if (telemetry_data.ack_result < 5) {
			if (telemetry_data.ack_result > 0) {
				telemetry_data.ack_result++;
			}
		}
		break;
	case 4: // MAVLINK_MSG_ID_PARAM_REQUEST_LIST
		if (watch_mav_req_params_list > 0) {
			watch_mav_req_params_list--;
			if (watch_mav_req_params_list == 0) {
				mav_req_params_nb_recv = 0;
				MAVLINK_msg_param_request_list_send();
				watch_mav_req_params_list = 20;
			}
		}
		break;

	case 6: // MAVLINK_MSG_ID_REQUEST_DATA_STREAM
		if (watch_mav_req_start_data_stream > 0) {
			watch_mav_req_start_data_stream--;
			if (watch_mav_req_start_data_stream == 0) {
				uint8_t req_stream_id = 2;
				uint16_t req_message_rate = 1;

				MAVLINK_msg_request_data_stream_pack_send(req_stream_id, req_message_rate, data_stream_start_stop);
				watch_mav_req_start_data_stream = 20;
				data_stream_start_stop = 1; // maybe start next time
			}
		}
		break;
	case 8: // MAVLINK_MSG_ID_PARAM_SET
		if (watch_mav_req_params_set > 0) {
			watch_mav_req_params_set--;
			if (watch_mav_req_params_set == 0) {
				for (uint8_t idx = 0; idx < NB_PARAMS; idx++) {
					if (getParam(idx)->repeat) {
						getParam(idx)->repeat--;
						MAVLINK_msg_param_set(idx);
						watch_mav_req_params_set = 3; // 300ms
						return;
					}
				}
			}
		}
		break;

	default:
		return;
	}
}

void telemetryWakeup() {
	uint16_t tmr10ms = get_tmr10ms();
	uint8_t count = tmr10ms & 0x0f; // 15*10ms
	if (!count) {
		if (mav_heartbeat > -30) {
			// TODO mavlink_system.sysid = g_eeGeneral.mavTargetSystem;
			mav_heartbeat--;

			if (mav_heartbeat == -30) {
				MAVLINK_reset(1);
			}
			SERIAL_startTX();
		}
	}
	if (mav_heartbeat_recv && !IS_TX_BUSY) {
		MAVLINK10mspoll(count);
	}
}

// Start of Mavlink menus <<<<<<<<<<<<<<<<<<<<<<<<<<<
void displayScreenIndex(uint8_t index, uint8_t count, uint8_t attr);

enum mavlink_menu_ {
	MENU_INFO = 0, //
	MENU_GPS, //
#ifdef DUMP_RX_TX
	MENU_DUMP_RX, //
	MENU_DUMP_TX, //
#endif
	MAX_MAVLINK_MENU
} MAVLINK_menu = MENU_INFO;

inline mavlink_menu_ operator++(mavlink_menu_ &eDOW, int) {
	int i = static_cast<int>(eDOW);
	i++;
	if (i < MAX_MAVLINK_MENU) {
		eDOW = static_cast<mavlink_menu_>(i);
	}
	return eDOW;
}

inline mavlink_menu_ operator--(mavlink_menu_ &eDOW, int) {
	int i = static_cast<int>(eDOW);
	if (i > 0) {
		eDOW = static_cast<mavlink_menu_>(--i);
	}
	return eDOW;
}

void mav_title(const pm_char * s, uint8_t index) {
	lcd_putsAtt(0, 0, PSTR("MAVLINK"), INVERS);
	lcd_putsAtt(10 * FW, 0, s, 0);
	displayScreenIndex(index, MAX_MAVLINK_MENU, INVERS);
	lcd_putcAtt(8 * FW, 0, (mav_heartbeat > 0) ? '*' : ' ', 0);
}

void lcd_outdezFloat(uint8_t x, uint8_t y, float val, uint8_t precis, uint8_t mode = 0) {
	char c;
	int16_t lnum = val;
	uint8_t x1 = x;
	val -= lnum;

	int8_t i = 0;
	lnum = abs(lnum);
	for (; i < 4; i++) {
		c = (lnum % 10) + '0';
		x1 -= FWNUM;
		lcd_putcAtt(x1, y, c, mode);
		lnum /= 10;
		if (lnum == 0) {
			break;
		}
	}
	if (lnum != 0) {
		// Error number too big
		x1 = x;
		for (i = 0; i < 4; i++) {
			x1 -= FW;
			lcd_putcAtt(x1, y, '?', mode);
		}
	} else {
		if (val < 0) {
			val = -val;
			x1 -= FWNUM;
			lcd_putcAtt(x1, y, '-', mode);
		}
		if (precis)
			lcd_putcAtt(x, y, '.', mode);

		for (i = 0; i < precis; i++) {
			val *= 10;
			int a = val;
			c = a + '0';
			x += FWNUM;
			lcd_putcAtt(x, y, c, mode);
			val -= a;
		}
	}
}

bool isValidReqControlMode()
{
	if (telemetry_data.req_mode < NUM_MODES) {
		if (telemetry_data.req_mode != telemetry_data.rcv_control_mode) {
			return false;
		}
	}
	return true;
}

void putsMavlinkControlMode(uint8_t x, uint8_t y, uint8_t len) {
	if (telemetry_data.status) {
		uint8_t attr = 0;
		uint8_t mode = telemetry_data.rcv_control_mode;
		if (telemetry_data.req_mode < NUM_MODES) {

			if (telemetry_data.req_mode != telemetry_data.rcv_control_mode) {
				attr = INVERS;
				switch (telemetry_data.ack_result) {
				case 5:
					AUDIO_ERROR();
					telemetry_data.req_mode = NUM_MODES;
					break;
				default:
					//mode = telemetry_data.req_mode;
					break;
				}
			}
		}
		putsControlMode(x, y, mode, attr, len);
	}
}

void MAVLINK_ReqMode(uint8_t mode, uint8_t send) {
	telemetry_data.req_mode = mode;
	telemetry_data.ack_result = 0;
	if (send) {
		watch_mav_req_id_action = 0x43;
	}
}

void menuTelemetryMavlinkInfos(void) {

	mav_title(PSTR("INFOS"), MAVLINK_menu);

	uint8_t x1, x2, xnum, y;
	x1 = FW;
	x2 = 7 * FW;
	xnum = x2 + 5 * FWNUM;
	y = FH;

	uint8_t * ptr = mav_statustext;
	for (uint8_t j = 0; j < LEN_STATUSTEXT; j++) {
		if (*ptr == 0) {
			lcd_putcAtt(x1, y, ' ', 0);
		} else {
			lcd_putcAtt(x1, y, *ptr++, 0);
		}
		x1 += FW;
	}
	x1 = FW;
	y += FH;

	if (telemetry_data.status) {
		if (!isValidReqControlMode()) {
		  lcd_putsnAtt(x1, y, PSTR("REQ"), 3, 0);
		  putsControlMode(x2, y, telemetry_data.req_mode, 0, 6);
		  y += FH;
		}
		lcd_putsnAtt(x1, y, PSTR("MODE"), 4, 0);
		putsMavlinkControlMode(x2, y, 6);

		y += FH;
		lcd_putsnAtt(x1, y, PSTR("BATT"), 4, 0);
		lcd_outdezNAtt(xnum, y, telemetry_data.vbat, PREC1, 5);

		y += FH;
		lcd_putsnAtt(x1, y, PSTR("DROP"), 4, 0);
		lcd_outdezAtt(xnum, y, telemetry_data.packet_drop, 0);

	}

}

void menuTelemetryMavlinkGPS(void) {
	mav_title(PSTR("GPS"), MAVLINK_menu);

	uint8_t x1, x2, xnum, y;
	x1 = FW;
	x2 = 5 * FW + FWNUM;
	xnum = 5 * FW + 3 * FWNUM;
	y = FH;

	lcd_putsnAtt(x1, y, PSTR("GPS"), 3, 0);
	uint8_t fix_type = telemetry_data.fix_type;
	if (fix_type <= 2) {
		lcd_putsnAtt(x2, y, PSTR("__NOOK") + 2 * fix_type, 2, 0);
	} else {
		lcd_outdezNAtt(xnum, y, fix_type, 0, 3);
	}
	lcd_putsnAtt(x2 + 5 * FW, y, PSTR("SAT"), 3, 0);
	lcd_outdezNAtt(x2 + 8 * FW + 3 * FWNUM, y, telemetry_data.satellites_visible, 0, 2);

//	if (telemetry_data.fix_type > 0) {
	y += FH;
	lcd_putsnAtt(0, y, PSTR("HDOP"), 4, 0);
	lcd_outdezFloat(xnum, y, telemetry_data.eph, 2);

	y += FH;
	lcd_putsnAtt(0, y, PSTR("COOR"), 4, 0);
	lcd_outdezFloat(xnum, y, telemetry_data.loc_current.lat, 5);

	//	y += FH;
	//	lcd_putsnAtt(x1, y, PSTR("LON"), 3, 0);
	lcd_outdezFloat(xnum + 10 * FWNUM, y, telemetry_data.loc_current.lon, 5);

	y += FH;
	lcd_putsnAtt(x1, y, PSTR("ALT"), 3, 0);
	lcd_outdezFloat(xnum, y, telemetry_data.loc_current.alt, 2);

	y += FH;
	lcd_putsnAtt(x1, y, PSTR("HDG"), 3, 0);
	lcd_outdezFloat(xnum, y, telemetry_data.hdg, 2);

	y += FH;
	lcd_putsnAtt(x1, y, PSTR("V"), 1, 0);
	lcd_outdezFloat(xnum, y, telemetry_data.v, 2);
	//}
}

#ifdef DUMP_RX_TX

void lcd_outhex2(uint8_t x, uint8_t y, uint8_t val) {
	x += FWNUM * 2;
	for (int i = 0; i < 2; i++) {
		x -= FWNUM;
		char c = val & 0xf;
		c = c > 9 ? c + 'A' - 10 : c + '0';
		lcd_putcAtt(x, y, c, c >= 'A' ? CONDENSED : 0);
		val >>= 4;
	}
}

void menuTelemetryMavlinkDump(uint8_t event) {
	uint8_t x = 0;
	uint8_t y = FH;
	uint16_t count = 0;
	uint16_t bufferLen = 0;
	uint8_t *ptr = NULL;
	switch (MAVLINK_menu) {
		case MENU_DUMP_RX:
		mav_dump_rx = 1;
		mav_title(PSTR("RX"), MAVLINK_menu);
		bufferLen = mavlinkRxBufferCount;
		ptr = mavlinkRxBuffer;
		break;

		case MENU_DUMP_TX:
		mav_title(PSTR("TX"), MAVLINK_menu);
		bufferLen = serialTxBufferCount;
		ptr = ptrTxISR;
		break;
		default:
		break;
	}
	for (uint16_t var = 0; var < bufferLen; var++) {
		uint8_t byte = *ptr++;
		lcd_outhex2(x, y, byte);
		x += 2 * FW;
		count++;
		if (count > 8) {
			count = 0;
			x = 0;
			y += FH;
			if (y == (6 * FH))
			break;
		}
	}
}
#endif

void menuTelemetryMavlink(uint8_t event) {

	switch (event) // new event received, branch accordingly
	{
	case EVT_ENTRY:
		MAVLINK_menu = MENU_INFO;
		break;

	case EVT_KEY_FIRST(KEY_UP):
		MAVLINK_menu--;
		break;
	case EVT_KEY_FIRST(KEY_DOWN):
		MAVLINK_menu++;
		break;
	case EVT_KEY_FIRST(KEY_MENU):
	case EVT_KEY_FIRST(KEY_EXIT):
		//MAVLINK_Quit();
		chainMenu(menuMainView);
		return;
	}

	switch (MAVLINK_menu) {
	case MENU_INFO:
	  menuTelemetryMavlinkInfos();
	  break;
	case MENU_GPS:
	  menuTelemetryMavlinkGPS();
	  break;
#ifdef DUMP_RX_TX
	case MENU_DUMP_TX:
	case MENU_DUMP_RX:
	  menuTelemetryMavlinkDump(event);
	  break;
#endif

	default:
		break;
	}

}

