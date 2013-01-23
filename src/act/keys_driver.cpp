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

uint32_t readKeys()
{
  register uint32_t x = GPIOC->IDR;
  register uint32_t result = 0;

  if (x & PIN_BUTTON_MENU)
    result |= 0x02 << KEY_MENU;
  if (x & PIN_BUTTON_EXIT)
    result |= 0x02 << KEY_EXIT;
  if (x & PIN_BUTTON_CLR)
    result |= 0x02 << KEY_CLR;
  if (x & PIN_BUTTON_PAGE)
    result |= 0x02 << KEY_PAGE;

  // printf("readKeys(): %x => %x\n", x, result); fflush(stdout);

  return result;
}

uint32_t readTrims()
{
  register uint32_t e = GPIOE->IDR;
  register uint32_t result = 0;

  if (~e & PIN_TRIM_LH_L)
    result |= 0x01;         // LH_L
  if (~e & PIN_TRIM_LH_R)
    result |= 0x02;         // LH_R
  if (~e & PIN_TRIM_LV_DN)
    result |= 0x04;         // LV_DN
  if (~e & PIN_TRIM_LV_UP)
    result |= 0x08;         // LV_UP

  if (~e & PIN_TRIM_RV_DN)
    result |= 0x10;         // RV_DN
  if (~e & PIN_TRIM_RV_UP)
    result |= 0x20;         // RV_UP
  if (~e & PIN_TRIM_RH_L)
    result |= 0x40;         // RH_L
  if (~e & PIN_TRIM_RH_R)
    result |= 0x80;         // RH_R

  // printf("readTrims(): %x %x => %x\n", c, e, result); fflush(stdout);

  return result;
}

uint8_t keyDown()
{
  return ~readKeys() & 0x1e ;
}

/* TODO common to ARM */
void readKeysAndTrims()
{
  register uint32_t i;

  uint8_t enuk = KEY_MENU;
  uint8_t in = ~readKeys();
  for (i = 1; i < 5; i++) {
    keys[enuk].input(in & (1 << i), (EnumKeys) enuk);
    ++enuk;
  }

  in = readTrims();

  for (i = 1; i < 256; i <<= 1) {
    keys[enuk].input(in & i, (EnumKeys) enuk);
    ++enuk;
  }

  keys[BTN_REa].input(!(GPIO_BUTTON_ENTER & PIN_BUTTON_ENTER), BTN_REa);
}

uint32_t switchState(EnumKeys enuk)
{
  return 0;
}

#if !defined(SIMU)
void keysInit()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    /* GPIOC GPIOD clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOBUTTON, ENABLE);

    /* GPIO  Configuration*/
    GPIO_InitStructure.GPIO_Pin = PIN_BUTTON_UP | PIN_BUTTON_ENTER | PIN_BUTTON_DOWN | PIN_TRIM3_UP | PIN_TRIM3_DN
                                  | PIN_TRIM4_UP |PIN_TRIM4_DN | PIN_SW_A_L | PIN_SW_D_L | PIN_SW_F_H | PIN_SW_G_H
                                    | PIN_SW_G_L | PIN_SW_H_L;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = PIN_HOME | PIN_ESC | PIN_PAGE;
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
