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

// TODO FrSky code included PIN_BIND and PIN_POWER

uint32_t readKeys()
{
  register uint32_t d = GPIOD->IDR;
  register uint32_t e = GPIOE->IDR;
  register uint32_t result = 0;

  if (e & PIN_BUTTON_ENTER)
    result |= 0x02 << KEY_ENTER;
  if (e & PIN_BUTTON_PLUS)
    result |= 0x02 << KEY_PLUS;
  if (e & PIN_BUTTON_MINUS)
    result |= 0x02 << KEY_MINUS;

  if (d & PIN_BUTTON_MENU)
    result |= 0x02 << KEY_MENU;
  if (d & PIN_BUTTON_PAGE)
    result |= 0x02 << KEY_PAGE;
  if (d & PIN_BUTTON_EXIT)
    result |= 0x02 << KEY_EXIT;

  // printf("readKeys(): %x %x => %x\n", d, e, result); fflush(stdout);

  return result;
}

uint32_t readTrims()
{
  register uint32_t c = GPIOC->IDR;
  register uint32_t e = GPIOE->IDR;
  register uint32_t result = 0;

  if (c & PIN_TRIM1_UP)
    result |= 0x02;
  if (c & PIN_TRIM1_DN)
    result |= 0x01;
  if (c & PIN_TRIM2_UP)
    result |= 0x04;
  if (c & PIN_TRIM2_DN)
    result |= 0x08;

  if (e & PIN_TRIM3_UP)
    result |= 0x10;
  if (e & PIN_TRIM3_DN)
    result |= 0x20;
  if (e & PIN_TRIM4_UP)
    result |= 0x40;
  if (e & PIN_TRIM4_DN)
    result |= 0x80;

  return result;
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
  uint32_t in = ~readKeys();
  for (i = 1; i < 7; i++) {
    keys[enuk].input(in & (1 << i), (EnumKeys) enuk);
    ++enuk;
  }

  in = readTrims();

  /* for (i = 1; i < 256; i <<= 1) {
    keys[enuk].input(in & i, (EnumKeys)enuk);
    ++enuk;
  } */
}

uint32_t keyState(EnumKeys enuk)
{
  register uint32_t a = GPIOA->IDR;
  register uint32_t b = GPIOB->IDR;
  register uint32_t e = GPIOE->IDR;

  register uint32_t xxx = 0;

  if (enuk < (int) DIM(keys)) return keys[enuk].state() ? 1 : 0;

  switch ((uint8_t) enuk) {
    case SW_SA0:
      xxx = ~e & PIN_SW_A_L;
      break;
    case SW_SA2:
      xxx = e & PIN_SW_A_L;
      break;

    case SW_SB0:
      xxx = !(b & (PIN_SW_B_L | PIN_SW_B_H));
      break;
    case SW_SB1:
      xxx = (b & PIN_SW_B_H) && (~b & PIN_SW_B_L);
      break;
    case SW_SB2:
      xxx = (~b & PIN_SW_B_H) && (b & PIN_SW_B_L);
      break;

    case SW_SC0:
      xxx = !(b & (PIN_SW_C_L | PIN_SW_C_H));
      break;
    case SW_SC1:
      xxx = (b & PIN_SW_C_H) && (~b & PIN_SW_C_L);
      break;
    case SW_SC2:
      xxx = (~b & PIN_SW_C_H) && (b & PIN_SW_C_L);
      break;

    case SW_SD0:
      xxx = (~e & PIN_SW_D_L) && (~b & PIN_SW_D_H);
      break;
    case SW_SD1:
      xxx = (b & PIN_SW_D_H) && (~e & PIN_SW_D_L);
      break;
    case SW_SD2:
      xxx = (~b & PIN_SW_D_H) && (e & PIN_SW_D_L);
      break;

    case SW_SE0:
      xxx = (~a & PIN_SW_E_L) && (~b & PIN_SW_E_H);
      break;
    case SW_SE1:
      xxx = (b & PIN_SW_E_H) && (~a & PIN_SW_E_L);
      break;
    case SW_SE2:
      xxx = (~b & PIN_SW_E_H) && (a & PIN_SW_E_L);
      break;

    case SW_SF0:
      xxx = (~b & PIN_SW_F_L) && (~e & PIN_SW_F_H);
      break;
    case SW_SF1:
      xxx = (e & PIN_SW_F_H) && (~b & PIN_SW_F_L);
      break;
    case SW_SF2:
      xxx = (~e & PIN_SW_F_H) && (b & PIN_SW_F_L);
      break;

    case SW_SG0:
      xxx = !(e & (PIN_SW_G_L | PIN_SW_G_H));
      break;
    case SW_SG1:
      xxx = (e & PIN_SW_G_H) && (~e & PIN_SW_G_L);
      break;
    case SW_SG2:
      xxx = (~e & PIN_SW_G_H) && (e & PIN_SW_G_L);
      break;

    case SW_SH0:
      xxx = ~e & PIN_SW_H_L;
      break;
    case SW_SH2:
      xxx = e & PIN_SW_H_L;
      break;

    default:
      break;
  }

  if (xxx) {
    return 1;
  }

  return 0;
}

#if !defined(SIMU)
void keysInit()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    /* GPIOC GPIOD clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOBUTTON, ENABLE);

    /* GPIO  Configuration*/
    GPIO_InitStructure.GPIO_Pin = PIN_BUTTON_PLUS | PIN_BUTTON_ENTER | PIN_BUTTON_MINUS | PIN_TRIM3_UP | PIN_TRIM3_DN
                                  | PIN_TRIM4_UP |PIN_TRIM4_DN | PIN_SW_A_L | PIN_SW_D_L | PIN_SW_F_H | PIN_SW_G_H
                                  | PIN_SW_G_L | PIN_SW_H_L;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = PIN_BUTTON_MENU | PIN_BUTTON_EXIT | PIN_BUTTON_PAGE;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = PIN_TRIM1_UP | PIN_TRIM1_DN | PIN_TRIM2_UP | PIN_TRIM2_DN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = PIN_SW_B_H | PIN_SW_B_L | PIN_SW_C_H | PIN_SW_C_L | PIN_SW_D_H | PIN_SW_E_H | PIN_SW_F_L;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = PIN_SW_E_L;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}
#endif
