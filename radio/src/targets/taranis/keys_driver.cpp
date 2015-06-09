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
  register uint32_t result = 0;
  register uint32_t d = ~GPIOD->IDR;

#if !defined(REV9E) || defined(SIMU)
  register uint32_t e = ~GPIOE->IDR;
  (void)e;
#endif

#if defined(REV9E)
  if (!(GPIOF->IDR & KEYS_GPIO_PIN_ENTER))
#else
  if (e & KEYS_GPIO_PIN_ENTER)
#endif
    result |= 0x02 << KEY_ENTER;

#if !defined(REV9E) || defined(SIMU)
  if (e & KEYS_GPIO_PIN_PLUS)
    result |= 0x02 << KEY_PLUS;
  if (e & KEYS_GPIO_PIN_MINUS)
    result |= 0x02 << KEY_MINUS;
#endif

  if (d & KEYS_GPIO_PIN_MENU)
    result |= 0x02 << KEY_MENU;
  if (d & KEYS_GPIO_PIN_PAGE)
    result |= 0x02 << KEY_PAGE;
  if (d & KEYS_GPIO_PIN_EXIT)
    result |= 0x02 << KEY_EXIT;

  // TRACE("readKeys(): %x %x => %x", d, e, result);

  return result;
}

uint32_t readTrims()
{
  register uint32_t c = GPIOC->IDR;
  register uint32_t e = GPIOE->IDR;
#if defined(REV9E)
  register uint32_t g = GPIOG->IDR;
#endif
  register uint32_t result = 0;

  if (~e & TRIMS_GPIO_PIN_LHL)
    result |= 0x01;         // LH_L
  if (~e & TRIMS_GPIO_PIN_LHR)
    result |= 0x02;         // LH_R

#if defined(REV9E)
  if (~g & TRIMS_GPIO_PIN_LVD)
    result |= 0x04;         // LV_DN
  if (~g & TRIMS_GPIO_PIN_LVU)
    result |= 0x08;         // LV_UP
#else
  if (~e & TRIMS_GPIO_PIN_LVD)
    result |= 0x04;         // LV_DN
  if (~e & TRIMS_GPIO_PIN_LVU)
    result |= 0x08;         // LV_UP
#endif

  if (~c & TRIMS_GPIO_PIN_RVD)
    result |= 0x10;         // RV_DN
  if (~c & TRIMS_GPIO_PIN_RVU)
    result |= 0x20;         // RV_UP
  if (~c & TRIMS_GPIO_PIN_RHL)
    result |= 0x40;         // RH_L
  if (~c & TRIMS_GPIO_PIN_RHR)
    result |= 0x80;         // RH_R

  // TRACE("readTrims(): %x %x => %x", c, e, result);

  return result;
}

uint8_t trimDown(uint8_t idx)
{
  return readTrims() & (1 << idx);
}

uint8_t keyDown()
{
  return readKeys();
}

#if defined(REV9E)
extern rotenc_t x9de_rotenc; 
#endif

/* TODO common to ARM */
void readKeysAndTrims()
{
  register uint32_t i;

  uint8_t enuk = KEY_MENU;
  uint32_t in = readKeys();
  for (i = 1; i <= TRM_BASE; i++) {
    keys[enuk].input(in & (1 << i));
    ++enuk;
  }

#if defined(REV9E) && !defined(SIMU)
  static rotenc_t rePreviousValue;
  rotenc_t reNewValue = (x9de_rotenc / 2);
  int8_t scrollRE = reNewValue - rePreviousValue;
  if (scrollRE) {
    rePreviousValue = reNewValue;
    if (scrollRE < 0) {
      putEvent(EVT_KEY_FIRST(KEY_MINUS));
    }
    else {
      putEvent(EVT_KEY_FIRST(KEY_PLUS)); 
    }
  }
#endif

  in = readTrims();

  for (i = 1; i < 256; i <<= 1) {
    keys[enuk].input(in & i);
    ++enuk;
  }
}

#if !defined(BOOT)

#if defined(REV9E)
#define ADD_3POS_CASE(x, i) \
    case SW_S ## x ## 0: \
      xxx = (SWITCHES_GPIO_REG_ ## x ## _H & SWITCHES_GPIO_PIN_ ## x ## _H) && (~SWITCHES_GPIO_REG_ ## x ## _L & SWITCHES_GPIO_PIN_ ## x ## _L); \
      break; \
    case SW_S ## x ## 1: \
      xxx = (SWITCHES_GPIO_REG_ ## x ## _H & SWITCHES_GPIO_PIN_ ## x ## _H) && (SWITCHES_GPIO_REG_ ## x ## _L & SWITCHES_GPIO_PIN_ ## x ## _L); \
      break; \
    case SW_S ## x ## 2: \
      xxx = (~SWITCHES_GPIO_REG_ ## x ## _H & SWITCHES_GPIO_PIN_ ## x ## _H) && (SWITCHES_GPIO_REG_ ## x ## _L & SWITCHES_GPIO_PIN_ ## x ## _L); \
      break
#define ADD_3POS_INVERTED_CASE(x, i) ADD_3POS_CASE(x, i)
#else
#define ADD_2POS_CASE(x) \
    case SW_S ## x ## 0: \
      xxx = SWITCHES_GPIO_REG_ ## x  & SWITCHES_GPIO_PIN_ ## x ; \
      break; \
    case SW_S ## x ## 2: \
      xxx = ~SWITCHES_GPIO_REG_ ## x  & SWITCHES_GPIO_PIN_ ## x ; \
      break
#define ADD_3POS_CASE(x, i) \
    case SW_S ## x ## 0: \
      xxx = (SWITCHES_GPIO_REG_ ## x ## _H & SWITCHES_GPIO_PIN_ ## x ## _H); \
      if (IS_3POS(i)) { \
        xxx = xxx && (~SWITCHES_GPIO_REG_ ## x ## _L & SWITCHES_GPIO_PIN_ ## x ## _L); \
      } \
      break; \
    case SW_S ## x ## 1: \
      xxx = (SWITCHES_GPIO_REG_ ## x ## _H & SWITCHES_GPIO_PIN_ ## x ## _H) && (SWITCHES_GPIO_REG_ ## x ## _L & SWITCHES_GPIO_PIN_ ## x ## _L); \
      break; \
    case SW_S ## x ## 2: \
      xxx = (~SWITCHES_GPIO_REG_ ## x ## _H & SWITCHES_GPIO_PIN_ ## x ## _H); \
      if (IS_3POS(i)) { \
        xxx = xxx && (SWITCHES_GPIO_REG_ ## x ## _L & SWITCHES_GPIO_PIN_ ## x ## _L); \
      } \
      break
#define ADD_3POS_INVERTED_CASE(x, i) \
    case SW_S ## x ## 0: \
      xxx = (~SWITCHES_GPIO_REG_ ## x ## _H & SWITCHES_GPIO_PIN_ ## x ## _H); \
      if (IS_3POS(i)) { \
        xxx = xxx && (SWITCHES_GPIO_REG_ ## x ## _L & SWITCHES_GPIO_PIN_ ## x ## _L); \
      } \
      break; \
    case SW_S ## x ## 1: \
      xxx = (SWITCHES_GPIO_REG_ ## x ## _H & SWITCHES_GPIO_PIN_ ## x ## _H) && (SWITCHES_GPIO_REG_ ## x ## _L & SWITCHES_GPIO_PIN_ ## x ## _L); \
      break; \
    case SW_S ## x ## 2: \
      xxx = (SWITCHES_GPIO_REG_ ## x ## _H & SWITCHES_GPIO_PIN_ ## x ## _H); \
      if (IS_3POS(i)) { \
        xxx = xxx && (~SWITCHES_GPIO_REG_ ## x ## _L & SWITCHES_GPIO_PIN_ ## x ## _L); \
      } \
      break
#endif

bool switchState(EnumKeys enuk)
{
  register uint32_t xxx = 0;

  if (enuk < (int) DIM(keys)) return keys[enuk].state() ? 1 : 0;

  switch ((uint8_t) enuk) {
    ADD_3POS_CASE(A, 0);
    ADD_3POS_CASE(B, 1);
    ADD_3POS_CASE(C, 2);
    ADD_3POS_CASE(D, 3);
    ADD_3POS_INVERTED_CASE(E, 4);
#if defined(REV9E)
    ADD_3POS_CASE(F, 5);
#else
    ADD_2POS_CASE(F);
#endif
    ADD_3POS_CASE(G, 6);
#if defined(REV9E)
    ADD_3POS_CASE(H, 7);
#else
    ADD_2POS_CASE(H);
#endif

#if defined(REV9E)
    ADD_3POS_CASE(I, 8);
    ADD_3POS_CASE(J, 9);
    ADD_3POS_CASE(K, 10);
    ADD_3POS_CASE(L, 11);
    ADD_3POS_CASE(M, 12);
    ADD_3POS_CASE(N, 13);
    ADD_3POS_CASE(O, 14);
    ADD_3POS_CASE(P, 15);
    ADD_3POS_CASE(Q, 16);
    ADD_3POS_CASE(R, 17);
#endif

    default:
      break;
  }

  // TRACE("switch %d => %d", enuk, xxx);
  return xxx;
}
#endif

#if !defined(SIMU)
void keysInit()
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(KEYS_RCC_AHB1Periph_GPIO, ENABLE);

#if defined(REV9E)
    GPIO_InitStructure.GPIO_Pin = TRIMS_GPIO_PIN_LHR | TRIMS_GPIO_PIN_LHL
                                  | SWITCHES_GPIO_PIN_F_H | SWITCHES_GPIO_PIN_A_L | SWITCHES_GPIO_PIN_B_H | SWITCHES_GPIO_PIN_B_L | SWITCHES_GPIO_PIN_C_H | SWITCHES_GPIO_PIN_D_H | SWITCHES_GPIO_PIN_D_L | SWITCHES_GPIO_PIN_G_H | SWITCHES_GPIO_PIN_G_L | SWITCHES_GPIO_PIN_L_L | SWITCHES_GPIO_PIN_Q_H | SWITCHES_GPIO_PIN_Q_L;
#elif defined(REVPLUS)
    GPIO_InitStructure.GPIO_Pin = KEYS_GPIO_PIN_PLUS | KEYS_GPIO_PIN_ENTER | KEYS_GPIO_PIN_MINUS | TRIMS_GPIO_PIN_LHR | TRIMS_GPIO_PIN_LHL
                                  | TRIMS_GPIO_PIN_LVD | TRIMS_GPIO_PIN_LVU
                                  | SWITCHES_GPIO_PIN_F | SWITCHES_GPIO_PIN_A_L | SWITCHES_GPIO_PIN_B_H | SWITCHES_GPIO_PIN_B_L | SWITCHES_GPIO_PIN_C_H | SWITCHES_GPIO_PIN_D_H | SWITCHES_GPIO_PIN_D_L | SWITCHES_GPIO_PIN_G_H | SWITCHES_GPIO_PIN_G_L;
#else
    GPIO_InitStructure.GPIO_Pin = KEYS_GPIO_PIN_PLUS | KEYS_GPIO_PIN_ENTER | KEYS_GPIO_PIN_MINUS | TRIMS_GPIO_PIN_LHR | TRIMS_GPIO_PIN_LHL
                                  | TRIMS_GPIO_PIN_LVD | TRIMS_GPIO_PIN_LVU
                                  | SWITCHES_GPIO_PIN_F | SWITCHES_GPIO_PIN_A_L | SWITCHES_GPIO_PIN_B_H | SWITCHES_GPIO_PIN_B_L | SWITCHES_GPIO_PIN_C_H | SWITCHES_GPIO_PIN_D_H | SWITCHES_GPIO_PIN_G_H | SWITCHES_GPIO_PIN_G_L | SWITCHES_GPIO_PIN_H;
#endif
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

#if defined(REV9E)
    GPIO_InitStructure.GPIO_Pin = KEYS_GPIO_PIN_ENTER | SWITCHES_GPIO_PIN_F_L | SWITCHES_GPIO_PIN_I_H | SWITCHES_GPIO_PIN_I_L | SWITCHES_GPIO_PIN_J_H | SWITCHES_GPIO_PIN_J_L | SWITCHES_GPIO_PIN_K_H | SWITCHES_GPIO_PIN_K_L | SWITCHES_GPIO_PIN_L_H | SWITCHES_GPIO_PIN_M_H | SWITCHES_GPIO_PIN_M_L | SWITCHES_GPIO_PIN_N_H | SWITCHES_GPIO_PIN_N_L;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = TRIMS_GPIO_PIN_LVD | TRIMS_GPIO_PIN_LVU | SWITCHES_GPIO_PIN_O_H | SWITCHES_GPIO_PIN_O_L | SWITCHES_GPIO_PIN_P_H | SWITCHES_GPIO_PIN_P_L | SWITCHES_GPIO_PIN_R_H | SWITCHES_GPIO_PIN_R_L;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
#endif

#if defined(REV9E)
    GPIO_InitStructure.GPIO_Pin = KEYS_GPIO_PIN_MENU | KEYS_GPIO_PIN_EXIT | KEYS_GPIO_PIN_PAGE | SWITCHES_GPIO_PIN_H_H | SWITCHES_GPIO_PIN_H_L;
#elif defined(REVPLUS)
    GPIO_InitStructure.GPIO_Pin = KEYS_GPIO_PIN_MENU | KEYS_GPIO_PIN_EXIT | KEYS_GPIO_PIN_PAGE | SWITCHES_GPIO_PIN_H;
#else
    GPIO_InitStructure.GPIO_Pin = KEYS_GPIO_PIN_MENU | KEYS_GPIO_PIN_EXIT | KEYS_GPIO_PIN_PAGE;
#endif
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = TRIMS_GPIO_PIN_RVD | TRIMS_GPIO_PIN_RVU | TRIMS_GPIO_PIN_RHL | TRIMS_GPIO_PIN_RHR;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

#if defined(REVPLUS)
    GPIO_InitStructure.GPIO_Pin =  SWITCHES_GPIO_PIN_E_L | SWITCHES_GPIO_PIN_E_H | SWITCHES_GPIO_PIN_A_H;
#else
    GPIO_InitStructure.GPIO_Pin =  SWITCHES_GPIO_PIN_E_L | SWITCHES_GPIO_PIN_E_H | SWITCHES_GPIO_PIN_A_H | SWITCHES_GPIO_PIN_D_L;
#endif
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SWITCHES_GPIO_PIN_C_L;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}
#endif
