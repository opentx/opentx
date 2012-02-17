/*
 * Authors (alphabetical order)
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Jean-Pierre Parisy
 * - Karl Szmutny <shadow@privy.de>
 * - Michael Blandford
 * - Michal Hlavinka
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
#include "templates.h"
#include "menus.h"

int16_t calibratedStick[NUM_STICKS+NUM_POTS];
int16_t ex_chans[NUM_CHNOUT];          // Outputs + intermidiates
uint8_t s_pgOfs;
int8_t s_editMode;
uint8_t s_noHi;
uint8_t s_noScroll;

int16_t g_chans512[NUM_CHNOUT]; // TODO not here!

void menu_lcd_onoff( uint8_t x,uint8_t y, uint8_t value, uint8_t mode )
{
  lcd_putsnAtt(x, y, STR_OFFON+LEN_OFFON*value, LEN_OFFON, mode ? INVERS:0) ;
}

void DisplayScreenIndex(uint8_t index, uint8_t count, uint8_t attr)
{
  lcd_outdezAtt(128,0,count,attr);
  lcd_putcAtt(1+128-FW*(count>9 ? 3 : 2),0,'/',attr);
  lcd_outdezAtt(1+128-FW*(count>9 ? 3 : 2),0,index+1,attr);
}

#if defined(NAVIGATION_RE1)
int8_t scrollRE;
int16_t p1valdiff;
#elif defined(NAVIGATION_POT1)
int16_t p1valdiff;
#endif

int8_t  checkIncDec_Ret;
int16_t checkIncDec(uint8_t event, int16_t val, int16_t i_min, int16_t i_max, uint8_t i_flags)
{
  int16_t newval = val;
  uint8_t kpl=KEY_RIGHT, kmi=KEY_LEFT, kother = -1;

  if(event & _MSK_KEY_DBL){ 
    uint8_t hlp=kpl;
    kpl=kmi;
    kmi=hlp;
    event=EVT_KEY_FIRST(EVT_KEY_MASK & event);
  }
  if(event==EVT_KEY_FIRST(kpl) || event== EVT_KEY_REPT(kpl) || (s_editMode>0 && (event==EVT_KEY_FIRST(KEY_UP) || event== EVT_KEY_REPT(KEY_UP))) ) {
    newval++;
    AUDIO_KEYPAD_UP();
    kother=kmi;
  }
  else if(event==EVT_KEY_FIRST(kmi) || event== EVT_KEY_REPT(kmi) || (s_editMode>0 && (event==EVT_KEY_FIRST(KEY_DOWN) || event== EVT_KEY_REPT(KEY_DOWN))) ) {
    newval--;
    AUDIO_KEYPAD_DOWN();
    kother=kpl;
  }
  if((kother != (uint8_t)-1) && keyState((EnumKeys)kother)){
    newval=-val;
    killEvents(kmi);
    killEvents(kpl);
  }
  if(i_min==0 && i_max==1 && event==EVT_KEY_FIRST(KEY_MENU)) {
    s_editMode = 0;
    newval=!val;
    killEvents(event);
  }

#if defined(NAVIGATION_POT1) || defined (NAVIGATION_RE1)
  //change values based on P1
  newval -= p1valdiff;
#endif

  if(newval > i_max)
  {
    newval = i_max;
    killEvents(event);
    AUDIO_WARNING2();
  }
  if(newval < i_min)
  {
    newval = i_min;
    killEvents(event);
    AUDIO_WARNING2();
  }
  if(newval != val){
    if(newval==0) {
      pauseEvents(event); // delay before auto-repeat continues
      if (newval>val) // TODO check if without AUDIO it's optimized!
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

int8_t checkIncDecModel(uint8_t event, int8_t i_val, int8_t i_min, int8_t i_max)
{
  return checkIncDec(event,i_val,i_min,i_max,EE_MODEL);
}

int8_t checkIncDecGen(uint8_t event, int8_t i_val, int8_t i_min, int8_t i_max)
{
  return checkIncDec(event,i_val,i_min,i_max,EE_GENERAL);
}

bool check_simple(uint8_t event, uint8_t curr, const MenuFuncP *menuTab, uint8_t menuTabSize, uint8_t maxrow)
{
  return check(event, curr, menuTab, menuTabSize, 0, 0, maxrow);
}

bool check_submenu_simple(uint8_t event, uint8_t maxrow)
{
  return check_simple(event, 0, 0, 0, maxrow);
}

#define SCROLL_TH      64
#define SCROLL_POT1_TH 32

#ifdef NAVIGATION_RE1
#define MAXCOL(row) ((horTab && row > 0) ? pgm_read_byte(horTab+min(row, (int8_t)horTabMax)) : (const uint8_t)0)
#else
#define MAXCOL(row) (horTab ? pgm_read_byte(horTab+min(row, horTabMax)) : (const uint8_t)0)
#endif
#define INC(val,max) if(val<max) {val++;} else {val=0;}
#define DEC(val,max) if(val>0  ) {val--;} else {val=max;}
bool check(uint8_t event, uint8_t curr, const MenuFuncP *menuTab, uint8_t menuTabSize, const pm_uint8_t *horTab, uint8_t horTabMax, uint8_t maxrow)
{
  int8_t maxcol = MAXCOL(m_posVert);

#ifdef NAVIGATION_RE1
  // check rotary encoder 1 if changed -> cursor down/up
  static int16_t re1valprev;
  p1valdiff = 0;
  scrollRE = re1valprev - g_rotenc[0];
  if (scrollRE) {
    re1valprev = g_rotenc[0];
    if (s_editMode > 0) {
      p1valdiff = -scrollRE;
      scrollRE = 0;
    }
  }
  if (m_posVert < 0 && (event==EVT_KEY_FIRST(BTN_RE1) || event==EVT_KEY_FIRST(KEY_MENU))) {
    popMenu();
    killEvents(event);
    return false;
  }
  if (event == EVT_KEY_BREAK(BTN_RE1)) {
    if (s_editMode > 0 && (maxcol & ZCHAR)) {
      if (m_posHorz < maxcol-ZCHAR) {
        m_posHorz++;
      }
      else {
        s_editMode = 0;
      }
    }
    else {
      scrollRE = 0;
      if (s_editMode++ > 0) s_editMode = 0;
      if (s_editMode > 0 && m_posVert == 0 && menuTab) s_editMode = -1;
    }
  }
#else
#define scrollRE 0
#endif

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

  if(scrollLR || scrollUD || p1valdiff) g_LightOffCounter = g_eeGeneral.lightAutoOff*500; // on keypress turn the light on 5*100

  if (menuTab) {
    uint8_t attr = 0;

    if (m_posVert==0 && !s_noScroll) {
      attr = INVERS;

      int8_t cc = curr;

      if (scrollLR || (scrollRE && s_editMode < 0)) {
        cc = limit((int8_t)0, (int8_t)(curr - scrollLR + scrollRE), (int8_t)(menuTabSize-1));
      }

      switch(event) {
        case EVT_KEY_FIRST(KEY_LEFT):
          if (curr > 0)
            cc = curr - 1;
          else
            cc = menuTabSize-1;
          break;

        case EVT_KEY_FIRST(KEY_RIGHT):
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
    }
    s_noScroll = 0;
    DisplayScreenIndex(curr, menuTabSize, attr);
  }
#ifdef NAVIGATION_RE1
  else if (m_posVert < 0) {
    lcd_putsAtt(DISPLAY_W-LEN_BACK*FW, 0, STR_BACK, INVERS);
  }
#endif

  theFile.DisplayProgressBar(menuTab ? lcd_lastPos-2*FW-((curr+1)/10*FWNUM)-2 : 20*FW+1);

  if (s_editMode<=0) {
    if (scrollUD) {
      m_posVert = limit((int8_t)0, (int8_t)(m_posVert - scrollUD), (int8_t)maxrow);
      m_posHorz = min(m_posHorz, MAXCOL(m_posVert));
      BLINK_SYNC;
    }

    if (scrollLR && m_posVert>0) {
      m_posHorz = limit((int8_t)0, (int8_t)(m_posHorz - scrollLR), (int8_t)maxcol);
      BLINK_SYNC;
    }

#ifdef NAVIGATION_RE1
    while (!s_editMode && scrollRE) {
      if (scrollRE > 0) {
        --scrollRE;
        maxcol = MAXCOL(m_posVert);
        if (maxcol & ZCHAR) maxcol = 0;
        if (++m_posHorz > maxcol) {
          if (m_posVert < maxrow) {
            ++m_posVert;
            maxcol = MAXCOL(m_posVert); // TODO not stored into maxcol?
            if (maxcol < 0) m_posVert++;
            m_posHorz = 0;
          }
          else {
            --m_posHorz;
            scrollRE = 0;
          }
        }
      }
      else {
        ++scrollRE;
        if (m_posHorz-- == 0) {
          if (m_posVert-- <= 0) {
#ifdef NAVIGATION_RE1
            m_posVert = menuTab ? 0 : -1;
#else
            m_posVert = 0;
#endif
            m_posHorz = 0;
            scrollRE = 0;
          }
          else {
            maxcol = MAXCOL(m_posVert);
            if (maxcol < 0) maxcol = MAXCOL(--m_posVert);
            m_posHorz = maxcol;
          }
        }
      }
    }
#endif

  }

  switch(event)
  {
    case EVT_ENTRY:
      minit();
#ifdef NAVIGATION_RE1
      if (menuTab) {
        s_editMode = -1;
        break;
      }
      // no break
#else
      s_editMode = -1;
      break;
#endif
#ifdef NAVIGATION_RE1
    case EVT_ENTRY_UP:
      s_editMode = 0;
      break;
#endif
    case EVT_KEY_FIRST(KEY_MENU):
      if (maxcol > 0)
        s_editMode = (s_editMode<=0);
      break;
    case EVT_KEY_LONG(KEY_EXIT):
      s_editMode = 0;
      popMenu();
      break;
    case EVT_KEY_BREAK(KEY_EXIT):
      if(s_editMode>0) {
        s_editMode = 0;
        break;
      }
      if (m_posVert==0 || !menuTab) {
        popMenu();  //beeps itself
      }
      else {
        AUDIO_MENUS();
        minit();
        BLINK_SYNC;
      }
      break;

    case EVT_KEY_REPT(KEY_RIGHT):  //inc
      if(m_posHorz==maxcol) break;
    case EVT_KEY_FIRST(KEY_RIGHT)://inc
      if(!horTab || s_editMode>0)break;
      INC(m_posHorz,maxcol);
      BLINK_SYNC;
      break;

    case EVT_KEY_REPT(KEY_LEFT):  //dec
      if(m_posHorz==0) break;
    case EVT_KEY_FIRST(KEY_LEFT)://dec
      if(!horTab || s_editMode>0)break;
      DEC(m_posHorz,maxcol);
      BLINK_SYNC;
      break;

    case EVT_KEY_REPT(KEY_DOWN):  //inc
      if(m_posVert==maxrow) break;
    case EVT_KEY_FIRST(KEY_DOWN): //inc
      if(s_editMode>0)break;
      do {
        INC(m_posVert, maxrow);
      } while(MAXCOL(m_posVert) == (uint8_t)-1);
#ifdef NAVIGATION_RE1
      s_editMode = 0;
#endif
      m_posHorz = min(m_posHorz, MAXCOL(m_posVert));
      BLINK_SYNC;
      break;

    case EVT_KEY_REPT(KEY_UP):  //dec
      if(m_posVert==0) break;
    case EVT_KEY_FIRST(KEY_UP): //dec
      if(s_editMode>0)break;
      do {
        DEC(m_posVert,maxrow);
      } while(MAXCOL(m_posVert) == (uint8_t)-1);
      m_posHorz = min(m_posHorz, MAXCOL(m_posVert));
      BLINK_SYNC;
      break;
  }

  uint8_t max = menuTab ? 7 : 6;
  if(m_posVert<1) s_pgOfs=0;
  else if(m_posVert-s_pgOfs>max) s_pgOfs = m_posVert-max;
  else if(m_posVert-s_pgOfs<1) s_pgOfs = m_posVert-1;
  return true;
}

MenuFuncP g_menuStack[5];
uint8_t g_menuPos[4];
uint8_t g_menuStackPtr = 0;

#ifdef NAVIGATION_RE1
int8_t m_posVert;
#else
uint8_t m_posVert;
#endif
uint8_t m_posHorz;

void popMenu()
{
  if (g_menuStackPtr>0) {
    g_menuStackPtr = g_menuStackPtr-1;
    AUDIO_KEYPAD_UP();
    m_posHorz = 0;
    m_posVert = g_menuPos[g_menuStackPtr];
    (*g_menuStack[g_menuStackPtr])(EVT_ENTRY_UP);
  }
  else {
    alert(STR_MENUSERROR);
  }
}

void chainMenu(MenuFuncP newMenu)
{
  g_menuStack[g_menuStackPtr] = newMenu;
  (*newMenu)(EVT_ENTRY);
  AUDIO_MENUS();
}

void pushMenu(MenuFuncP newMenu)
{
  g_menuPos[g_menuStackPtr] = m_posVert;

  g_menuStackPtr++;
  if(g_menuStackPtr >= DIM(g_menuStack))
  {
    g_menuStackPtr--;
    alert(STR_MENUSERROR);
    return;
  }
  AUDIO_MENUS();
  g_menuStack[g_menuStackPtr] = newMenu;
  (*newMenu)(EVT_ENTRY);
}

#ifdef NAVIGATION_RE1
void * s_inflight_value;
int16_t s_inflight_min;
int16_t s_inflight_max;
const pm_char *s_inflight_label
uint8_t s_inflight_flags;

void checkInflightIncDec(void *value, int16_t i_min, int16_t i_max, const pm_char *label, uint8_t flags)
{
  s_inflight_value = value;
  s_inflight_min = i_min;
  s_inflight_max = i_max;
  s_inflight_label = label;
  s_inflight_flags = flags;
}
#endif
