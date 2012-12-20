/*
 * Authors (alphabetical order)
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Jean-Pierre Parisy
 * - Karl Szmutny <shadow@privy.de>
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * open9x is based on code named
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

#include "../open9x.h"


// keys:
// KEY_EXIT    PA31 (PC24)
// KEY_MENU    PB6 (PB5)
// KEY_DOWN  LCD5  PC3 (PC5)
// KEY_UP    LCD6  PC2 (PC1)
// KEY_RIGHT LCD4  PC4 (PC4)
// KEY_LEFT  LCD3  PC5 (PC3)
// Reqd. bit 6 LEFT, 5 RIGHT, 4 UP, 3 DOWN 2 EXIT 1 MENU
// LCD pins 5 DOWN, 4 RIGHT, 3 LEFT, 1 UP
uint32_t readKeys()
{
  register uint32_t x = GPIOC->IDR; // 6 LEFT, 5 RIGHT, 4 DOWN, 3 UP
  register uint32_t y = 0;

  if (x & 0x00000020)
    y |= 0x02 << KEY_ENTER;
  if (x & 0x00000010)
    y |= 0x02 << KEY_PLUS;
  if (x & 0x00000008)
    y |= 0x02 << KEY_MINUS;
  if (x & 0x00000002)
    y |= 0x02 << KEY_MENU;
  if (x & 0x00000040)
    y |= 0x02 << KEY_PAGE;
  if (x & 0x01000000)
    y |= 0x02 << KEY_EXIT;

  // printf("readKeys(): %x => %x\n", x, y); fflush(stdout);

  return y ;
}

uint32_t readTrims()
{
#if 0

  uint32_t trims;
  uint32_t trima;

  trims = 0;

  trima = PIOA->PIO_PDSR;

// TRIM_LH_DOWN    PA7 (PA23)
#if defined(REVA)
  if ( ( trima & 0x0080 ) == 0 )
#else
    if ((trima & 0x00800000) == 0)
#endif
  {
    trims |= 1;
  }

// TRIM_LV_DOWN  PA27 (PA24)
#if defined(REVA)
  if ( ( trima & 0x08000000 ) == 0 )
#else
  if ((trima & 0x01000000) == 0)
#endif
  {
    trims |= 4;
  }

// TRIM_RV_UP    PA30 (PA1)
#if defined(REVA)
  if ( ( trima & 0x40000000 ) == 0 )
#else
  if ((trima & 0x00000002) == 0)
#endif
  {
    trims |= 0x20;
  }

// TRIM_RH_DOWN    PA29 (PA0)
#if defined(REVA)
  if ( ( trima & 0x20000000 ) == 0 )
#else
  if ((trima & 0x00000001) == 0)
#endif
  {
    trims |= 0x40;
  }

// TRIM_LH_UP PB4
  if ((PIOB->PIO_PDSR & 0x10) == 0) {
    trims |= 2;
  }

  trima = PIOC->PIO_PDSR;
// TRIM_LV_UP   PC28
  if ((trima & 0x10000000) == 0) {
    trims |= 8;
  }

// TRIM_RV_DOWN   PC10
  if ((trima & 0x00000400) == 0) {
    trims |= 0x10;
  }

// TRIM_RH_UP   PC9
  if ((trima & 0x00000200) == 0) {
    trims |= 0x80;
  }

  return trims;
#endif
  return 0;
}

uint8_t keyDown()
{
  return ~readKeys() & 0x7E ;
}

/* TODO common to ARM */
void readKeysAndTrims()
{
  register uint32_t i;

  uint8_t enuk = KEY_MENU;
  uint8_t in = ~readKeys();
  for (i = 1; i < 7; i++) {
    keys[enuk].input(in & (1 << i), (EnumKeys) enuk);
    ++enuk;
  }

  in = readTrims();

  for (i = 1; i < 256; i <<= 1) {
    keys[enuk].input(in & i, (EnumKeys) enuk);
    ++enuk;
  }
}
