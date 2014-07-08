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

#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtGui/QApplication>
#include <QtGui/QPainter>
#include <math.h>
#include <gtest/gtest.h>

#define SWAP_DEFINED
#include "opentx.h"


void doPaint(QPainter & p)
{
  QRgb rgb = qRgb(161, 161, 161);

  p.setBackground(QBrush(rgb));
  p.eraseRect(0, 0, LCD_W, LCD_H);

  if (1) {
#if !defined(PCBTARANIS)
    rgb = qRgb(0, 0, 0);
    p.setPen(rgb);
    p.setBrush(QBrush(rgb));
#endif

#if defined(PCBTARANIS)
    unsigned int previousDepth = 0xFF;
#endif

    for (int y=0; y<LCD_H; y++) {
#if defined(PCBTARANIS)
      unsigned int idx = (y/2) * LCD_W;
#else
      unsigned int idx = (y/8) * LCD_W;
      unsigned int mask = (1 << (y%8));
#endif
      for (int x=0; x<LCD_W; x++, idx++) {
#if !defined(PCBTARANIS)
        if (lcd_buf[idx] & mask) {
          p.drawPoint(x, y);
        }
#else
        unsigned int z = (y & 1) ? (lcd_buf[idx] >> 4) : (lcd_buf[idx] & 0x0F);
        if (z) {
          if (z != previousDepth) {
            previousDepth = z;
            rgb = qRgb(161-(z*161)/15, 161-(z*161)/15, 161-(z*161)/15);
            p.setPen(rgb);
            p.setBrush(QBrush(rgb));
          }
          p.drawPoint(x, y);
        }
#endif
      }
    }
  }
}

bool checkScreenshot(QString test)
{
  lcdRefresh();
  QImage buffer(LCD_W, LCD_H, QImage::Format_RGB32);
  QPainter p(&buffer);
  doPaint(p);
  QString filename(QString("%1_%2x%3.png").arg(test).arg(LCD_W).arg(LCD_H));
  buffer.save("/tmp/" + filename);
  QFile screenshot("/tmp/" + filename);
  if (!screenshot.open(QIODevice::ReadOnly))
    return false;
  QFile reference("./tests/" + filename);
  if (!reference.open(QIODevice::ReadOnly))
    return false;
  if (reference.readAll() != screenshot.readAll())
    return false;
  screenshot.remove();
  return true;
}

TEST(outdezNAtt, test_unsigned)
{
  lcd_clear();
  lcd_outdezNAtt(0, 0, 65530, LEFT|UNSIGN);
  EXPECT_TRUE(checkScreenshot("unsigned")) << "Unsigned numbers will be bad displayed";
}

#if defined(CPUARM)
TEST(outdezNAtt, testBigNumbers)
{
  lcd_clear();
  lcd_outdezNAtt(0, 0, 1234567, LEFT);
  lcd_outdezNAtt(0, FH, -1234567, LEFT);
  EXPECT_TRUE(checkScreenshot("big_numbers"));
}
#endif // #if defined(CPUARM)


TEST(Lcd, Invers_0_0)
{
  lcd_clear();
  lcd_putsAtt(0, 0, "Test", INVERS);
  EXPECT_TRUE(checkScreenshot("invers_0_0"));
}

TEST(Lcd, Invers_0_1)
{
  lcd_clear();
  lcd_putsAtt(0, 1, "Test", INVERS);
  EXPECT_TRUE(checkScreenshot("invers_0_1"));
}

TEST(Lcd, Prec2_Left)
{
  lcd_clear();
  lcd_outdezAtt(0, 0, 2, PREC2|LEFT);
  EXPECT_TRUE(checkScreenshot("prec2_left"));
}

TEST(Lcd, Prec2_Right)
{
  lcd_clear();
  lcd_outdezAtt(LCD_W, LCD_H-FH, 2, PREC2);
  EXPECT_TRUE(checkScreenshot("prec2_right"));
}

#if defined(CPUARM)
TEST(Lcd, Prec1_Dblsize_Invers)
{
  lcd_clear();
  lcd_outdezAtt(LCD_W, 10, 51, PREC1|DBLSIZE|INVERS);
  EXPECT_TRUE(checkScreenshot("prec1_dblsize_invers"));
}
#endif

TEST(Lcd, Line_Wrap)
{
  lcd_clear();
  lcd_puts(LCD_W-10, 0, "TEST");
  EXPECT_TRUE(checkScreenshot("line_wrap"));
}

#if defined(CPUARM)
TEST(Lcd, Smlsize_putsStrIdx)
{
  lcd_clear();
  putsStrIdx(0, 0, "FM", 0, SMLSIZE);
  EXPECT_TRUE(checkScreenshot("smlsize_putsstridx"));
}
#endif

TEST(Lcd, vline)
{
  lcd_clear();
  for (int x=0; x<100; x+=2) {
    lcd_vline(x, x/2, 12);
  }
  EXPECT_TRUE(checkScreenshot("vline"));
}
