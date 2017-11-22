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

#ifndef _FONTS_H_
#define _FONTS_H_

#if defined(COLORLCD)

#if !defined(BOOT)
extern const uint16_t * const fontspecsTable[16];
extern const uint8_t * const fontsTable[16];
#else
extern const uint16_t * const fontspecsTable[1];
extern const uint8_t * const fontsTable[1];
#endif

#if defined(PCBHORUS)
extern BitmapBuffer * fontCache[2];
void loadFontCache();
#endif

#else

extern const pm_uchar font_5x7[];
extern const pm_uchar font_10x14[];

#if defined(BOLD_FONT) && ((!defined(CPUM64) && !defined(PCBMEGA2560)) || defined(TELEMETRY_NONE)) && !defined(BOOT)
 #define BOLD_SPECIFIC_FONT
 extern const pm_uchar font_5x7_B[];
#endif

#if defined(CPUARM)
extern const pm_uchar font_3x5[];
extern const pm_uchar font_4x6[];
extern const pm_uchar font_8x10[];
extern const pm_uchar font_22x38_num[];
extern const pm_uchar font_5x7_extra[];
extern const pm_uchar font_10x14_extra[];
extern const pm_uchar font_4x6_extra[];
#endif

#endif

#endif // _FONTS_H_
