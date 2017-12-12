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

const char *warningText = NULL;
const char *warningInfoText;
uint8_t     warningInfoLength;
uint8_t     warningType;
uint8_t     warningResult = 0;
uint8_t     warningInfoFlags = ZCHAR;
int16_t     warningInputValue;
int16_t     warningInputValueMin;
int16_t     warningInputValueMax;
void        (*popupFunc)(event_t event) = NULL;
const char *popupMenuItems[POPUP_MENU_MAX_LINES];
uint8_t     s_menu_item = 0;
uint16_t    popupMenuNoItems = 0;
uint16_t    popupMenuOffset = 0;
uint8_t     popupMenuOffsetType = MENU_OFFSET_INTERNAL;
void        (*popupMenuHandler)(const char * result);

void runPopupWarningBox()
{
  // theme->drawMessageBox("", "", "", MESSAGEBOX_TYPE_WARNING);
  // lcdDrawSolidFilledRect(POPUP_X, POPUP_Y, POPUP_W, POPUP_H, TEXT_BGCOLOR);
  // lcdDrawSolidRect(POPUP_X, POPUP_Y, POPUP_W, POPUP_H, 2, ALARM_COLOR);
  // lcdDrawBitmap(POPUP_X+15, POPUP_Y+20, LBM_WARNING);
}

void drawMessageBox()
{
  // theme->drawMessageBox("", "", "", MESSAGEBOX_TYPE_INFO);
  // lcdDrawSolidFilledRect(POPUP_X, POPUP_Y, POPUP_W, POPUP_H, TEXT_BGCOLOR);
  // lcdDrawSolidRect(POPUP_X, POPUP_Y, POPUP_W, POPUP_H, 2, WARNING_COLOR);
  // lcdDrawBitmap(POPUP_X+15, POPUP_Y+20, LBM_MESSAGE);
}

void drawAlertBox(const char * title, const char * text, const char * action)
{
  theme->drawMessageBox(title, text, action, WARNING_TYPE_ALERT);
}

void showAlertBox(const char * title, const char * text, const char * action, uint8_t sound)
{
  drawAlertBox(title, text, action);
  AUDIO_ERROR_MESSAGE(sound);
  lcdRefresh();
  lcdSetContrast();
  clearKeyEvents();
  backlightOn();
  checkBacklight();
}

void showMessageBox(const char * title)
{
  // drawMessageBox();
  lcdDrawSizedText(WARNING_LINE_X, WARNING_LINE_Y, title, WARNING_LINE_LEN, DBLSIZE|WARNING_COLOR);
  lcdRefresh();
}

void runPopupWarning(event_t event)
{
  warningResult = false;

  theme->drawMessageBox(warningText, warningInfoText, warningType == WARNING_TYPE_ASTERISK ? STR_EXIT : STR_POPUPS, warningType);

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
    default:
      if (warningType != WARNING_TYPE_INPUT) break;
      s_editMode = EDIT_MODIFY_FIELD;
      warningInputValue = checkIncDec(event, warningInputValue, warningInputValueMin, warningInputValueMax);
      s_editMode = EDIT_SELECT_FIELD;
      break;
  }
}

const char * runPopupMenu(event_t event)
{
  const char * result = NULL;

  uint8_t display_count = min<unsigned int>(popupMenuNoItems, MENU_MAX_DISPLAY_LINES);

  switch (event) {
    case EVT_ROTARY_LEFT:
      if (s_menu_item > 0) {
        s_menu_item--;
      }
      else if (popupMenuOffset > 0) {
        popupMenuOffset--;
        result = STR_UPDATE_LIST;
      }
      else {
        s_menu_item = min<uint8_t>(display_count, MENU_MAX_DISPLAY_LINES) - 1;
        if (popupMenuNoItems > MENU_MAX_DISPLAY_LINES) {
          popupMenuOffset = popupMenuNoItems - MENU_MAX_DISPLAY_LINES;
          result = STR_UPDATE_LIST;
        }
      }
      break;

    case EVT_ROTARY_RIGHT:
      if (s_menu_item < display_count - 1 && popupMenuOffset + s_menu_item + 1 < popupMenuNoItems) {
        s_menu_item++;
      }
      else if (popupMenuNoItems > popupMenuOffset + display_count) {
        popupMenuOffset++;
        result = STR_UPDATE_LIST;
      }
      else {
        s_menu_item = 0;
        if (popupMenuOffset) {
          popupMenuOffset = 0;
          result = STR_UPDATE_LIST;
        }
      }
      break;

    case EVT_KEY_BREAK(KEY_ENTER):
      result = popupMenuItems[s_menu_item + (popupMenuOffsetType == MENU_OFFSET_INTERNAL ? popupMenuOffset : 0)];
      // no break

    case EVT_KEY_BREAK(KEY_EXIT):
      popupMenuNoItems = 0;
      s_menu_item = 0;
      popupMenuOffset = 0;
      break;
  }

  int y = (LCD_H - (display_count*(FH+1))) / 2;

  lcdDrawSolidFilledRect(MENU_X, y, MENU_W, display_count * (FH+1) + 1, TEXT_BGCOLOR);
  lcdDrawSolidRect(MENU_X, y, MENU_W, display_count * (FH+1) + 2, 1, ALARM_COLOR);

  for (uint8_t i=0; i<display_count; i++) {
    if (i == s_menu_item) {
      lcdDrawSolidFilledRect(MENU_X+1, i*(FH+1) + y + 1, MENU_W-2, FH+1, TEXT_INVERTED_BGCOLOR);
      lcdDrawText(MENU_X+6, i*(FH+1) + y + 2, popupMenuItems[i+(popupMenuOffsetType == MENU_OFFSET_INTERNAL ? popupMenuOffset : 0)], TEXT_INVERTED_COLOR);
    }
    else {
      lcdDrawText(MENU_X+6, i*(FH+1) + y + 2, popupMenuItems[i+(popupMenuOffsetType == MENU_OFFSET_INTERNAL ? popupMenuOffset : 0)], 0);
    }
  }

  if (popupMenuNoItems > display_count) {
    drawVerticalScrollbar(MENU_X+MENU_W-1, y+1, MENU_MAX_DISPLAY_LINES * (FH+1), popupMenuOffset, popupMenuNoItems, MENU_MAX_DISPLAY_LINES);
  }

  return result;
}
