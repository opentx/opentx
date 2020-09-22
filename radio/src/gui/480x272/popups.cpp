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

const char *warningText = nullptr;
const char *warningInfoText;
uint8_t     warningInfoLength;
uint8_t     warningType;
uint8_t     warningResult = 0;
uint8_t     warningInfoFlags = ZCHAR;
PopupFunc popupFunc = nullptr;
const char * popupMenuTitle = nullptr;
const char *popupMenuItems[POPUP_MENU_MAX_LINES];
uint8_t     popupMenuSelectedItem = 0;
uint16_t    popupMenuItemsCount = 0;
uint16_t    popupMenuOffset = 0;
uint8_t     popupMenuOffsetType = MENU_OFFSET_INTERNAL;
void        (*popupMenuHandler)(const char * result);

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
  waitKeysReleased();
  resetBacklightTimeout();
  checkBacklight();
}

void showMessageBox(const char * title)
{
  // drawMessageBox();
  lcdDrawSizedText(WARNING_LINE_X, WARNING_LINE_Y, title, WARNING_LINE_LEN, DBLSIZE|WARNING_COLOR);
  lcdRefresh();
}

void drawPopupBackgroundAndBorder(coord_t x, coord_t y, coord_t w, coord_t h)
{
  lcdDrawSolidFilledRect(x + 1, y + 1, w - 2, h - 2, TEXT_BGCOLOR);
  lcdDrawSolidRect(x, y, w, h, 1, ALARM_COLOR);
  if (popupMenuTitle) {
    lcdDrawSolidFilledRect(x, y - FH, w, FH, TEXT_INVERTED_BGCOLOR);
    lcdDrawText(x + w / 2, y - FH, popupMenuTitle, BOLD | CENTERED | TEXT_INVERTED_COLOR);
  }
}

void runPopupWarning(event_t event)
{
  warningResult = false;

  const char * action;
  switch (warningType) {
    case WARNING_TYPE_INFO:
      action = STR_OK;
      break;
    case WARNING_TYPE_ASTERISK:
    case WARNING_TYPE_WAIT:
      action = STR_EXIT;
      break;
    default:
      action = STR_POPUPS_ENTER_EXIT;
      break;
  }

  theme->drawMessageBox(warningText, warningInfoText, action, warningType);

  switch (event) {
    case EVT_KEY_BREAK(KEY_ENTER):
      if (warningType == WARNING_TYPE_ASTERISK)
        // key ignored, the user has to press [EXIT]
        break;

      if (warningType == WARNING_TYPE_CONFIRM) {
        warningType = WARNING_TYPE_ASTERISK;
        warningText = nullptr;
        if (popupMenuHandler)
          popupMenuHandler(STR_OK);
        else
          warningResult = true;
        break;
      }
      // no break

    case EVT_KEY_BREAK(KEY_EXIT):
      if (warningType == WARNING_TYPE_CONFIRM) {
        if (popupMenuHandler)
          popupMenuHandler(STR_EXIT);
      }
      warningText = nullptr;
      warningType = WARNING_TYPE_ASTERISK;
      break;
  }
}

const char * runPopupMenu(event_t event)
{
  const char * result = nullptr;

  uint8_t display_count = min<unsigned int>(popupMenuItemsCount, MENU_MAX_DISPLAY_LINES);

  switch (event) {
    case EVT_ROTARY_LEFT:
      if (popupMenuSelectedItem > 0) {
        popupMenuSelectedItem--;
      }
      else if (popupMenuOffset > 0) {
        popupMenuOffset--;
        result = STR_UPDATE_LIST;
      }
      else {
        popupMenuSelectedItem = min<uint8_t>(display_count, MENU_MAX_DISPLAY_LINES) - 1;
        if (popupMenuItemsCount > MENU_MAX_DISPLAY_LINES) {
          popupMenuOffset = popupMenuItemsCount - MENU_MAX_DISPLAY_LINES;
          result = STR_UPDATE_LIST;
        }
      }
      break;

    case EVT_ROTARY_RIGHT:
      if (popupMenuSelectedItem < display_count - 1 && popupMenuOffset + popupMenuSelectedItem + 1 < popupMenuItemsCount) {
        popupMenuSelectedItem++;
      }
      else if (popupMenuItemsCount > popupMenuOffset + display_count) {
        popupMenuOffset++;
        result = STR_UPDATE_LIST;
      }
      else {
        popupMenuSelectedItem = 0;
        if (popupMenuOffset) {
          popupMenuOffset = 0;
          result = STR_UPDATE_LIST;
        }
      }
      break;

    case EVT_KEY_BREAK(KEY_ENTER):
      result = popupMenuItems[popupMenuSelectedItem + (popupMenuOffsetType == MENU_OFFSET_INTERNAL ? popupMenuOffset : 0)];
      popupMenuItemsCount = 0;
      popupMenuSelectedItem = 0;
      popupMenuOffset = 0;
      break;

    case EVT_KEY_BREAK(KEY_EXIT):
      result = STR_EXIT;
      popupMenuItemsCount = 0;
      popupMenuSelectedItem = 0;
      popupMenuOffset = 0;
      break;
  }

  int y = (LCD_H - (display_count*(FH + 1)) + (popupMenuTitle ? FH : 0) + FH) / 2;

  drawPopupBackgroundAndBorder(MENU_X, y, MENU_W, display_count * (FH + 1) + 2);

  for (uint8_t i=0; i<display_count; i++) {
    if (i == popupMenuSelectedItem) {
      lcdDrawText(MENU_X+6, i*(FH+1) + y + 2, popupMenuItems[i+(popupMenuOffsetType == MENU_OFFSET_INTERNAL ? popupMenuOffset : 0)], INVERS);
    }
    else {
      lcdDrawText(MENU_X+6, i*(FH+1) + y + 2, popupMenuItems[i+(popupMenuOffsetType == MENU_OFFSET_INTERNAL ? popupMenuOffset : 0)], 0);
    }
  }

  if (popupMenuItemsCount > display_count) {
    drawVerticalScrollbar(MENU_X+MENU_W-1, y+1, MENU_MAX_DISPLAY_LINES * (FH+1), popupMenuOffset, popupMenuItemsCount, MENU_MAX_DISPLAY_LINES);
  }

  return result;
}
