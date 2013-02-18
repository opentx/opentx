/*
 * Author - Gerard Valade
 *
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

#ifndef rotarysw_h
#define rotarysw_h

#define ROTARY_SW_CHANNEL "UP  DOWN"
// Channel number for rotary switch
//#define MIX_SW_ROLL_CHAN (MIXSRC_LAST_CH+NUM_VIRTUAL) // GVA:Rotary switch
#define MIX_INC_ROTARY_SW (MIXSRC_LAST_CH+MAX_TIMERS+1)
#define MIX_DEC_ROTARY_SW (MIXSRC_LAST_CH+MAX_TIMERS+1)

extern int8_t rotarySwIdx;
extern int8_t rotarySwLastPPMVal;

extern void putsControlMode(uint8_t x, uint8_t y, uint8_t idx, uint8_t attr, uint8_t len);
extern void putsRotarySwPos(uint8_t x, uint8_t y, uint8_t idx1, uint8_t att);
extern void setRotarySwIdx(int8_t idx);
extern void setRotarySwDisplay(int8_t idx);
extern void animRotarySw(uint8_t x);

// Control mode define from arducoper
enum CONTROL_MODE {
	STABILIZE = 0, // hold level position
	ACRO, // rate control
	ALT_HOLD, // AUTO control
	AUTO, // AUTO control
	GUIDED, // AUTO control
	LOITER, // AUTO control
	RTL, // AUTO control
	CIRCLE, // AUTO control
	POSITION, // AUTO control
	LAND, // AUTO control
	OF_LOITER, // Hold a single location using optical flow sensor
	//
	// Adding control mode define from ardupilot
	MANUAL,
	FLY_BY_WIRE_A, // Fly By Wire A has left stick horizontal => desired roll angle, left stick vertical => desired pitch angle, right stick vertical = manual throttle
	FLY_BY_WIRE_B, // Fly By Wire B has left stick horizontal => desired roll angle, left stick vertical => desired pitch angle, right stick vertical => desired airspeed
	FLY_BY_WIRE_C, // Fly By Wire C has left stick horizontal => desired roll angle, left stick vertical => desired climb rate, right stick vertical => desired airspeed
// Fly By Wire B and Fly By Wire C require airspeed sensor

	// This only for display
	INITIALISING,

	NUM_MODES_ALL,
};
#define ACM_NUM_MODE (OF_LOITER+1)
#define NUM_MODES (FLY_BY_WIRE_C+1)

//                           0123456789012345678901234567890123456789012345678901234567890123456789
//                           0     1     2     3     4     5     6     7     8     9     0     1
#define CONROL_MODE_STR     "STAB  ACRO  ALT_H AUTO  GUIDEDLOITERRTL   CIRCLEPOSITILAND  OF_LOI"
#define CONROL_MODE_STR_APM "MANUALWIRE_AWIRE_BWIRE_C"
#define DISPLAY_ONLY_STR    "INIT"


inline void init_rotary_sw() {
	setRotarySwIdx(-1); // Reinit sw roll idx
	for (uint8_t i = 0; i < NUM_ROTARY_SW; i++) {
		if (g_model.mavlink.rotarySw[i].numMode >= NUM_MODES || g_model.mavlink.rotarySw[i].typeRotary > ROTARY_TYPE_MAVLINK)
		{
			g_model.mavlink.rotarySw[i].typeRotary = 0;
			g_model.mavlink.rotarySw[i].numMode = 0;
		}
#ifdef OLD_ROTARY
		for (uint8_t j = 0; j < sizeof(g_model.mavlink.rotarySw[i].name); j++) // makes sure name is valid
		{
			uint8_t idx = char2idx(g_model.mavlink.rotarySw[i].name[j]);
			g_model.mavlink.rotarySw[i].name[j] = idx2char(idx);
		}
#endif
	}
}

inline int8_t find_rotary_sw_pos(uint8_t srcRaw, uint8_t swTog, uint8_t swOn) {
	if ((swOn && swTog) || rotarySwIdx == -1) { // toggle switch on
		int8_t idx = rotarySwIdx;
		int8_t inc = srcRaw == MIX_INC_ROTARY_SW ? 1 : -1;
		for (uint8_t k = 0; k < NUM_ROTARY_SW; k++) { // find first enable slot
			idx = (idx + inc) & 0x7; // do modulo 8
			if (g_model.mavlink.rotarySw[idx].typeRotary) {
				setRotarySwIdx(idx);
				return idx;
			}
		}
		setRotarySwIdx(-1);
	}
	return rotarySwIdx;
}

inline int8_t get_rotary_value() {
	if (g_model.mavlink.rotarySw[rotarySwIdx].typeRotary == ROTARY_TYPE_PPM) {
		uint8_t num = g_model.mavlink.rotarySw[rotarySwIdx].numMode;
		rotarySwLastPPMVal = g_model.mavlink.modesVal[num];
	}
	return rotarySwLastPPMVal;
}

#endif

