/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
 * - Andreas Weitl
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Gabriel Birkus
 * - Jean-Pierre Parisy
 * - Karl Szmutny
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
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

#include "../../opentx.h"

const char *warningText = NULL;
const char *warningInfoText;
uint8_t     s_warning_info_len;
uint8_t     warningType;
uint8_t     warningResult = 0;
uint8_t     s_warning_info_flags = ZCHAR;
int16_t     warningInputValue;
int16_t     warningInputValueMin;
int16_t     warningInputValueMax;
void        (*popupFunc)(uint8_t event) = NULL;
const char *popupMenuItems[POPUP_MENU_MAX_LINES];
uint8_t     s_menu_item = 0;
uint16_t    popupMenuNoItems = 0;
uint16_t    popupMenuOffset = 0;
uint8_t     popupMenuOffsetType = MENU_OFFSET_INTERNAL;
void        (*popupMenuHandler)(const char *result);

void displayBox(const char *title)
{
  drawFilledRect(10, 16, LCD_W-20, 40, SOLID, ERASE);
  lcd_rect(10, 16, LCD_W-20, 40);
  lcd_putsn(WARNING_LINE_X, WARNING_LINE_Y, title, WARNING_LINE_LEN);
  // could be a place for a warningInfoText
}

void displayPopup(const char *title)
{
  displayBox(title);
  lcdRefresh();
}

void message(const pm_char *title, const pm_char *t, const char *last MESSAGE_SOUND_ARG)
{
  lcd_clear();
  lcd_bmp(0, 0, asterisk_lbm);

#define MESSAGE_LCD_OFFSET   60

#if defined(TRANSLATIONS_FR) || defined(TRANSLATIONS_IT) || defined(TRANSLATIONS_CZ)
  lcd_putsAtt(MESSAGE_LCD_OFFSET, 0, STR_WARNING, DBLSIZE);
  lcd_putsAtt(MESSAGE_LCD_OFFSET, 2*FH, title, DBLSIZE);
#else
  lcd_putsAtt(MESSAGE_LCD_OFFSET, 0, title, DBLSIZE);
  lcd_putsAtt(MESSAGE_LCD_OFFSET, 2*FH, STR_WARNING, DBLSIZE);
#endif

  drawFilledRect(MESSAGE_LCD_OFFSET, 0, LCD_W-MESSAGE_LCD_OFFSET, 32);
  if (t) lcd_puts(MESSAGE_LCD_OFFSET, 5*FH, t);
  if (last) {
    lcd_puts(MESSAGE_LCD_OFFSET, 7*FH, last);
    AUDIO_ERROR_MESSAGE(sound);
  }

#undef MESSAGE_LCD_OFFSET

  lcdRefresh();
  lcdSetContrast();
  clearKeyEvents();
}

void displayWarning(uint8_t event)
{
  warningResult = false;
  displayBox(warningText);
  if (warningInfoText) {
    lcd_putsnAtt(WARNING_LINE_X, WARNING_LINE_Y+FH, warningInfoText, s_warning_info_len, WARNING_INFO_FLAGS);
  }
  lcd_puts(WARNING_LINE_X, WARNING_LINE_Y+2*FH, warningType == WARNING_TYPE_ASTERISK ? STR_EXIT : STR_POPUPS);
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

const char * displayPopupMenu(uint8_t event)
{
  const char * result = NULL;

  uint8_t display_count = min<unsigned int>(popupMenuNoItems, MENU_MAX_DISPLAY_LINES);
  uint8_t y = (display_count >= 5 ? MENU_Y - FH - 1 : MENU_Y);
  drawFilledRect(MENU_X, y, MENU_W, display_count * (FH+1) + 2, SOLID, ERASE);
  lcd_rect(MENU_X, y, MENU_W, display_count * (FH+1) + 2);

  for (uint8_t i=0; i<display_count; i++) {
    lcd_putsAtt(MENU_X+6, i*(FH+1) + y + 2, popupMenuItems[i+(popupMenuOffsetType == MENU_OFFSET_INTERNAL ? popupMenuOffset : 0)], 0);
    if (i == s_menu_item) drawFilledRect(MENU_X+1, i*(FH+1) + y + 1, MENU_W-2, 9);
  }

  if (popupMenuNoItems > display_count) {
    displayScrollbar(MENU_X+MENU_W-1, y+1, MENU_MAX_DISPLAY_LINES * (FH+1), popupMenuOffset, popupMenuNoItems, display_count);
  }

  switch(event) {
    case EVT_KEY_FIRST(KEY_MOVE_UP):
    case EVT_KEY_REPT(KEY_MOVE_UP):
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

    case EVT_KEY_FIRST(KEY_MOVE_DOWN):
    case EVT_KEY_REPT(KEY_MOVE_DOWN):
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
    CASE_EVT_ROTARY_BREAK
    case EVT_KEY_BREAK(KEY_ENTER):
      result = popupMenuItems[s_menu_item + (popupMenuOffsetType == MENU_OFFSET_INTERNAL ? popupMenuOffset : 0)];
      // no break
    case EVT_KEY_BREAK(KEY_EXIT):
      popupMenuNoItems = 0;
      s_menu_item = 0;
      popupMenuOffset = 0;
      break;
  }

  return result;
}
