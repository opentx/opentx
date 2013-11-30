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
 * - Rienk de Jong
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
 
/*!	\file view_mavlink.cpp
 *	Mavlink menu
 *	Contains the menu specific code for Mavlink support.
 */

#include "gui/view_mavlink.h"

// Globals declaration

 
/*!	\brief Top Mavlink Menu definition
 *	\details Registers button events and handles that info. Buttons select menus,
 *	these are launched from the MAVLINK_menu switch statement. Setup menu is
 *	lanuched by the menu button. On exit (with exit button) the mavlink
 *	extension is reinitialized.
 */
void menuTelemetryMavlink(uint8_t event) {
	
	switch (event) // new event received, branch accordingly
	{
	case EVT_ENTRY:
		MAVLINK_menu = MENU_INFO;
		break;

	case EVT_KEY_FIRST(KEY_UP):
		if (MAVLINK_menu > 0)
		{
			MAVLINK_menu--;
			break;
		}
		else
		{
			chainMenu(menuMainView);
			return;
		}
	case EVT_KEY_FIRST(KEY_DOWN):
		if (MAVLINK_menu < MAX_MAVLINK_MENU - 1)
			MAVLINK_menu++;
		break;
	case EVT_KEY_FIRST(KEY_MENU):
		return;
	case EVT_KEY_FIRST(KEY_EXIT):
		chainMenu(menuMainView);
		return;
	}

	switch (MAVLINK_menu) {
	case MENU_INFO:
		menuTelemetryMavlinkInfos();
		break;
	case MENU_MODE:
		menuTelemetryMavlinkFlightMode();
		break;
	case MENU_BATT:
		menuTelemetryMavlinkBattery();
		break;
	case MENU_NAV:
		menuTelemetryMavlinkNavigation();
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

/*!	\brief Float variable display helper
 *	\param x x position on the screen
 *	\param y y position on the screen
 *	\param val value to display
 *	\param percis precision to display
 *	\param mode Use one of the defines in lcd.h line 81
 *	\details The maximum value is 9999. The position is the position of the ones, see below for explanation "*" marks
 *	the x position.
 *	\verbatim
          *
       9999.9999 \endverbatim
 */
void lcd_outdezFloat(uint8_t x, uint8_t y, float val, uint8_t precis, uint8_t mode) {
	char c;
	int16_t lnum = val;
	uint8_t x1 = x;
	val -= lnum;
	uint8_t xinc = FWNUM;
	if (mode & DBLSIZE)
		xinc *= 2;

	int8_t i = 0;
	lnum = abs(lnum);
	for (; i < 4; i++) {
		c = (lnum % 10) + '0';
		x1 -= xinc;
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
			x1 -= xinc;
			lcd_putcAtt(x1, y, '-', mode);
		}
		if (precis)
		{
			uint8_t y_temp = y;
			if (mode & DBLSIZE)
				y_temp += FH;
			lcd_putcAtt(x, y_temp, '.', (mode & (~DBLSIZE)));
			x -= (xinc / 2);
		}
		for (i = 0; i < precis; i++) {
			val *= 10;
			int a = val;
			c = a + '0';
			x += xinc;
			lcd_putcAtt(x, y, c, mode);
			val -= a;
		}
	}
}

/*!	\brief Fightmode string printer
 *	\details Decodes the flight mode from Mavlink custom mode enum to a string.
 *	This funtion can handle ArduPilot and ArduCoper code.
 *	To support new autopilot pilots add a STR_MAVLINK_... to the translations,
 *	and if requred a lut (see arduplane for examle) if there are unused modes
 *	in the sequence.
 */

void print_mav_mode(uint8_t x, uint8_t y, uint32_t custom_mode, uint8_t attr) //, const char * mode_text_p)
{
	uint8_t mode = (uint8_t) custom_mode;
	switch (telemetry_data.type_autopilot) {
	case MAVLINK_ARDUCOPTER:
		lcd_putsiAtt(x,y,STR_MAVLINK_AC_MODES,mode,attr);
		break;
	case MAVLINK_ARDUPLANE:
		lcd_putsiAtt(x,y,STR_MAVLINK_AP_MODES,ap_modes_lut[custom_mode],attr);
		break;
	default:
		lcd_putsAtt (FW, y, PSTR("INV. MAV TYPE"), attr);
		break;
	}
}

/*!	\brief Menu header
 *	\details Small helper function to print the standard header on the screen.
 */
void mav_title(const pm_char * s, uint8_t index)
{
  lcd_putsAtt(0, 0, PSTR("MAVLINK"), INVERS);
  lcd_puts(10 * FW, 0, s);
  displayScreenIndex(index, MAX_MAVLINK_MENU, INVERS);
  lcd_putc(7 * FW, 0, (mav_heartbeat > 0) ? '*' : ' ');
  if (telemetry_data.active)
  	  lcd_putcAtt(8 * FW, 0, 'A', BLINK);
}

/*!	\brief Global info menu
 *	\details Quick status overview menu. The menu should contain current mode, 
 *	armed | disarmed, battery status and RSSI info. Menu must be clean and
 *	readable with a quick glance.
 *	\todo Make menu as described as above.
 */
void menuTelemetryMavlinkInfos(void) {

	mav_title(STR_MAVLINK_INFOS, MAVLINK_menu);

	uint8_t x1, x2, xnum, y;
	x1 = FW;
	x2 = 7 * FW;
	xnum = x2 + 5 * FWNUM;
	y = FH;
/*
	char * ptr = mav_statustext;
	for (uint8_t j = 0; j < LEN_STATUSTEXT; j++) {
		if (*ptr == 0) {
			lcd_putc(x1, y, ' ');
		} else {
			lcd_putc(x1, y, *ptr++);
		}
		x1 += FW;
	}
	x1 = FW;
	y += FH;
*/
	if (telemetry_data.status) {

		lcd_putsnAtt(x1, y, STR_MAVLINK_MODE, 4, 0);
		if (telemetry_data.active)
			lcd_putsnAtt(x2, y, PSTR("A"), 1, 0);
		lcd_outdezAtt(xnum, y, telemetry_data.mode, 0);

		y += FH;
		lcd_puts(x1, y, PSTR("BATT"));
		lcd_outdezNAtt(xnum, y, telemetry_data.vbat, PREC1, 5);

		y += FH;
		lcd_puts(x1, y, PSTR("DROP"));
		lcd_outdezAtt(xnum, y, telemetry_data.packet_drop, 0);
/*		y += FH;
		lcd_puts(x1, y, PSTR("FIX"));
		lcd_outdezAtt(xnum, y, telemetry_data.packet_fixed, 0);
		y += FH;
		lcd_puts(x1, y, PSTR("MAV Comp"));
		lcd_outdezAtt(xnum, y, telemetry_data.mav_compid, 0);
		y += FH;
		lcd_puts(x1, y, PSTR("MAV Sys"));
		lcd_outdezAtt(xnum, y, telemetry_data.mav_sysid, 0);
		y += FH;
		lcd_puts(x1, y, PSTR("Rad Comp"));
		lcd_outdezAtt(xnum, y, telemetry_data.radio_compid, 0);
		y += FH;
		lcd_puts(x1, y, PSTR("Rad Sys"));
		lcd_outdezAtt(xnum, y, telemetry_data.radio_sysid, 0);
*/
		
	}
}

/*!	\brief Flight mode menu
 *	\details Clear display of current flight mode.
 *	\todo Add functionality to change flight mode.
 */
void menuTelemetryMavlinkFlightMode(void) {
	
	mav_title(STR_MAVLINK_MODE, MAVLINK_menu);
		
	uint8_t x, y;
	x = 0;
	y = FH;
	
    lcd_puts  (x, y, STR_MAVLINK_CUR_MODE);
    y += FH;
    print_mav_mode(FW, y, telemetry_data.custom_mode, DBLSIZE);
    y += 2 * FH;
	
	char * ptr = mav_statustext;
	for (uint8_t j = 0; j < LEN_STATUSTEXT; j++) {
		if (*ptr == 0) {
			lcd_putc(x, y, ' ');
		} else {
			lcd_putc(x, y, *ptr++);
		}
		x += FW;
	}
    y += FH;
    x = 0;
	
    if (telemetry_data.active)
    	lcd_putsAtt (FW, y, STR_MAVLINK_ARMED, DBLSIZE);
}

/*!	\brief Batterystatus dislplay
 *	\details Shows flight batery status.
 *	Also RC and PC RSSI are in this menu. 
 */
void menuTelemetryMavlinkBattery(void) {
	
	mav_title(STR_MAVLINK_BAT_MENU_TITLE, MAVLINK_menu);
	
	uint8_t x, y, ynum;
	x = 7 * FWNUM;
//	x = xnum + 0 * FW;
	ynum = 2 * FH;
	y = 3 * FH;
	
    lcd_puts(0, 1*FH, STR_MAVLINK_BATTERY_LABEL); 
	
	lcd_outdezAtt(x, ynum, telemetry_data.vbat, (DBLSIZE | PREC1 | UNSIGN));
	lcd_puts(x, y, PSTR("V"));
	x += 4 * (2 * FWNUM);
	lcd_outdezAtt(x, ynum, telemetry_data.ibat, (DBLSIZE | PREC1 | UNSIGN));
	lcd_puts(x, y, PSTR("A"));
	x += 4 * (2 * FWNUM);
	lcd_outdezAtt(x, ynum, telemetry_data.rem_bat, (DBLSIZE | UNSIGN));
	lcd_puts(x, y, PSTR("%"));
	y += FH;
	ynum += 3 * FH;
	
	x = 0;	
    lcd_puts  (x, y, STR_MAVLINK_RC_RSSI_LABEL);
	lcd_outdezAtt(x + 7 * FWNUM, ynum, telemetry_data.rc_rssi, (DBLSIZE | UNSIGN));
	lcd_puts(x + 7 * FWNUM, ynum + FH, PSTR("%"));
	if (g_model.mavlink.pc_rssi_en)
	{
		x += 8 * (2 * FWNUM);
		lcd_puts(x, y, STR_MAVLINK_PC_RSSI_LABEL);
		lcd_outdezAtt(x + 7 * FWNUM, ynum, telemetry_data.pc_rssi, (DBLSIZE));
		lcd_puts(x + 7 * FWNUM, ynum + FH,  PSTR("%"));
	}
    
}

/*!	\brief Navigation dislplay
 *	\details Shows Navigation telemetry.
 *	Altitude in this menu is the relative (to the home location) altitude. This
 *	is the same altitude used by the waypoints.
 *	\todo Add a similar menu to fly back to the home location.
 */
void menuTelemetryMavlinkNavigation(void) {
	
	mav_title(STR_MAVLINK_NAV_MENU_TITLE, MAVLINK_menu);
	
	uint8_t x, y, ynum;
	x = 7 * FWNUM;
//	x = xnum + 0 * FW;
	ynum = 2 * FH;
	y = FH;
	
    
	x = 0;	
    lcd_puts  (x, y, STR_MAVLINK_COURSE);
	lcd_outdezAtt(x + 7 * FWNUM, ynum, telemetry_data.course, (DBLSIZE | UNSIGN));
	lcd_puts(x + 7 * FWNUM, ynum, PSTR("o"));
	x += 8 * (2 * FWNUM);
    lcd_puts(x, y, STR_MAVLINK_HEADING);
	lcd_outdezAtt(x + 7 * FWNUM, ynum, telemetry_data.heading, (DBLSIZE | UNSIGN));
	lcd_puts(x + 7 * FWNUM, ynum,  PSTR("o"));
	y += 3 * FH;
	ynum += 3 * FH;
	
	x = 0;	
    lcd_puts  (x, y, STR_MAVLINK_BEARING);
	lcd_outdezAtt(x + 7 * FWNUM, ynum, telemetry_data.bearing, (DBLSIZE | UNSIGN));
	lcd_puts(x + 7 * FWNUM, ynum, PSTR("o"));
	x += 8 * (2 * FWNUM);
    lcd_puts(x, y, STR_MAVLINK_ALTITUDE);
	lcd_outdezFloat(x + 4 * FWNUM - 1, ynum, telemetry_data.loc_current.rel_alt, 1, (DBLSIZE));
	lcd_puts(x + 7 * FWNUM, ynum + FH,  PSTR("m"));
 
}



/*!	\brief GPS information menu
 *	\details Menu gives a lot of info from the gps like fix type, position,
 *	attitude, heading and velocity. Text is small and the user must focus to
 *	read it.
 *	\todo Text is small. Should we do something about this or leaf it like this.
 *	I don't think will be used much when a user is concentrated on flying.
 */
void menuTelemetryMavlinkGPS(void) {
	mav_title(STR_MAVLINK_GPS, MAVLINK_menu);

	uint8_t x1, x2, xnum, xnum2, y;
	x1 = FW;
	x2 = x1 + 12 * FW;
	xnum = 7 * FW + 3 * FWNUM;
	xnum2 = xnum + 11 * FWNUM;
	y = FH;

	lcd_putsnAtt(x1, y, STR_MAVLINK_GPS, 3, 0);
	if (telemetry_data.fix_type < 2) {
		lcd_putsnAtt(xnum, y, STR_MAVLINK_NO_FIX, 6, 0);
	} else {
		lcd_outdezNAtt(xnum, y, telemetry_data.fix_type, 0, 3);
		lcd_puts(xnum, y, PSTR("D"));
	}
	lcd_puts(x2, y, STR_MAVLINK_SAT);
	lcd_outdezNAtt(xnum2, y, telemetry_data.satellites_visible, 0, 2);

//	if (telemetry_data.fix_type > 0) {
	y += FH;
	lcd_puts(x1, y, STR_MAVLINK_HDOP);
	lcd_outdezFloat(xnum, y, telemetry_data.eph, 2);

	y += FH;
	lcd_puts(x1, y, STR_MAVLINK_LAT);
	lcd_outdezFloat(xnum, y, telemetry_data.loc_current.lat, 2);

	lcd_putsnAtt(x2, y, STR_MAVLINK_LON, 3, 0);
	lcd_outdezFloat(xnum2, y, telemetry_data.loc_current.lon, 2);

	y += FH;
	lcd_putsnAtt(x1, y, STR_MAVLINK_ALTITUDE, 3, 0);
	lcd_outdezAtt(xnum, y, telemetry_data.loc_current.gps_alt, 0);

	y += FH;
	lcd_putsnAtt(x1, y, STR_MAVLINK_COURSE, 6, 0);
	lcd_outdezFloat(xnum, y, telemetry_data.course, 2);

	y += FH;
	lcd_putsnAtt(x1, y, PSTR("V"), 1, 0);
	lcd_outdezAtt(xnum, y, telemetry_data.v, 0);
	//}
}

#ifdef DUMP_RX_TX
//! \brief Display one byte as hex.
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

//! \brief Hex dump of the current mavlink message.
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


/*!	\brief Mavlink General setup menu.
 *	\details Setup menu for generic mavlink settings.
 *	Current menu items
 *	- RC RSSI scale item. Used to adjust the scale of the RSSI indicator to match
 *	the actual rssi value
 *	- PC RSSI enable item. Can be used to dissable PC RSSI display if not used.
 *	This funcion is called from the model setup menus, not directly by the
 *	telemetry menus
 */
void menuTelemetryMavlinkSetup(uint8_t event) {
	
	MENU(STR_MAVMENUSETUP_TITLE, menuTabModel, e_MavSetup, ITEM_MAVLINK_MAX + 1, {0, 0, 1/*to force edit mode*/});
	
	uint8_t sub = m_posVert - 1;

	for (uint8_t i=0; i<LCD_LINES-1; i++) {
		uint8_t y = 1 + 1*FH + i*FH;
		uint8_t k = i+s_pgOfs;
		uint8_t blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
		uint8_t attr = (sub == k ? blink : 0);
		switch(k) {	
		case ITEM_MAVLINK_RC_RSSI_SCALE:
			lcd_putsLeft(y, STR_MAVLINK_RC_RSSI_SCALE_LABEL);
			lcd_outdezAtt(RADIO_SETUP_2ND_COLUMN, y, (25 + g_model.mavlink.rc_rssi_scale * 5), attr|LEFT);
			lcd_putc(lcdLastPos, y, '%');
			if (attr) CHECK_INCDEC_MODELVAR(event, g_model.mavlink.rc_rssi_scale, 0, 15);
			break;
		case ITEM_MAVLINK_PC_RSSI_EN:
			g_model.mavlink.pc_rssi_en = onoffMenuItem(g_model.mavlink.pc_rssi_en,
				RADIO_SETUP_2ND_COLUMN,
				y,
				STR_MAVLINK_PC_RSSI_EN_LABEL,
				attr,
				event);
			break;
		}
	}
}
