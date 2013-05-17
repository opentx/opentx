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

#include "../opentx.h"

#if defined(PCBTARANIS)
  const pm_uchar about_bmp[] PROGMEM = {
  #include "../bitmaps/about.lbm"
  };
#endif

enum AboutScreens {
  ABOUT_OPENTX,
  ABOUT_BERTRAND,
  ABOUT_MIKE,
  ABOUT_ROMOLO,
  ABOUT_ANDRE,
  ABOUT_ROB,
  ABOUT_MARTIN,
  ABOUT_HARDWARE,
  ABOUT_PARENTS,
  ABOUT_END,
  ABOUT_COUNT,
};

void menuAboutView(uint8_t event)
{
  static int screenIndex;
  static int greyIndex;

  switch(event)
  {
    case EVT_ENTRY:
      screenIndex = 0;
      greyIndex = 0;
      break;
    case EVT_KEY_FIRST(KEY_DOWN):
      screenIndex < ABOUT_PARENTS ? screenIndex++ : screenIndex = ABOUT_OPENTX;
      greyIndex = 0;
      break;
    case EVT_KEY_FIRST(KEY_UP):
      screenIndex > ABOUT_OPENTX ? screenIndex-- : screenIndex = ABOUT_PARENTS;
      greyIndex = 0;
      break;
    case EVT_KEY_FIRST(KEY_EXIT):
      chainMenu(menuMainView);
    return;
  }

  lcd_bmp(0, 0, about_bmp);
  lcd_putsAtt(64, 0, PSTR("ABOUT US"), DBLSIZE);
  lcd_hline(62, 18, 120);
  lcd_hline(62, 19, 130, GREY_DEFAULT);

  LcdFlags att = GREY(max(0, 15-greyIndex/2));
  int screenDuration = 150;

  switch (screenIndex) {
    case ABOUT_OPENTX:
    case ABOUT_END:
      lcd_putsAtt(62, 24, PSTR("OpenTX was developed for free"), SMLSIZE);
      lcd_putsAtt(62, 36, PSTR("Your donation will ensure"), SMLSIZE);
      lcd_putsAtt(62, 44, PSTR("that it remains free and open"), SMLSIZE);
      lcd_putsAtt(62, 56, PSTR("The amount is up to you!"), SMLSIZE);
      screenDuration = 200;
      break;

    case ABOUT_BERTRAND:
      lcd_putsAtt(62, 24, PSTR("Bertrand Songis"), att);
      lcd_putsAtt(68, 36, PSTR("OpenTX main author"), att|SMLSIZE);
      lcd_putsAtt(68, 44, PSTR("Companion9x co-developer"), att|SMLSIZE);
      break;

    case ABOUT_MIKE:
      lcd_putsAtt(62, 24, PSTR("Mike Blandford"), att);
      lcd_putsAtt(68, 36, PSTR("Code and drivers guru"), att|SMLSIZE);
      lcd_putsAtt(68, 44, PSTR("Arguably, one of the best"), att|SMLSIZE);
      lcd_putsAtt(67, 52, PSTR("Inspirational"), att|SMLSIZE);
      break;

    case ABOUT_ROMOLO:
      lcd_putsAtt(62, 24, PSTR("Romolo Manfredini"), att);
      lcd_putsAtt(68, 36, PSTR("Companion9x main developer"), att|SMLSIZE);
      break;

    case ABOUT_ANDRE:
      lcd_putsAtt(62, 24, PSTR("Andre Bernet"), att);
      lcd_putsAtt(68, 36, PSTR("Features / usability"), att|SMLSIZE);
      break;

    case ABOUT_ROB:
      lcd_putsAtt(62, 24, PSTR("Rob Thomson"), att);
      lcd_putsAtt(68, 36, PSTR("Openrcforums webmaster"), att|SMLSIZE);
      break;

    case ABOUT_MARTIN:
      lcd_putsAtt(62, 24, PSTR("Martin Hotar"), att);
      lcd_putsAtt(68, 36, PSTR("Graphics designer"), att|SMLSIZE);
      break;

    case ABOUT_HARDWARE:
      lcd_putsAtt(62, 24, PSTR("FrSky"), att);
      lcd_putsAtt(68, 36, PSTR("Hardware designer/producer"), att|SMLSIZE);
      screenDuration = 300;
      break;

    case ABOUT_PARENTS:
      lcd_putsAtt(62, 24, PSTR("Parent projects"), att);
      lcd_putsAtt(68, 36, PSTR("Ersky9x (Mike Blandford)"), att|SMLSIZE);
      lcd_putsAtt(68, 44, PSTR("Er9x (Erez Raviv)"), att|SMLSIZE);
      lcd_putsAtt(67, 52, PSTR("Th9x (Thomas Husterer)"), att|SMLSIZE);
      screenDuration = 300;
      break;
  }

  if (++greyIndex == screenDuration) {
    greyIndex = 0;
    if (++screenIndex == ABOUT_COUNT) {
      chainMenu(menuMainView);
    }
  }
}
