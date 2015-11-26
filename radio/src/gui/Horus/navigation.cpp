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

vertpos_t menuVerticalOffset;
vertpos_t menuVerticalPosition;
horzpos_t menuHorizontalPosition;
int8_t s_editMode;
uint8_t noHighlightCounter;
uint8_t calibrationState; // TODO rename this variable

#if defined(AUTOSWITCH)
int8_t checkIncDecMovedSwitch(int8_t val)
{
  if (s_editMode>0) {
    int8_t swtch = getMovedSwitch();
    if (swtch) {
      div_t info = switchInfo(swtch);
      if (IS_TOGGLE(info.quot)) {
        if (info.rem != 0) {
          val = (val == swtch ? swtch+2 : swtch);
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
  return newval;
}

#define CURSOR_NOT_ALLOWED_IN_ROW(row) ((int8_t)MAXCOL(row) < 0)
#define MAXCOL_RAW(row)                (horTab ? pgm_read_byte(horTab+min<int>(row, (vertpos_t)horTabMax)) : (const uint8_t)0)
#define MAXCOL(row)                    (MAXCOL_RAW(row) >= HIDDEN_ROW ? MAXCOL_RAW(row) : (const uint8_t)(MAXCOL_RAW(row) & (~NAVIGATION_LINE_BY_LINE)))
#define COLATTR(row)                   (MAXCOL_RAW(row) == (uint8_t)-1 ? (const uint8_t)0 : (const uint8_t)(MAXCOL_RAW(row) & NAVIGATION_LINE_BY_LINE))
#define INC(val, min, max)             if (val<max) {val++;} else {val=min;}
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

bool check(check_event_t event, uint8_t curr, const MenuHandlerFunc * menuTab, uint8_t menuTabSize, const pm_uint8_t * horTab, uint8_t horTabMax, vertpos_t rowcount, uint8_t flags)
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

  switch(event)
  {
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
      else {
        popMenu();
      }
      break;

    case EVT_KEY_FIRST(KEY_RIGHT):
    case EVT_KEY_REPT(KEY_RIGHT):
      if (s_editMode != 0) break;
      INC(menuHorizontalPosition, 0, maxcol);
      break;

    case EVT_KEY_FIRST(KEY_LEFT):
    case EVT_KEY_REPT(KEY_LEFT):
      if (s_editMode != 0) break;
      DEC(menuHorizontalPosition, 0, maxcol);
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
        menuHorizontalPosition = min((uint8_t)menuHorizontalPosition, MAXCOL(menuVerticalPosition));
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
  int maxBodyLines =  (menuTab ? NUM_BODY_LINES : NUM_BODY_LINES+1);

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
    if (rowcount > maxBodyLines) {
      while (1) {
        vertpos_t firstLine = 0;
        for (int numLines=0; firstLine<rowcount && numLines<menuVerticalOffset; firstLine++) {
          if (firstLine>=horTabMax || horTab[firstLine] != HIDDEN_ROW) {
            numLines++;
          }
        }
        if (menuVerticalPosition < firstLine) {
          menuVerticalOffset--;
        }
        else {
          vertpos_t lastLine = firstLine;
          for (int numLines=0; lastLine<rowcount && numLines<maxBodyLines; lastLine++) {
            if (lastLine >= horTabMax || horTab[lastLine] != HIDDEN_ROW) {
              numLines++;
            }
          }
          if (menuVerticalPosition >= lastLine) {
            menuVerticalOffset++;
          }
          else {
            linesCount = menuVerticalOffset + maxBodyLines;
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
    if (menuVerticalPosition >= maxBodyLines + menuVerticalOffset) {
      menuVerticalOffset = menuVerticalPosition-maxBodyLines+1;
    }
    else if (menuVerticalPosition < menuVerticalOffset) {
      menuVerticalOffset = menuVerticalPosition;
    }
  }

  return true;
}

bool check_simple(check_event_t event, uint8_t curr, const MenuHandlerFunc *menuTab, uint8_t menuTabSize, vertpos_t rowcount)
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
