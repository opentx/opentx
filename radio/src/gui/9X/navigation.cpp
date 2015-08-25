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

vertpos_t s_pgOfs;
int8_t s_editMode;
uint8_t s_noHi;
uint8_t calibrationState;
vertpos_t m_posVert;
horzpos_t m_posHorz;

#if defined(NAVIGATION_POT1)
int16_t p1valdiff;
#endif

#if defined(NAVIGATION_POT2)
int8_t p2valdiff;
#endif

#if defined(AUTOSWITCH)
int8_t checkIncDecMovedSwitch(int8_t val)
{
  if (s_editMode>0) {
    int8_t swtch = getMovedSwitch();
    if (swtch) {
      if (IS_MOMENTARY(val) && swtch==val)
        val = -val;
      else
        val = swtch;
    }
  }
  return val;
}
#endif

int8_t  checkIncDec_Ret;

#define DBLKEYS_PRESSED_RGT_LFT(in) ((in & (KEYS_GPIO_PIN_RIGHT + KEYS_GPIO_PIN_LEFT)) == (KEYS_GPIO_PIN_RIGHT + KEYS_GPIO_PIN_LEFT))
#define DBLKEYS_PRESSED_UP_DWN(in)  ((in & (KEYS_GPIO_PIN_UP + KEYS_GPIO_PIN_DOWN)) == (KEYS_GPIO_PIN_UP + KEYS_GPIO_PIN_DOWN))
#define DBLKEYS_PRESSED_RGT_UP(in)  ((in & (KEYS_GPIO_PIN_RIGHT + KEYS_GPIO_PIN_UP))  == (KEYS_GPIO_PIN_RIGHT + KEYS_GPIO_PIN_UP))
#define DBLKEYS_PRESSED_LFT_DWN(in) ((in & (KEYS_GPIO_PIN_LEFT + KEYS_GPIO_PIN_DOWN)) == (KEYS_GPIO_PIN_LEFT + KEYS_GPIO_PIN_DOWN))

#if defined(CPUARM)
INIT_STOPS(stops100, 3, -100, 0, 100)
INIT_STOPS(stops1000, 3, -1000, 0, 1000)
INIT_STOPS(stopsSwitch, 15, SWSRC_FIRST, CATEGORY_END(-SWSRC_FIRST_LOGICAL_SWITCH), CATEGORY_END(-SWSRC_FIRST_TRIM), CATEGORY_END(-SWSRC_LAST_SWITCH+1), 0, CATEGORY_END(SWSRC_LAST_SWITCH), CATEGORY_END(SWSRC_FIRST_TRIM-1), CATEGORY_END(SWSRC_FIRST_LOGICAL_SWITCH-1), SWSRC_LAST)

int checkIncDec(unsigned int event, int val, int i_min, int i_max, unsigned int i_flags, IsValueAvailable isValueAvailable, const CheckIncDecStops &stops)
{
  int newval = val;

#if defined(DBLKEYS)
  uint8_t in = KEYS_PRESSED();
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
      event = 0;
    }
  }
#endif

  if (event==EVT_KEY_FIRST(KEY_RIGHT) || event==EVT_KEY_REPT(KEY_RIGHT) || (s_editMode>0 && (IS_ROTARY_RIGHT(event) || event==EVT_KEY_FIRST(KEY_UP) || event==EVT_KEY_REPT(KEY_UP)))) {
	do {
	  newval++;
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
  else if (event==EVT_KEY_FIRST(KEY_LEFT) || event==EVT_KEY_REPT(KEY_LEFT) || (s_editMode>0 && (IS_ROTARY_LEFT(event) || event==EVT_KEY_FIRST(KEY_DOWN) || event==EVT_KEY_REPT(KEY_DOWN)))) {
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

  if (!READ_ONLY() && i_min==0 && i_max==1 && (event==EVT_KEY_BREAK(KEY_ENTER) || IS_ROTARY_BREAK(event))) {
    s_editMode = 0;
    newval = !val;
  }

#if defined(NAVIGATION_POT1)
  // change values based on P1
  newval -= p1valdiff;
  p1valdiff = 0;
#endif

#if defined(AUTOSWITCH)
  if (i_flags & INCDEC_SWITCH) {
    newval = checkIncDecMovedSwitch(newval);
  }
#endif

#if defined(AUTOSOURCE)
  if (i_flags & INCDEC_SOURCE) {
    if (s_editMode>0) {
      int8_t source = GET_MOVED_SOURCE(i_min, i_max);
      if (source) {
        newval = source;
      }
#if defined(AUTOSWITCH)
      else {
        uint8_t swtch = abs(getMovedSwitch());
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
    if (!(i_flags & NO_INCDEC_MARKS) && (newval != i_max) && (newval != i_min) && (newval==0 || newval==-100 || newval==+100) && !IS_ROTARY_EVENT(event)) {
      pauseEvents(event); // delay before auto-repeat continues
      if (newval>val) // without AUDIO it's optimized, because the 2 sounds are the same
        AUDIO_KEYPAD_UP();
      else
        AUDIO_KEYPAD_DOWN();
    }
    eeDirty(i_flags & (EE_GENERAL|EE_MODEL));
    checkIncDec_Ret = (newval > val ? 1 : -1);
  }
  else {
    checkIncDec_Ret = 0;
  }
  return newval;
}
#else
int16_t checkIncDec(uint8_t event, int16_t val, int16_t i_min, int16_t i_max, uint8_t i_flags)
{
  int16_t newval = val;

#if defined(DBLKEYS)
  uint8_t in = KEYS_PRESSED();
  if (!(i_flags & NO_DBLKEYS) && (EVT_KEY_MASK(event))) {
    bool dblkey = true;
    if (DBLKEYS_PRESSED_RGT_LFT(in))
      newval = -val;
    else if (DBLKEYS_PRESSED_RGT_UP(in)) {
      newval = (i_max > 100 ? 100 : i_max);
    }
    else if (DBLKEYS_PRESSED_LFT_DWN(in)) {
      newval = (i_min < -100 ? -100 : i_min);
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
      event = 0;
    }
  }
#endif

  if (event==EVT_KEY_FIRST(KEY_RIGHT) || event==EVT_KEY_REPT(KEY_RIGHT) || (s_editMode>0 && (IS_ROTARY_RIGHT(event) || event==EVT_KEY_FIRST(KEY_UP) || event==EVT_KEY_REPT(KEY_UP)))) {
    newval++;
    AUDIO_KEYPAD_UP();
  }
  else if (event==EVT_KEY_FIRST(KEY_LEFT) || event==EVT_KEY_REPT(KEY_LEFT) || (s_editMode>0 && (IS_ROTARY_LEFT(event) || event==EVT_KEY_FIRST(KEY_DOWN) || event==EVT_KEY_REPT(KEY_DOWN)))) {
    newval--;
    AUDIO_KEYPAD_DOWN();
  }

  if (!READ_ONLY() && i_min==0 && i_max==1 && (event==EVT_KEY_BREAK(KEY_ENTER) || IS_ROTARY_BREAK(event))) {
    s_editMode = 0;
    newval = !val;
  }

#if defined(NAVIGATION_POT1)
  // change values based on P1
  newval -= p1valdiff;
  p1valdiff = 0;
#endif

#if defined(AUTOSWITCH)
  if (i_flags & INCDEC_SWITCH) {
    newval = checkIncDecMovedSwitch(newval);
  }
#endif

#if defined(AUTOSOURCE)
  if (i_flags & INCDEC_SOURCE) {
    if (s_editMode>0) {
      int8_t source = GET_MOVED_SOURCE(i_min, i_max);
      if (source) {
        newval = source;
      }
#if defined(AUTOSWITCH)
      else {
        uint8_t swtch = abs(getMovedSwitch());
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
    if (!(i_flags & NO_INCDEC_MARKS) && (newval != i_max) && (newval != i_min) && (newval==0 || newval==-100 || newval==+100) && !IS_ROTARY_EVENT(event)) {
      pauseEvents(event); // delay before auto-repeat continues
      if (newval>val) // without AUDIO it's optimized, because the 2 sounds are the same
        AUDIO_KEYPAD_UP();
      else
        AUDIO_KEYPAD_DOWN();
    }
    eeDirty(i_flags & (EE_GENERAL|EE_MODEL));
    checkIncDec_Ret = (newval > val ? 1 : -1);
  }
  else {
    checkIncDec_Ret = 0;
  }
  return newval;
}
#endif

#if defined(CPUM64)
int8_t checkIncDecModel(uint8_t event, int8_t i_val, int8_t i_min, int8_t i_max)
{
  return checkIncDec(event, i_val, i_min, i_max, EE_MODEL);
}

int8_t checkIncDecModelZero(uint8_t event, int8_t i_val, int8_t i_max)
{
  return checkIncDecModel(event, i_val, 0, i_max);
}

int8_t checkIncDecGen(uint8_t event, int8_t i_val, int8_t i_min, int8_t i_max)
{
  return checkIncDec(event, i_val, i_min, i_max, EE_GENERAL);
}
#endif

#define SCROLL_TH      64
#define SCROLL_POT1_TH 32

#if defined(CPUARM)
  #define CURSOR_NOT_ALLOWED_IN_ROW(row)   ((int8_t)MAXCOL(row) < 0)
#else
  #define CURSOR_NOT_ALLOWED_IN_ROW(row)   (MAXCOL(row) == TITLE_ROW)
#endif

#define MAXCOL(row)     (horTab ? pgm_read_byte(horTab+min(row, (vertpos_t)horTabMax)) : (const uint8_t)0)
#define INC(val, min, max) if (val<max) {val++;} else {val=min;}
#define DEC(val, min, max) if (val>min) {val--;} else {val=max;}

#if defined(CPUARM)
tmr10ms_t menuEntryTime;
#endif

void check(check_event_t event, uint8_t curr, const MenuFuncP *menuTab, uint8_t menuTabSize, const pm_uint8_t *horTab, uint8_t horTabMax, vertpos_t maxrow)
{
  vertpos_t l_posVert = m_posVert;
  horzpos_t l_posHorz = m_posHorz;

  uint8_t maxcol = MAXCOL(l_posVert);

#if defined(NAVIGATION_POT1)
  // check pot 1 - if changed -> scroll values
  static int16_t p1val;
  static int16_t p1valprev;
  p1valdiff = (p1val-calibratedStick[6]) / SCROLL_POT1_TH;
  if (p1valdiff) {
    p1valdiff = (p1valprev-calibratedStick[6]) / 2;
    p1val = calibratedStick[6];
  }
  p1valprev = calibratedStick[6];
#endif

#if defined(NAVIGATION_POT2)
  // check pot 2 - if changed -> scroll menu
  static int16_t p2valprev;
  p2valdiff = (p2valprev-calibratedStick[4]) / SCROLL_TH;
  if (p2valdiff) p2valprev = calibratedStick[4];
#endif

#if defined(NAVIGATION_POT3)
  // check pot 3 if changed -> cursor down/up
  static int16_t p3valprev;
  int8_t scrollUD = (p3valprev-calibratedStick[5]) / SCROLL_TH;
  if (scrollUD) p3valprev = calibratedStick[5];
#else
  #define scrollUD 0
#endif

  if (p2valdiff || scrollUD || p1valdiff) backlightOn(); // on keypress turn the light on

  if (menuTab) {
    uint8_t attr = 0;


    if (l_posVert==0 && !calibrationState) {
      attr = INVERS;

      int8_t cc = curr;

      if (p2valdiff) {
        cc = limit((int8_t)0, (int8_t)(cc - p2valdiff), (int8_t)(menuTabSize-1));
      }

      switch(event) {
#if defined(ROTARY_ENCODER_NAVIGATION)
        case EVT_ROTARY_BREAK:
          if (s_editMode < 0 && maxrow > 0) {
            s_editMode = 0;
            // TODO ? l_posVert = (horTab && horTab[1]==0xff) ? 2 : 1;
            l_posHorz = 0;
          }
          else {
            s_editMode = -1;
          }
          event = 0;
          break;
#endif

#if defined(ROTARY_ENCODER_NAVIGATION)
        case EVT_ROTARY_LEFT:
          if (s_editMode >= 0)
            break;
#endif
        case EVT_KEY_FIRST(KEY_LEFT):
          if (curr > 0)
            cc = curr - 1;
          else
            cc = menuTabSize-1;
          break;

#if defined(ROTARY_ENCODER_NAVIGATION)
        case EVT_ROTARY_RIGHT:
          if (s_editMode >= 0)
            break;
#endif
        case EVT_KEY_FIRST(KEY_RIGHT):
          if (curr < (menuTabSize-1))
            cc = curr + 1;
          else
            cc = 0;
          break;
      }

      if (cc != curr) {
        chainMenu((MenuFuncP)pgm_read_adr(&menuTab[cc]));
      }

#if defined(ROTARY_ENCODER_NAVIGATION)
      if (IS_RE_NAVIGATION_ENABLE() && s_editMode < 0)
        attr = INVERS|BLINK;
#endif
    }

    calibrationState = 0;
    displayScreenIndex(curr, menuTabSize, attr);

  }

  DISPLAY_PROGRESS_BAR(menuTab ? lcdLastPos-2*FW-((curr+1)/10*FWNUM)-2 : 20*FW+1);

  if (s_editMode<=0) {
    if (scrollUD) {
      l_posVert = limit((int8_t)0, (int8_t)(l_posVert - scrollUD), (int8_t)maxrow);
      l_posHorz = min((uint8_t)l_posHorz, MAXCOL(l_posVert));
    }

    if (p2valdiff && l_posVert>0) {
      l_posHorz = limit((int8_t)0, (int8_t)((uint8_t)l_posHorz - p2valdiff), (int8_t)maxcol);
    }
  }

  switch(event)
  {
    case EVT_ENTRY:
#if defined(CPUARM)
      menuEntryTime = get_tmr10ms();
#endif
      l_posVert = POS_VERT_INIT;
      l_posHorz = POS_HORZ_INIT(l_posVert);
      SET_SCROLLBAR_X(LCD_W-1);
#if defined(ROTARY_ENCODER_NAVIGATION)
      if (menuTab) {
        s_editMode = EDIT_MODE_INIT;
        break;
      }
      // no break
#else
      s_editMode = EDIT_MODE_INIT;
      break;
#endif

#if defined(ROTARY_ENCODER_NAVIGATION)
    case EVT_ENTRY_UP:
      s_editMode = 0;
      SET_SCROLLBAR_X(LCD_W-1);
      break;

    case EVT_ROTARY_BREAK:
      if (s_editMode > 1) break;
#endif

    case EVT_KEY_FIRST(KEY_ENTER):
      if (!menuTab || l_posVert>0) {
        if (READ_ONLY_UNLOCKED()) {
          s_editMode = (s_editMode<=0);
        }
      }
      break;

#if defined(ROTARY_ENCODER_NAVIGATION)
    case EVT_ROTARY_LONG:
      if (s_editMode > 1) break;
      killEvents(event);
      if (l_posVert != POS_VERT_INIT) {
        l_posVert = POS_VERT_INIT;
        s_editMode = EDIT_MODE_INIT;
        break;
      }
      // no break
#endif
    case EVT_KEY_LONG(KEY_EXIT):
      s_editMode = 0; // TODO needed? we call ENTRY_UP after which does the same
      popMenu();
      break;

    case EVT_KEY_BREAK(KEY_EXIT):
#if defined(ROTARY_ENCODER_NAVIGATION)
      if (s_editMode == 0)
        s_editMode = EDIT_MODE_INIT;
      else
#endif
      if (s_editMode>0) {
        s_editMode = 0;
        break;
      }

      if (l_posVert==0 || !menuTab) {
        popMenu();  // beeps itself
      }
      else {
        AUDIO_MENUS();
        l_posVert = 0;
        l_posHorz = 0;
      }
      break;

    case EVT_KEY_REPT(KEY_RIGHT):  //inc
      if (l_posHorz==maxcol) break;
      // no break

    case EVT_KEY_FIRST(KEY_RIGHT)://inc
      if (!horTab || s_editMode>0) break;

#if defined(ROTARY_ENCODER_NAVIGATION)
    CASE_EVT_ROTARY_MOVE_RIGHT
      if (s_editMode != 0) break;
      if (l_posHorz < maxcol) {
        l_posHorz++;
        break;
      }
      else {
        l_posHorz = 0;
        if (!IS_ROTARY_MOVE_RIGHT(event))
          break;
      }
#else
      INC(l_posHorz, 0, maxcol);
      break;
#endif

    case EVT_KEY_REPT(KEY_DOWN):  //inc
      if (!IS_ROTARY_RIGHT(event) && l_posVert==maxrow) break;
      // no break

    case EVT_KEY_FIRST(KEY_DOWN): //inc
      if (s_editMode>0) break;
      do {
        INC(l_posVert, POS_VERT_INIT, maxrow);
      } while (CURSOR_NOT_ALLOWED_IN_ROW(l_posVert));

#if defined(ROTARY_ENCODER_NAVIGATION)
      s_editMode = 0; // if we go down, we must be in this mode
#endif

      l_posHorz = min(l_posHorz, MAXCOL(l_posVert));
      break;

    case EVT_KEY_REPT(KEY_LEFT):  //dec
      if (l_posHorz==0) break;
      // no break

    case EVT_KEY_FIRST(KEY_LEFT)://dec
      if (!horTab || s_editMode>0) break;

#if defined(ROTARY_ENCODER_NAVIGATION)
    CASE_EVT_ROTARY_MOVE_LEFT
      if (s_editMode != 0) break;
      if (l_posHorz > 0) {
        l_posHorz--;
        break;
      }
      else if (IS_ROTARY_MOVE_LEFT(event) && s_editMode == 0) {
        l_posHorz = 0xff;
      }
      else {
        l_posHorz = maxcol;
        break;
      }
#else
      DEC(l_posHorz, 0, maxcol);
      break;
#endif

    case EVT_KEY_REPT(KEY_UP):  //dec
      if (!IS_ROTARY_LEFT(event) && l_posVert==0) break;
      // no break
    case EVT_KEY_FIRST(KEY_UP): //dec
      if (s_editMode>0) break;

      do {
        DEC(l_posVert, POS_VERT_INIT, maxrow);
      } while (CURSOR_NOT_ALLOWED_IN_ROW(l_posVert));

#if defined(ROTARY_ENCODER_NAVIGATION)
      s_editMode = 0; // if we go up, we must be in this mode
#endif

      l_posHorz = min((uint8_t)l_posHorz, MAXCOL(l_posVert));
      break;
  }

#if defined(CPUARM)
  if (l_posVert<1) {
    s_pgOfs=0;
  }
  else if (menuTab && horTab) {
    vertpos_t realPosVert = l_posVert;
    vertpos_t realPgOfs = s_pgOfs;
    vertpos_t realMaxrow = maxrow;
    for (vertpos_t i=1; i<=maxrow; i++) {
      if (MAXCOL(i) == HIDDEN_ROW) {
        realMaxrow--;
        if (i < l_posVert)
          realPosVert--;
        if (i < s_pgOfs)
          realPgOfs--;
      }
    }
    if (realPosVert>(LCD_LINES-1)+realPgOfs) realPgOfs = realPosVert-(LCD_LINES-1);
    else if (realPosVert<1+realPgOfs) realPgOfs = realPosVert-1;
    s_pgOfs = realPgOfs;
    for (vertpos_t i=1; i<=realPgOfs; i++) {
      if (MAXCOL(i) == HIDDEN_ROW) {
        s_pgOfs++;
      }
    }
    maxrow = realMaxrow;
  }
  else {
    uint8_t max = menuTab ? LCD_LINES-1 : LCD_LINES-2;
    if (l_posVert>max+s_pgOfs) s_pgOfs = l_posVert-max;
    else if (l_posVert<1+s_pgOfs) s_pgOfs = l_posVert-1;
  }
#else
  uint8_t max = menuTab ? LCD_LINES-1 : LCD_LINES-2;
  if (l_posVert<1) s_pgOfs=0;
  else if (l_posVert>max+s_pgOfs) s_pgOfs = l_posVert-max;
  else if (l_posVert<1+s_pgOfs) s_pgOfs = l_posVert-1;
#endif
  m_posVert = l_posVert;
  m_posHorz = l_posHorz;
#if !defined(CPUM64)
  // cosmetics on 9x
  if (s_pgOfs > 0) {
    l_posVert--;
    if (l_posVert == s_pgOfs && CURSOR_NOT_ALLOWED_IN_ROW(l_posVert)) {
      s_pgOfs = l_posVert-1;
    }
  }
#endif
}

void check_simple(check_event_t event, uint8_t curr, const MenuFuncP *menuTab, uint8_t menuTabSize, vertpos_t maxrow)
{
  check(event, curr, menuTab, menuTabSize, 0, 0, maxrow);
}

void check_submenu_simple(check_event_t event, uint8_t maxrow)
{
  check_simple(event, 0, 0, 0, maxrow);
}

void repeatLastCursorMove(uint8_t event)
{
  if (CURSOR_MOVED_LEFT(event) || CURSOR_MOVED_RIGHT(event)) {
    putEvent(event);
  }
  else {
    m_posHorz = 0;
  }
}
