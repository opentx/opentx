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

#ifndef _STDLCD_POPUPS_H_
#define _STDLCD_POPUPS_H_

extern const char * warningText;
extern const char * warningInfoText;
extern uint8_t      warningInfoLength;
extern uint8_t      warningResult;
extern uint8_t      warningType;
extern uint8_t      warningInfoFlags;

extern void         (* popupFunc)(event_t event);
extern uint8_t      popupMenuOffsetType;

extern uint16_t     popupMenuOffset;
extern const char * popupMenuItems[POPUP_MENU_MAX_LINES];
extern uint16_t     popupMenuItemsCount;
extern void         (* popupMenuHandler)(const char * result);
extern const char * popupMenuTitle;
extern uint8_t      s_menu_item;

// Message box
void drawMessageBoxBackground(coord_t top, coord_t height);
void drawMessageBox(const char * title);
void showMessageBox(const char * title);

// Popup menu
const char * runPopupMenu(event_t event);
void runPopupWarning(event_t event);

// Full screen with an alert
void showAlertBox(const char * title, const char * text, const char * action , uint8_t sound);

// Full screen with 2 lines and a progress bar
void drawProgressScreen(const char * title, const char * message, int num, int den);

#endif // _STDLCD_POPUPS_H_
