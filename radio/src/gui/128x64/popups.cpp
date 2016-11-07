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

#include "opentx.h"

const pm_char * warningText = NULL;
const pm_char * warningInfoText;
uint8_t         warningInfoLength;
uint8_t         warningType;
uint8_t         warningResult = 0;

#if defined(CPUARM)
uint8_t         warningInfoFlags = ZCHAR;
int16_t         warningInputValue;
int16_t         warningInputValueMin;
int16_t         warningInputValueMax;
#endif

void drawMessageBox()
{
  lcdDrawFilledRect(10, 16, LCD_W-20, 40, SOLID, ERASE);
  lcdDrawRect(10, 16, LCD_W-20, 40);
#if defined(CPUARM)
  lcdDrawSizedText(WARNING_LINE_X, WARNING_LINE_Y, warningText, WARNING_LINE_LEN);
#else
  lcdDrawText(WARNING_LINE_X, WARNING_LINE_Y, warningText);
#endif
  // could be a place for a warningInfoText
}

void showMessageBox(const pm_char * str)
{
  warningText = str;
  drawMessageBox();
  warningText = NULL;
  lcdRefresh();
}

const pm_uchar ASTERISK_BITMAP[] PROGMEM = {
#include "asterisk.lbm"
};

void drawAlertBox(const pm_char * title, const pm_char * text, const char * action)
{
  lcdClear();
  lcd_img(2, 0, ASTERISK_BITMAP, 0, 0);

#define MESSAGE_LCD_OFFSET   6*FW

#if defined(TRANSLATIONS_FR) || defined(TRANSLATIONS_IT) || defined(TRANSLATIONS_CZ)
  lcdDrawText(MESSAGE_LCD_OFFSET, 0, STR_WARNING, DBLSIZE);
  lcdDrawText(MESSAGE_LCD_OFFSET, 2*FH, title, DBLSIZE);
#else
  lcdDrawText(MESSAGE_LCD_OFFSET, 0, title, DBLSIZE);
  lcdDrawText(MESSAGE_LCD_OFFSET, 2*FH, STR_WARNING, DBLSIZE);
#endif
  
  lcdDrawSolidFilledRect(0, 0, LCD_W, 32);
  if (text) {
    lcdDrawTextAlignedLeft(5*FH, text);
  }
  if (action) {
    lcdDrawTextAlignedLeft(7*FH, action);
  }
  
#undef MESSAGE_LCD_OFFSET
}

void showAlertBox(const pm_char * title, const pm_char * text, const char * action ALERT_SOUND_ARG)
{
  drawAlertBox(title, text, action);
  
  AUDIO_ERROR_MESSAGE(sound);
  
  lcdRefresh();
  lcdSetContrast();
  clearKeyEvents();
}

void runPopupWarning(event_t event)
{
  warningResult = false;
  drawMessageBox();
  if (warningInfoText) {
    lcdDrawSizedText(WARNING_LINE_X, WARNING_LINE_Y+FH, warningInfoText, warningInfoLength, WARNING_INFO_FLAGS);
  }
  lcdDrawText(WARNING_LINE_X, WARNING_LINE_Y+2*FH, warningType == WARNING_TYPE_ASTERISK ? STR_EXIT : STR_POPUPS);
  switch (event) {
    case EVT_KEY_BREAK(KEY_ENTER):
      if (warningType == WARNING_TYPE_ASTERISK)
        break;
      warningResult = true;
      // no break
    case EVT_KEY_BREAK(KEY_EXIT):
      warningText = NULL;
      warningType = WARNING_TYPE_ASTERISK;
      break;
#if defined(CPUARM)
    default:
      if (warningType != WARNING_TYPE_INPUT) break;
      s_editMode = EDIT_MODIFY_FIELD;
      warningInputValue = checkIncDec(event, warningInputValue, warningInputValueMin, warningInputValueMax);
      s_editMode = EDIT_SELECT_FIELD;
      break;
#endif
  }
}

#if defined(CPUARM)
void (*popupFunc)(event_t event) = NULL;
#endif

#if defined(NAVIGATION_MENUS)
const char *popupMenuItems[POPUP_MENU_MAX_LINES];
uint8_t s_menu_item = 0;
uint16_t popupMenuNoItems = 0;
uint8_t popupMenuFlags = 0;
uint16_t popupMenuOffset = 0;
void (*popupMenuHandler)(const char * result);

#if defined(CPUARM)
uint8_t      popupMenuOffsetType = MENU_OFFSET_INTERNAL;
#endif

const char * runPopupMenu(event_t event)
{
  const char * result = NULL;

  uint8_t display_count = min<uint8_t>(popupMenuNoItems, POPUP_MENU_MAX_LINES);
  uint8_t y = (display_count >= 5 ? MENU_Y - FH - 1 : MENU_Y);
  lcdDrawFilledRect(MENU_X, y, MENU_W, display_count * (FH+1) + 2, SOLID, ERASE);
  lcdDrawRect(MENU_X, y, MENU_W, display_count * (FH+1) + 2);

  for (uint8_t i=0; i<display_count; i++) {
    lcdDrawText(MENU_X+6, i*(FH+1) + y + 2, popupMenuItems[i], popupMenuFlags);
    if (i == s_menu_item) lcdDrawSolidFilledRect(MENU_X+1, i*(FH+1) + y + 1, MENU_W-2, 9);
  }

  if (popupMenuNoItems > display_count) {
    drawVerticalScrollbar(MENU_X+MENU_W-1, y+1, POPUP_MENU_MAX_LINES * (FH+1), popupMenuOffset, popupMenuNoItems, POPUP_MENU_MAX_LINES);
  }

  switch (event) {
#if defined(ROTARY_ENCODER_NAVIGATION)
    CASE_EVT_ROTARY_LEFT
#endif
    case EVT_KEY_FIRST(KEY_UP):
    case EVT_KEY_REPT(KEY_UP):
      if (s_menu_item > 0) {
        s_menu_item--;
      }
#if defined(SDCARD)
      else if (popupMenuOffset > 0) {
        popupMenuOffset--;
        result = STR_UPDATE_LIST;
      }
#endif
      else {
        s_menu_item = display_count - 1;
#if defined(SDCARD)
        if (popupMenuNoItems > POPUP_MENU_MAX_LINES) {
          popupMenuOffset = popupMenuNoItems - display_count;
          result = STR_UPDATE_LIST;
        }
#endif
      }
      break;

#if defined(ROTARY_ENCODER_NAVIGATION)
    CASE_EVT_ROTARY_RIGHT
#endif
    case EVT_KEY_FIRST(KEY_DOWN):
    case EVT_KEY_REPT(KEY_DOWN):
      if (s_menu_item < display_count - 1 && popupMenuOffset + s_menu_item + 1 < popupMenuNoItems) {
        s_menu_item++;
      }
#if defined(SDCARD)
      else if (popupMenuNoItems > popupMenuOffset + display_count) {
        popupMenuOffset++;
        result = STR_UPDATE_LIST;
      }
#endif
      else {
        s_menu_item = 0;
#if defined(SDCARD)
        if (popupMenuOffset) {
          popupMenuOffset = 0;
          result = STR_UPDATE_LIST;
        }
#endif
      }
      break;
    CASE_EVT_ROTARY_BREAK
    case EVT_KEY_BREAK(KEY_ENTER):
      result = popupMenuItems[s_menu_item];
      // no break
#if defined(ROTARY_ENCODER_NAVIGATION)
    CASE_EVT_ROTARY_LONG
      killEvents(event);
#endif
    case EVT_KEY_BREAK(KEY_EXIT):
      popupMenuNoItems = 0;
      s_menu_item = 0;
      popupMenuFlags = 0;
      popupMenuOffset = 0;
      break;
  }

  return result;
}
#endif
