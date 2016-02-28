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

#ifndef _BITMAP_BUFFER_H_
#define _BITMAP_BUFFER_H_

#include <inttypes.h>
#include "colors.h"

#define USE_STB
 
// TODO should go to lcd.h again
typedef int coord_t;
typedef uint32_t LcdFlags;
typedef uint16_t display_t;

template<class T>
class BitmapBufferBase
{
  public:
    BitmapBufferBase(int width, int height, T * data):
      width(width),
      height(height),
      data(data)
    {
    }

    int getWidth() const
    {
      return width;
    }

    int getHeight() const
    {
      return height;
    }

  protected:
    int width;
    int height;

  public: // TODO protected
    T * data;
};

typedef BitmapBufferBase<const uint16_t> Bitmap;

class BitmapBuffer: public BitmapBufferBase<uint16_t>
{
  public:

    BitmapBuffer(int width, int height):
      BitmapBufferBase<uint16_t>(width, height, NULL)
    {
      data = (uint16_t *)malloc(width*height*sizeof(uint16_t));
    }

    BitmapBuffer(int width, int height, uint16_t * data):
      BitmapBufferBase<uint16_t>(width, height, data)
    {
    }

    ~BitmapBuffer()
    {
      free(data);
    }

    inline void clear()
    {
      drawSolidFilledRect(0, 0, width, height, 0);
    }

    inline void drawPixel(display_t * p, display_t value)
    {
      *p = value;
    }

    inline display_t * getPixelPtr(coord_t x, coord_t y)
    {
      return &data[y*width + x];
    }

    inline void drawPixel(coord_t x, coord_t y, display_t value)
    {
      display_t * p = getPixelPtr(x, y);
      drawPixel(p, value);
    }

    void drawAlphaPixel(display_t * p, uint8_t opacity, uint16_t color);

    inline void drawAlphaPixel(coord_t x, coord_t y, uint8_t opacity, uint16_t color)
    {
      display_t * p = getPixelPtr(x, y);
      drawAlphaPixel(p, opacity, color);
    }

    void drawHorizontalLine(coord_t x, coord_t y, coord_t w, uint8_t pat, LcdFlags att);

    void drawVerticalLine(coord_t x, coord_t y, coord_t h, uint8_t pat, LcdFlags att);

    inline void drawSolidFilledRect(coord_t x, coord_t y, coord_t w, coord_t h, LcdFlags flags)
    {
      DMAFillRect(data, width, x, y, w, h, lcdColorTable[COLOR_IDX(flags)]);
    }

    void drawFilledRect(coord_t x, coord_t y, coord_t w, coord_t h, uint8_t pat, LcdFlags att);

    void invertRect(coord_t x, coord_t y, coord_t w, coord_t h, LcdFlags att);

    void drawCircle(int x0, int y0, int radius);

    void drawPie(int x0, int y0, int radius, int startAngle, int endAngle);

    void drawBitmapPie(int x0, int y0, const uint16_t * img, int startAngle, int endAngle);

    void drawBitmapPatternPie(coord_t x0, coord_t y0, const uint8_t * img, LcdFlags flags, int startAngle, int endAngle);

    static BitmapBuffer * load(const char * filename);

    void drawBitmapPattern(coord_t x, coord_t y, const uint8_t * bmp, LcdFlags flags, coord_t offset=0, coord_t width=0);

    void drawFontPattern(coord_t x, coord_t y, const uint8_t * font, const uint16_t * spec, int index, LcdFlags flags);

    void drawSizedText(coord_t x, coord_t y, const pm_char * s, uint8_t len, LcdFlags flags);

    template<class T>
    void drawBitmap(coord_t x, coord_t y, const T * bmp, coord_t srcx=0, coord_t srcy=0, coord_t w=0, coord_t h=0, float scale=0)
    {
      int srcw = bmp->getWidth();
      int srch = bmp->getHeight();

      if (w == 0)
        w = srcw;
      if (h == 0)
        h = srch;
      if (srcx+w > srcw)
        w = srcw - srcx;
      if (srcy+h > srch)
        h = srch - srcy;

      if (scale == 0) {
        if (x + w > width) {
          w = width - x;
        }
        if (y + h > height) {
          h = height - y;
        }
        DMACopyBitmap(data, width, x, y, bmp->data, srcw, srcx, srcy, w, h);
      }
      else {
        int scaledw = w * scale;
        int scaledh = h * scale;

        if (x + scaledw > width)
          scaledw = width - x;
        if (y + scaledh > height)
          scaledh = height - y;

        for (int i = 0; i < scaledh; i++) {
          uint16_t * p = &data[(y + i) * width + x];
          const uint16_t * qstart = &bmp->data[(srcy + int(i / scale)) * bmp->getWidth() + srcx];
          for (int j = 0; j < scaledw; j++) {
            const uint16_t * q = qstart + int(j / scale);
            *p = *q;
            p++;
          }
        }
      }
    }

    template<class T>
    void drawScaledBitmap(const T * bitmap, coord_t x, coord_t y, coord_t w, coord_t h)
    {
      coord_t bitmapWidth = bitmap->getWidth();

      float scale = float(h) / bitmap->getHeight();
      int width = (0.5 + bitmapWidth) * scale;
      if (width > w) {
        int ww = (0.5 + w) / scale;
        drawBitmap(x, y, bitmap, (bitmapWidth - ww)/2, 0, ww, 0, scale);
      }
      else {
        drawBitmap(x+(w-width)/2, y, bitmap, 0, 0, 0, 0, scale);
      }
    }

    template<class T>
    void drawAlphaBitmap(coord_t x, coord_t y, const T * bmp)
    {
      int width = bmp->getWidth();
      int height = bmp->getHeight();

      if (width == 0 || height == 0) {
        return;
      }

      DMACopyAlphaBitmap(data, this->width, x, y, bmp->data, width, height);
    }
};

extern BitmapBuffer * lcd;

#endif // _BITMAP_BUFFER_H_
