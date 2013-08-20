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
	
	static bool setup;

	switch (event) // new event received, branch accordingly
	{
	case EVT_ENTRY:
		MAVLINK_menu = MENU_INFO;
		setup = false;
		break;

	case EVT_KEY_FIRST(KEY_UP):
		if (setup)
			break;
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
		if (setup)
			break;
		if (MAVLINK_menu < MAX_MAVLINK_MENU - 1)
			MAVLINK_menu++;
		break;
	case EVT_KEY_FIRST(KEY_MENU):
		setup = true;
		return;
	case EVT_KEY_FIRST(KEY_EXIT):
		if (setup)
		{
			setup = false;
			MAVLINK_Init();
			break;
		}
		chainMenu(menuMainView);
		return;
	}

	if (setup)
		menuTelemetryMavlinkSetup(event);
	else
	{
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

/*!	\brief Fightmode decode helper
 *	\details Decodes the flight mode from Mavlink custom mode enum to a string.
 *	This funtion can handle ArduPilot and ArduCoper code.
 */

const char * mav_mode_to_string(uint32_t custom_mode) //, const char * mode_text_p)
{
	const char * mode_text_p;
	switch (telemetry_data.type_autopilot) {
	case MAVLINK_ARDUCOPTER:
		switch(custom_mode) {
		case AC_STABILIZE:
			mode_text_p = PSTR("Stabilize");
			break;
		case AC_ACRO:
			mode_text_p = PSTR("Acro");
			break;
		case AC_ALT_HOLD:
			mode_text_p = PSTR("Alt Hold");
			break;
		case AC_AUTO:
			mode_text_p = PSTR("Auto");
			break;
		case AC_GUIDED:
			mode_text_p = PSTR("Guided");
			break;
		case AC_LOITER:
			mode_text_p = PSTR("Loiter");
			break;
		case AC_RTL:
			mode_text_p = PSTR("RTL");
			break;
		case AC_CIRCLE:
			mode_text_p = PSTR("Circle");
			break;
		case AC_POSITION:
			mode_text_p = PSTR("Pos Hold");
			break;
		case AC_LAND:
			mode_text_p = PSTR("Land");
			break;
		case AC_OF_LOITER:
			mode_text_p = PSTR("OF Loiter");
			break;
		case AC_TOY_A:
			mode_text_p = PSTR("Toy A");
			break;
		case AC_TOY_M:
			mode_text_p = PSTR("Toy M");
			break;
		default:
			mode_text_p = PSTR("INVALID");
			break;
		}
		break;
	case MAVLINK_ARDUPLANE:
		switch(custom_mode) {
		case AP_MANUAL:
			mode_text_p = PSTR("Manual");
			break;
		case AP_CIRCLE:
			mode_text_p = PSTR("Circle");
			break;
		case AP_STABILIZE:
			mode_text_p = PSTR("Stabilize");
			break;
		case AP_TRAINING:
			mode_text_p = PSTR("Training");
			break;
		case AP_FLY_BY_WIRE_A:
			mode_text_p = PSTR("Fly by Wire A");
			break;
		case AP_FLY_BY_WIRE_B:
			mode_text_p = PSTR("Fly by Wire A");
			break;
		case AP_AUTO:
			mode_text_p = PSTR("Auto");
			break;
		case AP_RTL:
			mode_text_p = PSTR("RTL");
			break;
		case AP_LOITER:
			mode_text_p = PSTR("Loiter");
			break;
		case AP_GUIDED:
			mode_text_p = PSTR("Guided");
			break;
		case AP_INITIALISING:
			mode_text_p = PSTR("Initialising");
			break;
		default:
			mode_text_p = PSTR("INVALID");
			break;
		}
		break;
	default:
		mode_text_p = PSTR("INVALID MAV TYPE");
		break;
	}
	return mode_text_p;
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

	mav_title(PSTR("INFOS"), MAVLINK_menu);

	uint8_t x1, x2, xnum, y;
	x1 = FW;
	x2 = 7 * FW;
	xnum = x2 + 5 * FWNUM;
	y = FH;

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

	if (telemetry_data.status) {

		lcd_putsnAtt(x1, y, PSTR("MODE"), 4, 0);
		if (telemetry_data.active)
		//if (telemetry_data.mode & MAV_MODE_FLAG_SAFETY_ARMED)
			lcd_putsnAtt(x2, y, PSTR("A"), 1, 0);
		lcd_outdezAtt(xnum, y, telemetry_data.mode, 0);

		y += FH;
		lcd_puts(x1, y, PSTR("BATT"));
		lcd_outdezNAtt(xnum, y, telemetry_data.vbat, PREC1, 5);

		y += FH;
		lcd_puts(x1, y, PSTR("DROP"));
		lcd_outdezAtt(xnum, y, telemetry_data.packet_drop, 0);

	}
}

/*!	\brief Flight mode menu
 *	\details Clear display of current flight mode.
 *	\todo Add functionality to change flight mode.
 */
void menuTelemetryMavlinkFlightMode(void) {
	
	mav_title(PSTR("MODE"), MAVLINK_menu);
	
	const char * mode_text_p =  mav_mode_to_string(telemetry_data.custom_mode);
	
	uint8_t x, y;
	x = 0;
	y = FH;
	
    lcd_puts  (x, y, PSTR("Current Mode"));
    y += FH;
	lcd_putsAtt (FW, y, mode_text_p, DBLSIZE);
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
    	lcd_putsAtt (FW, y, PSTR("Armed"), DBLSIZE);
}

/*!	\brief Batterystatus dislplay
 *	\details Shows flight batery status.
 */
void menuTelemetryMavlinkBattery(void) {
	
	mav_title(PSTR("BAT RSSI"), MAVLINK_menu);
	
	uint8_t x, y, ynum;
	x = 7 * FWNUM;
//	x = xnum + 0 * FW;
	ynum = 2 * FH;
	y = 3 * FH;
	
    lcd_puts(0, 1*FH, PSTR("Flight Batery status")); 
	
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
    lcd_puts  (x, y, PSTR("RC RSSI"));
	lcd_outdezAtt(x + 7 * FWNUM, ynum, telemetry_data.rc_rssi, (DBLSIZE | UNSIGN));
	lcd_puts(x + 7 * FWNUM, ynum + FH, PSTR("%"));
	x += 8 * (2 * FWNUM);
    lcd_puts(x, y, PSTR("PC RSSI"));
	lcd_outdezAtt(x + 7 * FWNUM, ynum, telemetry_data.pc_rssi, (DBLSIZE));
	lcd_puts(x + 7 * FWNUM, ynum + FH,  PSTR("%"));
    
}

/*!	\brief Navigation dislplay
 *	\details Shows Navigation telemetry.
 */
void menuTelemetryMavlinkNavigation(void) {
	
	mav_title(PSTR("NAV"), MAVLINK_menu);
	
	uint8_t x, y, ynum;
	x = 7 * FWNUM;
//	x = xnum + 0 * FW;
	ynum = 2 * FH;
	y = FH;
	
    
	x = 0;	
    lcd_puts  (x, y, PSTR("Course"));
	lcd_outdezAtt(x + 7 * FWNUM, ynum, telemetry_data.course, (DBLSIZE | UNSIGN));
	lcd_puts(x + 7 * FWNUM, ynum, PSTR("o"));
	x += 8 * (2 * FWNUM);
    lcd_puts(x, y, PSTR("Heading"));
	lcd_outdezAtt(x + 7 * FWNUM, ynum, telemetry_data.heading, (DBLSIZE | UNSIGN));
	lcd_puts(x + 7 * FWNUM, ynum,  PSTR("o"));
	y += 3 * FH;
	ynum += 3 * FH;
	
	x = 0;	
    lcd_puts  (x, y, PSTR("Bearing"));
	lcd_outdezAtt(x + 7 * FWNUM, ynum, telemetry_data.bearing, (DBLSIZE | UNSIGN));
	lcd_puts(x + 7 * FWNUM, ynum, PSTR("o"));
	x += 8 * (2 * FWNUM);
    lcd_puts(x, y, PSTR("Altitude"));
	lcd_outdezFloat(x + 2 * FWNUM - 1, ynum, telemetry_data.altitude, 2, (DBLSIZE));
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
	mav_title(PSTR("GPS"), MAVLINK_menu);

	uint8_t x1, x2, xnum, y;
	x1 = FW;
	x2 = 5 * FW + FWNUM;
	xnum = 5 * FW + 3 * FWNUM;
	y = FH;

	lcd_putsnAtt(x1, y, PSTR("GPS"), 3, 0);
	if (telemetry_data.fix_type <= 2) {
		lcd_putsnAtt(x2, y, PSTR("__NOOK") + 2 * telemetry_data.fix_type, 2, 0);
	} else {
		lcd_outdezNAtt(xnum, y, telemetry_data.fix_type, 0, 3);
		lcd_puts(xnum, y, PSTR("D"));
	}
	lcd_puts(x2 + 5 * FW, y, PSTR("SAT"));
	lcd_outdezNAtt(x2 + 8 * FW + 3 * FWNUM, y, telemetry_data.satellites_visible, 0, 2);

//	if (telemetry_data.fix_type > 0) {
	y += FH;
	lcd_puts(0, y, PSTR("HDOP"));
	lcd_outdezFloat(xnum, y, telemetry_data.eph, 2);

	y += FH;
	lcd_puts(0, y, PSTR("COOR"));
	lcd_outdezFloat(xnum, y, telemetry_data.loc_current.lat, 2);

	//	y += FH;
	//	lcd_putsnAtt(x1, y, PSTR("LON"), 3, 0);
	lcd_outdezFloat(xnum + 10 * FWNUM, y, telemetry_data.loc_current.lon, 2);

	y += FH;
	lcd_putsnAtt(x1, y, PSTR("ALT"), 3, 0);
	lcd_outdezAtt(xnum, y, telemetry_data.loc_current.alt, 2);

	y += FH;
	lcd_putsnAtt(x1, y, PSTR("COG"), 3, 0);
	lcd_outdezFloat(xnum, y, telemetry_data.course, 2);

	y += FH;
	lcd_putsnAtt(x1, y, PSTR("V"), 1, 0);
	lcd_outdezAtt(xnum, y, telemetry_data.v, 2);
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
 *	- Baudrate select item.
 *	MENU funtion is used to create the tab based menu.
 */
void menuTelemetryMavlinkSetup(uint8_t event) {
	
	MENU(STR_MAVMENUSETUP, menuTabMav, e_MavSetup, ITEM_MAVLINK_MAX+1, {0, 0, 1/*to force edit mode*/});
	
	uint8_t sub = m_posVert - 1;

	for (uint8_t i=0; i<LCD_LINES-1; i++) {
		uint8_t y = 1 + 1*FH + i*FH;
		uint8_t k = i+s_pgOfs;
		uint8_t blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
		uint8_t attr = (sub == k ? blink : 0);
		switch(k) {	
		case ITEM_MAVLINK_BAUD:
			g_eeGeneral.spare2 = selectMenuItem(RADIO_SETUP_2ND_COLUMN,  //Y
				y, 					// Y
				STR_MAVLINK_BAUD_LABEL, 			// pm_char *label
				STR_MAVLINK_BAUDS, 		// pm_char *values
//				PSTR("4800""9600""14400""19200""38400""57600""76800""115200"),
				g_eeGeneral.spare2, 	// value
				0, 	// min
				7, 	// max
				attr,  // attr
				event);	// event
			break;
		}
	}
}
