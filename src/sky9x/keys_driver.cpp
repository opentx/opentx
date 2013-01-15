/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
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
  register uint32_t x;
  register uint32_t result = 0;

  x = lcdLock ? lcdInputs : PIOC->PIO_PDSR; // 6 LEFT, 5 RIGHT, 4 DOWN, 3 UP ()

#if defined(REVA)
  if (x & PIN_BUTTON_RIGHT)
    result |= 0x02 << KEY_RIGHT;
  if (x & PIN_BUTTON_LEFT)
    result |= 0x02 << KEY_LEFT;
  if (x & PIN_BUTTON_UP)
    result |= 0x02 << KEY_UP;
  if (x & PIN_BUTTON_DOWN)
    result |= 0x02 << KEY_DOWN;
  if (GPIO_BUTTON_EXIT & PIN_BUTTON_EXIT)
    result |= 0x02 << KEY_EXIT;
  if (GPIO_BUTTON_MENU & 0x000000040)
    result |= 0x02 << KEY_MENU;
#else
  if (x & PIN_BUTTON_RIGHT)
    result |= 0x02 << KEY_RIGHT;
  if (x & PIN_BUTTON_UP)
    result |= 0x02 << KEY_UP;
  if (x & PIN_BUTTON_LEFT)
    result |= 0x02 << KEY_LEFT;
  if (x & PIN_BUTTON_DOWN)
    result |= 0x02 << KEY_DOWN;
  if (x & PIN_BUTTON_EXIT)
    result |= 0x02 << KEY_EXIT;
  if (GPIO_BUTTON_MENU & PIN_BUTTON_MENU)
    result |= 0x02 << KEY_MENU;
#endif

  // printf("readKeys(): %x => %x\n", x, result); fflush(stdout);

  return result;
}

uint32_t readTrims()
{
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
}

uint8_t keyDown()
{
  return ~readKeys() & 0x7E ;
}

void readKeysAndTrims()
{
  register uint32_t i;

#if defined(ROTARY_ENCODERS)
  keys[BTN_REa].input(!(PIOB->PIO_PDSR & 0x40), BTN_REa);
#endif

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

uint32_t switchState(EnumKeys enuk)
{
  register uint32_t a;
  register uint32_t c;

  uint32_t xxx = 0;

  if (enuk < (int) DIM(keys)) return keys[enuk].state() ? 1 : 0;

  a = PIOA->PIO_PDSR ;
  c = PIOC->PIO_PDSR ;

  switch ((uint8_t) enuk) {
#if defined(REVA)
    case SW_ELE:
      xxx = a & 0x00000100; // ELE_DR   PA8
#else
    case SW_ELE:
      xxx = c & 0x80000000; // ELE_DR   PC31
#endif
      break;

    case SW_AIL:
      xxx = a & 0x00000004; // AIL-DR  PA2
      break;

    case SW_RUD:
      xxx = a & 0x00008000; // RUN_DR   PA15
      break;
      //     INP_G_ID1 INP_E_ID2
      // id0    0        1
      // id1    1        1
      // id2    1        0
    case SW_ID0:
      xxx = ~c & 0x00004000; // SW_IDL1     PC14
      break;
    case SW_ID1:
      xxx = (c & 0x00004000);
      if (xxx) xxx = (c & 0x00000800);
      break;
    case SW_ID2:
      xxx = ~c & 0x00000800; // SW_IDL2     PC11
      break;

    case SW_GEA:
      xxx = c & 0x00010000; // SW_GEAR     PC16
      break;

#if defined(REVA)
    case SW_THR:
      xxx = a & 0x10000000; // SW_TCUT     PA28
#else
    case SW_THR:
      xxx = c & 0x00100000; // SW_TCUT     PC20
#endif
      break;

    case SW_TRN:
      xxx = c & 0x00000100; // SW-TRAIN    PC8
      break;

    default:
      break;
  }

  if (xxx) {
    return 1;
  }
  return 0;
}
