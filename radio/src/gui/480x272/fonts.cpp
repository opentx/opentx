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
#include "rle.h"

#if !defined(BOOT)
const uint16_t font_tinsize_specs[] = {
#include "font_tinsize.specs"
};

const unsigned char font_tinsize[] = {
#include "font_tinsize.lbm"
};

const uint16_t font_smlsize_specs[] = {
#include "font_smlsize.specs"
};

const unsigned char font_smlsize[] = {
#include "font_smlsize.lbm"
};
#endif

const uint16_t font_stdsize_specs[] = {
#include "font_stdsize.specs"
};

const unsigned char font_stdsize[] = {
#include "font_stdsize.lbm"
};

#if !defined(BOOT)
const uint16_t font_midsize_specs[] = {
#include "font_midsize.specs"
};

const unsigned char font_midsize[] = {
#include "font_midsize.lbm"
};

const uint16_t font_dblsize_specs[] = {
#include "font_dblsize.specs"
};

const unsigned char font_dblsize[] = {
#include "font_dblsize.lbm"
};

const uint16_t font_xxlsize_specs[] = {
#include "font_xxlsize.specs"
};

const unsigned char font_xxlsize[] = {
#include "font_xxlsize.lbm"
};

const uint16_t font_stdsizebold_specs[] = {
#include "font_stdsizebold.specs"
};

const unsigned char font_stdsizebold[] = {
#include "font_stdsizebold.lbm"
};
#endif

#if !defined(BOOT)
const uint16_t * const fontspecsTable[16] = {
  font_stdsize_specs,     font_tinsize_specs, font_smlsize_specs, font_midsize_specs, font_dblsize_specs, font_xxlsize_specs, font_stdsize_specs, font_stdsize_specs,
  font_stdsizebold_specs, font_tinsize_specs, font_smlsize_specs, font_midsize_specs, font_dblsize_specs, font_xxlsize_specs, font_stdsize_specs, font_stdsize_specs
};

const uint8_t * fontsTable[16] = {
  font_stdsize,     font_tinsize, font_smlsize, font_midsize, font_dblsize, font_xxlsize, font_stdsize, font_stdsize,
  font_stdsizebold, font_tinsize, font_smlsize, font_midsize, font_dblsize, font_xxlsize, font_stdsize, font_stdsize
};

#else
const uint16_t * const fontspecsTable[1] = { font_stdsize_specs };
const uint8_t * fontsTable[1] = { font_stdsize };
#endif

BitmapBuffer * fontCache[2] = { NULL, NULL };

BitmapBuffer * createFontCache(const uint8_t * font, LcdFlags fg, LcdFlags bg)
{
  coord_t width = *((uint16_t *)font);
  coord_t height = *(((uint16_t *)font)+1);

  BitmapBuffer * buffer = new BitmapBuffer(BMP_RGB565, width, height);
  if (buffer) {
    buffer->clear(bg);
    buffer->drawBitmapPattern(0, 0, font, fg);
  }
  return buffer;
}

void loadFontCache()
{
  delete fontCache[0];
  delete fontCache[1];
  fontCache[0] = createFontCache(fontsTable[0], TEXT_COLOR, TEXT_BGCOLOR);
  fontCache[1] = createFontCache(fontsTable[0], TEXT_INVERTED_COLOR, TEXT_INVERTED_BGCOLOR);
}


static uint8_t* decompressFont(const uint8_t* font)
{
  uint16_t width = *((uint16_t *)font);
  uint16_t height = *(((uint16_t *)font)+1);

  size_t font_size = width * height;
  uint8_t* dec_buf = (uint8_t*)malloc(font_size + 4);

  // copy width / height
  memcpy(dec_buf,font,4);

  rle_decode_8bit(dec_buf+4, font_size, font+4);
  return dec_buf;
}

void loadFonts()
{
  static bool fonts_loaded = false;
  if (fonts_loaded) return;

#if !defined(BOOT)
  int i=0;
  for (; i<9; i++)
    fontsTable[i] = decompressFont(fontsTable[i]);

  for (; i<16; i++)
    fontsTable[i] = fontsTable[i-9];
#else
  fontsTable[0] = decompressFont(fontsTable[0]);
#endif

  fonts_loaded = true;
}
