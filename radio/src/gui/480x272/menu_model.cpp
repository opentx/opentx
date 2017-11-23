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

const MenuHandlerFunc menuTabModel[] = {
  menuModelSetup,
  CASE_HELI(menuModelHeli)
  CASE_FLIGHT_MODES(menuModelFlightModesAll)
  menuModelExposAll,
  menuModelMixAll,
  menuModelLimits,
  CASE_CURVES(menuModelCurvesAll)
#if defined(GVARS) && defined(FLIGHT_MODES)
  CASE_GVARS(menuModelGVars)
#endif
  menuModelLogicalSwitches,
  menuModelSpecialFunctions,
#if defined(LUA_MODEL_SCRIPTS)
  menuModelCustomScripts,
#endif
  CASE_FRSKY(menuModelTelemetryFrsky)
  CASE_MAVLINK(menuModelTelemetryMavlink)
};

void editCurveRef(coord_t x, coord_t y, CurveRef & curve, event_t event, LcdFlags attr);

uint8_t editDelay(coord_t x, coord_t y, event_t event, uint8_t attr, uint8_t delay)
{
  lcdDrawNumber(x+MIXES_2ND_COLUMN, y, (10/DELAY_STEP)*delay, attr|PREC1|LEFT);
  if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, delay, DELAY_MAX);
  return delay;
}

uint8_t s_copyMode = 0;
int8_t s_copySrcRow;
int8_t s_copyTgtOfs;
uint8_t s_copySrcIdx;
uint8_t s_copySrcCh;

uint8_t editNameCursorPos = 0;

void editName(coord_t x, coord_t y, char * name, uint8_t size, event_t event, uint8_t active, LcdFlags flags)
{
  if (active && s_editMode <= 0) {
    flags |= INVERS;
  }

  if (!active || s_editMode <= 0) {
    if (flags & ZCHAR) {
      uint8_t len = zlen(name, size);
      if (len == 0) {
        char tmp[] = "---";
        lcdDrawSizedText(x, y, tmp, size, flags-ZCHAR);
      }
      else {
        lcdDrawSizedText(x, y, name, len, flags);
      }
    }
    else {
      lcdDrawText(x, y, name, flags);
    }
  }

  if (active) {
    if (s_editMode > 0) {
      int8_t c = name[editNameCursorPos];
      if (!(flags & ZCHAR)) {
        c = char2idx(c);
      }
      int8_t v = c;

      if (event==EVT_ROTARY_RIGHT || event==EVT_ROTARY_LEFT) {
        v = checkIncDec(event, abs(v), 0, ZCHAR_MAX, 0);
        if (c <= 0) v = -v;
      }

      switch (event) {
        case EVT_KEY_BREAK(KEY_LEFT):
          if (editNameCursorPos>0) editNameCursorPos--;
          break;

        case EVT_KEY_BREAK(KEY_RIGHT):
          if (editNameCursorPos<size-1) editNameCursorPos++;
          break;

        case EVT_KEY_BREAK(KEY_ENTER):
          if (s_editMode == EDIT_MODIFY_FIELD) {
            s_editMode = EDIT_MODIFY_STRING;
            editNameCursorPos = 0;
          }
          else if (editNameCursorPos<size-1)
            editNameCursorPos++;
          else
            s_editMode = 0;
          break;

        case EVT_KEY_LONG(KEY_ENTER):
          if (v == 0) {
            s_editMode = 0;
            killEvents(event);
            break;
          }
          // no break

        case EVT_KEY_LONG(KEY_LEFT):
        case EVT_KEY_LONG(KEY_RIGHT):
          if (v>=-26 && v<=26) {
            v = -v; // toggle case
            if (event==EVT_KEY_LONG(KEY_LEFT))
              killEvents(KEY_LEFT);
          }
          break;
      }

      if (c != v) {
        if (!(flags & ZCHAR)) {
          if (v != '\0' || name[editNameCursorPos+1] != '\0')
            v = idx2char(v);
        }
        name[editNameCursorPos] = v;
        storageDirty(menuVerticalPositions[0] == 0 ? EE_MODEL : EE_GENERAL);
      }

      lcdDrawSizedText(x, y, name, size, flags);
      coord_t left = (editNameCursorPos == 0 ? 0 : getTextWidth(name, editNameCursorPos, flags));
      char s[] = { (flags & ZCHAR) ? idx2char(name[editNameCursorPos]) : name[editNameCursorPos], '\0' };
      lcdDrawSolidFilledRect(x+left-1, y, getTextWidth(s, 1)+1, INVERT_LINE_HEIGHT, TEXT_INVERTED_BGCOLOR);
      lcdDrawText(x+left, y, s, TEXT_INVERTED_COLOR);
    }
    else {
      editNameCursorPos = 0;
    }
  }
}

uint8_t s_currIdx;

void copySelection(char * dst, const char * src, uint8_t size)
{
  if (memcmp(src, "---", 3) == 0)
    memset(dst, 0, size);
  else
    memcpy(dst, src, size);
}
