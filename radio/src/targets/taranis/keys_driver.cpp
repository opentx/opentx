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
  register uint32_t f = GPIOF->IDR;
  register uint32_t result = 0;

  (void)e;
  (void)f;

#if defined(REV9E)
  if (f & PIN_BUTTON_ENTER)
#else
  if (e & PIN_BUTTON_ENTER)
#endif
    result |= 0x02 << KEY_ENTER;

#if !defined(REV9E) || defined(SIMU)
  if (e & PIN_BUTTON_PLUS)
    result |= 0x02 << KEY_PLUS;
  if (e & PIN_BUTTON_MINUS)
    result |= 0x02 << KEY_MINUS;
#else
  result |= 0x02 << KEY_PLUS;
  result |= 0x02 << KEY_MINUS;
#endif

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
#if defined(REV9E)
  register uint32_t g = GPIOG->IDR;
#endif
  register uint32_t result = 0;

  if (~e & PIN_TRIM_LH_L)
    result |= 0x01;         // LH_L
  if (~e & PIN_TRIM_LH_R)
    result |= 0x02;         // LH_R

#if defined(REV9E)
  if (~g & PIN_TRIM_LV_DN)
    result |= 0x04;         // LV_DN
  if (~g & PIN_TRIM_LV_UP)
    result |= 0x08;         // LV_UP
#else
  if (~e & PIN_TRIM_LV_DN)
    result |= 0x04;         // LV_DN
  if (~e & PIN_TRIM_LV_UP)
    result |= 0x08;         // LV_UP
#endif

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

#if defined(REV9E)
extern rotenc_t x9de_rotenc; 
#endif

/* TODO common to ARM */
void readKeysAndTrims()
{
  register uint32_t i;

  uint8_t enuk = KEY_MENU;
  uint32_t in = ~readKeys();
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
      xxx = (GPIO_PIN_SW_ ## x ## _H & PIN_SW_ ## x ## _H) && (~GPIO_PIN_SW_ ## x ## _L & PIN_SW_ ## x ## _L); \
      break; \
    case SW_S ## x ## 1: \
      xxx = (GPIO_PIN_SW_ ## x ## _H & PIN_SW_ ## x ## _H) && (GPIO_PIN_SW_ ## x ## _L & PIN_SW_ ## x ## _L); \
      break; \
    case SW_S ## x ## 2: \
      xxx = (~GPIO_PIN_SW_ ## x ## _H & PIN_SW_ ## x ## _H) && (GPIO_PIN_SW_ ## x ## _L & PIN_SW_ ## x ## _L); \
      break
#define ADD_3POS_INVERTED_CASE(x, i) ADD_3POS_CASE(x, i)
#define ADD_XTRA_2POS_CASE(...)
#else
#define ADD_2POS_CASE(x) \
    case SW_S ## x ## 0: \
      xxx = GPIO_PIN_SW_ ## x  & PIN_SW_ ## x ; \
      break; \
    case SW_S ## x ## 2: \
      xxx = ~GPIO_PIN_SW_ ## x  & PIN_SW_ ## x ; \
      break
#define ADD_3POS_CASE(x, i) \
    case SW_S ## x ## 0: \
      xxx = (GPIO_PIN_SW_ ## x ## _H & PIN_SW_ ## x ## _H); \
      if (IS_3POS(i)) { \
        xxx = xxx && (~GPIO_PIN_SW_ ## x ## _L & PIN_SW_ ## x ## _L); \
      } \
      break; \
    case SW_S ## x ## 1: \
      xxx = (GPIO_PIN_SW_ ## x ## _H & PIN_SW_ ## x ## _H) && (GPIO_PIN_SW_ ## x ## _L & PIN_SW_ ## x ## _L); \
      break; \
    case SW_S ## x ## 2: \
      xxx = (~GPIO_PIN_SW_ ## x ## _H & PIN_SW_ ## x ## _H); \
      if (IS_3POS(i)) { \
        xxx = xxx && (GPIO_PIN_SW_ ## x ## _L & PIN_SW_ ## x ## _L); \
      } \
      break
#define ADD_3POS_INVERTED_CASE(x, i) \
    case SW_S ## x ## 0: \
      xxx = (~GPIO_PIN_SW_ ## x ## _H & PIN_SW_ ## x ## _H); \
      if (IS_3POS(i)) { \
        xxx = xxx && (GPIO_PIN_SW_ ## x ## _L & PIN_SW_ ## x ## _L); \
      } \
      break; \
    case SW_S ## x ## 1: \
      xxx = (GPIO_PIN_SW_ ## x ## _H & PIN_SW_ ## x ## _H) && (GPIO_PIN_SW_ ## x ## _L & PIN_SW_ ## x ## _L); \
      break; \
    case SW_S ## x ## 2: \
      xxx = (GPIO_PIN_SW_ ## x ## _H & PIN_SW_ ## x ## _H); \
      if (IS_3POS(i)) { \
        xxx = xxx && (~GPIO_PIN_SW_ ## x ## _L & PIN_SW_ ## x ## _L); \
      } \
      break
#define ADD_XTRA_2POS_CASE(x, y) \
    case SW_S ## y ## 0: \
      xxx = GPIO_PIN_SW_ ## x ## _L & PIN_SW_ ## x ## _L; \
      break; \
    case SW_S ## y ## 2: \
      xxx = ~GPIO_PIN_SW_ ## x ## _L & PIN_SW_ ## x ## _L; \
      break
#endif

#if defined(REV3)
#define ADD_3POS_INVERTED_REV3_CASE(x, i) \
    case SW_S ## x ## 0: \
      xxx = (GPIO_PIN_SW_ ## x ## _H & PIN_SW_ ## x ## _H); \
      if (IS_3POS(i)) { \
        xxx = xxx && (~GPIO_PIN_SW_ ## x ## _L & PIN_SW_ ## x ## _L); \
      } \
      break; \
    case SW_S ## x ## 1: \
      xxx = (~GPIO_PIN_SW_ ## x ## _H & PIN_SW_ ## x ## _H) && (~GPIO_PIN_SW_ ## x ## _L & PIN_SW_ ## x ## _L); \
      break; \
    case SW_S ## x ## 2: \
      xxx = (~GPIO_PIN_SW_ ## x ## _H & PIN_SW_ ## x ## _H); \
      if (IS_3POS(i)) { \
        xxx = xxx && (GPIO_PIN_SW_ ## x ## _L & PIN_SW_ ## x ## _L); \
      } \
      break
#else
#define ADD_3POS_INVERTED_REV3_CASE(x, i) ADD_3POS_CASE(x, i)
#endif

bool switchState(EnumKeys enuk)
{
  register uint32_t xxx = 0;

  if (enuk < (int) DIM(keys)) return keys[enuk].state() ? 1 : 0;

  switch ((uint8_t) enuk) {
    ADD_3POS_INVERTED_REV3_CASE(A, 0);
    ADD_XTRA_2POS_CASE(A, I);

    ADD_3POS_CASE(B, 1);
    ADD_XTRA_2POS_CASE(B, J);

    ADD_3POS_INVERTED_REV3_CASE(C, 2);
    ADD_XTRA_2POS_CASE(C, K);

    ADD_3POS_INVERTED_REV3_CASE(D, 3);
    ADD_XTRA_2POS_CASE(D, L);

    ADD_3POS_INVERTED_CASE(E, 4);
    ADD_XTRA_2POS_CASE(E, M);

#if defined(REV9E)
    ADD_3POS_CASE(F, 5);
#else
    ADD_2POS_CASE(F);
#endif

    ADD_3POS_INVERTED_REV3_CASE(G, 6);
    ADD_XTRA_2POS_CASE(G, N);

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
#elif defined(REV9E)
    GPIO_InitStructure.GPIO_Pin = PIN_TRIM_LH_R | PIN_TRIM_LH_L
                                  | PIN_SW_F_H | PIN_SW_A_L | PIN_SW_B_H | PIN_SW_B_L | PIN_SW_C_H | PIN_SW_D_H | PIN_SW_D_L | PIN_SW_G_H | PIN_SW_G_L | PIN_SW_L_L | PIN_SW_Q_H | PIN_SW_Q_L;
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

#if defined(REV9E)
    GPIO_InitStructure.GPIO_Pin = PIN_BUTTON_ENTER | PIN_SW_F_L | PIN_SW_I_H | PIN_SW_I_L | PIN_SW_J_H | PIN_SW_J_L | PIN_SW_K_H | PIN_SW_K_L | PIN_SW_L_H | PIN_SW_M_H | PIN_SW_M_L | PIN_SW_N_H | PIN_SW_N_L;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = PIN_TRIM_LV_DN | PIN_TRIM_LV_UP | PIN_SW_O_H | PIN_SW_O_L | PIN_SW_P_H | PIN_SW_P_L | PIN_SW_R_H | PIN_SW_R_L;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
#endif

#if defined(REV9E)
    GPIO_InitStructure.GPIO_Pin = PIN_BUTTON_MENU | PIN_BUTTON_EXIT | PIN_BUTTON_PAGE | PIN_SW_H_H | PIN_SW_H_L;
#elif defined(REVPLUS)
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
