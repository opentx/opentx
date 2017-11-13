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
#include <stdio.h>

void drawStatusText(const char * text)
{
  lcdDrawText(MENUS_MARGIN_LEFT, MENU_FOOTER_TOP, text, TEXT_STATUSBAR_COLOR);
}

void drawColumnHeader(const char * const * headers, const char * const * descriptions, uint8_t index)
{
  lcdDrawText(LCD_W-MENUS_MARGIN_LEFT, MENU_TITLE_TOP + 2, headers[index], RIGHT | SMLSIZE | MENU_TITLE_COLOR);
  if (descriptions) {
    drawStatusText(descriptions[index]);
  }
}

#include "alpha_button_on.lbm"
#include "alpha_button_off.lbm"

void drawButton(coord_t x, coord_t y, const char * label, LcdFlags attr)
{
  int width = getTextWidth(label, 0, 0);
  int padding = 0;
  if (attr & (BUTTON_OFF|BUTTON_ON)) {
    padding = 5;
  }
  if (attr & INVERS) {
    lcdDrawSolidFilledRect(x, y, padding+width+16+padding, 19, TEXT_INVERTED_BGCOLOR);
    lcdDrawText(x+padding+8, y, label, TEXT_INVERTED_COLOR);
  }
  else {
    lcdDrawText(x+padding+8, y, label, TEXT_COLOR);
  }
  lcdDrawSolidRect(x-1, y-1, padding+width+18+padding, 21, 1, TEXT_COLOR);
  if (attr & BUTTON_OFF)
    lcd->drawBitmap(x-6, y+3, &ALPHA_BUTTON_OFF);
  else if (attr & BUTTON_ON)
    lcd->drawBitmap(x-6, y+3, &ALPHA_BUTTON_ON);
}

void drawCheckBox(coord_t x, coord_t y, uint8_t value, LcdFlags attr)
{
  if (attr) {
    lcdDrawSolidFilledRect(x-1, y+3, 14, 14, TEXT_INVERTED_BGCOLOR);
    lcdDrawSolidFilledRect(x+1, y+5, 10, 10, TEXT_BGCOLOR);
    if (value) {
      lcdDrawSolidFilledRect(x+2, y+6, 8, 8, TEXT_INVERTED_BGCOLOR);
    }
  }
  else {
    if (value) {
      lcdDrawSolidFilledRect(x+2, y+6, 8, 8, SCROLLBOX_COLOR);
      lcdDrawSolidRect(x, y+4, 12, 12, 1, LINE_COLOR);
    }
    else {
      lcdDrawSolidRect(x, y+4, 12, 12, 1, LINE_COLOR);
    }
  }
}

void drawVerticalScrollbar(coord_t x, coord_t y, coord_t h, uint16_t offset, uint16_t count, uint8_t visible)
{
  if (visible < count) {
    lcdDrawSolidVerticalLine(x, y, h, LINE_COLOR);
    coord_t yofs = (h*offset + count/2) / count;
    coord_t yhgt = (h*visible + count/2) / count;
    if (yhgt + yofs > h)
      yhgt = h - yofs;
    lcdDrawSolidFilledRect(x-1, y + yofs, 3, yhgt, SCROLLBOX_COLOR);
  }
}

void drawHorizontalScrollbar(coord_t x, coord_t y, coord_t w, uint16_t offset, uint16_t count, uint8_t visible)
{
  if (visible < count) {
    lcdDrawSolidHorizontalLine(x, y, w, LINE_COLOR);
    coord_t xofs = (w*offset + count/2) / count;
    coord_t xhgt = (w*visible + count/2) / count;
    if (xhgt + xofs > w)
      xhgt = w - xofs;
    lcdDrawSolidFilledRect(x+xofs, y-1, xhgt, 3, SCROLLBOX_COLOR);
  }
}

void drawProgressBar(const char * label, int num, int den)
{
  lcdClear();
  lcdSetColor(WHITE);
  if (label) {
    lcdDrawText(MENUS_MARGIN_LEFT, LCD_H-42, label, CUSTOM_COLOR);
  }
  lcdDrawRect(MENUS_MARGIN_LEFT, LCD_H-22, LCD_W-2*MENUS_MARGIN_LEFT, 15, 1, SOLID, CUSTOM_COLOR);
  lcdSetColor(RED);
  if (num > 0 && den > 0) {
    int width = ((LCD_W-2*MENUS_MARGIN_LEFT-4) * num) / den;
    lcdDrawSolidFilledRect(MENUS_MARGIN_LEFT+2, LCD_H-20, width, 11, CUSTOM_COLOR);
  }
  lcdRefresh();
}

void drawShadow(coord_t x, coord_t y, coord_t w, coord_t h)
{
  lcdDrawSolidVerticalLine(x+w, y+1, h, TEXT_COLOR);
  lcdDrawSolidHorizontalLine(x+1, y+h, w, TEXT_COLOR);
  lcdDrawSolidVerticalLine(x+w+1, y+2, h, LINE_COLOR);
  lcdDrawSolidHorizontalLine(x+2, y+h+1, w, LINE_COLOR);
}

void drawMenuTemplate(const char * title, uint8_t icon, const uint8_t * icons, uint32_t options)
{
  coord_t bodyTop, bodyBottom;

  theme->drawTopbarBackground(icons ? icons[0] : icon);

  // Menu title bar
  if (options & OPTION_MENU_TITLE_BAR) {
    linesDisplayed = NUM_BODY_LINES;
    bodyTop = MENU_BODY_TOP;
    lcdDrawSolidFilledRect(0, MENU_HEADER_HEIGHT, LCD_W, MENU_TITLE_TOP - MENU_HEADER_HEIGHT, TEXT_BGCOLOR);
    lcdDrawSolidFilledRect(0, MENU_TITLE_TOP, LCD_W, MENU_TITLE_HEIGHT, TITLE_BGCOLOR);
    if (title) {
      lcdDrawText(MENUS_MARGIN_LEFT, MENU_TITLE_TOP+1, title, MENU_TITLE_COLOR);
    }
    if (icons) {
      for (int i=0; i<menuPageCount; i++) {
        if (menuPageIndex != i) {
          theme->drawMenuIcon(icons[i+1], i, false);
        }
      }
      theme->drawMenuIcon(icons[menuPageIndex+1], menuPageIndex, true);
    }
  }
  else {
    linesDisplayed = NUM_BODY_LINES + 1;
    bodyTop = MENU_HEADER_HEIGHT;
    if (title) {
      lcdDrawText(50, 3, title, MENU_TITLE_COLOR);
    }
  }

  // Footer
  if (options & OPTION_MENU_NO_FOOTER) {
    bodyBottom = LCD_H;
  }
  else {
    bodyBottom = MENU_FOOTER_TOP;
    lcdDrawSolidFilledRect(0, MENU_FOOTER_TOP, LCD_W, MENU_FOOTER_HEIGHT, HEADER_BGCOLOR);
  }

  // Body
  lcdDrawSolidFilledRect(0, bodyTop, LCD_W, bodyBottom-bodyTop, TEXT_BGCOLOR);

  // Scrollbar
  if (!(options & OPTION_MENU_NO_SCROLLBAR) && linesCount>linesDisplayed) {
    drawVerticalScrollbar(DEFAULT_SCROLLBAR_X, bodyTop+3, bodyBottom-bodyTop-6, menuVerticalOffset, linesCount, linesDisplayed);
  }
}

void drawTrimSquare(coord_t x, coord_t y)
{
  lcdDrawSolidFilledRect(x-2, y, 15, 15, TRIM_BGCOLOR);
  lcdDrawBitmapPattern(x-2, y, LBM_TRIM_SHADOW, TRIM_SHADOW_COLOR);
}

void drawHorizontalTrimPosition(coord_t x, coord_t y, int16_t dir)
{
  drawTrimSquare(x, y);
  if (dir >= 0) {
    lcdDrawSolidVerticalLine(x+8, y+3, 9, TEXT_INVERTED_COLOR);
  }
  if (dir <= 0) {
    lcdDrawSolidVerticalLine(x+2, y+3, 9, TEXT_INVERTED_COLOR);
  }
  // if (exttrim) {
  //  lcdDrawSolidVerticalLine(xm, ym, 9, TEXT_INVERTED_COLOR);
  // }
}

void drawVerticalTrimPosition(coord_t x, coord_t y, int16_t dir)
{
  drawTrimSquare(x, y);
  if (dir >= 0) {
    lcdDrawSolidHorizontalLine(x+1, y+4, 9, TEXT_INVERTED_COLOR);
  }
  if (dir <= 0) {
    lcdDrawSolidHorizontalLine(x+1, y+10, 9, TEXT_INVERTED_COLOR);
  }
  // if (exttrim) {
  //   lcdDrawSolidHorizontalLine(xm-1, ym,  3, TEXT_INVERTED_COLOR);
  // }
}

void drawVerticalSlider(coord_t x, coord_t y, int len, int val, int min, int max, uint8_t steps, uint32_t options)
{
  val = limit(min, val, max);
  if (steps) {
    int delta = len / steps;
    for (int i = 0; i <= len; i += delta) {
      if ((options & OPTION_SLIDER_BIG_TICKS) && (i == 0 || i == len / 2 || i == len))
        lcdDrawSolidHorizontalLine(x, y + i, 13, TEXT_COLOR);
      else
        lcdDrawSolidHorizontalLine(x + 2, y + i, 9, TEXT_COLOR);
    }
  }
  else {
    lcdDrawBitmapPattern(x + 1, y, LBM_VTRIM_FRAME, TEXT_COLOR);
    /* if (g_model.displayTrims != DISPLAY_TRIMS_NEVER && trim != 0) {
      if (g_model.displayTrims == DISPLAY_TRIMS_ALWAYS || (trimsDisplayTimer > 0 && (trimsDisplayMask & (1<<i)))) {
        lcdDrawNumber((stickIndex==0 ? TRIM_LH_X : TRIM_RH_X)+(trim>0 ? -20 : 50), ym+1, trim, TINSIZE);
      }
    } */
  }
  y += len - divRoundClosest(len * (val - min), max - min) - 5;
  if (options & OPTION_SLIDER_TRIM_BUTTON) {
    drawVerticalTrimPosition(x, y - 2, val);
  }
  else if (options & OPTION_SLIDER_NUMBER_BUTTON) {
    drawTrimSquare(x, y - 2);
    lcdDrawChar(x + 2, y - 1, '0' + val, SMLSIZE | TEXT_INVERTED_COLOR);
  }
  else {
    drawTrimSquare(x, y - 2);
  }
}

void drawHorizontalSlider(coord_t x, coord_t y, int len, int val, int min, int max, uint8_t steps, uint32_t options)
{
  val = limit(min, val, max);
  int w = divRoundClosest(len * (val - min), max - min);
  if (options & OPTION_SLIDER_TICKS) {
    if (steps) {
      int delta = len / steps;
      for (int i = 0; i <= len; i += delta) {
        if ((options & OPTION_SLIDER_BIG_TICKS) && (i == 0 || i == len / 2 || i == len))
          lcdDrawSolidVerticalLine(x + i, y, 13, TEXT_COLOR);
        else
          lcdDrawSolidVerticalLine(x + i, y + 2, 9, TEXT_COLOR);
      }
    }
  }
  else if (options & OPTION_SLIDER_EMPTY_BAR) {
    lcdDrawBitmapPattern(x, y + 1, LBM_HTRIM_FRAME, TEXT_COLOR);
  }
  else if (options & OPTION_SLIDER_DBL_COLOR) {
    lcdDrawBitmapPattern(x, y + 8, LBM_SLIDER_BAR_LEFT, w <= 0 ? LINE_COLOR : TEXT_INVERTED_BGCOLOR);
    if (w > 4)
      lcdDrawSolidFilledRect(x + 4, y + 8, w - 4, 4, TEXT_INVERTED_BGCOLOR);
    if (w < len - 4)
      lcdDrawSolidFilledRect(x + w, y + 8, len - w - 4, 4, LINE_COLOR);
    lcdDrawBitmapPattern(x + len - 4, y + 8, LBM_SLIDER_BAR_RIGHT, w >= len ? TEXT_INVERTED_BGCOLOR : LINE_COLOR);
  }
  else {
    lcdDrawBitmapPattern(x, y + 8, LBM_SLIDER_BAR_LEFT, LINE_COLOR);
    lcdDrawSolidFilledRect(x + 4, y + 8, len - 8, 4, LINE_COLOR);
    lcdDrawBitmapPattern(x + len - 4, y + 8, LBM_SLIDER_BAR_RIGHT, LINE_COLOR);
    //
    /* if (g_model.displayTrims != DISPLAY_TRIMS_NEVER && trim != 0) {
      if (g_model.displayTrims == DISPLAY_TRIMS_ALWAYS || (trimsDisplayTimer > 0 && (trimsDisplayMask & (1<<i)))) {
        lcdDrawNumber((stickIndex==0 ? TRIM_LH_X : TRIM_RH_X)+(trim>0 ? -20 : 50), ym+1, trim, TINSIZE);
      }
    } */
  }
  x += w - 5;
  if (options & OPTION_SLIDER_TRIM_BUTTON) {
    drawHorizontalTrimPosition(x, y - 1, val);
  }
  else if (options & OPTION_SLIDER_NUMBER_BUTTON) {
    drawTrimSquare(x+2, y - 1);
    char text[] = { (char)('0' + val), '\0' };
    lcdDrawText(x + 7, y - 1, text, SMLSIZE | CENTERED | TEXT_INVERTED_COLOR);
  }
  else if (options & OPTION_SLIDER_SQUARE_BUTTON) {
    drawTrimSquare(x, y - 1);
  }
  else {
    lcdDrawBitmapPattern(x, y + 2, LBM_SLIDER_POINT_OUT, TEXT_COLOR);
    lcdDrawBitmapPattern(x, y + 2, LBM_SLIDER_POINT_MID, TEXT_BGCOLOR);
    if ((options & INVERS) && (!(options & BLINK) || !BLINK_ON_PHASE))
      lcdDrawBitmapPattern(x, y + 2, LBM_SLIDER_POINT_IN, TEXT_INVERTED_BGCOLOR);
  }
}

void drawSlider(coord_t x, coord_t y, int len, int val, int min, int max, uint8_t steps, uint32_t options)
{
  if (options & OPTION_SLIDER_VERTICAL)
    drawVerticalSlider(x, y, len, val, min, max, steps, options);
  else
    drawHorizontalSlider(x, y, len, val, min, max, steps, options);
}

#if defined(GVARS)
bool noZero(int val)
{
  return val != 0;
}

void drawGVarValue(coord_t x, coord_t y, uint8_t gvar, gvar_t value, LcdFlags flags)
{
  uint8_t prec = g_model.gvars[gvar].prec;
  if (prec > 0) {
    flags |= (prec == 1 ? PREC1 : PREC2);
  }
  drawValueWithUnit(x, y, value, g_model.gvars[gvar].unit ? UNIT_PERCENT : UNIT_RAW, flags);
}

int16_t editGVarFieldValue(coord_t x, coord_t y, int16_t value, int16_t min, int16_t max, LcdFlags attr, uint8_t editflags, event_t event)
{
  uint16_t delta = GV_GET_GV1_VALUE(max);
  bool invers = (attr & INVERS);

  // TRACE("editGVarFieldValue(val=%d min=%d max=%d)", value, min, max);

  if (invers && event == EVT_KEY_LONG(KEY_ENTER)) {
    s_editMode = !s_editMode;
    if (attr & PREC1)
      value = (GV_IS_GV_VALUE(value, min, max) ? GET_GVAR(value, min, max, mixerCurrentFlightMode)*10 : delta);
    else
      value = (GV_IS_GV_VALUE(value, min, max) ? GET_GVAR(value, min, max, mixerCurrentFlightMode) : delta);
    storageDirty(EE_MODEL);
  }

  if (GV_IS_GV_VALUE(value, min, max)) {
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
      drawStringWithIndex(x, y, STR_GV, idx, attr, "-");
    }
    else {
      drawStringWithIndex(x, y, STR_GV, idx, attr);
      value = (int16_t) GV_CALC_VALUE_IDX_POS(idx-1, delta);
    }
  }
  else {
    lcdDrawNumber(x, y, value, attr, 0, NULL, "%");
    if (invers) value = checkIncDec(event, value, min, max, EE_MODEL | editflags);
  }
  return value;
}
#else
int16_t editGVarFieldValue(coord_t x, coord_t y, int16_t value, int16_t min, int16_t max, LcdFlags attr, event_t event)
{
  if (attr & INVERS) value = checkIncDec(event, value, min, max, EE_MODEL);
  lcdDrawNumber(x, y, value, attr, 0, NULL, "%");
  return value;
}
#endif

void drawSleepBitmap()
{
  lcd->clear();
  const BitmapBuffer * bitmap = BitmapBuffer::load(getThemePath("sleep.bmp"));
  if (bitmap) {
    lcd->drawBitmap((LCD_W-bitmap->getWidth())/2, (LCD_H-bitmap->getHeight())/2, bitmap);
    delete bitmap;
  }
  lcdRefresh();
}

#define SHUTDOWN_CIRCLE_DIAMETER       150
void drawShutdownAnimation(uint32_t index, const char * message)
{
  static uint32_t last_index = 0xffffffff;
  static const BitmapBuffer * shutdown = BitmapBuffer::load(getThemePath("shutdown.bmp"));

  if (shutdown) {
    if (index < last_index) {
      theme->drawBackground();
      lcd->drawBitmap((LCD_W-shutdown->getWidth())/2, (LCD_H-shutdown->getHeight())/2, shutdown);
      lcdStoreBackupBuffer();
    }
    else {
      lcdRestoreBackupBuffer();
      int quarter = index / (PWR_PRESS_SHUTDOWN_DELAY / 5);
      if (quarter >= 1) lcdDrawBitmapPattern(LCD_W/2,                            (LCD_H-SHUTDOWN_CIRCLE_DIAMETER)/2, LBM_SHUTDOWN_CIRCLE, TEXT_COLOR, 0, SHUTDOWN_CIRCLE_DIAMETER/2);
      if (quarter >= 2) lcdDrawBitmapPattern(LCD_W/2,                            LCD_H/2,                            LBM_SHUTDOWN_CIRCLE, TEXT_COLOR, SHUTDOWN_CIRCLE_DIAMETER/2, SHUTDOWN_CIRCLE_DIAMETER/2);
      if (quarter >= 3) lcdDrawBitmapPattern((LCD_W-SHUTDOWN_CIRCLE_DIAMETER)/2, LCD_H/2,                            LBM_SHUTDOWN_CIRCLE, TEXT_COLOR, SHUTDOWN_CIRCLE_DIAMETER, SHUTDOWN_CIRCLE_DIAMETER/2);
      if (quarter >= 4) lcdDrawBitmapPattern((LCD_W-SHUTDOWN_CIRCLE_DIAMETER)/2, (LCD_H-SHUTDOWN_CIRCLE_DIAMETER)/2, LBM_SHUTDOWN_CIRCLE, TEXT_COLOR, SHUTDOWN_CIRCLE_DIAMETER*3/2, SHUTDOWN_CIRCLE_DIAMETER/2);
    }
  }
  else {
    lcd->clear();
    int quarter = index / (PWR_PRESS_SHUTDOWN_DELAY / 5);
    for (int i=1; i<=4; i++) {
      if (quarter >= i) {
        lcd->drawSolidFilledRect(LCD_W / 2 - 70 + 24 * i, LCD_H / 2 - 10, 20, 20, TEXT_BGCOLOR);
      }
    }
  }

  lcdRefresh();
  last_index = index;
}
