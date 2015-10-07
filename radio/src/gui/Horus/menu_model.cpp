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

// TODO elsewhere!
#define WCHART 64
#define X0     (LCD_W-WCHART-8)
#define Y0     118

enum EnumTabModel {
  // e_ModelSelect,
  e_ModelSetup,
  CASE_HELI(e_Heli)
  CASE_FLIGHT_MODES(e_FlightModesAll)
  e_InputsAll,
  e_MixAll,
  e_Limits,
  CASE_CURVES(e_CurvesAll)
  CASE_GVARS(e_GVars)
  e_LogicalSwitches,
  e_CustomFunctions,
#if defined(LUA_MODEL_SCRIPTS)
  e_CustomScripts,
#endif
  CASE_FRSKY(e_Telemetry)
};

void menuModelSelect(evt_t event);
void menuModelSetup(evt_t event);
void menuModelHeli(evt_t event);
void menuModelFlightModesAll(evt_t event);
void menuModelExposAll(evt_t event);
void menuModelMixAll(evt_t event);
void menuModelLimits(evt_t event);
void menuModelCurvesAll(evt_t event);
void menuModelCurveOne(evt_t event);
void menuModelGVars(evt_t event);
void menuModelLogicalSwitches(evt_t event);
void menuModelCustomFunctions(evt_t event);
void menuModelCustomScripts(evt_t event);
void menuModelTelemetry(evt_t event);
void menuModelExpoOne(evt_t event);

extern uint8_t s_curveChan;

#define FlightModesType uint16_t

void editCurveRef(coord_t x, coord_t y, CurveRef & curve, evt_t event, uint8_t attr);

#define MIXES_2ND_COLUMN    80

uint8_t editDelay(const coord_t x, const coord_t y, const evt_t event, const uint8_t attr, const pm_char *str, uint8_t delay)
{
  lcd_putsAtt(x+MENU_TITLE_LEFT, y, str, TEXT_COLOR);
  lcd_outdezAtt(x+MIXES_2ND_COLUMN, y, (10/DELAY_STEP)*delay, attr|PREC1|LEFT);
  if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, delay, DELAY_MAX);
  return delay;
}
#define EDIT_DELAY(x, y, event, attr, str, delay) editDelay(x, y, event, attr, str, delay)

const MenuFuncP_PROGMEM menuTabModel[] PROGMEM = {
//   menuModelSelect,
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
  menuModelCustomFunctions,
#if defined(LUA_MODEL_SCRIPTS)
  menuModelCustomScripts,
#endif
  CASE_FRSKY(menuModelTelemetry)
  CASE_MAVLINK(menuTelemetryMavlinkSetup)
  CASE_TEMPLATES(menuModelTemplates)
};

#define COPY_MODE 1
#define MOVE_MODE 2
static uint8_t s_copyMode = 0;
static int8_t s_copySrcRow;
static int8_t s_copyTgtOfs;

static uint8_t editNameCursorPos = 0;

void editName(coord_t x, coord_t y, char *name, uint8_t size, evt_t event, uint8_t active)
{
  uint8_t mode = 0;
  if (active && s_editMode <= 0) {
    mode = INVERS;
  }

  if (!active || s_editMode <= 0) {
    if (zlen(name, size) == 0) {
      char tmp[] = "---";
      lcd_putsnAtt(x, y, tmp, size, mode);
    }
    else {
      lcd_putsnAtt(x, y, name, size, ZCHAR | mode);
    }
  }

  if (active) {
    uint8_t cur = editNameCursorPos;
    if (s_editMode > 0) {
      int8_t c = name[cur];
      int8_t v = c;

      if (IS_ROTARY_RIGHT(event) || IS_ROTARY_LEFT(event) || event==EVT_KEY_FIRST(KEY_DOWN) || event==EVT_KEY_FIRST(KEY_UP)
          || event==EVT_KEY_REPT(KEY_DOWN) || event==EVT_KEY_REPT(KEY_UP)) {
         v = checkIncDec(event, abs(v), 0, ZCHAR_MAX, 0);
         if (c <= 0) v = -v;
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
          if (v==0) {
            s_editMode = 0;
            killEvents(event);
            break;
          }
          if (v>=-26 && v<=26) {
            v = -v; // toggle case
            if (event==EVT_KEY_LONG(KEY_LEFT))
              killEvents(KEY_LEFT);
          }
          break;
      }

      if (c != v) {
        name[cur] = v;
        eeDirty(g_menuPos[0] == 0 ? EE_MODEL : EE_GENERAL);
      }

      lcd_putsnAtt(x, y, name, size, ZCHAR | mode);
      coord_t w = (editNameCursorPos == 0 ? 0 : getTextWidth(name, editNameCursorPos, ZCHAR));
      char s[] = { idx2char(v), '\0' };
      lcdDrawFilledRect(x+w, y-1, getTextWidth(s, 1)-1, 12, TEXT_INVERTED_BGCOLOR);
      lcd_putsAtt(x+w, y, s, TEXT_INVERTED_COLOR);
    }
    else {
      cur = 0;
    }
    editNameCursorPos = cur;
  }
}

void editSingleName(coord_t x, coord_t y, const pm_char *label, char *name, uint8_t size, evt_t event, uint8_t active)
{
  lcd_putsLeft(y, label);
  editName(x, y, name, size, event, active);
}

static uint8_t s_currIdx;
