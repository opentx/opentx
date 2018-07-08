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

#include <math.h>
#include "opentx.h"

void BitmapBuffer::drawAlphaPixel(display_t * p, uint8_t opacity, uint16_t color)
{
  if (opacity == OPACITY_MAX) {
    drawPixel(p, color);
  }
  else if (opacity != 0) {
    uint8_t bgWeight = OPACITY_MAX - opacity;
    RGB_SPLIT(color, red, green, blue);
    RGB_SPLIT(*p, bgRed, bgGreen, bgBlue);
    uint16_t r = (bgRed * bgWeight + red * opacity) / OPACITY_MAX;
    uint16_t g = (bgGreen * bgWeight + green * opacity) / OPACITY_MAX;
    uint16_t b = (bgBlue * bgWeight + blue * opacity) / OPACITY_MAX;
    drawPixel(p, RGB_JOIN(r, g, b));
  }
}

void BitmapBuffer::drawHorizontalLine(coord_t x, coord_t y, coord_t w, uint8_t pat, LcdFlags att)
{
  if (y >= height) return;
  if (x+w > width) { w = width - x; }

  display_t * p = getPixelPtr(x, y);
  display_t color = lcdColorTable[COLOR_IDX(att)];
  uint8_t opacity = 0x0F - (att >> 24);

  if (pat == SOLID) {
    while (w--) {
      drawAlphaPixel(p, opacity, color);
      MOVE_TO_NEXT_RIGHT_PIXEL(p);
    }
  }
  else {
    while (w--) {
      if (pat & 1) {
        drawAlphaPixel(p, opacity, color);
        pat = (pat >> 1) | 0x80;
      }
      else {
        pat = pat >> 1;
      }
      MOVE_TO_NEXT_RIGHT_PIXEL(p);
    }
  }
}

void BitmapBuffer::drawVerticalLine(coord_t x, coord_t y, coord_t h, uint8_t pat, LcdFlags att)
{
  if (x >= width) return;
  if (y >= height) return;
  if (h<0) { y+=h; h=-h; }
  if (y<0) { h+=y; y=0; if (h<=0) return; }
  if (y+h > height) { h = height - y; }

  display_t color = lcdColorTable[COLOR_IDX(att)];
  uint8_t opacity = 0x0F - (att >> 24);

  if (pat == SOLID) {
    while (h--) {
      drawAlphaPixel(x, y, opacity, color);
      y++;
    }
  }
  else {
    if (pat==DOTTED && !(y%2)) {
      pat = ~pat;
    }
    while (h--) {
      if (pat & 1) {
        drawAlphaPixel(x, y, opacity, color);
        pat = (pat >> 1) | 0x80;
      }
      else {
        pat = pat >> 1;
      }
      y++;
    }
  }
}

void BitmapBuffer::drawRect(coord_t x, coord_t y, coord_t w, coord_t h, uint8_t thickness, uint8_t pat, LcdFlags att)
{
  for (int i=0; i<thickness; i++) {
    drawVerticalLine(x+i, y, h, pat, att);
    drawVerticalLine(x+w-1-i, y, h, pat, att);
    drawHorizontalLine(x, y+h-1-i, w, pat, att);
    drawHorizontalLine(x, y+i, w, pat, att);
  }
}

void BitmapBuffer::drawFilledRect(coord_t x, coord_t y, coord_t w, coord_t h, uint8_t pat, LcdFlags att)
{
  for (coord_t i=y; i<y+h; i++) {
    if ((att & ROUND) && (i==y || i==y+h-1))
      drawHorizontalLine(x+1, i, w-2, pat, att);
    else
      drawHorizontalLine(x, i, w, pat, att);
  }
}

void BitmapBuffer::invertRect(coord_t x, coord_t y, coord_t w, coord_t h, LcdFlags att)
{
  display_t color = lcdColorTable[COLOR_IDX(att)];
  RGB_SPLIT(color, red, green, blue);

  for (int i=y; i<y+h; i++) {
    display_t * p = getPixelPtr(x, i);
    for (int j=0; j<w; j++) {
      // TODO ASSERT_IN_DISPLAY(p);
      RGB_SPLIT(*p, bgRed, bgGreen, bgBlue);
      drawPixel(p, RGB_JOIN(0x1F + red - bgRed, 0x3F + green - bgGreen, 0x1F + blue - bgBlue));
      MOVE_TO_NEXT_RIGHT_PIXEL(p);
    }
  }
}

#if 0
void BitmapBuffer::drawCircle(int x0, int y0, int radius)
{
  int x = radius;
  int y = 0;
  int decisionOver2 = 1 - x;

  while (y <= x) {
    drawPixel(x+x0, y+y0, WHITE);
    drawPixel(y+x0, x+y0, WHITE);
    drawPixel(-x+x0, y+y0, WHITE);
    drawPixel(-y+x0, x+y0, WHITE);
    drawPixel(-x+x0, -y+y0, WHITE);
    drawPixel(-y+x0, -x+y0, WHITE);
    drawPixel(x+x0, -y+y0, WHITE);
    drawPixel(y+x0, -x+y0, WHITE);
    y++;
    if (decisionOver2 <= 0) {
      decisionOver2 += 2*y + 1;
    }
    else {
      x--;
      decisionOver2 += 2 * (y-x) + 1;
    }
  }
}
#endif

#define PI 3.14159265f

bool evalSlopes(int * slopes, int startAngle, int endAngle)
{
  if (startAngle >= 360 || endAngle <= 0)
    return false;

  if (startAngle == 0) {
    slopes[1] = 100000;
    slopes[2] = -100000;
  }
  else {
    float angle1 = float(startAngle) * (PI / 180.0f);
    if (startAngle >= 180) {
      slopes[1] = -100000;
      slopes[2] = cosf(angle1) * 100 / sinf(angle1);
    }
    else {
      slopes[1] = cosf(angle1) * 100 / sinf(angle1);
      slopes[2] = -100000;
    }
  }

  if (endAngle == 360) {
    slopes[0] = -100000;
    slopes[3] = 100000;
  }
  else {
    float angle2 = float(endAngle)  * (PI / 180.0f);
    if (endAngle >= 180) {
      slopes[0] = -100000;
      slopes[3] = -cosf(angle2) * 100 / sinf(angle2);
    }
    else {
      slopes[0] = cosf(angle2) * 100 / sinf(angle2);
      slopes[3] = -100000;
    }
  }

  return true;
}

void BitmapBuffer::drawPie(int x0, int y0, int radius, int startAngle, int endAngle)
{
  int slopes[4];
  if (!evalSlopes(slopes, startAngle, endAngle))
    return;

  for (int y=0; y<=radius; y++) {
    for (int x=0; x<=radius; x++) {
      if (x*x+y*y <= radius*radius) {
        int slope = (x==0 ? (y<0 ? -99000 : 99000) : y*100/x);
        if (slope >= slopes[0] && slope < slopes[1]) {
          drawPixel(x0+x, y0-y, WHITE);
        }
        if (-slope >= slopes[0] && -slope < slopes[1]) {
          drawPixel(x0+x, y0+y, WHITE);
        }
        if (slope >= slopes[2] && slope < slopes[3]) {
          drawPixel(x0-x, y0-y, WHITE);
        }
        if (-slope >= slopes[2] && -slope < slopes[3]) {
          drawPixel(x0-x, y0+y, WHITE);
        }
      }
    }
  }
}

void BitmapBuffer::drawMask(coord_t x, coord_t y, BitmapBuffer * mask, LcdFlags flags, coord_t offset, coord_t width)
{
  if (mask == NULL) {
    return;
  }

  coord_t w = mask->getWidth();
  coord_t height = mask->getHeight();

  if (!width || width > w) {
    width = w;
  }

  if (x+width > this->width) {
    width = this->width-x;
  }

  display_t color = lcdColorTable[COLOR_IDX(flags)];

  for (coord_t row=0; row<height; row++) {
    display_t * p = getPixelPtr(x, y+row);
    display_t * q = mask->getPixelPtr(offset, row);
    for (coord_t col=0; col<width; col++) {
      drawAlphaPixel(p, *((uint8_t *)q), color);
      MOVE_TO_NEXT_RIGHT_PIXEL(p);
      MOVE_TO_NEXT_RIGHT_PIXEL(q);
    }
  }
}

void BitmapBuffer::drawBitmapPattern(coord_t x, coord_t y, const uint8_t * bmp, LcdFlags flags, coord_t offset, coord_t width)
{
  coord_t w = *((uint16_t *)bmp);
  coord_t height = *(((uint16_t *)bmp)+1);

  if (!width || width > w) {
    width = w;
  }

  if (x+width > this->width) {
    width = this->width-x;
  }

  display_t color = lcdColorTable[COLOR_IDX(flags)];

  for (coord_t row=0; row<height; row++) {
    const uint8_t * q = bmp + 4 + row*w + offset;
    for (coord_t col=0; col<width; col++) {
      display_t * p;
      if (flags & VERTICAL)
        p = getPixelPtr(x+row, y-col);
      else
        p = getPixelPtr(x+col, y+row);
      drawAlphaPixel(p, *q, color);
      q++;
    }
  }
}

uint8_t BitmapBuffer::drawCharWithoutCache(coord_t x, coord_t y, const uint8_t * font, const uint16_t * spec, int index, LcdFlags flags)
{
  coord_t offset = spec[index];
  coord_t width = spec[index+1] - offset;
  if (width > 0)
    drawBitmapPattern(x, y, font, flags, offset, width);
  return width;
}

uint8_t BitmapBuffer::drawCharWithCache(coord_t x, coord_t y, const BitmapBuffer * font, const uint16_t * spec, int index, LcdFlags flags)
{
  coord_t offset = spec[index];
  coord_t width = spec[index+1] - offset;
  if (width > 0)
    drawBitmap(x, y, font, offset, 0, width);
  return width;
}

void BitmapBuffer::drawSizedText(coord_t x, coord_t y, const char * s, uint8_t len, LcdFlags flags)
{
#define INCREMENT_POS(delta) \
  do { if (flags & VERTICAL) y -= delta; else x += delta; } while(0)

  int width = getTextWidth(s, len, flags);
  int height = getFontHeight(flags);
  uint32_t fontindex = FONTINDEX(flags);
  const pm_uchar * font = fontsTable[fontindex];
  const uint16_t * fontspecs = fontspecsTable[fontindex];
  BitmapBuffer * fontcache = NULL;

  if (flags & RIGHT) {
    INCREMENT_POS(-width);
  }
  else if (flags & CENTERED) {
    INCREMENT_POS(-width/2);
  }

  coord_t & pos = (flags & VERTICAL) ? y : x;

  if ((flags & INVERS) && ((~flags & BLINK) || BLINK_ON_PHASE)) {
    uint16_t fgColor = lcdColorTable[COLOR_IDX(flags)];
    if (fgColor == lcdColorTable[TEXT_COLOR_INDEX]) {
      flags = TEXT_INVERTED_COLOR | (flags & 0x0ffff);
    }
    if (fontindex == STDSIZE_INDEX) {
      if (fgColor == lcdColorTable[TEXT_COLOR_INDEX]) {
        drawSolidFilledRect(x-INVERT_HORZ_MARGIN, y, INVERT_HORZ_MARGIN-1, INVERT_LINE_HEIGHT, TEXT_INVERTED_BGCOLOR);
        drawSolidFilledRect(x+width-1, y, INVERT_HORZ_MARGIN, INVERT_LINE_HEIGHT, TEXT_INVERTED_BGCOLOR);
        fontcache = fontCache[1];
      }
      else {
        drawSolidFilledRect(x-INVERT_HORZ_MARGIN, y, width+2*INVERT_HORZ_MARGIN-1, INVERT_LINE_HEIGHT, TEXT_INVERTED_BGCOLOR);
      }
    }
    else if (fontindex == TINSIZE_INDEX) {
      drawSolidFilledRect(x-INVERT_HORZ_MARGIN+2, y, width+2*INVERT_HORZ_MARGIN-5, height+2, TEXT_INVERTED_BGCOLOR);
    }
    else if (fontindex == SMLSIZE_INDEX) {
      drawSolidFilledRect(x-INVERT_HORZ_MARGIN, y, width+2*INVERT_HORZ_MARGIN-2, height+2, TEXT_INVERTED_BGCOLOR);
    }
    else if (fontindex == MIDSIZE_INDEX) {
      drawSolidFilledRect(x-INVERT_HORZ_MARGIN, y-1, width+2*INVERT_HORZ_MARGIN-2, height+3, TEXT_INVERTED_BGCOLOR);  // MIDSIZE and DBLSIZE font are missing a pixel on top compared to others
    }
    else if (fontindex == DBLSIZE_INDEX) {
      drawSolidFilledRect(x-INVERT_HORZ_MARGIN, y-1, width+2*INVERT_HORZ_MARGIN-2, height+3, TEXT_INVERTED_BGCOLOR); // MIDSIZE and DBLSIZE font are missing a pixel on top compared to others
    }
    else if (fontindex == XXLSIZE_INDEX) {
      drawSolidFilledRect(x-INVERT_HORZ_MARGIN, y, width+2*INVERT_HORZ_MARGIN-2, height+2, TEXT_INVERTED_BGCOLOR);
    }
    else {
      drawSolidFilledRect(x-INVERT_HORZ_MARGIN, y, width+2*INVERT_HORZ_MARGIN, INVERT_LINE_HEIGHT, TEXT_INVERTED_BGCOLOR);
    }
  }
  else if (!(flags & NO_FONTCACHE)) {
    if (fontindex == STDSIZE_INDEX) {
      uint16_t fgColor = lcdColorTable[COLOR_IDX(flags)];
      uint16_t bgColor = *getPixelPtr(x, y);
      if (fgColor == lcdColorTable[TEXT_COLOR_INDEX] && bgColor == lcdColorTable[TEXT_BGCOLOR_INDEX]) {
        fontcache = fontCache[0];
      }
      else if (fgColor == lcdColorTable[TEXT_INVERTED_COLOR_INDEX] && bgColor == lcdColorTable[TEXT_INVERTED_BGCOLOR_INDEX]) {
        fontcache = fontCache[1];
      }
      else {
        // TRACE("No cache for \"%s\"", s);
      }
    }
  }

  bool setpos = false;
  const coord_t orig_pos = pos;
  while (len--) {
#if defined(BOOT)
    unsigned char c = *s;
#else
    unsigned char c = (flags & ZCHAR) ? idx2char(*s) : *s;
#endif
    if (setpos) {
      pos = c;
      setpos = false;
    }
    else if (!c) {
      break;
    }
    else if (c >= 0x20) {
      uint8_t width;
      if (fontcache)
        width = drawCharWithCache(x-1, y, fontcache, fontspecs, getMappedChar(c), flags);
      else
        width = drawCharWithoutCache(x-1, y, font, fontspecs, getMappedChar(c), flags);
      INCREMENT_POS(width);
    }
    else if (c == 0x1F) {  // X-coord prefix
      setpos = true;
    }
    else if (c == 0x1E) {
      pos = orig_pos;
      if (flags & VERTICAL)
        x += height;
      else
        y += height;
    }
    else if (c == 1) {
      INCREMENT_POS(1);
    }
    else {
      INCREMENT_POS(2*(c-1));
    }
    s++;
  }
  lcdNextPos = pos;
}

void BitmapBuffer::drawBitmapPie(int x0, int y0, const uint16_t * img, int startAngle, int endAngle)
{
  const uint16_t * q = img;
  coord_t width = *q++;
  coord_t height = *q++;

  int slopes[4];
  if (!evalSlopes(slopes, startAngle, endAngle))
    return;

  int w2 = width/2;
  int h2 = height/2;

  for (int y=h2-1; y>=0; y--) {
    for (int x=w2-1; x>=0; x--) {
      int slope = (x==0 ? 99000 : y*100/x);
      if (slope >= slopes[0] && slope < slopes[1]) {
        *getPixelPtr(x0+w2+x, y0+h2-y) = q[(h2-y)*width + w2+x];
      }
      if (-slope >= slopes[0] && -slope < slopes[1]) {
        *getPixelPtr(x0+w2+x, y0+h2+y) = q[(h2+y)*width + w2+x];
      }
      if (slope >= slopes[2] && slope < slopes[3]) {
        *getPixelPtr(x0+w2-x, y0+h2-y) = q[(h2-y)*width + w2-x];
      }
      if (-slope >= slopes[2] && -slope < slopes[3]) {
        *getPixelPtr(x0+w2-x, y0+h2+y)  = q[(h2+y)*width + w2-x];
      }
    }
  }
}

void BitmapBuffer::drawBitmapPatternPie(coord_t x0, coord_t y0, const uint8_t * img, LcdFlags flags, int startAngle, int endAngle)
{
  coord_t width = *((uint16_t *)img);
  coord_t height = *(((uint16_t *)img)+1);
  const uint8_t * q = img+4;

  int slopes[4];
  if (!evalSlopes(slopes, startAngle, endAngle))
    return;

  display_t color = lcdColorTable[COLOR_IDX(flags)];

  int w2 = width/2;
  int h2 = height/2;

  for (int y=h2-1; y>=0; y--) {
    for (int x=w2-1; x>=0; x--) {
      int slope = (x==0 ? (y<0 ? -99000 : 99000) : y*100/x);
      if (slope >= slopes[0] && slope < slopes[1]) {
        drawAlphaPixel(x0+w2+x, y0+h2-y, q[(h2-y)*width + w2+x], color);
      }
      if (-slope >= slopes[0] && -slope < slopes[1]) {
        drawAlphaPixel(x0+w2+x, y0+h2+y, q[(h2+y)*width + w2+x], color);
      }
      if (slope >= slopes[2] && slope < slopes[3]) {
        drawAlphaPixel(x0+w2-x, y0+h2-y, q[(h2-y)*width + w2-x], color);
      }
      if (-slope >= slopes[2] && -slope < slopes[3]) {
        drawAlphaPixel(x0+w2-x, y0+h2+y, q[(h2+y)*width + w2-x], color);
      }
    }
  }
}

BitmapBuffer * BitmapBuffer::load(const char * filename)
{
  const char * ext = getFileExtension(filename);
  if (ext && !strcmp(ext, ".bmp"))
    return load_bmp(filename);
  else
    return load_stb(filename);
}

BitmapBuffer * BitmapBuffer::loadMask(const char * filename)
{
  BitmapBuffer * bitmap = BitmapBuffer::load(filename);
  if (bitmap) {
    display_t * p = bitmap->getPixelPtr(0, 0);
    for (int i = bitmap->getWidth() * bitmap->getHeight(); i > 0; i--) {
      *((uint8_t *)p) = OPACITY_MAX - ((*p) >> 12);
      MOVE_TO_NEXT_RIGHT_PIXEL(p);
    }
  }
  return bitmap;
}

BitmapBuffer * BitmapBuffer::loadMaskOnBackground(const char * filename, LcdFlags foreground, LcdFlags background)
{
  BitmapBuffer * result = NULL;
  BitmapBuffer * mask = BitmapBuffer::loadMask(getThemePath(filename));
  if (mask) {
    result = new BitmapBuffer(BMP_RGB565, mask->getWidth(), mask->getHeight());
    if (result) {
      result->clear(background);
      result->drawMask(0, 0, mask, foreground);
    }
    delete mask;
  }
  return result;
}

FIL imgFile __DMA;

BitmapBuffer * BitmapBuffer::load_bmp(const char * filename)
{
  UINT read;
  uint8_t palette[16];
  uint8_t bmpBuf[LCD_W]; /* maximum with LCD_W */
  uint8_t * buf = &bmpBuf[0];

  FRESULT result = f_open(&imgFile, filename, FA_OPEN_EXISTING | FA_READ);
  if (result != FR_OK) {
    return NULL;
  }

  if (f_size(&imgFile) < 14) {
    f_close(&imgFile);
    return NULL;
  }

  result = f_read(&imgFile, buf, 14, &read);
  if (result != FR_OK || read != 14) {
    f_close(&imgFile);
    return NULL;
  }

  if (buf[0] != 'B' || buf[1] != 'M') {
    f_close(&imgFile);
    return NULL;
  }

  uint32_t fsize  = *((uint32_t *)&buf[2]);
  uint32_t hsize  = *((uint32_t *)&buf[10]); /* header size */

  uint32_t len = limit((uint32_t)4, (uint32_t)(hsize-14), (uint32_t)32);
  result = f_read(&imgFile, buf, len, &read);
  if (result != FR_OK || read != len) {
    f_close(&imgFile);
    return NULL;
  }

  uint32_t ihsize = *((uint32_t *)&buf[0]); /* more header size */

  /* invalid header size */
  if (ihsize + 14 > hsize) {
    f_close(&imgFile);
    return NULL;
  }

  /* sometimes file size is set to some headers size, set a real size in that case */
  if (fsize == 14 || fsize == ihsize + 14)
    fsize = f_size(&imgFile) - 2;

  /* declared file size less than header size */
  if (fsize <= hsize) {
    f_close(&imgFile);
    return NULL;
  }

  uint32_t w, h;

  switch (ihsize){
    case  40: // windib
    case  56: // windib v3
    case  64: // OS/2 v2
    case 108: // windib v4
    case 124: // windib v5
      w  = *((uint32_t *)&buf[4]);
      h = *((uint32_t *)&buf[8]);
      buf += 12;
      break;
    case  12: // OS/2 v1
      w  = *((uint16_t *)&buf[4]);
      h = *((uint16_t *)&buf[6]);
      buf += 8;
      break;
    default:
      f_close(&imgFile);
      return NULL;
  }

  if (*((uint16_t *)&buf[0]) != 1) { /* planes */
    f_close(&imgFile);
    return NULL;
  }

  uint16_t depth = *((uint16_t *)&buf[2]);

  buf = &bmpBuf[0];

  if (depth == 4) {
    if (f_lseek(&imgFile, hsize-64) != FR_OK || f_read(&imgFile, buf, 64, &read) != FR_OK || read != 64) {
      f_close(&imgFile);
      return NULL;
    }
    for (uint8_t i=0; i<16; i++) {
      palette[i] = buf[4*i];
    }
  }
  else {
    if (f_lseek(&imgFile, hsize) != FR_OK) {
      f_close(&imgFile);
      return NULL;
    }
  }

  BitmapBuffer * bmp = new BitmapBuffer(BMP_RGB565, w, h);
  if (bmp == NULL || bmp->getData() == NULL) {
    f_close(&imgFile);
    return NULL;
  }

  uint32_t rowSize;
  bool hasAlpha = false;

  switch (depth) {
    case 32:
      for (int i=h-1; i>=0; i--) {
        display_t * dst = bmp->getPixelPtr(0, i);
        for (unsigned int j=0; j<w; j++) {
          uint32_t pixel;
          result = f_read(&imgFile, (uint8_t *)&pixel, 4, &read);
          if (result != FR_OK || read != 4) {
            f_close(&imgFile);
            delete bmp;
            return NULL;
          }
          if (hasAlpha) {
            *dst = ARGB(pixel & 0xff, (pixel >> 24) & 0xff, (pixel >> 16) & 0xff, (pixel >> 8) & 0xff);
          }
          else {
            if ((pixel & 0xff) == 0xff) {
              *dst = RGB(pixel >> 24, (pixel >> 16) & 0xff, (pixel >> 8) & 0xff);
            }
            else {
              hasAlpha = true;
              bmp->setFormat(BMP_ARGB4444);
              for (display_t * p = bmp->getPixelPtr(j, i); p != bmp->getPixelPtr(0, h); MOVE_TO_NEXT_RIGHT_PIXEL(p)) {
                display_t tmp = *p;
                *p = ((tmp >> 1) & 0x0f) + (((tmp >> 7) & 0x0f) << 4) + (((tmp >> 12) & 0x0f) << 8);
              }
              *dst = ARGB(pixel & 0xff, (pixel >> 24) & 0xff, (pixel >> 16) & 0xff, (pixel >> 8) & 0xff);
            }
          }
          MOVE_TO_NEXT_RIGHT_PIXEL(dst);
        }
      }
      break;

    case 1:
      break;

    case 4:
      rowSize = ((4*w+31)/32)*4;
      for (int32_t i=h-1; i>=0; i--) {
        result = f_read(&imgFile, buf, rowSize, &read);
        if (result != FR_OK || read != rowSize) {
          f_close(&imgFile);
          delete bmp;
          return NULL;
        }
        display_t * dst = bmp->getPixelPtr(0, i);
        for (uint32_t j=0; j<w; j++) {
          uint8_t index = (buf[j/2] >> ((j & 1) ? 0 : 4)) & 0x0F;
          uint8_t val = palette[index];
          *dst = RGB(val, val, val);
          MOVE_TO_NEXT_RIGHT_PIXEL(dst);
        }
      }
      break;

    default:
      f_close(&imgFile);
      delete bmp;
      return NULL;
  }

  f_close(&imgFile);
  return bmp;
}

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_ONLY_JPEG
#define STBI_ONLY_BMP
#define STBI_ONLY_GIF
#define STBI_NO_STDIO
#define STBI_NO_HDR
#define STBI_NO_LINEAR

// #define TRACE_STB_MALLOC

#if defined(TRACE_STB_MALLOC)
#define STBI_MALLOC(sz)                     stb_malloc(sz)
#define STBI_REALLOC_SIZED(p,oldsz,newsz)   stb_realloc(p,oldsz,newsz)
#define STBI_FREE(p)                        stb_free(p)

void * stb_malloc(unsigned int size)
{
  void * res = malloc(size);
  TRACE("malloc %d = %p", size, res);
  return res;
}

void stb_free(void *ptr)
{
  TRACE("free %p", ptr);
  free(ptr);
}

void *stb_realloc(void *ptr, unsigned int oldsz, unsigned int newsz)
{
  void * res =  realloc(ptr, newsz);
  TRACE("realloc %p, %d -> %d = %p", ptr, oldsz, newsz, res);
  return res;
}
#endif // #if defined(TRACE_STB_MALLOC)


#include "thirdparty/Stb/stb_image.h"

// fill 'data' with 'size' bytes.  return number of bytes actually read
int stbc_read(void *user, char *data, int size)
{
  FIL * fp = (FIL *)user;
  UINT br = 0;
  FRESULT res = f_read(fp, data, size, &br);
  if (res == FR_OK) {
    return (int)br;
  }
  return 0;
}

// skip the next 'n' bytes, or 'unget' the last -n bytes if negative
void stbc_skip(void *user, int n)
{
  FIL * fp = (FIL *)user;
  f_lseek(fp, f_tell(fp) + n);
}

// returns nonzero if we are at end of file/data
int stbc_eof(void *user)
{
  FIL * fp = (FIL *)user;
  int res = f_eof(fp);
  return res;
}

// callbacks for stb-image
const stbi_io_callbacks stbCallbacks = {
  stbc_read,
  stbc_skip,
  stbc_eof
};

BitmapBuffer * BitmapBuffer::load_stb(const char * filename)
{
  FRESULT result = f_open(&imgFile, filename, FA_OPEN_EXISTING | FA_READ);
  if (result != FR_OK) {
    return NULL;
  }

  int w, h, n;
  unsigned char * img = stbi_load_from_callbacks(&stbCallbacks, &imgFile, &w, &h, &n, 4);
  f_close(&imgFile);

  if (!img) {
    return NULL;
  }

  // convert to RGB565 or ARGB4444 format
  BitmapBuffer * bmp = new BitmapBuffer(n == 4 ? BMP_ARGB4444 : BMP_RGB565, w, h);
  if (bmp == NULL) {
    TRACE("load_stb() malloc failed");
    stbi_image_free(img);
    return NULL;
  }

#if 0
  DMABitmapConvert(bmp->data, img, w, h, n == 4 ? DMA2D_ARGB4444 : DMA2D_RGB565);
#else
  display_t * dest = bmp->getPixelPtr(0, 0);
  const uint8_t * p = img;
  if (n == 4) {
    for (int row = 0; row < h; ++row) {
      for (int col = 0; col < w; ++col) {
        *dest = ARGB(p[3], p[0], p[1], p[2]);
        MOVE_TO_NEXT_RIGHT_PIXEL(dest);
        p += 4;
      }
    }
  }
  else {
    for (int row = 0; row < h; ++row) {
      for (int col = 0; col < w; ++col) {
        *dest = RGB(p[0], p[1], p[2]);
        MOVE_TO_NEXT_RIGHT_PIXEL(dest);
        p += 4;
      }
    }
  }
#endif

  stbi_image_free(img);
  return bmp;
}
