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

#ifndef fonts_h
#define fonts_h

#if defined(COLORLCD)

extern const uint16_t *fontspecsTable[16];
extern const pm_uchar *fontsTable[16];

#else

extern const pm_uchar font_5x7[];
extern const pm_uchar font_10x14[];

#if defined(BOLD_FONT) && (!defined(CPUM64) || defined(EXTSTD)) && !defined(BOOT)
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

#endif
