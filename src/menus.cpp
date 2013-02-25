/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Jean-Pierre Parisy
 * - Karl Szmutny <shadow@privy.de>
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * open9x is based on code named
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

#include "open9x.h"

pgofs_t s_pgOfs;
int8_t s_editMode;
uint8_t s_noHi;
uint8_t s_noScroll;

void menu_lcd_onoff(uint8_t x,uint8_t y, uint8_t value, LcdFlags attr)
{
#if defined(GRAPHICS)
  if (value) {
    lcd_putc(x+1, y, '#');
  }
  if (attr)
    lcd_filled_rect(x, y, 7, 7);
  else
    lcd_square(x, y, 7);
#else
  /* ON / OFF version */
  lcd_putsiAtt(x, y, STR_OFFON, value, attr ? INVERS:0) ;
#endif
}

void displayScreenIndex(uint8_t index, uint8_t count, uint8_t attr)
{
  lcd_outdezAtt(LCD_W,0,count,attr);
  xcoord_t x = 1+LCD_W-FW*(count>9 ? 3 : 2);
  lcd_putcAtt(x,0,'/',attr);
  lcd_outdezAtt(x,0,index+1,attr);
}

#if !defined(PCBSTD)
void displayScrollbar(xcoord_t x, uint8_t y, uint8_t h, uint16_t offset, uint16_t count, uint8_t visible)
{
  lcd_vlineStip(x, y, h, SOLID, ERASE);
  lcd_vlineStip(x, y, h, DOTTED);
  uint8_t yofs = (h * offset) / count;
  uint8_t yhgt = (h * visible) / count;
  if (yhgt + yofs > h)
    yhgt = h - yofs;
  lcd_vlineStip(x, y + yofs, yhgt, SOLID, FORCE);
}
#endif

#if defined(NAVIGATION_POT1)
int16_t p1valdiff;
#endif

int8_t  checkIncDec_Ret;
int16_t checkIncDec(uint8_t event, int16_t val, int16_t i_min, int16_t i_max, uint8_t i_flags)
{
  int16_t newval = val;
  
#if defined(DBLKEYS)
  uint8_t in = KEYS_PRESSED();
  if (EVT_KEY_MASK(event)) {
    bool dblkey = true;
    if (DBLKEYS_PRESSED_RGT_LFT(in))
      newval = -val;
    else if (DBLKEYS_PRESSED_RGT_UP(in))
      newval = (i_max > 100 ? 100 : i_max);
    else if (DBLKEYS_PRESSED_LFT_DWN(in))
      newval = (i_min < -100 ? -100 : i_min);
    else if (DBLKEYS_PRESSED_UP_DWN(in))
      newval = 0;
    else
      dblkey = false;

    if (dblkey) {
      killEvents(KEY_UP);
      killEvents(KEY_DOWN);
      killEvents(KEY_RIGHT);
      killEvents(KEY_LEFT);
      event = 0;
    }
  }
#endif

#if defined(PCBX9D)
  if (s_editMode>0 && (IS_ROTARY_RIGHT(event) || event==EVT_KEY_FIRST(KEY_UP) || event==EVT_KEY_REPT(KEY_UP))) {
#else
  if (event==EVT_KEY_FIRST(KEY_RIGHT) || event==EVT_KEY_REPT(KEY_RIGHT) || (s_editMode>0 && (IS_ROTARY_RIGHT(event) || event==EVT_KEY_FIRST(KEY_UP) || event==EVT_KEY_REPT(KEY_UP)))) {
#endif
    newval++;
    AUDIO_KEYPAD_UP();
  }
#if defined(PCBX9D)
  else if (s_editMode>0 && (IS_ROTARY_LEFT(event) || event==EVT_KEY_FIRST(KEY_DOWN) || event==EVT_KEY_REPT(KEY_DOWN))) {
#else
  else if (event==EVT_KEY_FIRST(KEY_LEFT) || event==EVT_KEY_REPT(KEY_LEFT) || (s_editMode>0 && (IS_ROTARY_LEFT(event) || event==EVT_KEY_FIRST(KEY_DOWN) || event==EVT_KEY_REPT(KEY_DOWN)))) {
#endif
    newval--;
    AUDIO_KEYPAD_DOWN();
  }

  if (i_min==0 && i_max==1 && (event==EVT_KEY_BREAK(KEY_ENTER) || IS_ROTARY_BREAK(event))) {
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
    if (s_editMode>0) {
      int8_t swtch = getMovedSwitch();
      if (swtch) {
        if (newval == DSW(SW_TRN) && swtch == DSW(SW_TRN))
          newval = -newval;
        else
          newval = swtch;
      }
    }

    if (event == EVT_KEY_LONG(KEY_MENU) && i_max > SWITCH_ON) {
      s_editMode = !s_editMode;
      if (newval > SWITCH_ON)
        newval -= (MAX_SWITCH+1);
      else if (newval > 0)
        newval += (MAX_SWITCH+1);
      else if (newval < SWITCH_OFF)
        newval += (MAX_SWITCH+1);
      else if (newval < 0)
        newval -= (MAX_SWITCH+1);
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

bool check_simple(check_event_t event, uint8_t curr, const MenuFuncP *menuTab, uint8_t menuTabSize, maxrow_t maxrow)
{
  return check(event, curr, menuTab, menuTabSize, 0, 0, maxrow);
}

bool check_submenu_simple(check_event_t event, uint8_t maxrow)
{
  return check_simple(event, 0, 0, 0, maxrow);
}

void title(const pm_char * s)
{
  lcd_putsAtt(0, 0, s, INVERS);
}

#define SCROLL_TH      64
#define SCROLL_POT1_TH 32

#define MAXCOL(row) (horTab ? pgm_read_byte(horTab+min(row, (maxrow_t)horTabMax)) : (const uint8_t)0)
#define INC(val, min, max) if (val<max) {val++;} else {val=min;}
#define DEC(val, min, max) if (val>min) {val--;} else {val=max;}

bool check(check_event_t event, uint8_t curr, const MenuFuncP *menuTab, uint8_t menuTabSize, const pm_uint8_t *horTab, uint8_t horTabMax, maxrow_t maxrow)
{
  maxrow_t l_posVert = m_posVert;
  uint8_t  l_posHorz = m_posHorz;

  uint8_t maxcol = MAXCOL(l_posVert);

#ifdef NAVIGATION_POT1
  // check pot 1 - if changed -> scroll values
  static int16_t p1valprev;
  p1valdiff = (p1valprev-calibratedStick[6]) / SCROLL_POT1_TH;
  if (p1valdiff) p1valprev = calibratedStick[6];
#endif

#ifdef NAVIGATION_POT2
  // check pot 2 - if changed -> scroll menu
  static int16_t p2valprev;
  int8_t scrollLR = (p2valprev-calibratedStick[4]) / SCROLL_TH;
  if (scrollLR) p2valprev = calibratedStick[4];
#else
#define scrollLR 0
#endif

#ifdef NAVIGATION_POT3
  // check pot 3 if changed -> cursor down/up
  static int16_t p3valprev;
  int8_t scrollUD = (p3valprev-calibratedStick[5]) / SCROLL_TH;
  if (scrollUD) p3valprev = calibratedStick[5];
#else
#define scrollUD 0
#endif

  if (scrollLR || scrollUD || p1valdiff) backlightOn(); // on keypress turn the light on

  if (menuTab) {
    uint8_t attr = 0;

#if defined(PCBX9D)
    int8_t cc = curr;
    switch(event) {
      case EVT_KEY_LONG(KEY_PAGE):
        if (curr > 0)
          cc = curr - 1;
        else
          cc = menuTabSize-1;
        killEvents(event);
        break;

      case EVT_KEY_BREAK(KEY_PAGE):
        if (curr < (menuTabSize-1))
          cc = curr + 1;
        else
          cc = 0;
        break;
    }
    if (cc != curr) {
      chainMenu((MenuFuncP)pgm_read_adr(&menuTab[cc]));
      return false;
    }
#else

    if (l_posVert==0 && !s_noScroll) {
      attr = INVERS;

      int8_t cc = curr;

      if (scrollLR) {
        cc = limit((int8_t)0, (int8_t)(cc - scrollLR), (int8_t)(menuTabSize-1));
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

        case EVT_KEY_FIRST(KEY_LEFT):
#if defined(ROTARY_ENCODER_NAVIGATION)
        case EVT_ROTARY_LEFT:
          if (s_editMode >= 0)
            break;
#endif
          if (curr > 0)
            cc = curr - 1;
          else
            cc = menuTabSize-1;
          break;

        case EVT_KEY_FIRST(KEY_RIGHT):
#if defined(ROTARY_ENCODER_NAVIGATION)
        case EVT_ROTARY_RIGHT:
          if (s_editMode >= 0)
            break;
#endif
          if (curr < (menuTabSize-1))
            cc = curr + 1;
          else
            cc = 0;
          break;
      }

      if (cc != curr) {
        chainMenu((MenuFuncP)pgm_read_adr(&menuTab[cc]));
        return false;
      }

#if defined(ROTARY_ENCODER_NAVIGATION)
      if (IS_RE_NAVIGATION_ENABLE() && s_editMode < 0)
        attr = INVERS|BLINK;
#endif
    }
#endif

    s_noScroll = 0;
    displayScreenIndex(curr, menuTabSize, attr);

#if LCD_W >= 212
    if (maxrow > LCD_LINES-1)
      displayScrollbar(LCD_W-1, FH, LCD_H-FH, s_pgOfs, maxrow, LCD_LINES-1);
#endif
  }

  DISPLAY_PROGRESS_BAR(menuTab ? lcdLastPos-2*FW-((curr+1)/10*FWNUM)-2 : 20*FW+1);

  if (s_editMode<=0) {
    if (scrollUD) {
      l_posVert = limit((int8_t)0, (int8_t)(l_posVert - scrollUD), (int8_t)maxrow);
      l_posHorz = min(l_posHorz, MAXCOL(l_posVert));
    }

    if (scrollLR && l_posVert>0) {
      l_posHorz = limit((int8_t)0, (int8_t)(l_posHorz - scrollLR), (int8_t)maxcol);
    }
  }

  switch(event)
  {
    case EVT_ENTRY:
      l_posVert = POS_VERT_INIT;
      l_posHorz = 0;
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
      break;
#endif

#if defined(ROTARY_ENCODER_NAVIGATION) || defined(PCBX9D)
    case EVT_ROTARY_BREAK:
      if (s_editMode > 1) break;
#endif

#if !defined(PCBX9D)
    case EVT_KEY_FIRST(KEY_ENTER):
#endif
      if (!menuTab || l_posVert>0)
        s_editMode = (s_editMode<=0);
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
      return false;

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
#if defined(PCBX9D)
      {
        uint8_t posVertInit = POS_VERT_INIT;
        if (s_pgOfs != 0 || l_posHorz != 0 || l_posVert != posVertInit) {
          s_pgOfs = 0;
          l_posVert = posVertInit;
          l_posHorz = 0;
        }
        else {
          popMenu();
          return false;
        }
      }
#else
      if (l_posVert==0 || !menuTab) {
        popMenu();  // beeps itself
        return false;
      }
      else {
        AUDIO_MENUS();
        l_posVert = 0;
        l_posHorz = 0;
      }
#endif
      break;

#if !defined(PCBX9D)
    case EVT_KEY_REPT(KEY_RIGHT):  //inc
      if (l_posHorz==maxcol) break;
      // no break

    case EVT_KEY_FIRST(KEY_RIGHT)://inc
      if (!horTab || s_editMode>0) break;
#endif

#if defined(ROTARY_ENCODER_NAVIGATION) || defined(PCBX9D)
    CASE_EVT_ROTARY_MOVE_RIGHT
      if (s_editMode != 0) break;
      // TODO s_editMode test is duplicated
      // TODO could be written in a smarter way!
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

#if !defined(PCBX9D)
    case EVT_KEY_REPT(KEY_DOWN):  //inc
      if (!IS_ROTARY_RIGHT(event) && l_posVert==maxrow) break;
      // no break

    case EVT_KEY_FIRST(KEY_DOWN): //inc
      if (s_editMode>0) break;
#endif
      do {
        INC(l_posVert, POS_VERT_INIT, maxrow);
      } while (MAXCOL(l_posVert) == (uint8_t)-1);

#if defined(ROTARY_ENCODER_NAVIGATION) || defined(PCBX9D)
      s_editMode = 0; // TODO why?
#endif
      l_posHorz = min(l_posHorz, MAXCOL(l_posVert));
      break;

#if !defined(PCBX9D)
    case EVT_KEY_REPT(KEY_LEFT):  //dec
      if (l_posHorz==0) break;
      // no break

    case EVT_KEY_FIRST(KEY_LEFT)://dec
      if (!horTab || s_editMode>0) break;
#endif

#if defined(ROTARY_ENCODER_NAVIGATION) || defined(PCBX9D)
    CASE_EVT_ROTARY_MOVE_LEFT
      // TODO could be written in a smarter way!
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

#if !defined(PCBX9D)
    case EVT_KEY_REPT(KEY_UP):  //dec
      if (!IS_ROTARY_LEFT(event) && l_posVert==0) break;
      // no break
    case EVT_KEY_FIRST(KEY_UP): //dec
      if (s_editMode>0) break;
#endif

      do {
        DEC(l_posVert, POS_VERT_INIT, maxrow);
      } while(MAXCOL(l_posVert) == (uint8_t)-1);
      l_posHorz = min(l_posHorz, MAXCOL(l_posVert));
      break;

  }

  uint8_t max = menuTab ? LCD_LINES-1 : LCD_LINES-2;
  if (l_posVert<1) s_pgOfs=0;
  else if (l_posVert-s_pgOfs>max) s_pgOfs = l_posVert-max;
  else if (l_posVert-s_pgOfs<1) s_pgOfs = l_posVert-1;
  m_posVert = l_posVert;
  m_posHorz = l_posHorz;
  return true;
}

MenuFuncP g_menuStack[5];
uint8_t g_menuPos[4];
uint8_t g_menuStackPtr = 0;
maxrow_t m_posVert;
uint8_t m_posHorz;

void popMenu()
{
  assert(g_menuStackPtr>0);

  g_menuStackPtr = g_menuStackPtr-1;
  AUDIO_KEYPAD_UP();
  m_posHorz = 0;
  m_posVert = g_menuPos[g_menuStackPtr];
  (*g_menuStack[g_menuStackPtr])(EVT_ENTRY_UP);
}

void chainMenu(MenuFuncP newMenu)
{
  g_menuStack[g_menuStackPtr] = newMenu;
  (*newMenu)(EVT_ENTRY);
  AUDIO_MENUS();
}

void pushMenu(MenuFuncP newMenu)
{
  killEvents(KEY_ENTER);

  if (g_menuStackPtr == 0) {
    if (newMenu == menuGeneralSetup)
      g_menuPos[0] = 1;
    if (newMenu == menuModelSelect)
      g_menuPos[0] = 0;
  }
  else {
    g_menuPos[g_menuStackPtr] = m_posVert;
  }

  g_menuStackPtr++;

  assert(g_menuStackPtr < DIM(g_menuStack));

  AUDIO_MENUS();
  g_menuStack[g_menuStackPtr] = newMenu;
  (*newMenu)(EVT_ENTRY);
}

const pm_char * s_warning = 0;
const pm_char * s_warning_info;
uint8_t         s_warning_info_len;
// uint8_t s_warning_info_att not needed now
uint8_t         s_warning_type;
uint8_t         s_warning_result = 0;
const pm_char * s_global_warning = 0;

void displayBox()
{
  lcd_filled_rect(10, 16, LCD_W-20, 40, SOLID, ERASE);
  lcd_rect(10, 16, LCD_W-20, 40);
  lcd_puts(16, 3*FH, s_warning);
  // could be a place for a s_warning_info
}

void displayPopup(const pm_char * pstr)
{
  s_warning = pstr;
  displayBox();
  s_warning = 0;
  lcdRefresh();
}

void displayWarning(uint8_t event)
{
  s_warning_result = false;
  displayBox();
  if (s_warning_info)
    lcd_putsnAtt(16, 4*FH, s_warning_info, s_warning_info_len, ZCHAR);
  lcd_puts(16, 5*FH, s_warning_type == WARNING_TYPE_CONFIRM ? STR_POPUPS : STR_EXIT);
  switch(event) {
#if defined(ROTARY_ENCODERS)
    case EVT_ROTARY_BREAK:
#endif
    case EVT_KEY_BREAK(KEY_ENTER):
      if (s_warning_type == WARNING_TYPE_ASTERISK)
        break;
      s_warning_result = true;
      // no break
    case EVT_KEY_BREAK(KEY_EXIT):
      s_warning = 0;
      s_warning_type = WARNING_TYPE_ASTERISK;
      break;
  }
}

int8_t selectMenuItem(uint8_t x, uint8_t y, const pm_char *label, const pm_char *values, int8_t value, int8_t min, int8_t max, LcdFlags attr, uint8_t event)
{
  lcd_putsColumnLeft(x, y, label);
  if (values) lcd_putsiAtt(x, y, values, value-min, attr);
  if (attr) value = checkIncDec(event, value, min, max, (g_menuPos[0] == 0) ? EE_MODEL : EE_GENERAL);
  return value;
}

uint8_t onoffMenuItem(uint8_t value, uint8_t x, uint8_t y, const pm_char *label, LcdFlags attr, uint8_t event )
{
#if defined(GRAPHICS)
  menu_lcd_onoff(x, y, value, attr);
  return selectMenuItem(x, y, label, NULL, value, 0, 1, attr, event);
#else
  return selectMenuItem(x, y, label, STR_OFFON, value, 0, 1, attr, event);
#endif
}

int8_t switchMenuItem(uint8_t x, uint8_t y, int8_t value, LcdFlags attr, uint8_t event)
{
  lcd_putsColumnLeft(x, y, STR_SWITCH);
  putsSwitches(x,  y, value, attr);
  if (attr) CHECK_INCDEC_MODELSWITCH(event, value, -MAX_SWITCH, MAX_SWITCH);
  return value;
}

#if defined(GVARS)
int16_t gvarMenuItem(uint8_t x, uint8_t y, int16_t value, int16_t min, int16_t max, LcdFlags attr, uint8_t event)  // @@@ open.20.fsguruh
{
  uint8_t delta = (max <= 100 ? GV1_SMALL-1 : GV1_LARGE-1);
  bool invers = (attr & INVERS);
  if (invers && event == EVT_KEY_LONG(KEY_ENTER)) {
    s_editMode = !s_editMode;
    value = (value > max ? GET_GVAR(value, min, max, s_perout_flight_phase) : delta+1);
    eeDirty(EE_MODEL);
  }
  if (value > max) {
    if (attr & LEFT)
      attr -= LEFT; /* because of ZCHAR */
    else
      x -= 2*FW+FWNUM;
    int8_t idx = value - delta;
    if (invers) {
      CHECK_INCDEC_MODELVAR(event, idx, -4, +5);
      value = (int16_t)idx + delta;
    }
    if (idx <= 0) { idx = 1-idx; lcd_putcAtt(x-6, y, '-', attr); }
    putsStrIdx(x, y, STR_GV, idx, attr);
  }
  else {
    lcd_outdezAtt(x, y, value, attr);
	if (invers) value = checkIncDec(event, value, min, max,EE_MODEL);  // as proposed, no change to makro, but directly call this function    
  }
  return value;
}
#else
int8_t gvarMenuItem(uint8_t x, uint8_t y, int8_t value, int16_t min, int16_t max, LcdFlags attr, uint8_t event)
{
  lcd_outdezAtt(x, y, value, attr);
  if (attr&INVERS) value = checkIncDec(event, value, min, max,EE_MODEL);
  return value;
}
#endif

#if LCD_W >= 212
#define MENU_X   30
#define MENU_W   LCD_W-60
#else
#define MENU_X   10
#define MENU_W   LCD_W-20
#endif

#if defined(SDCARD)
const char *s_menu[MENU_MAX_LINES];
uint8_t s_menu_item = 0;
uint16_t s_menu_count = 0;
uint8_t s_menu_flags = 0;
uint16_t s_menu_offset = 0;
const char * displayMenu(uint8_t event)
{
  const char * result = NULL;

  uint8_t display_count = min(s_menu_count, (uint16_t)MENU_MAX_LINES);

  lcd_filled_rect(MENU_X, 16, MENU_W, display_count * (FH+1) + 2, SOLID, ERASE);
  lcd_rect(MENU_X, 16, MENU_W, display_count * (FH+1) + 2);

  for (uint8_t i=0; i<display_count; i++) {
    lcd_putsAtt(MENU_X+6, i*(FH+1) + 2*FH + 2, s_menu[i], s_menu_flags);
    if (i == s_menu_item) lcd_filled_rect(MENU_X+1, i*(FH+1) + 2*FH + 1, MENU_W-2, 9);
  }

  if (s_menu_count > display_count) {
    displayScrollbar(MENU_X+MENU_W-1, 17, MENU_MAX_LINES * (FH+1), s_menu_offset, s_menu_count, MENU_MAX_LINES);
  }

  switch(event) {
    CASE_EVT_ROTARY_RIGHT
    case EVT_KEY_BREAK(KEY_MOVE_UP):
      if (s_menu_item > 0)
        s_menu_item--;
      else if (s_menu_offset > 0) {
        s_menu_offset--;
        result = STR_UPDATE_LIST;
      }
      break;

#if defined(ROTARY_ENCODER_NAVIGATION)
    CASE_EVT_ROTARY_LEFT
#endif
    case EVT_KEY_BREAK(KEY_MOVE_DOWN):
      if (s_menu_item < display_count - 1 && s_menu_offset + s_menu_item + 1 < s_menu_count)
        s_menu_item++;
      else if (s_menu_count > s_menu_offset + display_count) {
        s_menu_offset++;
        result = STR_UPDATE_LIST;
      }
      break;
    CASE_EVT_ROTARY_BREAK
    case EVT_KEY_BREAK(KEY_ENTER):
      result = s_menu[s_menu_item];
      // no break
#if defined(ROTARY_ENCODER_NAVIGATION)
    CASE_EVT_ROTARY_LONG
      killEvents(event);
#endif
    case EVT_KEY_BREAK(KEY_EXIT):
      s_menu_count = 0;
      s_menu_item = 0;
      s_menu_flags = 0;
      s_menu_offset = 0;
      break;
  }

  return result;
}

char statusLineMsg[STATUS_LINE_LENGTH];
tmr10ms_t statusLineTime = 0;
uint8_t statusLineHeight = 0;

void showStatusLine()
{
  statusLineTime = get_tmr10ms();
}

#define STATUS_LINE_DELAY (3 * 100) /* 3s */
void drawStatusLine()
{
  if (statusLineTime) {
    if ((tmr10ms_t)(get_tmr10ms() - statusLineTime) <= (tmr10ms_t)STATUS_LINE_DELAY) {
      if (statusLineHeight < 8) statusLineHeight++;
    }
    else if (statusLineHeight) {
      statusLineHeight--;
    }
    else {
      statusLineTime = 0;
    }

    lcd_filled_rect(0, 8*FH-statusLineHeight, LCD_W, 8, SOLID, ERASE);
    lcd_putsAtt(5, 8*FH+1-statusLineHeight, statusLineMsg, BSS);
    lcd_filled_rect(0, 8*FH-statusLineHeight, LCD_W, 8, SOLID);
  }
}

#endif



