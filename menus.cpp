/*
 * Authors (alphabetical order)
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 *
 * gruvin9x is based on code named er9x by
 * Author - Erez Raviv <erezraviv@gmail.com>, which is in turn
 * was based on the original (and ongoing) project by Thomas Husterer,
 * th9x -- http://code.google.com/p/th9x/
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


#include "gruvin9x.h"
#include "templates.h"
#include "menus.h"

int16_t calibratedStick[NUM_STICKS+NUM_POTS];
int16_t ex_chans[NUM_CHNOUT];          // Outputs + intermidiates
uint8_t s_pgOfs;
uint8_t s_editMode;
uint8_t s_noHi;
uint8_t s_noScroll;

int16_t g_chans512[NUM_CHNOUT];

void menu_lcd_onoff( uint8_t x,uint8_t y, uint8_t value, uint8_t mode )
{
  lcd_putsnAtt( x, y, PSTR("OFFON ")+3*value,3,mode ? INVERS:0) ;
}

void menu_lcd_HYPHINV( uint8_t x,uint8_t y, uint8_t value, uint8_t mode )
{
  lcd_putsnAtt( x, y, PSTR("---INV")+3*value,3,mode ? INVERS:0) ;
}

void DisplayScreenIndex(uint8_t index, uint8_t count, uint8_t attr)
{
  lcd_outdezAtt(128,0,count,attr);
  lcd_putcAtt(1+128-FW*(count>9 ? 3 : 2),0,'/',attr);
  lcd_outdezAtt(1+128-FW*(count>9 ? 3 : 2),0,index+1,attr);
}

int16_t p1valdiff;
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
  if(event==EVT_KEY_FIRST(kpl) || event== EVT_KEY_REPT(kpl) || (s_editMode && (event==EVT_KEY_FIRST(KEY_UP) || event== EVT_KEY_REPT(KEY_UP))) ) {
    newval++;
#if defined (BEEPSPKR)
    beepKeySpkr(BEEP_KEY_UP_FREQ);
#else
    beepKey();
#endif
    kother=kmi;
  }else if(event==EVT_KEY_FIRST(kmi) || event== EVT_KEY_REPT(kmi) || (s_editMode && (event==EVT_KEY_FIRST(KEY_DOWN) || event== EVT_KEY_REPT(KEY_DOWN))) ) {
    newval--;
#if defined (BEEPSPKR)
    beepKeySpkr(BEEP_KEY_DOWN_FREQ);
#else
    beepKey();
#endif
    kother=kpl;
  }
  if((kother != (uint8_t)-1) && keyState((EnumKeys)kother)){
    newval=-val;
    killEvents(kmi);
    killEvents(kpl);
  }
  if(i_min==0 && i_max==1 && event==EVT_KEY_FIRST(KEY_MENU)) {
    s_editMode = false;
    newval=!val;
    killEvents(event);
  }

  //change values based on P1
  newval -= p1valdiff;

  if(newval > i_max)
  {
    newval = i_max;
    killEvents(event);
#if defined (BEEPSPKR)
    beepWarn2Spkr(BEEP_KEY_UP_FREQ);
#else
    beepWarn2();
#endif
  }
  if(newval < i_min)
  {
    newval = i_min;
    killEvents(event);
#if defined (BEEPSPKR)
    beepWarn2Spkr(BEEP_KEY_DOWN_FREQ);
#else
    beepWarn2();
#endif
  }
  if(newval != val){
    if(newval==0) {
      pauseEvents(event); // delay before auto-repeat continues
#if defined (BEEPSPKR)
      if (newval>val)
        beepWarn2Spkr(BEEP_KEY_UP_FREQ);
      else
        beepWarn2Spkr(BEEP_KEY_DOWN_FREQ);
#else
      beepKey();
#endif
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

bool check_simple(uint8_t event, uint8_t curr, MenuFuncP *menuTab, uint8_t menuTabSize, uint8_t maxrow)
{
  return check(event, curr, menuTab, menuTabSize, 0, 0, maxrow);
}

bool check_submenu_simple(uint8_t event, uint8_t maxrow)
{
  return check_simple(event, 0, 0, 0, maxrow);
}

#define MAXCOL(row) (horTab ? pgm_read_byte(horTab+min(row, horTabMax)) : (const uint8_t)0)
#define INC(val,max) if(val<max) {val++;} else {val=0;}
#define DEC(val,max) if(val>0  ) {val--;} else {val=max;}
bool check(uint8_t event, uint8_t curr, MenuFuncP *menuTab, uint8_t menuTabSize, prog_uint8_t *horTab, uint8_t horTabMax, uint8_t maxrow)
{
  if (menuTab) {
    uint8_t attr = 0;

    if (m_posVert==0 && !s_noScroll) {
      attr = INVERS;
      switch(event)
      {
        case EVT_KEY_FIRST(KEY_LEFT):
          if(curr>0)
            chainMenu((MenuFuncP)pgm_read_adr(&menuTab[curr-1]));
          else
            chainMenu((MenuFuncP)pgm_read_adr(&menuTab[menuTabSize-1]));
          return false;
        case EVT_KEY_FIRST(KEY_RIGHT):
          if(curr < (menuTabSize-1))
            chainMenu((MenuFuncP)pgm_read_adr(&menuTab[curr+1]));
          else
            chainMenu((MenuFuncP)pgm_read_adr(&menuTab[0]));
          return false;
      }
    }
    s_noScroll = 0;
    DisplayScreenIndex(curr, menuTabSize, attr);
  }

  theFile.DisplayProgressBar();

  uint8_t maxcol = MAXCOL(m_posVert);
  switch(event)
  {
    case EVT_ENTRY:
      minit();
      s_editMode = false;
      break;
    case EVT_KEY_FIRST(KEY_MENU):
      if (maxcol > 0)
        s_editMode = !s_editMode;
      break;
    case EVT_KEY_LONG(KEY_EXIT):
      s_editMode = false;
      popMenu();
      break;
    case EVT_KEY_BREAK(KEY_EXIT):
      if(s_editMode) {
        s_editMode = false;
        break;
      }
      if(m_posVert==0 || !menuTab) {
        popMenu();  //beeps itself
      }
      else {
        beepKey();
        minit();BLINK_SYNC;
      }
      break;

    case EVT_KEY_REPT(KEY_RIGHT):  //inc
      if(m_posHorz==maxcol) break;
    case EVT_KEY_FIRST(KEY_RIGHT)://inc
      if(!horTab || s_editMode)break;
      INC(m_posHorz,maxcol);
      BLINK_SYNC;
      break;

    case EVT_KEY_REPT(KEY_LEFT):  //dec
      if(m_posHorz==0) break;
    case EVT_KEY_FIRST(KEY_LEFT)://dec
      if(!horTab || s_editMode)break;
      DEC(m_posHorz,maxcol);
      BLINK_SYNC;
      break;

    case EVT_KEY_REPT(KEY_DOWN):  //inc
      if(m_posVert==maxrow) break;
    case EVT_KEY_FIRST(KEY_DOWN): //inc
      if(s_editMode)break;
      do {
        INC(m_posVert,maxrow);
      } while(MAXCOL(m_posVert) == (uint8_t)-1);
      m_posHorz = min(m_posHorz, MAXCOL(m_posVert));
      BLINK_SYNC;
      break;

    case EVT_KEY_REPT(KEY_UP):  //dec
      if(m_posVert==0) break;
    case EVT_KEY_FIRST(KEY_UP): //dec
      if(s_editMode)break;
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

uint8_t m_posVert;
uint8_t m_posHorz;

void popMenu()
{
  if (g_menuStackPtr>0) {
    g_menuStackPtr = g_menuStackPtr-1;
    beepKey();
    m_posHorz = 0;
    m_posVert = g_menuPos[g_menuStackPtr];
    (*g_menuStack[g_menuStackPtr])(EVT_ENTRY_UP);
  }
  else {
    alert(PSTR("menuStack underflow"));
  }
}

void chainMenu(MenuFuncP newMenu)
{
  g_menuStack[g_menuStackPtr] = newMenu;
  (*newMenu)(EVT_ENTRY);
  beepKey();
}

void pushMenu(MenuFuncP newMenu)
{
  g_menuPos[g_menuStackPtr] = m_posVert;

  g_menuStackPtr++;
  if(g_menuStackPtr >= DIM(g_menuStack))
  {
    g_menuStackPtr--;
    alert(PSTR("menuStack overflow"));
    return;
  }
  beepKey();
  g_menuStack[g_menuStackPtr] = newMenu;
  (*newMenu)(EVT_ENTRY);
}
