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

enum EnglishPrompts {
  PROMPT_NUMBERS_BASE = 0,
  PROMPT_ZERO = PROMPT_NUMBERS_BASE+0,       //02-99
  PROMPT_HUNDRED = PROMPT_NUMBERS_BASE+100,  //100,200 .. 900
  PROMPT_THOUSAND = PROMPT_NUMBERS_BASE+109, //1000
  PROMPT_AND = PROMPT_NUMBERS_BASE+110,
  PROMPT_MINUS = PROMPT_NUMBERS_BASE+111,
  PROMPT_POINT = PROMPT_NUMBERS_BASE+112,

  PROMPT_UNITS_BASE = 115,
  PROMPT_VOLTS = PROMPT_UNITS_BASE+UNIT_VOLTS, //(one)volt,(two)volts
  PROMPT_AMPS = PROMPT_UNITS_BASE+(UNIT_AMPS*2),
  PROMPT_METERS_PER_SECOND = PROMPT_UNITS_BASE+(UNIT_METERS_PER_SECOND*2),
  PROMPT_SPARE1 = PROMPT_UNITS_BASE+(UNIT_RAW*2),
  PROMPT_KMH = PROMPT_UNITS_BASE+(UNIT_KMH*2),
  PROMPT_METERS = PROMPT_UNITS_BASE+(UNIT_METERS*2),
  PROMPT_DEGREES = PROMPT_UNITS_BASE+(UNIT_DEGREES*2),
  PROMPT_PERCENT = PROMPT_UNITS_BASE+(UNIT_PERCENT*2),
  PROMPT_MILLIAMPS = PROMPT_UNITS_BASE+(UNIT_MILLIAMPS*2),
  PROMPT_MAH = PROMPT_UNITS_BASE+(UNIT_MAH*2),
  PROMPT_WATTS = PROMPT_UNITS_BASE+(UNIT_WATTS*2),
  PROMPT_DB = PROMPT_UNITS_BASE+(UNIT_DBM*2),
  PROMPT_FEET = PROMPT_UNITS_BASE+(UNIT_FEET*2),
  PROMPT_KTS = PROMPT_UNITS_BASE+(UNIT_KTS*2),
  PROMPT_HOURS = PROMPT_UNITS_BASE+(UNIT_HOURS*2),
  PROMPT_MINUTES = PROMPT_UNITS_BASE+(UNIT_MINUTES*2),
  PROMPT_SECONDS = PROMPT_UNITS_BASE+(UNIT_SECONDS*2),
  PROMPT_RPMS = PROMPT_UNITS_BASE+(UNIT_RPMS*2),
  PROMPT_G = PROMPT_UNITS_BASE+(UNIT_G*2),
  
  PROMPT_POINT_BASE = 160, //.0 - .9

};

#if defined(VOICE)

#if defined(PCBSTD)
#define PUSH_UNIT_PROMPT(p, u) pushUnitPrompt((p), (u))
#else
#define PUSH_UNIT_PROMPT(p, u) pushUnitPrompt((p), (u), id)
#endif

PLAY_FUNCTION(pushUnitPrompt, int16_t number, uint8_t unitprompt)
{
  if (number == 1)
    PUSH_PROMPT(unitprompt);
  else
    PUSH_PROMPT(unitprompt+1);
}

PLAY_FUNCTION(playNumber, int16_t number, uint8_t unit, uint8_t att)
{
  if (number < 0) {
    PUSH_PROMPT(PROMPT_MINUS);
    number = -number;
  }

  int8_t mode = MODE(att);
  if (mode > 0) {
    // we assume that we are PREC1
    div_t qr = div(number, 10);
    if (qr.rem) {
      PLAY_NUMBER(qr.quot, 0, 0);
      PUSH_PROMPT(PROMPT_POINT_BASE + qr.rem);
      number = -1;
    }
    else {
      number = qr.quot;
    }
  }

  int16_t tmp = number;

  if (number >= 1000) {
    PLAY_NUMBER(number / 1000, 0, 0);
    PUSH_PROMPT(PROMPT_THOUSAND);
    number %= 1000;
    if (number == 0)
      number = -1;
  }
  if (number >= 100) {
    PUSH_PROMPT(PROMPT_HUNDRED + (number/100)-1);
    number %= 100;
    if (number == 0)
      number = -1;
  }
  if (number >= 0) {
    PUSH_PROMPT(PROMPT_ZERO + number);
  }
  
  if (unit) {
    PUSH_UNIT_PROMPT(tmp, (PROMPT_UNITS_BASE+((unit-1)*2)));
  }
}

PLAY_FUNCTION(playDuration, int16_t seconds)
{
  if (seconds < 0) {
    PUSH_PROMPT(PROMPT_MINUS);
    seconds = -seconds;
  }

  uint8_t tmp = seconds / 3600;
  seconds %= 3600;
  if (tmp > 0) {
    PLAY_NUMBER(tmp, UNIT_HOURS+1 , 0);
  }

  tmp = seconds / 60;
  seconds %= 60;
  if (tmp > 0) {
    PLAY_NUMBER(tmp, UNIT_MINUTES+1 , 0);
    if (seconds > 0)
      PUSH_PROMPT(PROMPT_AND);
  }

  if (seconds > 0) {
    PLAY_NUMBER(seconds, UNIT_SECONDS+1 , 0);
  }
}

#endif
