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

#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QApplication>
#include <QPainter>
#include <math.h>
#include <gtest/gtest.h>

#define SWAP_DEFINED
#include "opentx.h"
#include "location.h"
#include "targets/simu/simulcd.h"

#if defined(COLORLCD)

void doPaint_480x272(QPainter & p)
{
  QRgb rgb = qRgb(0, 0, 0);
  p.setBackground(QBrush(rgb));
  p.eraseRect(0, 0, LCD_W, LCD_H);

  uint16_t previousColor = 0xFF;
  for (int y=0; y<LCD_H; y++) {
    for (int x=0; x<LCD_W; x++) {
#if defined(PCBX10)
      uint16_t color = simuLcdBuf[(LCD_W-1-x)+LCD_W*(LCD_H-1-y)];  // color in RGB565
#else
      uint16_t color = simuLcdBuf[x+LCD_W*y];  // color in RGB565
#endif
      if (color) {
        if (color != previousColor) {
          previousColor = color;
          RGB_SPLIT(color, r, g, b);
          rgb = qRgb(r<<3, g<<2, b<<3);
          p.setPen(rgb);
          p.setBrush(QBrush(rgb));
        }
        p.drawPoint(x, y);
      }
    }
  }
}

bool checkScreenshot_480x272(const QString & test)
{
  lcdRefresh();
  QImage buffer(LCD_W, LCD_H, QImage::Format_RGB32);
  QPainter p(&buffer);
  doPaint_480x272(p);
  QString filename(QString("%1_%2x%3.png").arg(test).arg(LCD_W).arg(LCD_H));
  QImage reference(TESTS_PATH "/tests/" + filename);

  if (buffer == reference) {
    return true;
  }
  else {
    QString filename(QString("%1_%2x%3.png").arg(test).arg(LCD_W).arg(LCD_H));
    buffer.save("/tmp/" + filename);
    return false;
  }
}


TEST(Lcd_480x272, vline)
{
  lcd->clear(TEXT_BGCOLOR);
  for (int x=0; x<100; x+=2) {
    lcdDrawSolidVerticalLine(x, x/2, 12, TEXT_COLOR);
  }
  EXPECT_TRUE(checkScreenshot_480x272("vline"));
}

TEST(Lcd_480x272, primitives)
{
  lcd->clear(TEXT_BGCOLOR);
  lcdDrawText(8, 8, "The quick brown fox jumps over the lazy dog", CURVE_AXIS_COLOR);
  lcdDrawText(5, 5, "The quick brown fox jumps over the lazy dog", TEXT_COLOR);

  lcdDrawFilledRect(10, 30, 30, 30, SOLID, TITLE_BGCOLOR);
  lcdDrawFilledRect(50, 30, 30, 30, DOTTED, TEXT_COLOR);
  lcdDrawFilledRect(90, 30, 30, 30, SOLID, ROUND|TITLE_BGCOLOR);
  lcdDrawRect(130, 30, 30, 30, 1, SOLID, TITLE_BGCOLOR);
  lcdDrawRect(170, 30, 30, 30, 2, SOLID, TITLE_BGCOLOR);
  lcdDrawRect(210, 30, 30, 30, 5, SOLID, TITLE_BGCOLOR);

  lcdDrawVerticalLine(10, 70, 100, SOLID, TITLE_BGCOLOR);
  lcdDrawVerticalLine(15, 70,  90, SOLID, TITLE_BGCOLOR);
  lcdDrawVerticalLine(20, 70,  80, SOLID, TITLE_BGCOLOR);
  lcdDrawVerticalLine(25, 70,  70, SOLID, TITLE_BGCOLOR);

  lcdDrawHorizontalLine(30, 70, 100, SOLID, TEXT_COLOR);
  lcdDrawHorizontalLine(30, 75,  90, SOLID, TEXT_COLOR);
  lcdDrawHorizontalLine(30, 80,  80, SOLID, TEXT_COLOR);
  lcdDrawHorizontalLine(30, 85,  70, SOLID, TEXT_COLOR);


  EXPECT_TRUE(checkScreenshot_480x272("primitives"));
}

#endif