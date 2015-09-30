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

const pm_char * s_warning = NULL;
const pm_char * s_warning_info;
uint8_t         s_warning_info_len;
uint8_t         s_warning_type;
uint8_t         s_warning_result = 0;

#if defined(CPUARM)
uint8_t         s_warning_info_flags = ZCHAR;
int16_t         s_warning_input_value;
int16_t         s_warning_input_min;
int16_t         s_warning_input_max;
#endif

void displayBox()
{
  drawFilledRect(10, 16, LCD_W-20, 40, SOLID, ERASE);
  lcd_rect(10, 16, LCD_W-20, 40);
#if defined(CPUARM)
  lcd_putsn(WARNING_LINE_X, WARNING_LINE_Y, s_warning, WARNING_LINE_LEN);
#else
  lcd_puts(WARNING_LINE_X, WARNING_LINE_Y, s_warning);
#endif
  // could be a place for a s_warning_info
}

void displayPopup(const pm_char * pstr)
{
  s_warning = pstr;
  displayBox();
  s_warning = NULL;
  lcdRefresh();
}

void message(const pm_char *title, const pm_char *t, const char *last MESSAGE_SOUND_ARG)
{
  lcd_clear();
  lcd_img(2, 0, asterisk_lbm, 0, 0);

#define MESSAGE_LCD_OFFSET   6*FW

#if defined(TRANSLATIONS_FR) || defined(TRANSLATIONS_IT) || defined(TRANSLATIONS_CZ)
  lcd_putsAtt(MESSAGE_LCD_OFFSET, 0, STR_WARNING, DBLSIZE);
  lcd_putsAtt(MESSAGE_LCD_OFFSET, 2*FH, title, DBLSIZE);
#else
  lcd_putsAtt(MESSAGE_LCD_OFFSET, 0, title, DBLSIZE);
  lcd_putsAtt(MESSAGE_LCD_OFFSET, 2*FH, STR_WARNING, DBLSIZE);
#endif

  drawFilledRect(0, 0, LCD_W, 32);
  if (t) lcd_putsLeft(5*FH, t);
  if (last) {
    lcd_putsLeft(7*FH, last);
    AUDIO_ERROR_MESSAGE(sound);
  }

#undef MESSAGE_LCD_OFFSET

  lcdRefresh();
  lcdSetContrast();
  clearKeyEvents();
}

void displayWarning(uint8_t event)
{
  s_warning_result = false;
  displayBox();
  if (s_warning_info) {
    lcd_putsnAtt(WARNING_LINE_X, WARNING_LINE_Y+FH, s_warning_info, s_warning_info_len, WARNING_INFO_FLAGS);
  }
  lcd_puts(WARNING_LINE_X, WARNING_LINE_Y+2*FH, s_warning_type == WARNING_TYPE_ASTERISK ? STR_EXIT : STR_POPUPS);
  switch (event) {
#if defined(ROTARY_ENCODER_NAVIGATION)
    case EVT_ROTARY_BREAK:
#endif
    case EVT_KEY_BREAK(KEY_ENTER):
      if (s_warning_type == WARNING_TYPE_ASTERISK)
        break;
      s_warning_result = true;
      // no break
#if defined(ROTARY_ENCODER_NAVIGATION)
    case EVT_ROTARY_LONG:
      killEvents(event);
#endif
    case EVT_KEY_BREAK(KEY_EXIT):
      s_warning = NULL;
      s_warning_type = WARNING_TYPE_ASTERISK;
      break;
#if defined(CPUARM)
    default:
      if (s_warning_type != WARNING_TYPE_INPUT) break;
      s_editMode = EDIT_MODIFY_FIELD;
      s_warning_input_value = checkIncDec(event, s_warning_input_value, s_warning_input_min, s_warning_input_max);
      s_editMode = EDIT_SELECT_FIELD;
      break;
#endif
  }
}

#if defined(CPUARM)
void (*popupFunc)(uint8_t event) = NULL;
#endif

#if defined(NAVIGATION_MENUS)
const char *s_menu[MENU_MAX_LINES];
uint8_t s_menu_item = 0;
uint16_t s_menu_count = 0;
uint8_t s_menu_flags = 0;
uint16_t s_menu_offset = 0;
void (*menuHandler)(const char *result);
const char * displayMenu(uint8_t event)
{
  const char * result = NULL;

  uint8_t display_count = min<uint8_t>(s_menu_count, MENU_MAX_LINES);
  uint8_t y = (display_count >= 5 ? MENU_Y - FH - 1 : MENU_Y);
  drawFilledRect(MENU_X, y, MENU_W, display_count * (FH+1) + 2, SOLID, ERASE);
  lcd_rect(MENU_X, y, MENU_W, display_count * (FH+1) + 2);

  for (uint8_t i=0; i<display_count; i++) {
    lcd_putsAtt(MENU_X+6, i*(FH+1) + y + 2, s_menu[i], s_menu_flags);
    if (i == s_menu_item) drawFilledRect(MENU_X+1, i*(FH+1) + y + 1, MENU_W-2, 9);
  }

  if (s_menu_count > display_count) {
    displayScrollbar(MENU_X+MENU_W-1, y+1, MENU_MAX_LINES * (FH+1), s_menu_offset, s_menu_count, MENU_MAX_LINES);
  }

  switch(event) {
#if defined(ROTARY_ENCODER_NAVIGATION)
    CASE_EVT_ROTARY_LEFT
#endif
    case EVT_KEY_FIRST(KEY_MOVE_UP):
    case EVT_KEY_REPT(KEY_MOVE_UP):
      if (s_menu_item > 0) {
        s_menu_item--;
      }
#if defined(SDCARD)
      else if (s_menu_offset > 0) {
        s_menu_offset--;
        result = STR_UPDATE_LIST;
      }
#endif
      else {
        s_menu_item = display_count - 1;
#if defined(SDCARD)
        if (s_menu_count > MENU_MAX_LINES) {
          s_menu_offset = s_menu_count - display_count;
          result = STR_UPDATE_LIST;
        }
#endif
      }
      break;

#if defined(ROTARY_ENCODER_NAVIGATION)
    CASE_EVT_ROTARY_RIGHT
#endif
    case EVT_KEY_FIRST(KEY_MOVE_DOWN):
    case EVT_KEY_REPT(KEY_MOVE_DOWN):
      if (s_menu_item < display_count - 1 && s_menu_offset + s_menu_item + 1 < s_menu_count) {
        s_menu_item++;
      }
#if defined(SDCARD)
      else if (s_menu_count > s_menu_offset + display_count) {
        s_menu_offset++;
        result = STR_UPDATE_LIST;
      }
#endif
      else {
        s_menu_item = 0;
#if defined(SDCARD)
        if (s_menu_offset) {
          s_menu_offset = 0;
          result = STR_UPDATE_LIST;
        }
#endif
      }
      break;
    CASE_EVT_ROTARY_BREAK
    case EVT_KEY_BREAK(KEY_ENTER):
      result = s_menu[s_menu_item];
      // no break
#if defined(ROTARY_ENCODER_NAVIGATION)
    CASE_EVT_ROTARY_LONG
      killEvents(event);
#endif
    case EVT_KEY_BREAK(KEY_EXIT):
      s_menu_count = 0;
      s_menu_item = 0;
      s_menu_flags = 0;
      s_menu_offset = 0;
      break;
  }

  return result;
}
#endif
