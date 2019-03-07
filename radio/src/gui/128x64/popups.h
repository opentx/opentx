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

#ifndef _POPUPS_H_
#define _POPUPS_H_

#include <keys.h>

#define MENU_X                         10
#define MENU_Y                         16
#define MENU_W                         LCD_W-(2*MENU_X)
#define WARNING_LINE_LEN               20
#define WARNING_LINE_X                 16
#define WARNING_LINE_Y                 3*FH

void drawMessageBox();
void showMessageBox(const char * title);
void runPopupWarning(event_t event);

#define DRAW_MESSAGE_BOX(title)        (warningText = title, drawMessageBox(), warningText = NULL)

extern void (*popupFunc)(event_t event);
extern uint8_t warningInfoFlags;

#if !defined(GUI)
  #define DISPLAY_WARNING(...)
  #define POPUP_WARNING(...)
  #define POPUP_CONFIRMATION(...)
  #define POPUP_INPUT(...)
  #define WARNING_INFO_FLAGS           0
  #define SET_WARNING_INFO(...)
#else
  #define DISPLAY_WARNING              (*popupFunc)
  #define POPUP_INFORMATION(s)        (warningText = s, warningType = WARNING_TYPE_INFO, warningInfoText = 0, popupFunc = runPopupWarning)
  #define POPUP_WARNING(s)             (warningText = s, warningInfoText = 0, popupFunc = runPopupWarning)
  #define POPUP_CONFIRMATION(s)        (warningText = s, warningType = WARNING_TYPE_CONFIRM, warningInfoText = 0, popupFunc = runPopupWarning)
  #define POPUP_INPUT(s, func)         (warningText = s, popupFunc = func)
  #define WARNING_INFO_FLAGS           warningInfoFlags
  #define SET_WARNING_INFO(info, len, flags) (warningInfoText = info, warningInfoLength = len, warningInfoFlags = flags)
#endif

#if defined(SDCARD)
  #define POPUP_MENU_ADD_SD_ITEM(s)    POPUP_MENU_ADD_ITEM(s)
#else
  #define POPUP_MENU_ADD_SD_ITEM(s)
#endif

  #define NAVIGATION_MENUS
  #define POPUP_MENU_ADD_ITEM(s)       do { popupMenuOffsetType = MENU_OFFSET_INTERNAL; if (popupMenuItemsCount < POPUP_MENU_MAX_LINES) popupMenuItems[popupMenuItemsCount++] = s; } while (0)
  #define POPUP_MENU_SELECT_ITEM(s)    s_menu_item =  (s > 0 ? (s < popupMenuItemsCount ? s : popupMenuItemsCount) : 0)
  #define POPUP_MENU_START(func)       do { popupMenuHandler = (func); AUDIO_KEY_PRESS(); } while (0)
  #define POPUP_MENU_MAX_LINES         12
  #define MENU_MAX_DISPLAY_LINES       6
  #define MENU_LINE_LENGTH             (LEN_MODEL_NAME+12)
  #define POPUP_MENU_SET_BSS_FLAG()
  #define POPUP_MENU_UNSET_BSS_FLAG()
  enum {
    MENU_OFFSET_INTERNAL,
    MENU_OFFSET_EXTERNAL
  };
  extern uint8_t popupMenuOffsetType;
  extern uint8_t s_menu_item;

#if defined(NAVIGATION_MENUS)
  extern uint16_t popupMenuOffset;
  extern const char * popupMenuItems[POPUP_MENU_MAX_LINES];
  extern uint16_t popupMenuItemsCount;
  const char * runPopupMenu(event_t event);
  extern void (*popupMenuHandler)(const char * result);
  extern const char * popupMenuTitle;
#endif

#endif // _POPUPS_H_
