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
#include "opentx.h"

#define CTIME_ROTARY_ANIM 500
uint16_t rotarySwChanged = 0;
int8_t rotarySwIdx = -1;
int8_t rotarySwLastPPMVal = 0;

void putsRotarySwPos(uint8_t x, uint8_t y, uint8_t idx, uint8_t att) {
	lcd_putcAtt(x, y, 'P', att);
	lcd_putcAtt(x + FW, y, '1' + idx, att);
}

#ifdef MAVLINK
extern bool isValidReqControlMode();
extern void putsMavlinkControlMode(uint8_t x, uint8_t y, uint8_t len);
extern void MAVLINK_ReqMode(uint8_t mode, uint8_t send);
#endif

void putsControlMode(uint8_t x, uint8_t y, uint8_t idx, uint8_t attr, uint8_t len) {
	if (idx < ACM_NUM_MODE) {
		lcd_putsnAtt(x, y, PSTR(CONROL_MODE_STR) + 6 * idx, len, attr);
	} else if (idx < NUM_MODES) {
		idx -= ACM_NUM_MODE;
		lcd_putsnAtt(x, y, PSTR(CONROL_MODE_STR_APM) + 6 * idx, len, attr);
	} else if (idx < NUM_MODES_ALL) {
		idx -= NUM_MODES;
		lcd_putsnAtt(x, y, PSTR(DISPLAY_ONLY_STR) + 6 * idx, len, attr);
	} else {
		for (uint8_t i = 0; i < len; i++) {
			lcd_putcAtt(x, y, '-', attr);
		}
	}
}

void setRotarySwIdx(int8_t idx) {
	if (rotarySwIdx != idx) {
		rotarySwIdx = idx;
		if (rotarySwIdx >= 0) {
			rotarySwChanged = (get_tmr10ms() >> 3) + 3;
#ifdef MAVLINK
			uint8_t send = g_model.mavlink.rotarySw[rotarySwIdx].typeRotary == ROTARY_TYPE_MAVLINK ? 1 : 0;
			MAVLINK_ReqMode(g_model.mavlink.rotarySw[rotarySwIdx].numMode, send);
#endif
		}
	}
}


void animRotarySw(uint8_t x) {

	uint16_t s_time = get_tmr10ms() >> 3; // 80ms time unit
	uint8_t swToggle = (s_time < rotarySwChanged);
	uint8_t att1 = 0;

	if (s_timerState[0] != TMR_OFF) {
		att1 = DBLSIZE | (s_timerState[0] == TMR_BEEPING ? BLINK : 0);
		putsTime(x + 14 * FW - 2+5*FWNUM+1, FH * 2, s_timerVal[0], att1, att1);
	}

	s_time = s_time >> 4; // 1280ms time unit
	uint8_t num4Display = s_time & 0x01; // 1/4 display time
#ifdef MAVLINK
	s_time = telemetry_data.status ? (s_time >> 1) : 0; // 2560ms time unit
#else
	s_time = 0;
#endif
	uint8_t num2Display = s_time & 0x01; // 1/2 time display time

	if (rotarySwIdx >= 0) {
		att1 = swToggle ? INVERS : 0;
		putsControlMode(x + 4 * FW, 2 * FH, g_model.mavlink.rotarySw[rotarySwIdx].numMode, att1 | NO_UNIT, 6);
	}

	switch (num2Display) {
	case 0:
		att1 = (g_vbat100mV <= g_eeGeneral.vBatWarn ? BLINK : 0);
		putsVBat(x + 4 * FW, 2 * FH, att1 | NO_UNIT | DBLSIZE);

		switch (num4Display) {
		case 0:
			lcd_putsnAtt(x + 4 * FW, 3 * FH, PSTR("ExpExFFneMedCrs") + 3 * g_model.trimInc, 3, 0);
			lcd_putsnAtt(x + 8 * FW - FW / 2, 3 * FH, PSTR("   TTm") + 3 * g_model.thrTrim, 3, 0);
			break;
		default:
			lcd_putcAtt(x + 4 * FW, 3 * FH, 'V', 0);
			if (s_timerState != TMR_OFF) {
				putsTmrMode(x + 7 * FW - FW / 2, 3 * FH, g_model.timers[0].mode, 0);
			}
			break;
		}
		break;

	default:
#ifdef MAVLINK
		att1 = (telemetry_data.vbat_low ? BLINK : 0);
		lcd_outdezAtt(x + 4 * FW, 2 * FH, telemetry_data.vbat, att1 | PREC1 | DBLSIZE);
		if (isValidReqControlMode())
		{
			lcd_putsnAtt(x + 4 * FW, 3 * FH, PSTR("MAVLNK"), 6, 0);
		} else {
			putsMavlinkControlMode(x + 4 * FW, 3 * FH, 6);
		}

#endif
		break;
	}
}
