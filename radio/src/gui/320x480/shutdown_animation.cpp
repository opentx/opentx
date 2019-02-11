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

#include "shutdown_animation.h"
#include "mainwindow.h"
#include "opentx.h"

void drawShutdownAnimation(uint32_t index, const char * message)
{
  static uint32_t last_index = 0xffffffff;
  static const BitmapBuffer * shutdownBitmap = BitmapBuffer::load(getThemePath("shutdown.bmp"));
  static BitmapBuffer * circleBitmap = BitmapBuffer::loadMask(getThemePath("mask_shutdown_circle.png"));

  int quarter = index / (PWR_PRESS_SHUTDOWN_DELAY / 5);
  int last_quarter = (index < last_index) ? -1 : last_index / (PWR_PRESS_SHUTDOWN_DELAY / 5);

  last_index = index;

  if (last_quarter >= 0 && quarter == last_quarter)
    return;

  lcd->setOffset(0, 0);
  lcd->clearClippingRect();

  if (shutdownBitmap && circleBitmap) {
    if (last_quarter < 0) {
      lcdDrawBlackOverlay();
      lcd->drawBitmap((LCD_W - shutdownBitmap->getWidth()) / 2, (LCD_H - shutdownBitmap->getHeight()) / 2, shutdownBitmap);
    }
    auto diameter = 2 * circleBitmap->getHeight();
    for (int i=1; i<=4; i++) {
      if (quarter == i) {
        lcd->drawMask(i <= 2 ?  LCD_W / 2 : (LCD_W - diameter) / 2,
                      (i & 2) ? LCD_H / 2 : (LCD_H - diameter) / 2,
                      circleBitmap, TEXT_COLOR, (diameter / 2) * (i - 1), diameter / 2);
      }
    }
  }
  else {
    if (last_quarter < 0) {
      lcd->clear();
    }
    for (int i=1; i<=4; i++) {
      if (quarter == i) {
        lcd->drawSolidFilledRect(LCD_W / 2 - 70 + 24 * i, LCD_H / 2 - 10, 20, 20, TEXT_BGCOLOR);
      }
    }
  }

  // force a refresh if the user stops the animation
  mainWindow.invalidate();
}

void drawSleepBitmap()
{
  lcd->clear();
  const BitmapBuffer * bitmap = BitmapBuffer::load(getThemePath("sleep.bmp"));
  if (bitmap) {
    lcd->drawBitmap((LCD_W-bitmap->getWidth())/2, (LCD_H-bitmap->getHeight())/2, bitmap);
    delete bitmap;
  }
}
