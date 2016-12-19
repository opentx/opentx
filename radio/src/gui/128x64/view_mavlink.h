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

/*!	\file view_mavlink.h
 *	Mavlink menu include file
 */

#ifndef _VIEW_MAVLINK_H_
#define _VIEW_MAVLINK_H_

#include "opentx.h"
#include "telemetry/mavlink.h"

#define APSIZE (BSS | DBLSIZE)

void menuViewTelemetryMavlink(event_t event);
void lcd_outdezFloat(uint8_t x, uint8_t y, float val, uint8_t precis, uint8_t mode = 0);
void mav_title(const pm_char * s, uint8_t index);
void menuTelemetryMavlinkInfos(void);
void menuTelemetryMavlinkFlightMode(void);
void menuTelemetryMavlinkBattery(void);
void menuTelemetryMavlinkNavigation(void);
void menuTelemetryMavlinkGPS(void);
#ifdef DUMP_RX_TX
void lcd_outhex2(uint8_t x, uint8_t y, uint8_t val);
void menuTelemetryMavlinkDump(event_t event);
#endif

/*!	\brief Mavlink menu enumerator
 *	\details Used to create a readable case statement for the
 *	\link menuTelemetryMavlink menuTelemetryMavlink \endlink funtion.
 */
enum mavlink_menu_ {
	MENU_INFO = 0, //
	MENU_MODE,
	MENU_BATT,
	MENU_NAV,
	MENU_GPS, //
#ifdef DUMP_RX_TX
	MENU_DUMP_RX, //
	MENU_DUMP_TX, //
#endif
	MAX_MAVLINK_MENU
};

//! \brief Pointer array to mavlink settings menus.
const MenuHandlerFunc menuTabMav[] PROGMEM = {
  menuViewTelemetryMavlink
};

//! \brief Mavlink setup menu configuration items list.
enum menuMavlinkSetupItems {
	ITEM_MAVLINK_RC_RSSI_SCALE,
	ITEM_MAVLINK_PC_RSSI_EN,
	ITEM_MAVLINK_MAX
};

//! \brief Menu index varible, initialized on info menu.
extern uint8_t MAVLINK_menu;

#endif // _VIEW_MAVLINK_H_
