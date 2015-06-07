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

TEST(Lcd, DblsizeBottomRight)
{
  lcd_clear();
  lcd_putsAtt(LCD_W-20, LCD_H-16, "TEST", DBLSIZE);
  EXPECT_TRUE(checkScreenshot("dblsize_bottom_right"));
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

#if defined(CPUARM)
TEST(Lcd, vline_x_lt0)
{
  lcd_clear();
  lcd_vline(50, -10, 12);
  lcd_vline(100, -10, 1);
  EXPECT_TRUE(checkScreenshot("vline_lt0"));
}
#endif

#if defined(CPUARM)
TEST(Lcd, Smlsize)
{
  lcd_clear();
  lcd_putsAtt(0, 0, "TESTgy,", SMLSIZE);
  lcd_putsAtt(10, 22, "TESTgy,", SMLSIZE|INVERS);
  drawFilledRect(8, 40, 100, 20);
  lcd_putsAtt(10, 42, "TESTgy,", SMLSIZE);

  bool invert = false;
  for(int i=0; i<3; i++) {
    lcd_putsAtt(40+(4*i), 0+(4*i), "ABC", SMLSIZE|(invert?INVERS:0));  
    invert = !invert;
  }

  EXPECT_TRUE(checkScreenshot("smlsize"));
}

TEST(Lcd, Stdsize)
{
  lcd_clear();
  lcd_putsAtt(0, 0, "TEST", 0);
  lcd_putsAtt(10, 22, "TEST", INVERS);
  drawFilledRect(8, 40, 100, 20);
  lcd_putsAtt(10, 42, "TEST", 0);

  bool invert = false;
  for(int i=0; i<3; i++) {
    lcd_putsAtt(40+(4*i), 0+(4*i), "ABC", (invert?INVERS:0));  
    invert = !invert;
  }

  EXPECT_TRUE(checkScreenshot("stdsize"));
}

TEST(Lcd, Midsize)
{
  lcd_clear();
  lcd_putsAtt(0, 0, "TEST", MIDSIZE);
  lcd_putsAtt(10, 22, "TEST", MIDSIZE|INVERS);
  drawFilledRect(8, 40, 100, 20);
  lcd_putsAtt(10, 42, "TEST", MIDSIZE);

  bool invert = false;
  for(int i=0; i<3; i++) {
    lcd_putsAtt(40+(4*i), 0+(4*i), "ABC", MIDSIZE|(invert?INVERS:0));  
    invert = !invert;
  }

  EXPECT_TRUE(checkScreenshot("midsize"));
}

TEST(Lcd, Dblsize)
{
  lcd_clear();
  lcd_putsAtt(2, 10, "TST", DBLSIZE);
  lcd_putsAtt(42, 10, "TST", DBLSIZE|INVERS);
  drawFilledRect(80, 8, 46, 24);
  lcd_putsAtt(82, 10, "TST", DBLSIZE);

  bool invert = false;
  for(int i=0; i<3; i++) {
    lcd_putsAtt(10+(4*i), 30+(4*i), "ABC", DBLSIZE|(invert?INVERS:0));  
    invert = !invert;
  }

  EXPECT_TRUE(checkScreenshot("dblsize"));
}
#endif

#if defined(PCBTARANIS)
TEST(Lcd, DrawSwitch)
{
  lcd_clear();
  putsSwitches(0,  10, SWSRC_SA0, 0);
  putsSwitches(30, 10, SWSRC_SA0, SMLSIZE);
  // putsSwitches(60, 10, SWSRC_SA0, MIDSIZE); missing arrows in this font
  putsSwitches(90, 10, SWSRC_SA0, DBLSIZE);
  EXPECT_TRUE(checkScreenshot("drawswitch"));
}
#endif

#if defined(PCBTARANIS)
TEST(Lcd, BMPWrapping)
{
  lcd_clear();
  uint8_t bitmap[2+40*40/2];
  bmpLoad(bitmap, "./tests/plane.bmp", 40, 40);
  lcd_bmp(200, 0, bitmap);
  lcd_bmp(200, 60, bitmap);
  lcd_bmp(240, 60, bitmap);     // x too big
  lcd_bmp(20, 200, bitmap);     // y too big
  EXPECT_TRUE(checkScreenshot("bmpwrapping"));
}
#endif

#if defined(PCBTARANIS)
TEST(Lcd, lcd_hlineStip)
{
  lcd_clear();
  lcd_hlineStip(0, 10, LCD_W, DOTTED);
  lcd_hlineStip(0, 20, LCD_W, SOLID);
  lcd_hlineStip(50, 30, LCD_W, 0xEE);    //too wide
  lcd_hlineStip(50, LCD_H + 10, 20, SOLID);    //too low
  lcd_hlineStip(250, 30, LCD_W, SOLID);    //x outside display
  EXPECT_TRUE(checkScreenshot("lcd_hlineStip"));
}
#endif

#if defined(PCBTARANIS)
TEST(Lcd, lcd_vlineStip)
{
  lcd_clear();
  lcd_vlineStip(10, 0, LCD_H, DOTTED);
  lcd_vlineStip(20, 0, LCD_H, SOLID);
  lcd_vlineStip(30, 30, LCD_H, 0xEE);    //too high
  lcd_vlineStip(40, LCD_H + 10, 20, SOLID);    //too low
  lcd_vlineStip(250, LCD_H + 10, LCD_H, SOLID);    //x outside display
  EXPECT_TRUE(checkScreenshot("lcd_vlineStip"));
}
#endif

template <int padding> class TestBuffer
{
private:
  uint8_t * buf;
  uint32_t size;
public:
  TestBuffer(uint32_t size) : buf(0), size(size) {
    buf = new uint8_t[size + padding * 2];
    memset(buf, 0xA5, padding);
    memset(buf+padding, 0x00, size);
    memset(buf+padding+size, 0x5A, padding);
  };
  ~TestBuffer() { if (buf) delete[] buf; };
  uint8_t * buffer() { return buf + padding; };
  void leakCheck() const { 
    uint8_t paddingCompareBuf[padding];
    memset(paddingCompareBuf, 0xA5, padding);
    if (memcmp(buf, paddingCompareBuf, padding) != 0) {
      ADD_FAILURE() << "buffer leaked low";  
    };
    memset(paddingCompareBuf, 0x5A, padding);
    if (memcmp(buf+padding+size, paddingCompareBuf, padding) != 0) {
      ADD_FAILURE() << "buffer leaked high";
    }
  };
};

#if defined(PCBTARANIS)
TEST(Lcd, lcd_bmpLoadAndDisplay)
{
  lcd_clear();
  // Test proper BMP files, they should display correctly
  {
    TestBuffer<1000>  bitmap(BITMAP_BUFFER_SIZE(7, 32));
    EXPECT_EQ(bmpLoad(bitmap.buffer(), "./tests/4b_7x32.bmp", 7, 32), (char *)0);
    bitmap.leakCheck();
    lcd_bmp(10, 2, bitmap.buffer());
  }
  {
    TestBuffer<1000>  bitmap(BITMAP_BUFFER_SIZE(6, 32));
    EXPECT_EQ(bmpLoad(bitmap.buffer(), "./tests/1b_6x32.bmp", 6, 32), (char *)0);
    bitmap.leakCheck();
    lcd_bmp(20, 2, bitmap.buffer());
  }
  {
    TestBuffer<1000>  bitmap(BITMAP_BUFFER_SIZE(31, 31));
    EXPECT_EQ(bmpLoad(bitmap.buffer(), "./tests/4b_31x31.bmp", 31, 31), (char *)0);
    bitmap.leakCheck();
    lcd_bmp(30, 2, bitmap.buffer());
  }
  {
    TestBuffer<1000>  bitmap(BITMAP_BUFFER_SIZE(39, 32));
    EXPECT_EQ(bmpLoad(bitmap.buffer(), "./tests/1b_39x32.bmp", 39, 32), (char *)0);
    bitmap.leakCheck();
    lcd_bmp(70, 2, bitmap.buffer());
  }
  {
    TestBuffer<1000>  bitmap(BITMAP_BUFFER_SIZE(20, 20));
    EXPECT_EQ(bmpLoad(bitmap.buffer(), "./tests/4b_20x20.bmp", 20, 20), (char *)0);
    bitmap.leakCheck();
    lcd_bmp(120, 2, bitmap.buffer());
  }
  EXPECT_TRUE(checkScreenshot("lcd_bmpLoadAndDisplay"));

  // Test various bad BMP files, they should not display
  {
    TestBuffer<1000>  bitmap(BITMAP_BUFFER_SIZE(LCD_W+1, 32));
    EXPECT_EQ(bmpLoad(bitmap.buffer(), "", LCD_W+1, 32), STR_INCOMPATIBLE) << "to wide";
    bitmap.leakCheck();
  }
  {
    TestBuffer<1000>  bitmap(BITMAP_BUFFER_SIZE(10, 10));
    EXPECT_EQ(bmpLoad(bitmap.buffer(), "./tests/1b_39x32.bmp", 10, 10), STR_INCOMPATIBLE) << "to small buffer";
    bitmap.leakCheck();
  }
}
#endif

#if defined(PCBTARANIS)
TEST(Lcd, lcd_line)
{
  int start, length, xOffset;
  uint8_t pattern; 

  lcd_clear();

  start = 5;
  pattern = SOLID; 
  length = 40;
  xOffset = 0;
  lcd_line(start+(length>0?1:-1)+xOffset, start, start+(length>0?1:-1)+xOffset+length, start, pattern, 0);
  lcd_line(start+xOffset, start+(length>0?1:-1), start+xOffset, start+(length>0?1:-1)+length, pattern, 0);

  start = 10;
  pattern = DOTTED; 
  length = 40;
  xOffset = 0;
  lcd_line(start+(length>0?1:-1)+xOffset, start, start+(length>0?1:-1)+xOffset+length, start, pattern, 0);
  lcd_line(start+xOffset, start+(length>0?1:-1), start+xOffset, start+(length>0?1:-1)+length, pattern, 0);

  start = 55;
  pattern = SOLID; 
  length = -40;
  xOffset = 80;
  lcd_line(start+(length>0?1:-1)+xOffset, start, start+(length>0?1:-1)+xOffset+length, start, pattern, 0);
  lcd_line(start+xOffset, start+(length>0?1:-1), start+xOffset, start+(length>0?1:-1)+length, pattern, 0);

  start = 50;
  pattern = DOTTED; 
  length = -40;
  xOffset = 80;
  lcd_line(start+(length>0?1:-1)+xOffset, start, start+(length>0?1:-1)+xOffset+length, start, pattern, 0);
  lcd_line(start+xOffset, start+(length>0?1:-1), start+xOffset, start+(length>0?1:-1)+length, pattern, 0);

  // 45 deg lines
  lcd_line( 35, 40, 45, 40, SOLID, FORCE );
  lcd_line( 40, 35, 40, 45, SOLID, FORCE );

  lcd_line( 20, 40, 40, 20, SOLID, FORCE );
  lcd_line( 40, 20, 60, 40, SOLID, FORCE );
  lcd_line( 60, 40, 40, 60, SOLID, FORCE );
  lcd_line( 40, 60, 20, 40, SOLID, FORCE );

  lcd_line( 31, 39, 39, 31, SOLID, FORCE );
  lcd_line( 41, 31, 49, 39, SOLID, FORCE );
  lcd_line( 49, 41, 41, 49, SOLID, FORCE );
  lcd_line( 39, 49, 31, 41, SOLID, FORCE );

  // slanted lines
  lcd_line( 150, 10, 190, 10, SOLID, FORCE );
  lcd_line( 150, 10, 190, 20, SOLID, FORCE );
  lcd_line( 150, 10, 190, 30, SOLID, FORCE );
  lcd_line( 150, 10, 190, 40, SOLID, FORCE );
  lcd_line( 150, 10, 190, 50, SOLID, FORCE );

  lcd_line( 150, 10, 190, 50, SOLID, FORCE );
  lcd_line( 150, 10, 180, 50, SOLID, FORCE );
  lcd_line( 150, 10, 170, 50, SOLID, FORCE );
  lcd_line( 150, 10, 160, 50, SOLID, FORCE );
  lcd_line( 150, 10, 150, 50, SOLID, FORCE );

  EXPECT_TRUE(checkScreenshot("lcd_line"));
}
#endif
