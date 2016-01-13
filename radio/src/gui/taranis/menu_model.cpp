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

#include "../../opentx.h"

#if MENU_COLUMNS > 1
uint8_t editDelay(const coord_t x, const coord_t y, const uint8_t event, const uint8_t attr, const pm_char *str, uint8_t delay)
{
  lcdDrawText(x, y, str);
  lcdDrawNumber(x+MIXES_2ND_COLUMN, y, (10/DELAY_STEP)*delay, attr|PREC1|LEFT);
  if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, delay, DELAY_MAX);
  return delay;
}
#else
uint8_t editDelay(const coord_t y, const uint8_t event, const uint8_t attr, const pm_char *str, uint8_t delay)
{
  lcd_putsLeft(y, str);
  lcdDrawNumber(MIXES_2ND_COLUMN, y, (10/DELAY_STEP)*delay, attr|PREC1|LEFT);
  if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, delay, DELAY_MAX);
  return delay;
}
#endif

#define COPY_MODE 1
#define MOVE_MODE 2
uint8_t s_copyMode = 0;
int8_t s_copySrcRow;
int8_t s_copyTgtOfs;
uint8_t s_maxLines = 8;
uint8_t s_copySrcIdx;
uint8_t s_copySrcCh;

uint8_t editNameCursorPos = 0;

void editName(coord_t x, coord_t y, char *name, uint8_t size, uint8_t event, uint8_t active, uint8_t attr)
{
  uint8_t mode = 0;
  if (active) {
    if (s_editMode <= 0)
      mode = INVERS + FIXEDWIDTH;
    else
      mode = FIXEDWIDTH;
  }

  lcdDrawSizedText(x, y, name, size, attr | mode);
  coord_t backupNextPos = lcdNextPos;

  if (active) {
    uint8_t cur = editNameCursorPos;
    if (s_editMode > 0) {
      int8_t c = name[cur];
      int8_t v = c;

      if (event==EVT_KEY_FIRST(KEY_DOWN) || event==EVT_KEY_FIRST(KEY_UP) || event==EVT_KEY_REPT(KEY_DOWN) || event==EVT_KEY_REPT(KEY_UP)) {
         if (attr == ZCHAR) {
           v = checkIncDec(event, abs(v), 0, ZCHAR_MAX, 0);
           if (c <= 0) v = -v;
         }
         else {
           v = checkIncDec(event, abs(v), '0', 'z', 0);
         }

      }

      switch (event) {
        case EVT_ROTARY_BREAK:
          if (s_editMode == EDIT_MODIFY_FIELD) {
            s_editMode = EDIT_MODIFY_STRING;
            cur = 0;
          }
          else if (cur<size-1)
            cur++;
          else
            s_editMode = 0;
          break;

        case EVT_ROTARY_LONG:
          if (attr & ZCHAR) {
            if (v == 0) {
              s_editMode = 0;
              killEvents(event);
            }
            else if (v>=-26 && v<=26) {
              v = -v; // toggle case
            }
          }
          else {
            if (v == ' ') {
              s_editMode = 0;
              killEvents(event);
              break;
            }
            else if (v>='A' && v<='Z') {
              v = 'a'+v-'A'; // toggle case
            }
            else if (v>='a' && v<='z') {
              v = 'A'+v-'a'; // toggle case
            }
          }
          break;
      }

      if (c != v) {
        name[cur] = v;
        storageDirty(menuVerticalPositions[0] == 0 ? EE_MODEL : EE_GENERAL);
      }

      if (attr == ZCHAR) {
        lcdDrawChar(x+editNameCursorPos*FW, y, idx2char(v), ERASEBG|INVERS|FIXEDWIDTH);
      }
      else {
        lcdDrawChar(x+editNameCursorPos*FW, y, v, ERASEBG|INVERS|FIXEDWIDTH);
      }
    }
    else {
      cur = 0;
    }
    editNameCursorPos = cur;
    lcdNextPos = backupNextPos;
  }
}

void editSingleName(coord_t x, coord_t y, const pm_char *label, char *name, uint8_t size, uint8_t event, uint8_t active)
{
  lcd_putsLeft(y, label);
  editName(x, y, name, size, event, active);
}

uint8_t s_currIdx;
