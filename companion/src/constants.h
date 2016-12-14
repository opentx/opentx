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

#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

enum BoardEnum {
  BOARD_STOCK,
  BOARD_M128,
  BOARD_MEGA2560,
  BOARD_GRUVIN9X,
  BOARD_SKY9X,
  BOARD_9XRPRO,
  BOARD_AR9X,
  BOARD_TARANIS_X7,
  BOARD_TARANIS_X9D,
  BOARD_TARANIS_X9DP,
  BOARD_TARANIS_X9E,
  BOARD_FLAMENCO,
  BOARD_HORUS
};

#define CPN_MAX_MODELS                 60
#define CPN_MAX_TIMERS                 3
#define CPN_MAX_FLIGHT_MODES           9
#define CPN_MAX_MIXERS                 64
#define CPN_MAX_INPUTS                 32
#define CPN_MAX_EXPOS                  64
#define CPN_MAX_CURVES                 32
#define CPN_MAX_POINTS                 17
#define CPN_MAX_GVARS                  9
#define CPN_MAX_ENCODERS               2
#define CPN_MAX_CHNOUT                 32 // number of real output channels
#define CPN_MAX_CSW                    64 // number of custom switches
#define CPN_MAX_CUSTOM_FUNCTIONS       64 // number of functions assigned to switches
#define CPN_MAX_MODULES                2
#define CPN_MAX_STICKS                 4
#define CPN_MAX_AUX_TRIMS              2
#define CPN_MAX_POTS                   8
#define CPN_MAX_CYC                    3
#define CPN_MAX_SWITCHES               32
#define CPN_MAX_KEYS                   32
#define CPN_MAX_MOUSE_ANALOGS          2

const char * const ARROW_LEFT = "\xE2\x86\x90";
const char * const ARROW_UP = "\xE2\x86\x91";
const char * const ARROW_RIGHT = "\xE2\x86\x92";
const char * const ARROW_DOWN = "\xE2\x86\x93";

#if defined(DEBUG)
#define HORUS_READY_FOR_RELEASE()     true
#else
#define HORUS_READY_FOR_RELEASE()     false
#endif

#endif // _CONSTANTS_H_
