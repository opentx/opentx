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

#if defined(MAVLINK)
#include "gui/9X/view_mavlink.h"
#endif

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
  e_LogicalSwitches,
  e_CustomFunctions,
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
void menuModelTelemetry(uint8_t event);
void menuModelTemplates(uint8_t event);
void menuModelExpoOne(uint8_t event);

extern uint8_t s_curveChan;

#if defined(CPUARM)
  #define FlightModesType uint16_t
#else
  #define FlightModesType uint8_t
#endif

#if LCD_W >= 212
  #define MIXES_2ND_COLUMN  (18*FW)
#else
  #define MIXES_2ND_COLUMN  (12*FW)
#endif

uint8_t editDelay(const coord_t y, const uint8_t event, const uint8_t attr, const pm_char *str, uint8_t delay)
{
  lcd_putsLeft(y, str);
  lcd_outdezAtt(MIXES_2ND_COLUMN, y, (10/DELAY_STEP)*delay, attr|PREC1|LEFT);
  if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, delay, DELAY_MAX);
  return delay;
}
#define EDIT_DELAY(x, y, event, attr, str, delay) editDelay(y, event, attr, str, delay)

const MenuFuncP_PROGMEM menuTabModel[] PROGMEM = {
  menuModelSelect,
  menuModelSetup,
  CASE_HELI(menuModelHeli)
  CASE_FLIGHT_MODES(menuModelFlightModesAll)
  menuModelExposAll,
  menuModelMixAll,
  menuModelLimits,
  CASE_CURVES(menuModelCurvesAll)
  menuModelLogicalSwitches,
  menuModelCustomFunctions,
  CASE_FRSKY(menuModelTelemetry)
  CASE_MAVLINK(menuTelemetryMavlinkSetup)
  CASE_TEMPLATES(menuModelTemplates)
};

#define COPY_MODE 1
#define MOVE_MODE 2
static uint8_t s_copyMode = 0;
static int8_t s_copySrcRow;
static int8_t s_copyTgtOfs;

#if defined(CPUM64)
  #define editNameCursorPos m_posHorz
#else
  static uint8_t editNameCursorPos = 0;
#endif

void editName(coord_t x, coord_t y, char *name, uint8_t size, uint8_t event, uint8_t active)
{
#if defined(CPUM64)
  // in order to save flash
  lcd_putsLeft(y, STR_NAME);
#endif

  uint8_t mode = 0;
  if (active) {
    if (s_editMode <= 0)
      mode = INVERS + FIXEDWIDTH;
    else
      mode = FIXEDWIDTH;
  }

  lcd_putsnAtt(x, y, name, size, ZCHAR | mode);

  if (active) {
    uint8_t cur = editNameCursorPos;
    if (s_editMode > 0) {
      int8_t c = name[cur];
      int8_t v = c;

      if (p1valdiff || IS_ROTARY_RIGHT(event) || IS_ROTARY_LEFT(event) || event==EVT_KEY_FIRST(KEY_DOWN) || event==EVT_KEY_FIRST(KEY_UP)
          || event==EVT_KEY_REPT(KEY_DOWN) || event==EVT_KEY_REPT(KEY_UP)) {
         v = checkIncDec(event, abs(v), 0, ZCHAR_MAX, 0);
         if (c <= 0) v = -v;
      }

      switch (event) {
#if defined(ROTARY_ENCODER_NAVIGATION)
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
#endif

        case EVT_KEY_BREAK(KEY_LEFT):
          if (cur>0) cur--;
          break;
        case EVT_KEY_BREAK(KEY_RIGHT):
          if (cur<size-1) cur++;
          break;

#if defined(ROTARY_ENCODER_NAVIGATION)
        case EVT_ROTARY_LONG:
          if (v==0) {
            s_editMode = 0;
            killEvents(event);
            break;
          }
          // no break
#endif

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
        name[cur] = v;
        eeDirty(EE_MODEL);
      }

      lcd_putcAtt(x+editNameCursorPos*FW, y, idx2char(v), ERASEBG|INVERS|FIXEDWIDTH);
    }
    else {
      cur = 0;
    }
    editNameCursorPos = cur;
  }
}

#if defined(CPUM64)
#define editSingleName(x, y, label, name, size, event, active) editName(x, y, name, size, event, active)
#else
void editSingleName(coord_t x, coord_t y, const pm_char *label, char *name, uint8_t size, uint8_t event, uint8_t active)
{
  lcd_putsLeft(y, label);
  editName(x, y, name, size, event, active);
}
#endif

static uint8_t s_currIdx;
