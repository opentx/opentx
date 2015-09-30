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

void drawStick(coord_t centrex, int16_t xval, int16_t yval)
{
#define BOX_CENTERY   (LCD_H-9-BOX_WIDTH/2)
#define MARKER_WIDTH  5
  lcd_square(centrex-BOX_WIDTH/2, BOX_CENTERY-BOX_WIDTH/2, BOX_WIDTH);
  lcd_vline(centrex, BOX_CENTERY-1, 3);
  lcd_hline(centrex-1, BOX_CENTERY, 3);
  lcd_square(centrex + (xval/((2*RESX)/(BOX_WIDTH-MARKER_WIDTH))) - MARKER_WIDTH/2, BOX_CENTERY - (yval/((2*RESX)/(BOX_WIDTH-MARKER_WIDTH))) - MARKER_WIDTH/2, MARKER_WIDTH, ROUND);
#undef BOX_CENTERY
#undef MARKER_WIDTH
}

void menu_lcd_onoff(coord_t x, coord_t y, uint8_t value, LcdFlags attr)
{
#if defined(GRAPHICS)
  if (value)
    lcd_putc(x+1, y, '#');
  if (attr)
    drawFilledRect(x, y, 7, 7);
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
  lcd_vlineStip(x, y, h, DOTTED);
  coord_t yofs = (h * offset) / count;
  coord_t yhgt = (h * visible) / count;
  if (yhgt + yofs > h)
    yhgt = h - yofs;
  lcd_vlineStip(x, y + yofs, yhgt, SOLID, FORCE);
}
#endif

void title(const pm_char * s)
{
  lcd_putsAtt(0, 0, s, INVERS);
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
  if (attr && (!(attr & BLINK) || !BLINK_ON_PHASE)) drawFilledRect(x, y, 5*FW-1, FH-1);
}
#elif defined(GRAPHICS)
void display5posSlider(coord_t x, coord_t y, uint8_t value, uint8_t attr)
{
  lcd_putc(x+2*FW+(value*FW), y, '$');
  lcd_hline(x, y+3, 5*FW-1, SOLID);
  if (attr && (!(attr & BLINK) || !BLINK_ON_PHASE)) drawFilledRect(x, y, 5*FW-1, FH-1);
}
#endif

#if defined(GVARS) && defined(CPUARM)
bool noZero(int val)
{
  return val != 0;
}

int16_t gvarMenuItem(coord_t x, coord_t y, int16_t value, int16_t min, int16_t max, LcdFlags attr, uint8_t editflags, uint8_t event)
{
  uint16_t delta = GV_GET_GV1_VALUE(max);
  bool invers = (attr & INVERS);

  // TRACE("gvarMenuItem(val=%d min=%d max=%d)", value, min, max);

  if (invers && event == EVT_KEY_LONG(KEY_ENTER)) {
    s_editMode = !s_editMode;
    if (attr & PREC1)
      value = (GV_IS_GV_VALUE(value, min, max) ? GET_GVAR(value, min, max, mixerCurrentFlightMode)*10 : delta);
    else
      value = (GV_IS_GV_VALUE(value, min, max) ? GET_GVAR(value, min, max, mixerCurrentFlightMode) : delta);
    eeDirty(EE_MODEL);
  }

  if (GV_IS_GV_VALUE(value, min, max)) {
    if (attr & LEFT)
      attr -= LEFT; /* because of ZCHAR */
    else
      x -= 2*FW+FWNUM;

    attr &= ~PREC1;

    int8_t idx = (int16_t) GV_INDEX_CALC_DELTA(value, delta);
    if (idx >= 0) ++idx;    // transform form idx=0=GV1 to idx=1=GV1 in order to handle double keys invert
    if (invers) {
      CHECK_INCDEC_MODELVAR_CHECK(event, idx, -MAX_GVARS, MAX_GVARS, noZero);
      if (idx == 0) idx = 1;    // handle reset to zero, map to GV1
    }
    if (idx < 0) {
      value = (int16_t) GV_CALC_VALUE_IDX_NEG(idx, delta);
      idx = -idx;
      lcd_putcAtt(x-6, y, '-', attr);
    }
    else {
      value = (int16_t) GV_CALC_VALUE_IDX_POS(idx-1, delta);
    }
    putsStrIdx(x, y, STR_GV, idx, attr);
  }
  else {
    lcd_outdezAtt(x, y, value, attr);
    if (invers) value = checkIncDec(event, value, min, max, EE_MODEL | editflags);
  }
  return value;
}
#elif defined(GVARS)
int16_t gvarMenuItem(coord_t x, coord_t y, int16_t value, int16_t min, int16_t max, LcdFlags attr, uint8_t event)
{
  uint16_t delta = GV_GET_GV1_VALUE(max);
  bool invers = (attr & INVERS);

  // TRACE("gvarMenuItem(val=%d min=%d max=%d)", value, min, max);

  if (invers && event == EVT_KEY_LONG(KEY_ENTER)) {
    s_editMode = !s_editMode;
    value = (GV_IS_GV_VALUE(value, min, max) ? GET_GVAR(value, min, max, mixerCurrentFlightMode) : delta);
    eeDirty(EE_MODEL);
  }
  if (GV_IS_GV_VALUE(value, min, max)) {
    if (attr & LEFT)
      attr -= LEFT; /* because of ZCHAR */
    else
      x -= 2*FW+FWNUM;

    int8_t idx = (int16_t) GV_INDEX_CALC_DELTA(value, delta);
    if (invers) {
      idx = checkIncDec(event, idx, -MAX_GVARS, MAX_GVARS-1, EE_MODEL|NO_DBLKEYS);   // disable double keys
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
    if (invers) value = checkIncDec(event, value, min, max, EE_MODEL);
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

    drawFilledRect(0, LCD_H-statusLineHeight, LCD_W, FH, SOLID, ERASE);
    lcd_putsAtt(5, LCD_H+1-statusLineHeight, statusLineMsg, BSS);
    drawFilledRect(0, LCD_H-statusLineHeight, LCD_W, FH, SOLID);
  }
}
#endif
