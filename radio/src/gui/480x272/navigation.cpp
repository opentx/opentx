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
  int newval = val;

  uint32_t in = KEYS_PRESSED();
  if (!(i_flags & NO_DBLKEYS) && (EVT_KEY_MASK(event))) {
    bool dblkey = true;
    if (DBLKEYS_PRESSED_RGT_LFT(in)) {
      if (!isValueAvailable || isValueAvailable(-val)) {
        newval = -val;
      }
    }
    else if (DBLKEYS_PRESSED_RGT_UP(in)) {
      newval = (i_max > stops.max() ? stops.max() : i_max);
      while (isValueAvailable && !isValueAvailable(newval) && newval>i_min) {
        --newval;
      }
    }
    else if (DBLKEYS_PRESSED_LFT_DWN(in)) {
      newval = (i_min < stops.min() ? stops.min() : i_min);
      while (isValueAvailable && !isValueAvailable(newval) && newval<i_max) {
        ++newval;
      }
    }
    else if (DBLKEYS_PRESSED_UP_DWN(in)) {
      newval = 0;
    }
    else {
      dblkey = false;
    }

    if (dblkey) {
      killEvents(KEY_UP);
      killEvents(KEY_DOWN);
      killEvents(KEY_RIGHT);
      killEvents(KEY_LEFT);
      killEvents(KEY_ENTER);
      killEvents(KEY_EXIT);
      event = 0;
    }
  }

  if (s_editMode>0 && event==EVT_ROTARY_RIGHT) {
    newval += min<int>(rotencSpeed, i_max-val);
    while (isValueAvailable && !isValueAvailable(newval) && newval<=i_max) {
      newval++;
    }
    if (newval > i_max) {
      newval = val;
      AUDIO_KEY_ERROR();
    }
  }
  else if (s_editMode>0 && event==EVT_ROTARY_LEFT) {
    newval -= min<int>(rotencSpeed, val-i_min);
    while (isValueAvailable && !isValueAvailable(newval) && newval>=i_min) {
      newval--;
    }
    if (newval < i_min) {
      newval = val;
      AUDIO_KEY_ERROR();
    }
  }

  if (!READ_ONLY() && i_min==0 && i_max==1 && event==EVT_KEY_BREAK(KEY_ENTER)) {
    s_editMode = 0;
    newval = !val;
  }

#if defined(AUTOSWITCH)
  if (i_flags & INCDEC_SWITCH) {
    newval = checkIncDecMovedSwitch(newval);
  }
#endif

#if defined(AUTOSOURCE)
  if (i_flags & INCDEC_SOURCE) {
    if (s_editMode>0) {
      int source = GET_MOVED_SOURCE(i_min, i_max);
      if (source) {
        newval = source;
      }
#if defined(AUTOSWITCH)
      else {
        unsigned int swtch = abs(getMovedSwitch());
        if (swtch) {
          newval = switchToMix(swtch);
        }
      }
#endif
    }
  }
#endif

  if (newval != val) {
    storageDirty(i_flags & (EE_GENERAL|EE_MODEL));
    checkIncDec_Ret = (newval > val ? 1 : -1);
    AUDIO_KEY_PRESS();
  }
  else {
    checkIncDec_Ret = 0;
  }

  if (i_flags & INCDEC_SOURCE) {
    if (event == EVT_KEY_LONG(KEY_ENTER)) {
      killEvents(event);
      checkIncDecSelection = MIXSRC_NONE;

      if (i_min <= MIXSRC_FIRST_INPUT && i_max >= MIXSRC_FIRST_INPUT) {
        if (getFirstAvailable(MIXSRC_FIRST_INPUT, MIXSRC_LAST_INPUT, isInputAvailable) != MIXSRC_NONE) {
          POPUP_MENU_ADD_ITEM(STR_MENU_INPUTS);
        }
      }
#if defined(LUA_MODEL_SCRIPTS)
      if (i_min <= MIXSRC_FIRST_LUA && i_max >= MIXSRC_FIRST_LUA) {
        if (getFirstAvailable(MIXSRC_FIRST_LUA, MIXSRC_LAST_LUA, isSourceAvailable) != MIXSRC_NONE) {
          POPUP_MENU_ADD_ITEM(STR_MENU_LUA);
        }
      }
#endif
      if (i_min <= MIXSRC_FIRST_STICK && i_max >= MIXSRC_FIRST_STICK)      POPUP_MENU_ADD_ITEM(STR_MENU_STICKS);
      if (i_min <= MIXSRC_FIRST_POT && i_max >= MIXSRC_FIRST_POT)          POPUP_MENU_ADD_ITEM(STR_MENU_POTS);
      if (i_min <= MIXSRC_MAX && i_max >= MIXSRC_MAX)                      POPUP_MENU_ADD_ITEM(STR_MENU_MAX);
#if defined(HELI)
      if (i_min <= MIXSRC_FIRST_HELI && i_max >= MIXSRC_FIRST_HELI)        POPUP_MENU_ADD_ITEM(STR_MENU_HELI);
#endif
      if (i_min <= MIXSRC_FIRST_TRIM && i_max >= MIXSRC_FIRST_TRIM)        POPUP_MENU_ADD_ITEM(STR_MENU_TRIMS);
      if (i_min <= MIXSRC_FIRST_SWITCH && i_max >= MIXSRC_FIRST_SWITCH)    POPUP_MENU_ADD_ITEM(STR_MENU_SWITCHES);
      if (i_min <= MIXSRC_FIRST_TRAINER && i_max >= MIXSRC_FIRST_TRAINER)  POPUP_MENU_ADD_ITEM(STR_MENU_TRAINER);
      if (i_min <= MIXSRC_FIRST_CH && i_max >= MIXSRC_FIRST_CH)            POPUP_MENU_ADD_ITEM(STR_MENU_CHANNELS);
      if (i_min <= MIXSRC_FIRST_GVAR && i_max >= MIXSRC_FIRST_GVAR && isValueAvailable(MIXSRC_FIRST_GVAR)) {
        POPUP_MENU_ADD_ITEM(STR_MENU_GVARS);
      }

      if (i_min <= MIXSRC_FIRST_TELEM && i_max >= MIXSRC_FIRST_TELEM) {
        for (int i = 0; i < MAX_TELEMETRY_SENSORS; i++) {
          TelemetrySensor * sensor = & g_model.telemetrySensors[i];
          if (sensor->isAvailable()) {
            POPUP_MENU_ADD_ITEM(STR_MENU_TELEMETRY);
            break;
          }
        }
      }
      POPUP_MENU_START(onSourceLongEnterPress);
    }
    if (checkIncDecSelection != 0) {
      newval = checkIncDecSelection;
      if (checkIncDecSelection != MIXSRC_MAX)
        s_editMode = EDIT_MODIFY_FIELD;
      checkIncDecSelection = 0;
    }
  }
  else if (i_flags & INCDEC_SWITCH) {
    if (event == EVT_KEY_LONG(KEY_ENTER)) {
      killEvents(event);
      checkIncDecSelection = SWSRC_NONE;
      if (i_min <= SWSRC_FIRST_SWITCH && i_max >= SWSRC_LAST_SWITCH)       POPUP_MENU_ADD_ITEM(STR_MENU_SWITCHES);
      if (i_min <= SWSRC_FIRST_TRIM && i_max >= SWSRC_LAST_TRIM)           POPUP_MENU_ADD_ITEM(STR_MENU_TRIMS);
      if (i_min <= SWSRC_FIRST_LOGICAL_SWITCH && i_max >= SWSRC_LAST_LOGICAL_SWITCH) {
        for (int i = 0; i < MAX_LOGICAL_SWITCHES; i++) {
          if (isValueAvailable && isValueAvailable(SWSRC_FIRST_LOGICAL_SWITCH+i)) {
            POPUP_MENU_ADD_ITEM(STR_MENU_LOGICAL_SWITCHES);
            break;
          }
        }
      }
      if (isValueAvailable && isValueAvailable(SWSRC_ON))                  POPUP_MENU_ADD_ITEM(STR_MENU_OTHER);
      if (isValueAvailable && isValueAvailable(-newval))                   POPUP_MENU_ADD_ITEM(STR_MENU_INVERT);
      POPUP_MENU_START(onSwitchLongEnterPress);
      s_editMode = EDIT_MODIFY_FIELD;
    }
    if (checkIncDecSelection != 0) {
      newval = (checkIncDecSelection == SWSRC_INVERT ? -newval : checkIncDecSelection);
      s_editMode = EDIT_MODIFY_FIELD;
      checkIncDecSelection = 0;
    }
  }
  return newval;
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
  int8_t result = 0;

  int prevPosHorz = menuHorizontalPosition;
  int prevPosVert = menuVerticalPosition;

  int maxrow = ((count+columns-1) / columns) - 1;
  int maxlastcol = count-maxrow*columns-1;
  int maxcol = (menuVerticalPosition != maxrow ? columns-1 : maxlastcol);

  if (menuVerticalPosition > maxrow) {
    menuVerticalPosition = maxrow;
    menuHorizontalPosition = maxlastcol;
  }

  switch (event) {
    case EVT_ROTARY_LEFT:
      result = -1;
      if (menuHorizontalPosition > 0) {
        menuHorizontalPosition--;
      }
      else if (loop) {
        DEC(menuVerticalPosition, 0, maxrow);
        menuHorizontalPosition = (menuVerticalPosition != maxrow ? columns-1 : maxlastcol);
      }
      else if (menuVerticalPosition > 0) {
        menuVerticalPosition -= 1;
        menuHorizontalPosition = columns-1;
      }
      break;

    case EVT_ROTARY_RIGHT:
      result = +1;
      if (menuHorizontalPosition < maxcol) {
        menuHorizontalPosition++;
      }
      else if (loop) {
        INC(menuVerticalPosition, 0, maxrow);
        menuHorizontalPosition = 0;
      }
      else if (menuVerticalPosition < maxrow) {
        menuVerticalPosition += 1;
        menuHorizontalPosition = 0;
      }
      break;
  }

  if (menuVerticalOffset > menuVerticalPosition) {
    menuVerticalOffset = menuVerticalPosition;
  }
  else if (menuVerticalOffset <= menuVerticalPosition - rows) {
    menuVerticalOffset = menuVerticalPosition - rows + 1;
  }

  return (prevPosHorz != menuHorizontalPosition || prevPosVert != menuVerticalPosition) ? result : 0;
}

bool check(event_t event, uint8_t curr, const MenuHandlerFunc * menuTab, uint8_t menuTabSize, const pm_uint8_t * horTab, uint8_t horTabMax, int rowcount, uint8_t flags)
{
  uint8_t maxcol = MAXCOL(menuVerticalPosition);

  if (menuTab == menuTabModel && event == EVT_KEY_FIRST(KEY_MODEL)) {
    killEvents(event);
    pushMenu(menuTabMonitors[lastMonitorPage]);
    return false;
  }

  if (menuTab && !menuCalibrationState) {
    int cc = curr;
    switch (event) {
#if defined(PCBX12S)
      case EVT_KEY_FIRST(KEY_PGDN):
#elif defined(PCBX10)
      case EVT_KEY_BREAK(KEY_PGDN):
#endif
        if (++cc == menuTabSize)
          cc = 0;
        break;

      case EVT_KEY_FIRST(KEY_PGUP):
#if defined(PCBX10)
      case EVT_KEY_LONG(KEY_PGDN):
#endif
        if (cc-- == 0)
          cc = menuTabSize-1;
        killEvents(event);
        break;
    }

    if (cc != curr) {
      chainMenu((MenuHandlerFunc)pgm_read_adr(&menuTab[cc]));
      return false;
    }

    menuPageIndex = curr;
    menuPageCount = menuTabSize;
  }

  switch (event) {
    case EVT_ENTRY:
      s_editMode = EDIT_MODE_INIT;
      menuVerticalPosition = MENU_FIRST_LINE_EDIT;
      menuHorizontalPosition = POS_HORZ_INIT(menuVerticalPosition);
      break;

    case EVT_ENTRY_UP:
      s_editMode = 0;
      menuHorizontalPosition = POS_HORZ_INIT(menuVerticalPosition);
      break;

    case EVT_ROTARY_BREAK:
      if (s_editMode > 1) break;
      if (menuHorizontalPosition < 0 && maxcol > 0 && READ_ONLY_UNLOCKED()) {
        menuHorizontalPosition = 0;
        AUDIO_KEY_PRESS();
      }
      else if (READ_ONLY_UNLOCKED() && rowcount > 0) {
        s_editMode = (s_editMode<=0);
        AUDIO_KEY_PRESS();
      }
      break;

    case EVT_KEY_FIRST(KEY_EXIT):
      if (s_editMode > 0) {
        s_editMode = 0;
        AUDIO_KEY_PRESS();
        break;
      }

      if (s_copyMode > 0) {
        break;
      }

      if (menuHorizontalPosition >= 0 && (COLATTR(menuVerticalPosition) & NAVIGATION_LINE_BY_LINE)) {
        menuHorizontalPosition = -1;
        AUDIO_KEY_PRESS();
      }
      else {
        uint8_t posVertInit = MENU_FIRST_LINE_EDIT;
        if (menuVerticalOffset != 0 || menuVerticalPosition != posVertInit) {
          menuVerticalOffset = 0;
          menuVerticalPosition = posVertInit;
          menuHorizontalPosition = POS_HORZ_INIT(menuVerticalPosition);
          AUDIO_KEY_PRESS();
        }
        else if (!menuCalibrationState) {
          popMenu();
        }
      }
      break;

    case EVT_KEY_FIRST(KEY_RIGHT):
      AUDIO_KEY_PRESS();
      // no break
    case EVT_KEY_REPT(KEY_RIGHT):
      if (s_editMode == 0) {
        INC(menuHorizontalPosition, 0, maxcol);
      }
      break;

    case EVT_KEY_FIRST(KEY_LEFT):
      AUDIO_KEY_PRESS();
      // no break
    case EVT_KEY_REPT(KEY_LEFT):
      if (s_editMode == 0) {
        DEC(menuHorizontalPosition, 0, maxcol);
      }
      break;

    case EVT_ROTARY_RIGHT:
      if (s_editMode != 0) break;
      AUDIO_KEY_PRESS();
      if ((COLATTR(menuVerticalPosition) & NAVIGATION_LINE_BY_LINE)) {
        if (menuHorizontalPosition >= 0) {
          INC(menuHorizontalPosition, 0, maxcol);
          break;
        }
      }
      else if (menuHorizontalPosition < maxcol) {
        menuHorizontalPosition++;
        break;
      }
      do {
        INC(menuVerticalPosition, MENU_FIRST_LINE_EDIT, rowcount-1);
      } while (CURSOR_NOT_ALLOWED_IN_ROW(menuVerticalPosition));
      menuHorizontalPosition = POS_HORZ_INIT(menuVerticalPosition);
      break;

    case EVT_ROTARY_LEFT:
      if (s_editMode != 0) break;
      AUDIO_KEY_PRESS();
      if ((COLATTR(menuVerticalPosition) & NAVIGATION_LINE_BY_LINE)) {
        if (menuHorizontalPosition >= 0) {
          DEC(menuHorizontalPosition, 0, maxcol);
          break;
        }
      }
      else if (menuHorizontalPosition > 0) {
        menuHorizontalPosition--;
        break;
      }
      do {
        DEC(menuVerticalPosition, MENU_FIRST_LINE_EDIT, rowcount-1);
      } while (CURSOR_NOT_ALLOWED_IN_ROW(menuVerticalPosition));
      if (COLATTR(menuVerticalPosition) & NAVIGATION_LINE_BY_LINE)
        menuHorizontalPosition = -1;
      else
        menuHorizontalPosition = MAXCOL(menuVerticalPosition);
      break;
  }

  linesCount = rowcount;

  if (menuVerticalPosition <= MENU_FIRST_LINE_EDIT) {
    menuVerticalOffset = 0;
    if (horTab) {
      linesCount = 0;
      for (int i=0; i<rowcount; i++) {
        if (HORTAB(i) != HIDDEN_ROW) {
          linesCount++;
        }
      }
    }
  }
  else if (horTab) {
    if (rowcount > linesDisplayed) {
      while (1) {
        int firstLine = 0;
        for (int numLines=0; firstLine<rowcount && numLines<menuVerticalOffset; firstLine++) {
          if (HORTAB(firstLine) != HIDDEN_ROW) {
            numLines++;
          }
        }
        if (menuVerticalPosition < firstLine) {
          menuVerticalOffset--;
        }
        else {
          int lastLine = firstLine;
          for (int numLines=0; lastLine<rowcount && numLines<linesDisplayed; lastLine++) {
            if (HORTAB(lastLine) != HIDDEN_ROW) {
              numLines++;
            }
          }
          if (menuVerticalPosition >= lastLine || HORTAB(firstLine) == ORPHAN_ROW) {
            menuVerticalOffset++;
          }
          else {
            linesCount = menuVerticalOffset + linesDisplayed;
            for (int i=lastLine; i<rowcount; i++) {
              if (HORTAB(i) != HIDDEN_ROW) {
                linesCount++;
              }
            }
            break;
          }
        }
      }
    }
  }
  else {
    if (menuVerticalPosition >= linesDisplayed + menuVerticalOffset) {
      menuVerticalOffset = menuVerticalPosition-linesDisplayed+1;
    }
    else if (menuVerticalPosition < menuVerticalOffset) {
      menuVerticalOffset = menuVerticalPosition;
    }
  }

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

void repeatLastCursorMove(event_t event)
{
  if (CURSOR_MOVED_LEFT(event) || CURSOR_MOVED_RIGHT(event)) {
    putEvent(event);
  }
  else {
    menuHorizontalPosition = 0;
  }
}
