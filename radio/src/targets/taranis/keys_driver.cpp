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

#include "../../opentx.h"

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

  if (~e & PIN_TRIM_LH_L)
    result |= 0x01;         // LH_L
  if (~e & PIN_TRIM_LH_R)
    result |= 0x02;         // LH_R
  if (~e & PIN_TRIM_LV_DN)
    result |= 0x04;         // LV_DN
  if (~e & PIN_TRIM_LV_UP)
    result |= 0x08;         // LV_UP

  if (~c & PIN_TRIM_RV_DN)
    result |= 0x10;         // RV_DN
  if (~c & PIN_TRIM_RV_UP)
    result |= 0x20;         // RV_UP
  if (~c & PIN_TRIM_RH_L)
    result |= 0x40;         // RH_L
  if (~c & PIN_TRIM_RH_R)
    result |= 0x80;         // RH_R

  // printf("readTrims(): %x %x => %x\n", c, e, result); fflush(stdout);

  return result;
}

uint8_t trimDown(uint8_t idx)
{
  return readTrims() & (1 << idx);
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

  for (i = 1; i < 256; i <<= 1) {
    keys[enuk].input(in & i, (EnumKeys)enuk);
    ++enuk;
  }
}

#if !defined(BOOT)
bool switchState(EnumKeys enuk)
{
  register uint32_t xxx = 0;

  if (enuk < (int) DIM(keys)) return keys[enuk].state() ? 1 : 0;

  switch ((uint8_t) enuk) {
    case SW_SA0:
      xxx = (GPIO_PIN_SW_A_H & PIN_SW_A_H);
      if (IS_3POS(0)) {
        xxx = xxx && (~GPIO_PIN_SW_A_L & PIN_SW_A_L);
      }
      break;
    case SW_SA1:
#if defined(REV3)
      xxx = (~GPIO_PIN_SW_A_H & PIN_SW_A_H) && (~GPIO_PIN_SW_A_L & PIN_SW_A_L);
#else
      xxx = (GPIO_PIN_SW_A_H & PIN_SW_A_H) && (GPIO_PIN_SW_A_L & PIN_SW_A_L);
#endif
      break;
    case SW_SA2:
      xxx = (~GPIO_PIN_SW_A_H & PIN_SW_A_H);
      if (IS_3POS(0)) {
        xxx = xxx && (GPIO_PIN_SW_A_L & PIN_SW_A_L);
      }
      break;

    case SW_SI0:
      xxx = GPIO_PIN_SW_A_L & PIN_SW_A_L;
      break;
    case SW_SI2:
      xxx = ~GPIO_PIN_SW_A_L & PIN_SW_A_L;
      break;

    case SW_SB0:
      xxx = (GPIO_PIN_SW_B_H & PIN_SW_B_H);
      if (IS_3POS(1)) {
        xxx = xxx && (~GPIO_PIN_SW_B_L & PIN_SW_B_L);
      }
      break;
    case SW_SB1:
      xxx = (GPIO_PIN_SW_B_H & PIN_SW_B_H) && (GPIO_PIN_SW_B_L & PIN_SW_B_L);
      break;
    case SW_SB2:
      xxx = (~GPIO_PIN_SW_B_H & PIN_SW_B_H);
      if (IS_3POS(1)) {
        xxx = xxx && (GPIO_PIN_SW_B_L & PIN_SW_B_L);
      }
      break;

    case SW_SJ0:
      xxx = GPIO_PIN_SW_B_L & PIN_SW_B_L;
      break;
    case SW_SJ2:
      xxx = ~GPIO_PIN_SW_B_L & PIN_SW_B_L;
      break;

    case SW_SC0:
      xxx = (GPIO_PIN_SW_C_H & PIN_SW_C_H);
      if (IS_3POS(2)) {
        xxx = xxx && (~GPIO_PIN_SW_C_L & PIN_SW_C_L);
      }
      break;
    case SW_SC1:
#if defined(REV3)
      xxx = (~GPIO_PIN_SW_C_H & PIN_SW_C_H) && (~GPIO_PIN_SW_C_L & PIN_SW_C_L);
#else
      xxx = (GPIO_PIN_SW_C_H & PIN_SW_C_H) && (GPIO_PIN_SW_C_L & PIN_SW_C_L);
#endif
      break;
    case SW_SC2:
      xxx = (~GPIO_PIN_SW_C_H & PIN_SW_C_H);
      if (IS_3POS(2)) {
        xxx = xxx && (GPIO_PIN_SW_C_L & PIN_SW_C_L);
      }
      break;

    case SW_SK0:
      xxx = GPIO_PIN_SW_C_L & PIN_SW_C_L;
      break;
    case SW_SK2:
      xxx = ~GPIO_PIN_SW_C_L & PIN_SW_C_L;
      break;

    case SW_SD0:
      xxx = (GPIO_PIN_SW_D_H & PIN_SW_D_H);
      if (IS_3POS(3)) {
        xxx = xxx && (~GPIO_PIN_SW_D_L & PIN_SW_D_L);
      }
      break;
    case SW_SD1:
#if defined(REV3)
      xxx = (~GPIO_PIN_SW_D_H & PIN_SW_D_H) && (~GPIO_PIN_SW_D_L & PIN_SW_D_L);
#else
      xxx = (GPIO_PIN_SW_D_H & PIN_SW_D_H) && (GPIO_PIN_SW_D_L & PIN_SW_D_L);
#endif
      break;
    case SW_SD2:
      xxx = (~GPIO_PIN_SW_D_H & PIN_SW_D_H);
      if (IS_3POS(3)) {
        xxx = xxx && (GPIO_PIN_SW_D_L & PIN_SW_D_L);
      }
      break;

    case SW_SL0:
      xxx = GPIO_PIN_SW_D_L & PIN_SW_D_L;
      break;
    case SW_SL2:
      xxx = ~GPIO_PIN_SW_D_L & PIN_SW_D_L;
      break;

    case SW_SE0:
      xxx = (~GPIO_PIN_SW_E_H & PIN_SW_E_H);
      if (IS_3POS(4)) {
        xxx = xxx && (GPIO_PIN_SW_E_L & PIN_SW_E_L);
      }
      break;
    case SW_SE1:
      xxx = (GPIO_PIN_SW_E_H & PIN_SW_E_H) && (GPIO_PIN_SW_E_L & PIN_SW_E_L);
      break;
    case SW_SE2:
      xxx = (GPIO_PIN_SW_E_H & PIN_SW_E_H);
      if (IS_3POS(4)) {
        xxx = xxx && (~GPIO_PIN_SW_E_L & PIN_SW_E_L);
      }
      break;

    case SW_SM0:
      xxx = GPIO_PIN_SW_E_L & PIN_SW_E_L;
      break;
    case SW_SM2:
      xxx = ~GPIO_PIN_SW_E_L & PIN_SW_E_L;
      break;

    case SW_SF0:
      xxx = GPIO_PIN_SW_F & PIN_SW_F;
      break;
    case SW_SF2:
      xxx = ~GPIO_PIN_SW_F & PIN_SW_F;
      break;

    case SW_SG0:
      xxx = (GPIO_PIN_SW_G_H & PIN_SW_G_H);
      if (IS_3POS(6)) {
        xxx = xxx && (~GPIO_PIN_SW_G_L & PIN_SW_G_L);
      }
      break;
    case SW_SG1:
#if defined(REV3)
      xxx = (~GPIO_PIN_SW_G_H & PIN_SW_G_H) && (~GPIO_PIN_SW_G_L & PIN_SW_G_L);
#else
      xxx = (GPIO_PIN_SW_G_H & PIN_SW_G_H) && (GPIO_PIN_SW_G_L & PIN_SW_G_L);
#endif
      break;
    case SW_SG2:
      xxx = (~GPIO_PIN_SW_G_H & PIN_SW_G_H);
      if (IS_3POS(6)) {
        xxx = xxx && (GPIO_PIN_SW_G_L & PIN_SW_G_L);
      }
      break;

    case SW_SN0:
      xxx = GPIO_PIN_SW_G_L & PIN_SW_G_L;
      break;
    case SW_SN2:
      xxx = ~GPIO_PIN_SW_G_L & PIN_SW_G_L;
      break;

    case SW_SH0:
      xxx = GPIO_PIN_SW_H & PIN_SW_H;
      break;
    case SW_SH2:
      xxx = ~GPIO_PIN_SW_H & PIN_SW_H;
      break;

    default:
      break;
  }

  return xxx;
}
#endif

#if !defined(SIMU)
void keysInit()
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOBUTTON, ENABLE);

#if defined(REV3)
    GPIO_InitStructure.GPIO_Pin = PIN_BUTTON_PLUS | PIN_BUTTON_ENTER | PIN_BUTTON_MINUS | PIN_TRIM_LH_R | PIN_TRIM_LH_L
                                  | PIN_TRIM_LV_DN | PIN_TRIM_LV_UP
                                  | PIN_SW_A_L | PIN_SW_D_L | PIN_SW_F | PIN_SW_C_L | PIN_SW_D_H | PIN_SW_H;
#elif defined(REVPLUS)
    GPIO_InitStructure.GPIO_Pin = PIN_BUTTON_PLUS | PIN_BUTTON_ENTER | PIN_BUTTON_MINUS | PIN_TRIM_LH_R | PIN_TRIM_LH_L
                                  | PIN_TRIM_LV_DN | PIN_TRIM_LV_UP
                                  | PIN_SW_F | PIN_SW_A_L | PIN_SW_B_H | PIN_SW_B_L | PIN_SW_C_H | PIN_SW_D_H | PIN_SW_D_L | PIN_SW_G_H | PIN_SW_G_L;
#else
    GPIO_InitStructure.GPIO_Pin = PIN_BUTTON_PLUS | PIN_BUTTON_ENTER | PIN_BUTTON_MINUS | PIN_TRIM_LH_R | PIN_TRIM_LH_L
                                  | PIN_TRIM_LV_DN | PIN_TRIM_LV_UP
                                  | PIN_SW_F | PIN_SW_A_L | PIN_SW_B_H | PIN_SW_B_L | PIN_SW_C_H | PIN_SW_D_H | PIN_SW_G_H | PIN_SW_G_L | PIN_SW_H;
#endif
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

#if defined(REVPLUS)
    GPIO_InitStructure.GPIO_Pin = PIN_BUTTON_MENU | PIN_BUTTON_EXIT | PIN_BUTTON_PAGE | PIN_SW_H;
#else
    GPIO_InitStructure.GPIO_Pin = PIN_BUTTON_MENU | PIN_BUTTON_EXIT | PIN_BUTTON_PAGE;
#endif
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = PIN_TRIM_RV_DN | PIN_TRIM_RV_UP | PIN_TRIM_RH_L | PIN_TRIM_RH_R;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

#if defined(REV3)
    GPIO_InitStructure.GPIO_Pin = PIN_SW_B_H | PIN_SW_B_L | PIN_SW_C_H | PIN_SW_E_L | PIN_SW_E_H | PIN_SW_A_H | PIN_SW_G_L;
#elif defined(REVPLUS)
    GPIO_InitStructure.GPIO_Pin =  PIN_SW_E_L | PIN_SW_E_H | PIN_SW_A_H;
#else
    GPIO_InitStructure.GPIO_Pin =  PIN_SW_E_L | PIN_SW_E_H | PIN_SW_A_H | PIN_SW_D_L;
#endif
    GPIO_Init(GPIOB, &GPIO_InitStructure);

#if defined(REV3)
    GPIO_InitStructure.GPIO_Pin = PIN_SW_G_H;
#else
    GPIO_InitStructure.GPIO_Pin = PIN_SW_C_L;
#endif
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}
#endif
