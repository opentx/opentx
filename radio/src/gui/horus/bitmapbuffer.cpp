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
    COLOR_SPLIT(color, red, green, blue);
    COLOR_SPLIT(*p, bgRed, bgGreen, bgBlue);
    uint16_t r = (bgRed * bgWeight + red * opacity) / OPACITY_MAX;
    uint16_t g = (bgGreen * bgWeight + green * opacity) / OPACITY_MAX;
    uint16_t b = (bgBlue * bgWeight + blue * opacity) / OPACITY_MAX;
    drawPixel(p, COLOR_JOIN(r, g, b));
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
      p++;
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
      p++;
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
  COLOR_SPLIT(color, red, green, blue);

  for (int i=y; i<y+h; i++) {
    display_t * p = getPixelPtr(x, i);
    for (int j=0; j<w; j++) {
      // TODO ASSERT_IN_DISPLAY(p);
      COLOR_SPLIT(*p, bgRed, bgGreen, bgBlue);
      drawPixel(p++, COLOR_JOIN(0x1F + red - bgRed, 0x3F + green - bgGreen, 0x1F + blue - bgBlue));
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

#define PI 3.14159265

bool evalSlopes(int * slopes, int startAngle, int endAngle)
{
  if (startAngle >= 360 || endAngle <= 0)
    return false;

  if (startAngle == 0) {
    slopes[1] = 100000;
    slopes[2] = -100000;
  }
  else {
    float angle1 = float(startAngle) * PI / 180;
    if (startAngle >= 180) {
      slopes[1] = -100000;
      slopes[2] = cos(angle1) * 100 / sin(angle1);
    }
    else {
      slopes[1] = cos(angle1) * 100 / sin(angle1);
      slopes[2] = -100000;
    }
  }

  if (endAngle == 360) {
    slopes[0] = -100000;
    slopes[3] = 100000;
  }
  else {
    float angle2 = float(endAngle) * PI / 180;
    if (endAngle >= 180) {
      slopes[0] = -100000;
      slopes[3] = -cos(angle2) * 100 / sin(angle2);
    }
    else {
      slopes[0] = cos(angle2) * 100 / sin(angle2);
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
    display_t * p = getPixelPtr(x, y+row);
    const uint8_t * q = bmp + 4 + row*w + offset;
    for (coord_t col=0; col<width; col++) {
      drawAlphaPixel(p, *q, color);
      p++; q++;
    }
  }
}

#define FONT_MAX_HEIGHT 42
void BitmapBuffer::drawFontPattern(coord_t x, coord_t y, const uint8_t * font, const uint16_t * spec, int index, LcdFlags flags)
{
  coord_t offset = spec[index];
  coord_t width = spec[index+1] - offset;
  if (width > 0) drawBitmapPattern(x, y, font, flags, offset, width);
  lcdNextPos = x + width;
}

void BitmapBuffer::drawSizedText(coord_t x, coord_t y, const pm_char * s, uint8_t len, LcdFlags flags)
{
  int width = getTextWidth(s, len, flags);
  int height = getFontHeight(flags);
  int fontindex = FONTSIZE(flags) >> 8;
  const pm_uchar * font = fontsTable[fontindex];
  const uint16_t * fontspecs = fontspecsTable[fontindex];

  if (flags & RIGHT)
    x -= width;
  else if (flags & CENTERED)
    x -= width/2;

  if ((flags&INVERS) && ((~flags & BLINK) || BLINK_ON_PHASE)) {
    flags = TEXT_INVERTED_COLOR | (flags & 0x0ffff);
    if (FONTSIZE(flags) == TINSIZE)
      drawSolidFilledRect(x-INVERT_HORZ_MARGIN+2, y-INVERT_VERT_MARGIN+2, width+2*INVERT_HORZ_MARGIN-5, INVERT_LINE_HEIGHT-7, TEXT_INVERTED_BGCOLOR);
    else if (FONTSIZE(flags) == SMLSIZE)
      drawSolidFilledRect(x-INVERT_HORZ_MARGIN+1, y-INVERT_VERT_MARGIN, width+2*INVERT_HORZ_MARGIN-2, INVERT_LINE_HEIGHT, TEXT_INVERTED_BGCOLOR);
    else
      drawSolidFilledRect(x-INVERT_HORZ_MARGIN, y/*-INVERT_VERT_MARGIN*/, width+2*INVERT_HORZ_MARGIN, INVERT_LINE_HEIGHT, TEXT_INVERTED_BGCOLOR);
  }

  char str[256];
  if (flags & ZCHAR)
    strcat_zchar(str, s, len);
  else
    strAppend(str, s, len);

  const coord_t orig_x = x;
  bool setx = false;
  while (len--) {
    unsigned char c;
    if (flags & ZCHAR)
      c = idx2char(*s);
    else
      c = pgm_read_byte(s);
    if (setx) {
      x = c;
      setx = false;
    }
    else if (!c) {
      break;
    }
    else if (c >= 0x20) {
      drawFontPattern(x, y, font, fontspecs, getMappedChar(c), flags);
      x = lcdNextPos;
    }
    else if (c == 0x1F) {  // X-coord prefix
      setx = true;
    }
    else if (c == 0x1E) {
      x = orig_x;
      y += height;
    }
    else if (c == 1) {
      x += 1;
    }
    else {
      x += 2*(c-1);
    }
    s++;
  }
  lcdNextPos = x;
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
      int slope = (x==0 ? (y<0 ? -99000 : 99000) : y*100/x);
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


#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_ONLY_JPEG
#define STBI_ONLY_BMP
#define STBI_ONLY_GIF
#define STBI_NO_STDIO

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

BitmapBuffer * BitmapBuffer::load(const char * filename)
{
  FIL imgFile;

  FRESULT result = f_open(&imgFile, filename, FA_OPEN_EXISTING | FA_READ);
  if (result != FR_OK) {
    return NULL;
  }

  int w, h, n;
  unsigned char *data = stbi_load_from_callbacks(&stbCallbacks, &imgFile, &w, &h, &n, 3);
  f_close(&imgFile);

  if (!data) {
    return NULL;
  }

  //convert to 565 fromat
  // todo use dma2d for conversion from 888 to 565
  unsigned char *p = data;
  BitmapBuffer * bmp = new BitmapBuffer(w, h);
  uint16_t * dest = bmp->data;
  if (bmp == NULL) {
    TRACE("load_stb() malloc failed");
    stbi_image_free(data);
    return NULL;
  }

  *dest++ = w;
  *dest++ = h;

  for(int row = 0; row < h; ++row) {
    unsigned char *l = p;
    for(int col = 0; col < w; ++col) {
      *dest = RGB(l[0], l[1], l[2]);
      ++dest;
      l += 3;
    }
    p += 3 * w;
  }
  stbi_image_free(data);
  return bmp;
}
