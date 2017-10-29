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

#if defined(PCBX10) && !defined(SIMU)
  #define MOVE_PIXEL_RIGHT(p, count)   p -= count
#else
  #define MOVE_PIXEL_RIGHT(p, count)   p += count
#endif

#define MOVE_TO_NEXT_RIGHT_PIXEL(p)    MOVE_PIXEL_RIGHT(p, 1)


#define USE_STB

// TODO should go to lcd.h again
typedef int coord_t;
typedef uint32_t LcdFlags;
typedef uint16_t display_t;

enum BitmapFormats
{
  BMP_RGB565,
  BMP_ARGB4444
};

template<class T>
class BitmapBufferBase
{
  public:
    BitmapBufferBase(uint8_t format, uint16_t width, uint16_t height, T * data):
      format(format),
      width(width),
      height(height),
      data(data),
      data_end(data + (width * height))
    {
    }

    inline uint8_t getFormat() const
    {
      return format;
    }

    inline uint16_t getWidth() const
    {
      return width;
    }

    inline uint16_t getHeight() const
    {
      return height;
    }

    inline T * getData() const
    {
      return data;
    }

    uint32_t getDataSize() const
    {
      return width * height * sizeof(T);
    }

    inline const display_t * getPixelPtr(coord_t x, coord_t y) const
    {
#if defined(PCBX10) && !defined(SIMU)
      x = width - x - 1;
      y = height - y - 1;
#endif
      return &data[y*width + x];
    }

  protected:
    uint8_t format;
    uint16_t width;
    uint16_t height;
    T * data;
    T * data_end;
};

typedef BitmapBufferBase<const uint16_t> Bitmap;

class BitmapBuffer: public BitmapBufferBase<uint16_t>
{
  private:
    bool dataAllocated;
#if defined(DEBUG)
    bool leakReported;
#endif

  public:

    BitmapBuffer(uint8_t format, uint16_t width, uint16_t height):
      BitmapBufferBase<uint16_t>(format, width, height, NULL),
      dataAllocated(true)
#if defined(DEBUG)
      , leakReported(false)
#endif
    {
      data = (uint16_t *)malloc(width*height*sizeof(uint16_t));
      data_end = data + (width * height);
    }

    BitmapBuffer(uint8_t format, uint16_t width, uint16_t height, uint16_t * data):
      BitmapBufferBase<uint16_t>(format, width, height, data),
      dataAllocated(false)
#if defined(DEBUG)
      , leakReported(false)
#endif
    {
    }

    ~BitmapBuffer()
    {
      if (dataAllocated) {
        free(data);
      }
    }

    inline void setFormat(uint8_t format)
    {
      this->format = format;
    }

    inline void clear(LcdFlags flags=0)
    {
      drawSolidFilledRect(0, 0, width, height, flags);
    }

    inline void drawPixel(display_t * p, display_t value)
    {
      if (data && (data <= p || p < data_end)) {
        *p = value;
      }
#if defined(DEBUG)
      else if (!leakReported) {
        leakReported = true;
        TRACE("BitmapBuffer(%p).drawPixel(): buffer overrun, data: %p, written at: %p", this, data, p);
      }
#endif
    }

    inline const display_t * getPixelPtr(coord_t x, coord_t y) const
    {
#if defined(PCBX10) && !defined(SIMU)
      x = width - x - 1;
      y = height - y - 1;
#endif
      return &data[y*width + x];
    }

    inline display_t * getPixelPtr(coord_t x, coord_t y)
    {
#if defined(PCBX10) && !defined(SIMU)
      x = width - x - 1;
      y = height - y - 1;
#endif
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

    inline void drawSolidHorizontalLine(coord_t x, coord_t y, coord_t w, LcdFlags att)
    {
      drawSolidFilledRect(x, y, w, 1, att);
    }

    inline void drawSolidVerticalLine(coord_t x, coord_t y, coord_t h, LcdFlags att)
    {
      drawSolidFilledRect(x, y, 1, h, att);
    }

    void drawRect(coord_t x, coord_t y, coord_t w, coord_t h, uint8_t thickness, uint8_t pat, LcdFlags att);

    inline void drawSolidFilledRect(coord_t x, coord_t y, coord_t w, coord_t h, LcdFlags flags)
    {
      if (!data || h==0 || w==0) return;
      if (h<0) { y+=h; h=-h; }
      if (w<0) { x+=w; w=-w; }
      DMAFillRect(data, width, height, (x>0)?x:0, (y>0)?y:0, w, h, lcdColorTable[COLOR_IDX(flags)]);
    }

    void drawFilledRect(coord_t x, coord_t y, coord_t w, coord_t h, uint8_t pat, LcdFlags att);

    void invertRect(coord_t x, coord_t y, coord_t w, coord_t h, LcdFlags att);

    void drawCircle(int x0, int y0, int radius);

    void drawPie(int x0, int y0, int radius, int startAngle, int endAngle);

    void drawBitmapPie(int x0, int y0, const uint16_t * img, int startAngle, int endAngle);

    void drawBitmapPatternPie(coord_t x0, coord_t y0, const uint8_t * img, LcdFlags flags, int startAngle, int endAngle);

    static BitmapBuffer * load(const char * filename);

    static BitmapBuffer * loadMask(const char * filename);

    static BitmapBuffer * loadMaskOnBackground(const char * filename, LcdFlags foreground, LcdFlags background);

    void drawMask(coord_t x, coord_t y, BitmapBuffer * mask, LcdFlags flags, coord_t offset=0, coord_t width=0);

    void drawBitmapPattern(coord_t x, coord_t y, const uint8_t * bmp, LcdFlags flags, coord_t offset=0, coord_t width=0);

    uint8_t drawCharWithoutCache(coord_t x, coord_t y, const uint8_t * font, const uint16_t * spec, int index, LcdFlags flags);

    uint8_t drawCharWithCache(coord_t x, coord_t y, const BitmapBuffer * font, const uint16_t * spec, int index, LcdFlags flags);

    void drawText(coord_t x, coord_t y, const char * s, LcdFlags flags)
    {
      drawSizedText(x, y, s, 255, flags);
    }

    void drawSizedText(coord_t x, coord_t y, const char * s, uint8_t len, LcdFlags flags);

    template<class T>
    void drawBitmap(coord_t x, coord_t y, const T * bmp, coord_t srcx=0, coord_t srcy=0, coord_t w=0, coord_t h=0, float scale=0)
    {
      if (!data || !bmp || x < 0 || x >= width || y < 0 || y >= height)
        return;

      coord_t srcw = bmp->getWidth();
      coord_t srch = bmp->getHeight();

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
        if (bmp->getFormat() == BMP_ARGB4444) {
          DMACopyAlphaBitmap(data, width, height, x, y, bmp->getData(), srcw, srch, srcx, srcy, w, h);
        }
        else {
          DMACopyBitmap(data, width, height, x, y, bmp->getData(), srcw, srch, srcx, srcy, w, h);
        }
      }
      else {
        int scaledw = w * scale;
        int scaledh = h * scale;

        if (x + scaledw > width)
          scaledw = width - x;
        if (y + scaledh > height)
          scaledh = height - y;

        for (int i = 0; i < scaledh; i++) {
          display_t * p = getPixelPtr(x, y + i);
          const display_t * qstart = bmp->getPixelPtr(srcx, srcy + int(i / scale));
          for (int j = 0; j < scaledw; j++) {
            const display_t * q = qstart;
            MOVE_PIXEL_RIGHT(q, int(j / scale));
            if (bmp->getFormat() == BMP_ARGB4444) {
              ARGB_SPLIT(*q, a, r, g, b);
              drawAlphaPixel(p, a, RGB_JOIN(r<<1, g<<2, b<<1));
            }
            else {
              drawPixel(p, *q);
            }
            MOVE_TO_NEXT_RIGHT_PIXEL(p);
          }
        }
      }
    }

    template<class T>
    void drawScaledBitmap(const T * bitmap, coord_t x, coord_t y, coord_t w, coord_t h)
    {
      float vscale = float(h) / bitmap->getHeight();
      float hscale = float(w) / bitmap->getWidth();
      float scale = vscale < hscale ? vscale : hscale;

      int xshift = (w - (bitmap->getWidth() * scale)) / 2;
      int yshift = (h - (bitmap->getHeight() * scale)) / 2;
      drawBitmap(x + xshift, y + yshift, bitmap, 0, 0, 0, 0, scale);
    }

  protected:
    static BitmapBuffer * load_bmp(const char * filename);
    static BitmapBuffer * load_stb(const char * filename);
};

extern BitmapBuffer * lcd;

#endif // _BITMAP_BUFFER_H_
