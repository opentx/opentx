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

/*!	\file mavlink.h
 *	Mavlink include file
 */

#ifndef _MAVLINK_H_
#define _MAVLINK_H_

#define MAVLINK_USE_CONVENIENCE_FUNCTIONS
#define MAVLINK_COMM_NUM_BUFFERS 1

#include "GCS_MAVLink/include_v1.0/mavlink_types.h"
#include "opentx.h"

extern int8_t mav_heartbeat;
extern mavlink_system_t mavlink_system;
#define LEN_STATUSTEXT 20
extern char mav_statustext[LEN_STATUSTEXT];

extern void SERIAL_start_uart_send();
extern void SERIAL_end_uart_send();
extern void SERIAL_send_uart_bytes(const uint8_t * buf, uint16_t len);

# define MAV_SYSTEM_ID	1
//mavlink_system.type = 2; //MAV_QUADROTOR;


#define MAVLINK_START_UART_SEND(chan,len) SERIAL_start_uart_send()
#define MAVLINK_END_UART_SEND(chan,len) SERIAL_end_uart_send()
#define MAVLINK_SEND_UART_BYTES(chan,buf,len) SERIAL_send_uart_bytes(buf,len)

#if __clang__
// clang does not like packed member access at all. Since mavlink is a 3rd party library, ignore the errors
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Waddress-of-packed-member"
#endif
#include "GCS_MAVLink/include_v1.0/ardupilotmega/mavlink.h"
#if __clang__
// Restore warnings about packed member access
#pragma clang diagnostic pop
#endif

//#define MAVLINK_PARAMS
//#define DUMP_RX_TX
#define ERROR_NUM_MODES 99
#define ERROR_MAV_ACTION_NB 99


// Auto Pilot modes
// ----------------
#define AC_STABILIZE 0		// hold level position
#define AC_ACRO 1			// rate control
#define AC_ALT_HOLD 2		// AUTO control
#define AC_AUTO 3			// AUTO control
#define AC_GUIDED 4			// AUTO control
#define AC_LOITER 5			// Hold a single location
#define AC_RTL 6			// AUTO control
#define AC_CIRCLE 7			// AUTO control
#define AC_POSITION 8		// AUTO control
#define AC_LAND 9			// AUTO control
#define AC_OF_LOITER 10		// Hold a single location using optical flow
							// sensor
#define AC_TOY_A 11			// THOR Enum for Toy mode
#define AC_TOY_M 12			// THOR Enum for Toy mode
#define AC_NUM_MODES 13

// Mavlink airframe types
#define MAVLINK_ARDUCOPTER 0
#define MAVLINK_ARDUPLANE 1
#define MAVLINK_INVALID_TYPE 2


#define AP_MANUAL        0
#define AP_CIRCLE        1
#define AP_STABILIZE     2
#define AP_TRAINING      3
#define AP_FLY_BY_WIRE_A 5
#define AP_FLY_BY_WIRE_B 6
#define AP_AUTO          10
#define AP_RTL           11
#define AP_LOITER        12
#define AP_GUIDED        15
#define AP_INITIALISING  16
#define AP_NUM_MODES 17

static const uint8_t ap_modes_lut[18] PROGMEM = {0,1,2,3,12,4,5,12,12,12,6,7,8,9,12,12,10,11};

/*
 * Type definitions
 */

#ifdef MAVLINK_PARAMS

enum ACM_PARAMS {
	RATE_YAW_P, // Rate Yaw
	RATE_YAW_I, // Rate Yaw
	STB_YAW_P, // Stabilize Yaw
	STB_YAW_I, // Stabilize Yaw
	RATE_PIT_P, // Rate Pitch
	RATE_PIT_I, // Rate Pitch
	STB_PIT_P, // Stabilize Pitch
	STB_PIT_I, // Stabilize Pitch
	RATE_RLL_P, // Rate Roll
	RATE_RLL_I, // Rate Roll
	STB_RLL_P, // Stabilize Roll
	STB_RLL_I, // Stabilize Roll
	THR_ALT_P, // THR_BAR, // Altitude Hold
	THR_ALT_I, // THR_BAR, // Altitude Hold
	HLD_LON_P, // Loiter
	HLD_LON_I, // Loiter
	HLD_LAT_P, // Loiter
	HLD_LAT_I, // Loiter
	NAV_LON_P, // Nav WP
	NAV_LON_I, // Nav WP
	NAV_LAT_P, // Nav WPs
	NAV_LAT_I, // Nav WP
	NB_PID_PARAMS, // Number of PI Parameters
	LOW_VOLT = NB_PID_PARAMS,
	IN_VOLT, //
	BATT_MONITOR, //
	BATT_CAPACITY, //
	NB_PARAMS
};
//#define NB_PID_PARAMS 24
#define NB_COL_PARAMS 2
#define NB_ROW_PARAMS ((NB_PARAMS+1)/NB_COL_PARAMS)

typedef struct MavlinkParam_ {
	uint8_t repeat :4;
	uint8_t valid :4;
	float value;
} MavlinkParam_t;

#endif

typedef struct Location_ {
	float lat; ///< Latitude in degrees
	float lon; ///< Longitude in degrees
	float gps_alt; ///< Altitude in meters
	float rel_alt;
} Location_t;

typedef struct Telemetry_Data_ {
	// INFOS
	uint8_t status; ///< System status flag, see MAV_STATUS ENUM
	uint8_t type;
	uint8_t autopilot;
	uint8_t type_autopilot;
	uint16_t packet_drop;
	uint16_t packet_fixed;
	uint8_t radio_sysid;
	uint8_t radio_compid;
	uint8_t mav_sysid;
	uint8_t mav_compid;
	uint8_t mode;
	uint32_t custom_mode;
	bool active;
	uint8_t nav_mode;
	uint8_t rcv_control_mode; ///< System mode, see MAV_MODE ENUM in mavlink/include/mavlink_types.h
	uint16_t load; ///< Maximum usage in percent of the mainloop time, (0%: 0, 100%: 1000) should be always below 1000
	uint8_t vbat; ///< Battery voltage, in millivolts (1 = 1 millivolt)
	uint8_t ibat; ///< Battery voltage, in millivolts (1 = 1 millivolt)
	uint8_t rem_bat; ///< Battery voltage, in millivolts (1 = 1 millivolt)
	bool vbat_low;
	
	uint8_t rc_rssi;
	uint8_t pc_rssi;
	
	uint8_t debug;

	// MSG ACTION / ACK
	uint8_t req_mode;
	int8_t ack_result;

	// GPS
	uint8_t fix_type; ///< 0-1: no fix, 2: 2D fix, 3: 3D fix. Some applications will not use the value of this field unless it is at least two, so always correctly fill in the fix.
	uint8_t satellites_visible; ///< Number of satellites visible
	Location_t loc_current;
	float eph;
	uint16_t course;
	float v; // Ground speed
	// Navigation
	uint16_t heading;
	uint16_t bearing;

#ifdef MAVLINK_PARAMS
	// Params
	MavlinkParam_t params[NB_PARAMS];
#endif

} Telemetry_Data_t;

// Telemetry data hold
extern Telemetry_Data_t telemetry_data;

/*
 * Funtion definitions
 */




#if 0
extern inline uint8_t MAVLINK_CtrlMode2Action(uint8_t mode) {
	uint8_t action;
	
	return action;
}

extern inline uint8_t MAVLINK_Action2CtrlMode(uint8_t action) {
	uint8_t mode = ERROR_NUM_MODES;
	switch (action) {
	
	return action;
	}
}
#endif
void telemetryWakeup();
void MAVLINK_Init(void);
void menuViewTelemetryMavlink(event_t event);
void MAVLINK10mspoll(uint16_t time);

#ifdef MAVLINK_PARAMS

void putsMavlinParams(uint8_t x, uint8_t y, uint8_t idx, uint8_t att);
void setMavlinParamsValue(uint8_t idx, float val);

inline uint8_t getIdxParam(uint8_t rowIdx, uint8_t colIdx) {
	return (rowIdx * NB_COL_PARAMS) + colIdx;
}

inline MavlinkParam_t * getParam(uint8_t idx) {
	return &telemetry_data.params[idx];
}

inline float getMavlinParamsValue(uint8_t idx) {
	return telemetry_data.params[idx].value;
}

inline uint8_t isDirtyParamsValue(uint8_t idx) {
	return telemetry_data.params[idx].repeat;
}

inline uint8_t isValidParamsValue(uint8_t idx) {
	return telemetry_data.params[idx].valid;
}

inline float getCoefPrecis(uint8_t precis) {
	switch (precis) {
	case 1:
		return 10.0;
	case 2:
		return 100.0;
	case 3:
		return 1000.0;
	}
	return 1.0;

}

inline int16_t getMaxMavlinParamsValue(uint8_t idx) {
	int16_t max = 0;
	switch (idx) {
	case LOW_VOLT:
		max = 2500; // 25.0 Volt max
		break;
	case IN_VOLT:
		max = 900; // 7.00 Volt max
		break;
	case BATT_MONITOR:
		max = 3;
		break;
	case BATT_CAPACITY:
		max = 7000; // 7000 mAh max
		break;
	default:
		if (idx < NB_PID_PARAMS) {
			max = (idx & 0x01) ? 1000 : 750;
		}
		break;
	}
	return max;
}

inline uint8_t getPrecisMavlinParamsValue(uint8_t idx) {
	uint8_t precis = 2;
	switch (idx) {
	case LOW_VOLT:
		precis = 2;
		break;
	case IN_VOLT:
		precis = 2;
		break;
	case BATT_MONITOR:
		precis = 0;
		break;
	case BATT_CAPACITY:
		precis = 0;
		break;
	default:
		if (idx < NB_PID_PARAMS) {
			if (idx & 0x01)
				precis = 3;
		}
		break;
	}
	return precis;
}

void lcd_outdezFloat(uint8_t x, uint8_t y, float val, uint8_t precis, uint8_t mode);
#endif

void telemetryPortInit(uint8_t baudrate);

#endif // _MAVLINK_H_

