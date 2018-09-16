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

int menuVerticalOffset;
int menuVerticalPosition;
int menuHorizontalPosition;
int8_t s_editMode;
uint8_t noHighlightCounter;
uint8_t menuCalibrationState; // TODO rename this variable
int checkIncDecSelection = 0;
int8_t  checkIncDec_Ret;

INIT_STOPS(stops100, 3, -100, 0, 100)
INIT_STOPS(stops1000, 3, -1000, 0, 1000)
INIT_STOPS(stopsSwitch, 15, SWSRC_FIRST, CATEGORY_END(-SWSRC_FIRST_LOGICAL_SWITCH), CATEGORY_END(-SWSRC_FIRST_TRIM), CATEGORY_END(-SWSRC_LAST_SWITCH+1), 0, CATEGORY_END(SWSRC_LAST_SWITCH), CATEGORY_END(SWSRC_FIRST_TRIM-1), CATEGORY_END(SWSRC_FIRST_LOGICAL_SWITCH-1), SWSRC_LAST)

void onSourceLongEnterPress(const char * result)
{
  if (result == STR_MENU_INPUTS)
    checkIncDecSelection = getFirstAvailable(MIXSRC_FIRST_INPUT, MIXSRC_LAST_INPUT, isInputAvailable)+1;
#if defined(LUA_MODEL_SCRIPTS)
  else if (result == STR_MENU_LUA)
    checkIncDecSelection = getFirstAvailable(MIXSRC_FIRST_LUA, MIXSRC_LAST_LUA, isSourceAvailable);
#endif
  else if (result == STR_MENU_STICKS)
    checkIncDecSelection = MIXSRC_FIRST_STICK;
  else if (result == STR_MENU_POTS)
    checkIncDecSelection = MIXSRC_FIRST_POT;
  else if (result == STR_MENU_MAX)
    checkIncDecSelection = MIXSRC_MAX;
  else if (result == STR_MENU_HELI)
    checkIncDecSelection = MIXSRC_FIRST_HELI;
  else if (result == STR_MENU_TRIMS)
    checkIncDecSelection = MIXSRC_FIRST_TRIM;
  else if (result == STR_MENU_SWITCHES)
    checkIncDecSelection = MIXSRC_FIRST_SWITCH;
  else if (result == STR_MENU_TRAINER)
    checkIncDecSelection = MIXSRC_FIRST_TRAINER;
  else if (result == STR_MENU_CHANNELS)
    checkIncDecSelection = getFirstAvailable(MIXSRC_FIRST_CH, MIXSRC_LAST_CH, isSourceAvailable);
  else if (result == STR_MENU_GVARS)
    checkIncDecSelection = MIXSRC_FIRST_GVAR;
  else if (result == STR_MENU_TELEMETRY) {
    for (int i = 0; i < MAX_TELEMETRY_SENSORS; i++) {
      TelemetrySensor * sensor = & g_model.telemetrySensors[i];
      if (sensor->isAvailable()) {
        checkIncDecSelection = MIXSRC_FIRST_TELEM + 3*i;
        break;
      }
    }
  }
}

void onSwitchLongEnterPress(const char *result)
{
  if (result == STR_MENU_SWITCHES)
    checkIncDecSelection = SWSRC_FIRST_SWITCH;
  else if (result == STR_MENU_TRIMS)
    checkIncDecSelection = SWSRC_FIRST_TRIM;
  else if (result == STR_MENU_LOGICAL_SWITCHES)
    checkIncDecSelection = SWSRC_FIRST_LOGICAL_SWITCH + getFirstAvailable(0, MAX_LOGICAL_SWITCHES, isLogicalSwitchAvailable);
  else if (result == STR_MENU_OTHER)
    checkIncDecSelection = SWSRC_ON;
  else if (result == STR_MENU_INVERT)
    checkIncDecSelection = SWSRC_INVERT;
}

#define DBLKEYS_PRESSED_RGT_LFT(in)    ((in & ((1<<KEY_RIGHT) + (1<<KEY_LEFT))) == ((1<<KEY_RIGHT) + (1<<KEY_LEFT)))
#define DBLKEYS_PRESSED_UP_DWN(in)     ((in & ((1<<KEY_UP) + (1<<KEY_DOWN))) == ((1<<KEY_UP) + (1<<KEY_DOWN)))
#define DBLKEYS_PRESSED_RGT_UP(in)     ((in & ((1<<KEY_RIGHT) + (1<<KEY_UP))) == ((1<<KEY_RIGHT) + (1<<KEY_UP)))
#define DBLKEYS_PRESSED_LFT_DWN(in)    ((in & ((1<<KEY_LEFT) + (1<<KEY_DOWN))) == ((1<<KEY_LEFT) + (1<<KEY_DOWN)))

int checkIncDec(event_t event, int val, int i_min, int i_max, unsigned int i_flags, IsValueAvailable isValueAvailable, const CheckIncDecStops &stops)
{

}

#define CURSOR_NOT_ALLOWED_IN_ROW(row) ((int8_t)MAXCOL(row) < 0)
#define HORTAB(row)                    (int8_t(row) > (int8_t)horTabMax ? horTab[horTabMax] : horTab[row])
#define MAXCOL_RAW(row)                (horTab ? HORTAB(row) : (const uint8_t)0)
#define MAXCOL(row)                    (MAXCOL_RAW(row) >= HIDDEN_ROW ? MAXCOL_RAW(row) : (const uint8_t)(MAXCOL_RAW(row) & (~NAVIGATION_LINE_BY_LINE)))
#define COLATTR(row)                   (MAXCOL_RAW(row) == (uint8_t)-1 ? (const uint8_t)0 : (const uint8_t)(MAXCOL_RAW(row) & NAVIGATION_LINE_BY_LINE))
#define INC(val, min, max)             if (val<max) {val++;} else if (max>min) {val=min;}
#define DEC(val, min, max)             if (val>min) {val--;} else {val=max;}

uint8_t menuPageIndex;
uint8_t menuPageCount;
uint16_t linesCount;
uint8_t linesDisplayed;

int8_t navigate(event_t event, int count, int rows, int columns, bool loop)
{
return 0;
}

bool check(event_t event, uint8_t curr, const MenuHandlerFunc * menuTab, uint8_t menuTabSize, const pm_uint8_t * horTab, uint8_t horTabMax, int rowcount, uint8_t flags)
{

  return true;
}

bool check_simple(event_t event, uint8_t curr, const MenuHandlerFunc *menuTab, uint8_t menuTabSize, int rowcount)
{
  return check(event, curr, menuTab, menuTabSize, NULL, 0, rowcount);
}

bool check_submenu_simple(event_t event, uint8_t rowcount)
{
  return check_simple(event, 0, NULL, 0, rowcount);
}
