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
#define WCHART (LCD_H/2)
#define X0     (LCD_W-WCHART-2)
#define Y0     (LCD_H/2)

enum EnumTabModel {
  e_ModelSelect,
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
  CASE_MAVLINK(e_MavSetup)
  CASE_TEMPLATES(e_Templates)
};

void menuModelSelect(uint8_t event);
void menuModelSetup(uint8_t event);
void menuModelHeli(uint8_t event);
void menuModelFlightModesAll(uint8_t event);
void menuModelExposAll(uint8_t event);
void menuModelMixAll(uint8_t event);
void menuModelLimits(uint8_t event);
void menuModelCurvesAll(uint8_t event);
void menuModelCurveOne(uint8_t event);
void menuModelGVars(uint8_t event);
void menuModelLogicalSwitches(uint8_t event);
void menuModelCustomFunctions(uint8_t event);
void menuModelCustomScripts(uint8_t event);
void menuModelTelemetry(uint8_t event);
void menuModelExpoOne(uint8_t event);

extern uint8_t s_curveChan;

#define FlightModesType uint16_t

void editCurveRef(coord_t x, coord_t y, CurveRef & curve, uint8_t event, uint8_t attr);

#if MENU_COLUMNS < 2
  #define MIXES_2ND_COLUMN  (18*FW)
#else
  #define MIXES_2ND_COLUMN    (9*FW)
#endif

#if MENU_COLUMNS > 1
uint8_t editDelay(const coord_t x, const coord_t y, const uint8_t event, const uint8_t attr, const pm_char *str, uint8_t delay)
{
  lcd_puts(x, y, str);
  lcd_outdezAtt(x+MIXES_2ND_COLUMN, y, (10/DELAY_STEP)*delay, attr|PREC1|LEFT);
  if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, delay, DELAY_MAX);
  return delay;
}
#define EDIT_DELAY(x, y, event, attr, str, delay) editDelay(x, y, event, attr, str, delay)
#else
uint8_t editDelay(const coord_t y, const uint8_t event, const uint8_t attr, const pm_char *str, uint8_t delay)
{
  lcd_putsLeft(y, str);
  lcd_outdezAtt(MIXES_2ND_COLUMN, y, (10/DELAY_STEP)*delay, attr|PREC1|LEFT);
  if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, delay, DELAY_MAX);
  return delay;
}
#define EDIT_DELAY(x, y, event, attr, str, delay) editDelay(y, event, attr, str, delay)
#endif

const MenuFuncP_PROGMEM menuTabModel[] PROGMEM = {
  menuModelSelect,
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

void editName(coord_t x, coord_t y, char *name, uint8_t size, uint8_t event, uint8_t active, uint8_t attr)
{
  uint8_t mode = 0;
  if (active) {
    if (s_editMode <= 0)
      mode = INVERS + FIXEDWIDTH;
    else
      mode = FIXEDWIDTH;
  }

  lcd_putsnAtt(x, y, name, size, attr | mode);
  coord_t backupNextPos = lcdNextPos;

  if (active) {
    uint8_t cur = editNameCursorPos;
    if (s_editMode > 0) {
      int8_t c = name[cur];
      int8_t v = c;

      if (IS_ROTARY_RIGHT(event) || IS_ROTARY_LEFT(event) || event==EVT_KEY_FIRST(KEY_DOWN) || event==EVT_KEY_FIRST(KEY_UP)
          || event==EVT_KEY_REPT(KEY_DOWN) || event==EVT_KEY_REPT(KEY_UP)) {
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
        eeDirty(g_menuPos[0] == 0 ? EE_MODEL : EE_GENERAL);
      }

      if (attr == ZCHAR) {
        lcd_putcAtt(x+editNameCursorPos*FW, y, idx2char(v), ERASEBG|INVERS|FIXEDWIDTH);
      }
      else {
        lcd_putcAtt(x+editNameCursorPos*FW, y, v, ERASEBG|INVERS|FIXEDWIDTH);
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

static uint8_t s_currIdx;
