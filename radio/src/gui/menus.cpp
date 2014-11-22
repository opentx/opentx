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

#include "../opentx.h"

vertpos_t s_pgOfs;
int8_t s_editMode;
uint8_t s_noHi;
uint8_t calibrationState;

void menu_lcd_onoff(coord_t x, coord_t y, uint8_t value, LcdFlags attr)
{
#if defined(GRAPHICS)
  if (value)
    lcd_putc(x+1, y, '#');
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
  lcd_outdezAtt(LCD_W, 0, count, attr);
  coord_t x = 1+LCD_W-FW*(count>9 ? 3 : 2);
  lcd_putcAtt(x, 0, '/', attr);
  lcd_outdezAtt(x, 0, index+1, attr);
}

#if !defined(CPUM64)
void displayScrollbar(coord_t x, coord_t y, coord_t h, uint16_t offset, uint16_t count, uint8_t visible)
{
  lcd_vlineStip(x, y, h, SOLID, ERASE);
  lcd_vlineStip(x, y, h, DOTTED);
  coord_t yofs = (h * offset) / count;
  coord_t yhgt = (h * visible) / count;
  if (yhgt + yofs > h)
    yhgt = h - yofs;
  lcd_vlineStip(x, y + yofs, yhgt, SOLID, FORCE);
}
#endif

#if defined(NAVIGATION_POT1)
int16_t p1valdiff;
#endif

#if defined(NAVIGATION_POT2)
int8_t p2valdiff;
#endif

uint8_t switchToMix(uint8_t source)
{
#if defined(PCBTARANIS)
  if (source <= 5*3)
    return MIXSRC_FIRST_SWITCH + (source-1) / 3;
  else if (source <= 17)
    return MIXSRC_SF;
  else if (source <= 20)
    return MIXSRC_SG;
  else
    return MIXSRC_SH;
#else
  if (source <= 3)
    return MIXSRC_3POS;
  else
    return MIXSRC_FIRST_SWITCH - 3 + source;
#endif
}

int8_t  checkIncDec_Ret;

#if defined(CPUARM)
int checkIncDec(uint8_t event, int val, int i_min, int i_max, uint8_t i_flags, IsValueAvailable isValueAvailable)
#else
int16_t checkIncDec(uint8_t event, int16_t val, int16_t i_min, int16_t i_max, uint8_t i_flags)
#endif
{
  int newval = val;

#if defined(DBLKEYS)
  uint8_t in = KEYS_PRESSED();
  if (EVT_KEY_MASK(event)) {
    bool dblkey = true;
    if (DBLKEYS_PRESSED_RGT_LFT(in))
      newval = -val;
    else if (DBLKEYS_PRESSED_RGT_UP(in)) {
      newval = (i_max > 100 ? 100 : i_max);
#if defined(CPUARM)
      if (i_flags & DBLKEYS_1000) newval *= 10;
#endif
    }
    else if (DBLKEYS_PRESSED_LFT_DWN(in)) {
      newval = (i_min < -100 ? -100 : i_min);
#if defined(CPUARM)
      if (i_flags & DBLKEYS_1000) newval *= 10;
#endif
    }
    else if (DBLKEYS_PRESSED_UP_DWN(in))
      newval = 0;
    else
      dblkey = false;

#if defined(CPUARM)

#endif

    if (dblkey) {
      killEvents(KEY_UP);
      killEvents(KEY_DOWN);
      killEvents(KEY_RIGHT);
      killEvents(KEY_LEFT);
#if defined(PCBTARANIS)
      killEvents(KEY_PAGE);
      killEvents(KEY_MENU);
      killEvents(KEY_ENTER);
      killEvents(KEY_EXIT);
#endif
      event = 0;
    }
  }
#endif

#if defined(PCBTARANIS)
  if (s_editMode>0 && (IS_ROTARY_RIGHT(event) || event==EVT_KEY_FIRST(KEY_UP) || event==EVT_KEY_REPT(KEY_UP))) {
#else
  if (event==EVT_KEY_FIRST(KEY_RIGHT) || event==EVT_KEY_REPT(KEY_RIGHT) || (s_editMode>0 && (IS_ROTARY_RIGHT(event) || event==EVT_KEY_FIRST(KEY_UP) || event==EVT_KEY_REPT(KEY_UP)))) {
#endif
#if defined(CPUARM)
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
    else
#else
    newval++;
#endif
    AUDIO_KEYPAD_UP();
  }
#if defined(PCBTARANIS)
  else if (s_editMode>0 && (IS_ROTARY_LEFT(event) || event==EVT_KEY_FIRST(KEY_DOWN) || event==EVT_KEY_REPT(KEY_DOWN))) {
#else
  else if (event==EVT_KEY_FIRST(KEY_LEFT) || event==EVT_KEY_REPT(KEY_LEFT) || (s_editMode>0 && (IS_ROTARY_LEFT(event) || event==EVT_KEY_FIRST(KEY_DOWN) || event==EVT_KEY_REPT(KEY_DOWN)))) {
#endif
#if defined(CPUARM)
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
    else
#else
    newval--;
#endif
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
    if (s_editMode>0) {
      int8_t swtch = getMovedSwitch();
      if (swtch) {
#if defined(PCBTARANIS)
        if(swtch == SWSRC_SH2)
          newval = (newval == SWSRC_SH2 ? SWSRC_SH0 : SWSRC_SH2);
        else if(swtch != SWSRC_SH0)
          newval = swtch;
#else
        if (IS_MOMENTARY(newval) && IS_MOMENTARY(swtch))
          newval = -newval;
        else
          newval = swtch;
#endif
      }
    }
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

void check_simple(check_event_t event, uint8_t curr, const MenuFuncP *menuTab, uint8_t menuTabSize, vertpos_t maxrow)
{
  check(event, curr, menuTab, menuTabSize, 0, 0, maxrow);
}

void check_submenu_simple(check_event_t event, uint8_t maxrow)
{
  check_simple(event, 0, 0, 0, maxrow);
}

void title(const pm_char * s)
{
  lcd_putsAtt(0, 0, s, INVERS);
}

#define SCROLL_TH      64
#define SCROLL_POT1_TH 32

#if defined(CPUARM)
  #define CURSOR_NOT_ALLOWED_IN_ROW(row)   ((int8_t)MAXCOL(row) < 0)
#else
  #define CURSOR_NOT_ALLOWED_IN_ROW(row)   (MAXCOL(row) == TITLE_ROW)
#endif

#if defined(PCBTARANIS)
  #define MAXCOL_RAW(row) (horTab ? pgm_read_byte(horTab+min(row, (vertpos_t)horTabMax)) : (const uint8_t)0)
  #define MAXCOL(row)     (MAXCOL_RAW(row) >= HIDDEN_ROW ? MAXCOL_RAW(row) : (const uint8_t)(MAXCOL_RAW(row) & (~NAVIGATION_LINE_BY_LINE)))
  #define COLATTR(row)    (MAXCOL_RAW(row) == (uint8_t)-1 ? (const uint8_t)0 : (const uint8_t)(MAXCOL_RAW(row) & NAVIGATION_LINE_BY_LINE))
#else
  #define MAXCOL(row)     (horTab ? pgm_read_byte(horTab+min(row, (vertpos_t)horTabMax)) : (const uint8_t)0)
#endif

#define INC(val, min, max) if (val<max) {val++;} else {val=min;}
#define DEC(val, min, max) if (val>min) {val--;} else {val=max;}

#if LCD_W >= 212
coord_t scrollbar_X = LCD_W-1;
#endif

#if defined(CPUARM)
bool modelHasNotes()
{
  char filename[sizeof(MODELS_PATH)+1+sizeof(g_model.header.name)+sizeof(TEXT_EXT)] = MODELS_PATH "/";
  char *buf = strcat_modelname(&filename[sizeof(MODELS_PATH)], g_eeGeneral.currModel);
  strcpy(buf, TEXT_EXT);
  return isFileAvailable(filename);
}

void pushModelNotes()
{
  char filename[sizeof(MODELS_PATH)+1+sizeof(g_model.header.name)+sizeof(TEXT_EXT)] = MODELS_PATH "/";
  char *buf = strcat_modelname(&filename[sizeof(MODELS_PATH)], g_eeGeneral.currModel);
  strcpy(buf, TEXT_EXT);
  pushMenuTextView(filename);
}
#endif

#if defined(PCBTARANIS)
void onLongMenuPress(const char *result)
{
  if (result == STR_VIEW_CHANNELS) {
    pushMenu(menuChannelsView);
  }
  else if (result == STR_VIEW_NOTES) {
    pushModelNotes();
  }
}
#endif

#if defined(CPUARM)
tmr10ms_t menuEntryTime;
#endif

#if defined(PCBTARANIS)
void check(check_event_t event, uint8_t curr, const MenuFuncP *menuTab, uint8_t menuTabSize, const pm_uint8_t *horTab, uint8_t horTabMax, vertpos_t maxrow, uint8_t flags)
{
  vertpos_t l_posVert = m_posVert;
  horzpos_t l_posHorz = m_posHorz;

  uint8_t maxcol = MAXCOL(l_posVert);

#define scrollUD 0

  if (p2valdiff || scrollUD || p1valdiff) backlightOn(); // on keypress turn the light on

  if (menuTab) {
    int8_t cc = curr;
    switch (event) {
      case EVT_KEY_LONG(KEY_MENU):
        if (menuTab == menuTabModel) {
          killEvents(event);
          if (modelHasNotes()) {
            MENU_ADD_SD_ITEM(STR_VIEW_CHANNELS);
            MENU_ADD_ITEM(STR_VIEW_NOTES);
            menuHandler = onLongMenuPress;
          }
          else {
            pushMenu(menuChannelsView);
          }
        }
        break;

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

    if (!calibrationState && cc != curr) {
      chainMenu((MenuFuncP)pgm_read_adr(&menuTab[cc]));
    }

    if (!(flags&CHECK_FLAG_NO_SCREEN_INDEX)) {
      displayScreenIndex(curr, menuTabSize, 0);
    }

    lcd_filled_rect(0, 0, LCD_W, MENU_TITLE_HEIGHT, SOLID, FILL_WHITE|GREY_DEFAULT);
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
      menuEntryTime = get_tmr10ms();
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

    case EVT_ENTRY_UP:
      menuEntryTime = get_tmr10ms();
      s_editMode = 0;
      l_posHorz = POS_HORZ_INIT(l_posVert);
      SET_SCROLLBAR_X(LCD_W-1);
      break;

    case EVT_ROTARY_BREAK:
      if (s_editMode > 1) break;
      if (m_posHorz < 0 && maxcol > 0 && READ_ONLY_UNLOCKED()) {
        l_posHorz = 0;
        break;
      }
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

      if (l_posHorz >= 0 && (COLATTR(l_posVert) & NAVIGATION_LINE_BY_LINE)) {
        l_posHorz = -1;
      }
      else
      {
        uint8_t posVertInit = POS_VERT_INIT;
        if (s_pgOfs != 0 || l_posVert != posVertInit) {
          s_pgOfs = 0;
          l_posVert = posVertInit;
          l_posHorz = POS_HORZ_INIT(l_posVert);
        }
        else {
          popMenu();
        }
      }
      break;

    CASE_EVT_ROTARY_MOVE_RIGHT
      if (s_editMode != 0) break;
      if ((COLATTR(l_posVert) & NAVIGATION_LINE_BY_LINE)) {
        if (l_posHorz >= 0) {
          INC(l_posHorz, 0, maxcol);
          break;
        }
      }
      else {
        if (l_posHorz < maxcol) {
          l_posHorz++;
          break;
        }
        else {
          l_posHorz = 0;
          if (!IS_ROTARY_MOVE_RIGHT(event))
            break;
        }
      }

      do {
        INC(l_posVert, POS_VERT_INIT, maxrow);
      } while (CURSOR_NOT_ALLOWED_IN_ROW(l_posVert));

      s_editMode = 0; // if we go down, we must be in this mode

      l_posHorz = POS_HORZ_INIT(l_posVert);
      break;

    CASE_EVT_ROTARY_MOVE_LEFT
      if (s_editMode != 0) break;
      if ((COLATTR(l_posVert) & NAVIGATION_LINE_BY_LINE)) {
        if (l_posHorz >= 0) {
          DEC(l_posHorz, 0, maxcol);
          break;
        }
      }
      else {
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
      }

      do {
        DEC(l_posVert, POS_VERT_INIT, maxrow);
      } while (CURSOR_NOT_ALLOWED_IN_ROW(l_posVert));

      s_editMode = 0; // if we go up, we must be in this mode

      if ((COLATTR(l_posVert) & NAVIGATION_LINE_BY_LINE))
        l_posHorz = -1;
      else
        l_posHorz = min((uint8_t)l_posHorz, MAXCOL(l_posVert));

      break;
  }

  if (l_posVert == 0 || (l_posVert==2 && MAXCOL(vertpos_t(1)) >= HIDDEN_ROW) || (l_posVert==3 && MAXCOL(vertpos_t(1)) >= HIDDEN_ROW && MAXCOL(vertpos_t(2)) >= HIDDEN_ROW)) {
    s_pgOfs = 0;
  }
  else if (horTab) {
    uint8_t max = menuTab ? LCD_LINES-1 : LCD_LINES-2;
    if (maxrow > max) {
      while (1) {
        vertpos_t firstLine = 0;
        for (int numLines=0; firstLine<=maxrow && numLines<s_pgOfs; firstLine++) {
          if (horTab[firstLine+1] != HIDDEN_ROW) {
            numLines++;
          }
        }
        if (l_posVert <= firstLine) {
          s_pgOfs--;
        }
        else {
          vertpos_t lastLine = firstLine;
          for (int numLines=0; lastLine<=maxrow && numLines<max; lastLine++) {
            if (horTab[lastLine+1] != HIDDEN_ROW) {
              numLines++;
            }
          }
          if (l_posVert > lastLine) {
            s_pgOfs++;
          }
          else {
            break;
          }
        }
      }
    }
  }
  else {
    uint8_t max = menuTab ? LCD_LINES-1 : LCD_LINES-2;
    if (l_posVert>max+s_pgOfs) {
      s_pgOfs = l_posVert-max;
    }
    else if (l_posVert<1+s_pgOfs) {
      s_pgOfs = l_posVert-1;
    }
  }

  if (maxrow > LCD_LINES-1 && scrollbar_X) {
    displayScrollbar(scrollbar_X, MENU_TITLE_HEIGHT, LCD_H-MENU_TITLE_HEIGHT-MENU_NAVIG_HEIGHT, s_pgOfs, menuTab ? maxrow : maxrow+1, LCD_LINES-1);
  }

  m_posVert = l_posVert;
  m_posHorz = l_posHorz;
}

#else // defined(PCBTARANIS)

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

#if defined(ROTARY_ENCODER_NAVIGATION) || defined(PCBTARANIS)
      s_editMode = 0; // if we go up, we must be in this mode
#endif

      l_posHorz = min((uint8_t)l_posHorz, MAXCOL(l_posVert));
      break;
  }

#if defined(CPUARM)
  if (l_posVert<1) s_pgOfs=0;
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

#if LCD_W >= 212
  if (maxrow > LCD_LINES-1 && scrollbar_X)
    displayScrollbar(scrollbar_X, FH, LCD_H-FH, s_pgOfs, menuTab ? maxrow : maxrow+1, LCD_LINES-1);
#endif

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
#endif

MenuFuncP g_menuStack[5];
uint8_t menuEvent = 0;
uint8_t g_menuPos[4];
uint8_t g_menuStackPtr = 0;
vertpos_t m_posVert;
horzpos_t m_posHorz;

void popMenu()
{
  assert(g_menuStackPtr>0);
  g_menuStackPtr = g_menuStackPtr-1;
  menuEvent = EVT_ENTRY_UP;
}

void chainMenu(MenuFuncP newMenu)
{
  g_menuStack[g_menuStackPtr] = newMenu;
  menuEvent = EVT_ENTRY;
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

  g_menuStack[g_menuStackPtr] = newMenu;
  menuEvent = EVT_ENTRY;
}

const pm_char * s_warning = NULL;
const pm_char * s_warning_info;
uint8_t         s_warning_info_len;
uint8_t         s_warning_type;
uint8_t         s_warning_result = 0;

#if defined(CPUARM)
uint8_t s_warning_info_flags = ZCHAR;
int16_t s_warning_input_value;
int16_t s_warning_input_min;
int16_t s_warning_input_max;
#endif

void displayBox()
{
  lcd_filled_rect(10, 16, LCD_W-20, 40, SOLID, ERASE);
  lcd_rect(10, 16, LCD_W-20, 40);
#if defined(CPUARM)
  lcd_putsn(WARNING_LINE_X, WARNING_LINE_Y, s_warning, WARNING_LINE_LEN);
#else
  lcd_puts(WARNING_LINE_X, WARNING_LINE_Y, s_warning);
#endif
  // could be a place for a s_warning_info
}

void displayPopup(const pm_char * pstr)
{
  s_warning = pstr;
  displayBox();
  s_warning = NULL;
  lcdRefresh();
}

void displayWarning(uint8_t event)
{
  s_warning_result = false;
  displayBox();
  if (s_warning_info) {
    lcd_putsnAtt(16, WARNING_LINE_Y+FH, s_warning_info, s_warning_info_len, WARNING_INFO_FLAGS);
  }
  lcd_puts(16, WARNING_LINE_Y+2*FH, s_warning_type == WARNING_TYPE_ASTERISK ? STR_EXIT : STR_POPUPS);
  switch (event) {
#if defined(ROTARY_ENCODER_NAVIGATION)
    case EVT_ROTARY_BREAK:
#endif
    case EVT_KEY_BREAK(KEY_ENTER):
      if (s_warning_type == WARNING_TYPE_ASTERISK)
        break;
      s_warning_result = true;
      // no break
#if defined(ROTARY_ENCODER_NAVIGATION)
    case EVT_ROTARY_LONG:
      killEvents(event);
#endif
    case EVT_KEY_BREAK(KEY_EXIT):
      s_warning = NULL;
      s_warning_type = WARNING_TYPE_ASTERISK;
      break;
#if defined(CPUARM)
    default:
      if (s_warning_type != WARNING_TYPE_INPUT) break;
      s_editMode = EDIT_MODIFY_FIELD;
      s_warning_input_value = checkIncDec(event, s_warning_input_value, s_warning_input_min, s_warning_input_max);
      s_editMode = EDIT_SELECT_FIELD;
      break;
#endif
  }
}

select_menu_value_t selectMenuItem(coord_t x, coord_t y, const pm_char *label, const pm_char *values, select_menu_value_t value, select_menu_value_t min, select_menu_value_t max, LcdFlags attr, uint8_t event)
{
  lcd_putsColumnLeft(x, y, label);
  if (values) lcd_putsiAtt(x, y, values, value-min, attr);
  if (attr) value = checkIncDec(event, value, min, max, (g_menuPos[0] == 0) ? EE_MODEL : EE_GENERAL);
  return value;
}

uint8_t onoffMenuItem(uint8_t value, coord_t x, coord_t y, const pm_char *label, LcdFlags attr, uint8_t event )
{
#if defined(GRAPHICS)
  menu_lcd_onoff(x, y, value, attr);
  return selectMenuItem(x, y, label, NULL, value, 0, 1, attr, event);
#else
  return selectMenuItem(x, y, label, STR_OFFON, value, 0, 1, attr, event);
#endif
}

int8_t switchMenuItem(coord_t x, coord_t y, int8_t value, LcdFlags attr, uint8_t event)
{
  lcd_putsColumnLeft(x, y, STR_SWITCH);
  putsSwitches(x,  y, value, attr);
  if (attr) CHECK_INCDEC_MODELSWITCH(event, value, SWSRC_FIRST_IN_MIXES, SWSRC_LAST_IN_MIXES, isSwitchAvailableInMixes);
  return value;
}

#if !defined(CPUM64)
void displaySlider(coord_t x, coord_t y, uint8_t value, uint8_t max, uint8_t attr)
{
  lcd_putc(x+(value*4*FW)/max, y, '$');
  lcd_hline(x, y+3, 5*FW-1, FORCE);
  if (attr && (!(attr & BLINK) || !BLINK_ON_PHASE)) lcd_filled_rect(x, y, 5*FW-1, FH-1);
}
#elif defined(GRAPHICS)
void display5posSlider(coord_t x, coord_t y, uint8_t value, uint8_t attr)
{
  lcd_putc(x+2*FW+(value*FW), y, '$');
  lcd_hline(x, y+3, 5*FW-1, SOLID);
  if (attr && (!(attr & BLINK) || !BLINK_ON_PHASE)) lcd_filled_rect(x, y, 5*FW-1, FH-1);
}
#endif

#if defined(GVARS)
#if defined(CPUARM)
int16_t gvarMenuItem(coord_t x, coord_t y, int16_t value, int16_t min, int16_t max, LcdFlags attr, uint8_t editflags, uint8_t event)
#else
int16_t gvarMenuItem(coord_t x, coord_t y, int16_t value, int16_t min, int16_t max, LcdFlags attr, uint8_t event)
#endif
{
  uint16_t delta = GV_GET_GV1_VALUE(max);
  bool invers = (attr & INVERS);

  // TRACE("gvarMenuItem(val=%d min=%d max=%d)", value, min, max);

  if (invers && event == EVT_KEY_LONG(KEY_ENTER)) {
    s_editMode = !s_editMode;
#if defined(CPUARM)
    if (attr & PREC1)
      value = (GV_IS_GV_VALUE(value, min, max) ? GET_GVAR(value, min, max, mixerCurrentFlightMode)*10 : delta);
    else
      value = (GV_IS_GV_VALUE(value, min, max) ? GET_GVAR(value, min, max, mixerCurrentFlightMode) : delta);
#else
    value = (GV_IS_GV_VALUE(value, min, max) ? GET_GVAR(value, min, max, mixerCurrentFlightMode) : delta);
#endif
    eeDirty(EE_MODEL);
  }
  if (GV_IS_GV_VALUE(value, min, max)) {
    if (attr & LEFT)
      attr -= LEFT; /* because of ZCHAR */
    else
      x -= 2*FW+FWNUM;
    
#if defined(CPUARM)
    attr &= ~PREC1;
#endif

    int8_t idx = (int16_t) GV_INDEX_CALC_DELTA(value, delta);
    if (invers) {
      CHECK_INCDEC_MODELVAR(event, idx, -MAX_GVARS, MAX_GVARS-1);
    }

    if (idx < 0) { 
      value = (int16_t) GV_CALC_VALUE_IDX_NEG(idx, delta);
      idx = -idx;
      lcd_putcAtt(x-6, y, '-', attr);
    }
    else {
      value = (int16_t) GV_CALC_VALUE_IDX_POS(idx, delta);
      idx++;
    }
    putsStrIdx(x, y, STR_GV, idx, attr);
  }
  else {
    lcd_outdezAtt(x, y, value, attr);
#if defined(CPUARM)
    if (invers) value = checkIncDec(event, value, min, max, EE_MODEL | editflags);
#else
    if (invers) value = checkIncDec(event, value, min, max, EE_MODEL);
#endif
  }
  return value;
}
#else
int16_t gvarMenuItem(coord_t x, coord_t y, int16_t value, int16_t min, int16_t max, LcdFlags attr, uint8_t event)
{
  lcd_outdezAtt(x, y, value, attr);
  if (attr&INVERS) value = checkIncDec(event, value, min, max, EE_MODEL);
  return value;
}
#endif

void repeatLastCursorMove(uint8_t event)
{
  if (CURSOR_MOVED_LEFT(event) || CURSOR_MOVED_RIGHT(event)) {
    putEvent(event);
  }
  else {
    m_posHorz = 0;
  }
}

#if LCD_W >= 212
#define MENU_X   30
#define MENU_Y   16
#define MENU_W   LCD_W-60
#else
#define MENU_X   10
#define MENU_Y   16
#define MENU_W   LCD_W-20
#endif

#if defined(CPUARM)
void (*popupFunc)(uint8_t event) = NULL;
#endif

#if defined(NAVIGATION_MENUS)
const char *s_menu[MENU_MAX_LINES];
uint8_t s_menu_item = 0;
uint16_t s_menu_count = 0;
uint8_t s_menu_flags = 0;
uint16_t s_menu_offset = 0;
void (*menuHandler)(const char *result);
const char * displayMenu(uint8_t event)
{
  const char * result = NULL;

  uint8_t display_count = min(s_menu_count, (uint16_t)MENU_MAX_LINES);
  uint8_t y = (display_count >= 5 ? MENU_Y - FH - 1 : MENU_Y);
  lcd_filled_rect(MENU_X, y, MENU_W, display_count * (FH+1) + 2, SOLID, ERASE);
  lcd_rect(MENU_X, y, MENU_W, display_count * (FH+1) + 2);

  for (uint8_t i=0; i<display_count; i++) {
    lcd_putsAtt(MENU_X+6, i*(FH+1) + y + 2, s_menu[i], s_menu_flags);
    if (i == s_menu_item) lcd_filled_rect(MENU_X+1, i*(FH+1) + y + 1, MENU_W-2, 9);
  }

  if (s_menu_count > display_count) {
    displayScrollbar(MENU_X+MENU_W-1, y+1, MENU_MAX_LINES * (FH+1), s_menu_offset, s_menu_count, MENU_MAX_LINES);
  }

  switch(event) {
#if defined(ROTARY_ENCODER_NAVIGATION)
    CASE_EVT_ROTARY_LEFT
#endif
    case EVT_KEY_FIRST(KEY_MOVE_UP):
    case EVT_KEY_REPT(KEY_MOVE_UP):
      if (s_menu_item > 0) {
        s_menu_item--;
      }
#if defined(SDCARD)
      else if (s_menu_offset > 0) {
        s_menu_offset--;
        result = STR_UPDATE_LIST;
      }
#endif
      else {
        s_menu_item = display_count - 1;
#if defined(SDCARD)
        if (s_menu_count > MENU_MAX_LINES) {
          s_menu_offset = s_menu_count - display_count;
          result = STR_UPDATE_LIST;
        }
#endif
      }
      break;

#if defined(ROTARY_ENCODER_NAVIGATION)
    CASE_EVT_ROTARY_RIGHT
#endif
    case EVT_KEY_FIRST(KEY_MOVE_DOWN):
    case EVT_KEY_REPT(KEY_MOVE_DOWN):
      if (s_menu_item < display_count - 1 && s_menu_offset + s_menu_item + 1 < s_menu_count) {
        s_menu_item++;
      }
#if defined(SDCARD)
      else if (s_menu_count > s_menu_offset + display_count) {
        s_menu_offset++;
        result = STR_UPDATE_LIST;
      }
#endif
      else {
        s_menu_item = 0;
#if defined(SDCARD)
        if (s_menu_offset) {
          s_menu_offset = 0;
          result = STR_UPDATE_LIST;
        }
#endif
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
#endif

#if defined(SDCARD)
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
      if (statusLineHeight < FH) statusLineHeight++;
    }
    else if (statusLineHeight) {
      statusLineHeight--;
    }
    else {
      statusLineTime = 0;
    }

    lcd_filled_rect(0, LCD_H-statusLineHeight, LCD_W, FH, SOLID, ERASE);
    lcd_putsAtt(5, LCD_H+1-statusLineHeight, statusLineMsg, BSS);
    lcd_filled_rect(0, LCD_H-statusLineHeight, LCD_W, FH, SOLID);
  }
}
#endif

#if defined(PCBTARANIS)
bool isInputAvailable(int input)
{
  for (int i=0; i<MAX_EXPOS; i++) {
    ExpoData * expo = expoAddress(i);
    if (!EXPO_VALID(expo))
      break;
    if (expo->chn == input)
      return true;
  }
  return false;
}
#endif

#if defined(CPUARM)
bool isSourceAvailable(int source)
{
#if defined(PCBTARANIS)
  if (source>=MIXSRC_FIRST_INPUT && source<=MIXSRC_LAST_INPUT) {
    return isInputAvailable(source - MIXSRC_FIRST_INPUT);
  }
#endif

#if defined(LUA_MODEL_SCRIPTS)
  if (source>=MIXSRC_FIRST_LUA && source<=MIXSRC_LAST_LUA) {
    div_t qr = div(source-MIXSRC_FIRST_LUA, MAX_SCRIPT_OUTPUTS);
    return (qr.rem<scriptInputsOutputs[qr.quot].outputsCount);
  }
#elif defined(PCBTARANIS)
  if (source>=MIXSRC_FIRST_LUA && source<=MIXSRC_LAST_LUA)
    return false;
#endif

#if defined(PCBTARANIS)
  if (source>=MIXSRC_FIRST_POT && source<=MIXSRC_LAST_POT) {
    return IS_POT_AVAILABLE(POT1+source-MIXSRC_FIRST_POT);
  }
#endif

#if !defined(HELI)
  if (source>=MIXSRC_CYC1 && source<=MIXSRC_CYC3)
    return false;
#endif

  if (source>=MIXSRC_CH1 && source<=MIXSRC_LAST_CH) {
    uint8_t destCh = source-MIXSRC_CH1;
    for (uint8_t i = 0; i < MAX_MIXERS; i++) {
      MixData *md = mixAddress(i);
      if (md->srcRaw == 0) return false;
      if (md->destCh==destCh) return true;
    }
    return false;
  }

#if defined(PCBTARANIS)
  if (source>=MIXSRC_SI && source<=MIXSRC_SN) {
    if (!IS_2x2POS(source-MIXSRC_SI)) {
      return false;
    }
  }
#endif

  if (source>=MIXSRC_SW1 && source<=MIXSRC_LAST_LOGICAL_SWITCH) {
    LogicalSwitchData * cs = lswAddress(source-MIXSRC_SW1);
    return (cs->func != LS_FUNC_NONE);
  }

#if !defined(GVARS)
  if (source>=MIXSRC_GVAR1 && source<=MIXSRC_LAST_GVAR)
    return false;
#endif

  if (source>=MIXSRC_RESERVE1 && source<=MIXSRC_RESERVE5)
    return false;

  if (source>=MIXSRC_FIRST_TELEM && source<=MIXSRC_LAST_TELEM) {
    div_t qr = div(source-MIXSRC_FIRST_TELEM, 3);
    if (qr.rem == 0)
      return isTelemetryFieldAvailable(qr.quot);
    else
      return isTelemetryFieldComparisonAvailable(qr.quot);
  }

  return true;
}

bool isSourceAvailableInCustomSwitches(int source)
{
  bool result = isSourceAvailable(source);

  if (result && source>=MIXSRC_FIRST_TELEM && source<=MIXSRC_LAST_TELEM) {
    div_t qr = div(source-MIXSRC_FIRST_TELEM, 3);
    result = isTelemetryFieldComparisonAvailable(qr.quot);
  }

  return result;
}

bool isInputSourceAvailable(int source)
{
#if defined(PCBTARANIS) && !defined(REVPLUS)
  if (source == MIXSRC_POT3)
    return false;
#endif

  if (source>=MIXSRC_Rud && source<=MIXSRC_MAX)
    return true;

  if (source>=MIXSRC_Rud && source<=MIXSRC_MAX)
    return true;

  if (source>=MIXSRC_TrimRud && source<MIXSRC_SW1)
    return true;

  if (source>=MIXSRC_FIRST_CH && source<=MIXSRC_LAST_CH)
    return true;

  if (source>=MIXSRC_FIRST_TRAINER && source<=MIXSRC_LAST_TRAINER)
    return true;

  if (source>=MIXSRC_FIRST_TELEM && source<=MIXSRC_LAST_TELEM)
    return isTelemetryFieldAvailable(source-MIXSRC_FIRST_TELEM);

  return false;
}

enum SwitchContext
{
  LogicalSwitchesContext,
  ModelCustomFunctionsContext,
  GeneralCustomFunctionsContext,
  TimersContext,
  MixesContext
};

bool isSwitchAvailable(int swtch, SwitchContext context)
{
  if (swtch < 0) {
#if defined(PCBTARANIS)
    if (swtch == -SWSRC_SF0 || swtch == -SWSRC_SF2 || swtch == -SWSRC_SH0 || swtch == -SWSRC_SH2)
      return false;
#endif
    if (swtch == -SWSRC_ON || swtch == -SWSRC_One) {
      return false;
    }
    swtch = -swtch;
  }

#if defined(PCBTARANIS)
  if (swtch == SWSRC_SA1 || swtch == SWSRC_SB1 || swtch == SWSRC_SC1 || swtch == SWSRC_SD1 || swtch == SWSRC_SE1 || swtch == SWSRC_SG1) {
    return IS_3POS((swtch-SWSRC_SA0)/3);
  }
  if (swtch >= SWSRC_SI0 && swtch <= SWSRC_SM2) {
    return IS_2x2POS((swtch-SWSRC_SI0)/2);
  }
  if (swtch >= SWSRC_SN0 && swtch <= SWSRC_SN2) {
    return IS_2x2POS(6);
  }
#endif

#if defined(PCBTARANIS)
  if (swtch >= SWSRC_FIRST_MULTIPOS_SWITCH && swtch <= SWSRC_LAST_MULTIPOS_SWITCH) {
    int index = (swtch - SWSRC_FIRST_MULTIPOS_SWITCH) / XPOTS_MULTIPOS_COUNT;
    if (IS_POT_MULTIPOS(POT1+index)) {
      StepsCalibData * calib = (StepsCalibData *) &g_eeGeneral.calib[POT1+index];
      return (calib->count >= ((swtch - SWSRC_FIRST_MULTIPOS_SWITCH) % XPOTS_MULTIPOS_COUNT));
    }
    else {
      return false;
    }
  }
#endif

  if (swtch >= SWSRC_FIRST_LOGICAL_SWITCH && swtch <= SWSRC_LAST_LOGICAL_SWITCH) {
    if (context == GeneralCustomFunctionsContext) {
      return false;
    }
    else if (context != LogicalSwitchesContext) {
      LogicalSwitchData * cs = lswAddress(swtch-SWSRC_FIRST_LOGICAL_SWITCH);
      return (cs->func != LS_FUNC_NONE);
    }
  }

  if (context != ModelCustomFunctionsContext && context != GeneralCustomFunctionsContext && (swtch == SWSRC_ON || swtch == SWSRC_One)) {
    return false;
  }

  if (swtch >= SWSRC_FIRST_FLIGHT_MODE && swtch <= SWSRC_LAST_FLIGHT_MODE) {
    if (context == MixesContext || context == GeneralCustomFunctionsContext) {
      return false;
    }
    else {
      swtch -= SWSRC_FIRST_FLIGHT_MODE;
      if (swtch == 0) {
        return true;
      }
      FlightModeData * fm = flightModeAddress(swtch);
      return (fm->swtch != SWSRC_NONE);
    }
  }
  
  return true;
}

bool isSwitchAvailableInLogicalSwitches(int swtch)
{
  return isSwitchAvailable(swtch, LogicalSwitchesContext);
}

bool isSwitchAvailableInCustomFunctions(int swtch)
{
  if (g_menuStack[g_menuStackPtr] == menuModelCustomFunctions)
    return isSwitchAvailable(swtch, ModelCustomFunctionsContext);
  else
    return isSwitchAvailable(swtch, GeneralCustomFunctionsContext);
}

bool isSwitchAvailableInMixes(int swtch)
{
  return isSwitchAvailable(swtch, MixesContext);
}

bool isSwitchAvailableInTimers(int swtch)
{
  if (swtch >= 0) {
    if (swtch < TMRMODE_COUNT) {
      return true;
    }
    swtch -= TMRMODE_COUNT-1;
  }

  return isSwitchAvailable(swtch, TimersContext);
}

bool isThrottleSourceAvailable(int source)
{
#if defined(PCBTARANIS)
  if (source == THROTTLE_SOURCE_S3 && !IS_POT_AVAILABLE(POT3))
    return false;
#endif
  return true;
}

bool isLogicalSwitchFunctionAvailable(int function)
{
  return function != LS_FUNC_RANGE;
}

bool isAssignableFunctionAvailable(int function)
{
  bool modelFunctions = (g_menuStack[g_menuStackPtr] == menuModelCustomFunctions);

  switch (function) {
    case FUNC_OVERRIDE_CHANNEL:
#if defined(OVERRIDE_CHANNEL_FUNCTION)
      return modelFunctions;
#else
      return false;
#endif
    case FUNC_ADJUST_GVAR:
#if defined(GVARS)
      return modelFunctions;
#else
      return false;
#endif
#if !defined(HAPTIC)
    case FUNC_HAPTIC:
#endif
    case FUNC_PLAY_DIFF:
    case FUNC_RESERVE1:
    case FUNC_RESERVE2:
    case FUNC_RESERVE3:
#if !defined(LUA)
    case FUNC_PLAY_SCRIPT:
#endif
    case FUNC_RESERVE5:
      return false;

    default:
      return true;
  }
}

bool isModuleAvailable(int module)
{
#if defined(PCBSKY9X)
  if (module == MODULE_TYPE_NONE)
    return false;
#endif

#if !defined(PXX)
  if (module == MODULE_TYPE_XJT)
    return false;
#endif

  return true;
}

#endif
