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

int menuVerticalOffset;
int menuVerticalPosition;
int menuHorizontalPosition;
int8_t s_editMode;
uint8_t noHighlightCounter;
uint8_t calibrationState; // TODO rename this variable
int checkIncDecSelection = 0;

#if defined(AUTOSWITCH)
swsrc_t checkIncDecMovedSwitch(swsrc_t val)
{
  if (s_editMode>0) {
    swsrc_t swtch = getMovedSwitch();
    if (swtch) {
      div_t info = switchInfo(swtch);
      if (IS_TOGGLE(info.quot)) {
        if (info.rem != 0) {
          val = (val == swtch ? swtch-2 : swtch);
        }
      }
      else {
        val = swtch;
      }
    }
  }
  return val;
}
#endif

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
    for (int i = 0; i < MAX_SENSORS; i++) {
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
    checkIncDecSelection = SWSRC_FIRST_LOGICAL_SWITCH + getFirstAvailable(0, NUM_LOGICAL_SWITCH, isLogicalSwitchAvailable);
  else if (result == STR_MENU_OTHER)
    checkIncDecSelection = SWSRC_ON;
  else if (result == STR_MENU_INVERT)
    checkIncDecSelection = SWSRC_INVERT;
}

int checkIncDec(evt_t event, int val, int i_min, int i_max, unsigned int i_flags, IsValueAvailable isValueAvailable, const CheckIncDecStops &stops)
{
  int newval = val;

#if defined(DBLKEYS)
  uint32_t in = KEYS_PRESSED();
  if (!(i_flags & NO_DBLKEYS) && (EVT_KEY_MASK(event))) {
    bool dblkey = true;
    if (DBLKEYS_PRESSED_RGT_LFT(in)) {
      if (!isValueAvailable || isValueAvailable(-val))
        newval = -val;
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
      killEvents(KEY_MENU);
      killEvents(KEY_ENTER);
      killEvents(KEY_EXIT);
      event = 0;
    }
  }
#endif

  if (s_editMode>0 && event==EVT_ROTARY_RIGHT) {
    do {
      if (IS_KEY_REPT(event) && (i_flags & INCDEC_REP10)) {
        newval += min(10, i_max-val);
      }
      else {
        newval++;
      }
    } while (isValueAvailable && !isValueAvailable(newval) && newval<=i_max);

    if (newval > i_max) {
      newval = val;
      killEvents(event);
      AUDIO_WARNING2();
    }
    else {
      AUDIO_KEYPAD_UP();
    }
  }
  else if (s_editMode>0 && event==EVT_ROTARY_LEFT) {
    do {
      if (IS_KEY_REPT(event) && (i_flags & INCDEC_REP10)) {
        newval -= min(10, val-i_min);
      }
      else {
        newval--;
      }
    } while (isValueAvailable && !isValueAvailable(newval) && newval>=i_min);

    if (newval < i_min) {
      newval = val;
      killEvents(event);
      AUDIO_WARNING2();
    }
    else {
      AUDIO_KEYPAD_DOWN();
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

  if (newval > i_max || newval < i_min) {
    newval = (newval > i_max ? i_max : i_min);
    killEvents(event);
    AUDIO_WARNING2();
  }

  if (newval != val) {
#if 0
    if (!(i_flags & NO_INCDEC_MARKS) && (newval != i_max) && (newval != i_min) && stops.contains(newval) && !IS_ROTARY_EVENT(event)) {
      bool pause = (newval > val ? !stops.contains(newval+1) : !stops.contains(newval-1));
      if (pause) {
        pauseEvents(event); // delay before auto-repeat continues
        if (newval>val) // without AUDIO it's optimized, because the 2 sounds are the same
          AUDIO_KEYPAD_UP();
        else
          AUDIO_KEYPAD_DOWN();
      }
    }
#endif
    storageDirty(i_flags & (EE_GENERAL|EE_MODEL));
    checkIncDec_Ret = (newval > val ? 1 : -1);
  }
  else {
    checkIncDec_Ret = 0;
  }

  if (i_flags & INCDEC_SOURCE) {
    if (event == EVT_KEY_LONG(KEY_ENTER)) {
      killEvents(event);
      checkIncDecSelection = MIXSRC_NONE;

      TRACE("count items avant = %d", popupMenuNoItems);

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
        for (int i = 0; i < MAX_SENSORS; i++) {
          TelemetrySensor * sensor = & g_model.telemetrySensors[i];
          if (sensor->isAvailable()) {
            POPUP_MENU_ADD_ITEM(STR_MENU_TELEMETRY);
            break;
          }
        }
      }
      popupMenuHandler = onSourceLongEnterPress;
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
        for (int i = 0; i < NUM_LOGICAL_SWITCH; i++) {
          if (isValueAvailable && isValueAvailable(SWSRC_FIRST_LOGICAL_SWITCH+i)) {
            POPUP_MENU_ADD_ITEM(STR_MENU_LOGICAL_SWITCHES);
            break;
          }
        }
      }
      if (isValueAvailable && isValueAvailable(SWSRC_ON))                  POPUP_MENU_ADD_ITEM(STR_MENU_OTHER);
      if (isValueAvailable && isValueAvailable(-newval))                   POPUP_MENU_ADD_ITEM(STR_MENU_INVERT);
      popupMenuHandler = onSwitchLongEnterPress;
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
#define MAXCOL_RAW(row)                ((row) >= 0 && horTab ? pgm_read_byte(horTab+min<int>(row, (int)horTabMax)) : (const uint8_t)0)
#define MAXCOL(row)                    (MAXCOL_RAW(row) >= HIDDEN_ROW ? MAXCOL_RAW(row) : (const uint8_t)(MAXCOL_RAW(row) & (~NAVIGATION_LINE_BY_LINE)))
#define COLATTR(row)                   (MAXCOL_RAW(row) == (uint8_t)-1 ? (const uint8_t)0 : (const uint8_t)(MAXCOL_RAW(row) & NAVIGATION_LINE_BY_LINE))
#define INC(val, min, max)             if (val<max) {val++;} else if (max>=0) {val=min;}
#define DEC(val, min, max)             if (val>min) {val--;} else {val=max;}

uint8_t menuPageIndex;
uint8_t menuPageCount;
uint16_t linesCount;

bool navigate(evt_t event, int count, int rows, int columns)
{
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
    case EVT_KEY_FIRST(KEY_RIGHT):
    case EVT_KEY_REPT(KEY_RIGHT):
      INC(menuHorizontalPosition, 0, maxcol);
      break;

    case EVT_KEY_FIRST(KEY_LEFT):
    case EVT_KEY_REPT(KEY_LEFT):
      DEC(menuHorizontalPosition, 0, maxcol);
      break;

    case EVT_KEY_FIRST(KEY_DOWN):
    case EVT_KEY_REPT(KEY_DOWN):
      INC(menuVerticalPosition, 0, maxrow);
      if (menuVerticalPosition == maxrow && menuHorizontalPosition > maxlastcol)  {
        menuHorizontalPosition = maxlastcol;
      }
      break;

    case EVT_KEY_FIRST(KEY_UP):
    case EVT_KEY_REPT(KEY_UP):
      DEC(menuVerticalPosition, 0, maxrow);
      if (menuVerticalPosition == maxrow && menuHorizontalPosition > maxlastcol)  {
        menuHorizontalPosition = maxlastcol;
      }
      break;

    case EVT_ROTARY_LEFT:
      if (menuHorizontalPosition > 0) {
        menuHorizontalPosition--;
      }
      else {
        DEC(menuVerticalPosition, 0, maxrow);
        menuHorizontalPosition = (menuVerticalPosition != maxrow ? columns-1 : maxlastcol);
      }
      break;

    case EVT_ROTARY_RIGHT:
      if (menuHorizontalPosition < maxcol) {
        menuHorizontalPosition++;
      }
      else {
        INC(menuVerticalPosition, 0, maxrow);
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

  return (prevPosHorz != menuHorizontalPosition || prevPosVert != menuVerticalPosition);
}

bool check(check_event_t event, uint8_t curr, const MenuHandlerFunc * menuTab, uint8_t menuTabSize, const pm_uint8_t * horTab, uint8_t horTabMax, int rowcount, uint8_t flags)
{
  uint8_t maxcol = MAXCOL(menuVerticalPosition);

  if (menuTab && !calibrationState && menuVerticalPosition<0) {
    int cc = curr;
    switch (event) {
      case EVT_KEY_BREAK(KEY_RIGHT):
      case EVT_ROTARY_RIGHT:
        if (++cc == menuTabSize)
          cc = 0;
        break;

      case EVT_KEY_BREAK(KEY_LEFT):
      case EVT_ROTARY_LEFT:
        if (cc-- == 0)
          cc = menuTabSize-1;
        break;

      case EVT_KEY_BREAK(KEY_ENTER):
        if (rowcount > 0) {
          menuVerticalPosition = MENU_FIRST_LINE_EDIT;
          event = 0;
        }
        break;

      case EVT_KEY_BREAK(KEY_DOWN):
      case EVT_KEY_BREAK(KEY_UP):
        menuHorizontalPosition = -1;
        break;
    }

    if (cc != curr) {
      chainMenu((MenuHandlerFunc)pgm_read_adr(&menuTab[cc]));
      return false;
    }

    menuPageIndex = curr;
    menuPageCount = menuTabSize;
  }

  switch(event) {
    case EVT_ENTRY:
      menuVerticalPosition = (menuTab ? -1 : MENU_FIRST_LINE_EDIT);
      menuHorizontalPosition = POS_HORZ_INIT(0);
      s_editMode = EDIT_MODE_INIT;
      break;

    case EVT_ENTRY_UP:
      s_editMode = 0;
      menuHorizontalPosition = POS_HORZ_INIT(menuVerticalPosition);
      break;

    case EVT_ROTARY_BREAK:
      if (s_editMode > 1) break;
      if (menuHorizontalPosition < 0 && maxcol > 0 && READ_ONLY_UNLOCKED()) {
        menuHorizontalPosition = 0;
      }
      else if (READ_ONLY_UNLOCKED()) {
        s_editMode = (s_editMode<=0);
      }
      break;

    case EVT_KEY_LONG(KEY_EXIT):
      s_editMode = 0; // TODO needed? we call ENTRY_UP after which does the same
      popMenu();
      break;

    case EVT_KEY_BREAK(KEY_EXIT):
      if (s_editMode > 0) {
        s_editMode = 0;
        break;
      }

      if (s_copyMode > 0) {
        break;
      }

      if (menuHorizontalPosition >= 0 && (COLATTR(menuVerticalPosition) & NAVIGATION_LINE_BY_LINE)) {
        menuHorizontalPosition = -1;
      }
      else if (menuTab && menuVerticalPosition >= 0) {
        menuVerticalPosition = -1;
        menuHorizontalPosition = 0;
#if 0
        int posVertInit = -1;
        if (menuVerticalOffset != 0 || menuVerticalPosition != posVertInit) {
          menuVerticalOffset = 0;
          menuVerticalPosition = posVertInit;
          menuHorizontalPosition = POS_HORZ_INIT(menuVerticalPosition);
        }
#endif
      }
      else if (!calibrationState) {
        popMenu();
      }
      break;

    case EVT_KEY_FIRST(KEY_RIGHT):
    case EVT_KEY_REPT(KEY_RIGHT):
      if (s_editMode == 0) {
        INC(menuHorizontalPosition, 0, maxcol);
      }
      break;

    case EVT_KEY_FIRST(KEY_LEFT):
    case EVT_KEY_REPT(KEY_LEFT):
      if (s_editMode == 0) {
        DEC(menuHorizontalPosition, 0, maxcol);
      }
      break;

    case EVT_ROTARY_RIGHT:
      if (s_editMode != 0) break;
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

    case EVT_KEY_FIRST(KEY_DOWN):
    case EVT_KEY_REPT(KEY_DOWN):
    {
      do {
        INC(menuVerticalPosition, MENU_FIRST_LINE_EDIT, rowcount-1);
      } while (CURSOR_NOT_ALLOWED_IN_ROW(menuVerticalPosition));

      s_editMode = 0; // if we go down, we must be in this mode
      uint8_t newmaxcol = MAXCOL(menuVerticalPosition);
      if (COLATTR(menuVerticalPosition) & NAVIGATION_LINE_BY_LINE) {
        menuHorizontalPosition = -1;
      }
      else if (menuHorizontalPosition < 0 || menuHorizontalPosition > newmaxcol) {
        menuHorizontalPosition = POS_HORZ_INIT(menuVerticalPosition);
      }
      break;
    }

    case EVT_ROTARY_LEFT:
      if (s_editMode != 0) break;
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

    case EVT_KEY_FIRST(KEY_UP):
    case EVT_KEY_REPT(KEY_UP):
    {
      do {
        DEC(menuVerticalPosition, MENU_FIRST_LINE_EDIT, rowcount-1);
      } while (CURSOR_NOT_ALLOWED_IN_ROW(menuVerticalPosition));
      s_editMode = 0; // if we go up, we must be in this mode
      uint8_t newmaxcol = MAXCOL(menuVerticalPosition);
      if ((COLATTR(menuVerticalPosition) & NAVIGATION_LINE_BY_LINE)) {
        menuHorizontalPosition = -1;
      }
      else if (menuHorizontalPosition < 0) {
        menuHorizontalPosition = POS_HORZ_INIT(menuVerticalPosition);
      }
      else if (menuHorizontalPosition > newmaxcol) {
        menuHorizontalPosition = min((uint8_t)menuHorizontalPosition, newmaxcol);
      }
      break;
    }
  }

  linesCount = rowcount;

  if (menuVerticalPosition <= MENU_FIRST_LINE_EDIT) {
    menuVerticalOffset = 0;
    if (horTab) {
      linesCount = 0;
      for (int i=0; i<rowcount; i++) {
        if (i>horTabMax || horTab[i] != HIDDEN_ROW) {
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
          if (firstLine>=horTabMax || horTab[firstLine] != HIDDEN_ROW) {
            numLines++;
          }
        }
        if (menuVerticalPosition < firstLine) {
          menuVerticalOffset--;
        }
        else {
          int lastLine = firstLine;
          for (int numLines=0; lastLine<rowcount && numLines<linesDisplayed; lastLine++) {
            if (lastLine >= horTabMax || horTab[lastLine] != HIDDEN_ROW) {
              numLines++;
            }
          }
          if (menuVerticalPosition >= lastLine || horTab[firstLine] == ORPHAN_ROW) {
            menuVerticalOffset++;
          }
          else {
            linesCount = menuVerticalOffset + linesDisplayed;
            for (int i=lastLine; i<rowcount; i++) {
              if (i > horTabMax || horTab[i] != HIDDEN_ROW) {
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

bool check_simple(check_event_t event, uint8_t curr, const MenuHandlerFunc *menuTab, uint8_t menuTabSize, int rowcount)
{
  return check(event, curr, menuTab, menuTabSize, NULL, 0, rowcount);
}

bool check_submenu_simple(check_event_t event, uint8_t rowcount)
{
  return check_simple(event, 0, NULL, 0, rowcount);
}

void repeatLastCursorMove(evt_t event)
{
  if (CURSOR_MOVED_LEFT(event) || CURSOR_MOVED_RIGHT(event)) {
    putEvent(event);
  }
  else {
    menuHorizontalPosition = 0;
  }
}
