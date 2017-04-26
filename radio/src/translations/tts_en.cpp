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

#include "opentx.h"

enum EnglishPrompts {
  EN_PROMPT_NUMBERS_BASE = 0,
  EN_PROMPT_ZERO = EN_PROMPT_NUMBERS_BASE+0,       //02-99
  EN_PROMPT_HUNDRED = EN_PROMPT_NUMBERS_BASE+100,  //100,200 .. 900
  EN_PROMPT_THOUSAND = EN_PROMPT_NUMBERS_BASE+109, //1000
  EN_PROMPT_AND = EN_PROMPT_NUMBERS_BASE+110,
  EN_PROMPT_MINUS = EN_PROMPT_NUMBERS_BASE+111,
  EN_PROMPT_POINT = EN_PROMPT_NUMBERS_BASE+112,
  EN_PROMPT_UNITS_BASE = 113,
  EN_PROMPT_POINT_BASE = 167, //.0 - .9
};

#if defined(VOICE)

#if defined(CPUARM)
  #define EN_PUSH_UNIT_PROMPT(u, p) en_pushUnitPrompt((u), (p), id)
#else
  #define EN_PUSH_UNIT_PROMPT(u, p) pushUnitPrompt((u), (p))
#endif

I18N_PLAY_FUNCTION(en, pushUnitPrompt, int16_t number, uint8_t unitprompt)
{
#if defined(CPUARM)
  if (number == 1)
    PUSH_UNIT_PROMPT(unitprompt, 0);
  else
    PUSH_UNIT_PROMPT(unitprompt, 1);
#else
  unitprompt = EN_PROMPT_UNITS_BASE + unitprompt*2;
  if (number == 1)
    PUSH_NUMBER_PROMPT(unitprompt);
  else
    PUSH_NUMBER_PROMPT(unitprompt+1);
#endif
}

I18N_PLAY_FUNCTION(en, playNumber, getvalue_t number, uint8_t unit, uint8_t att)
{
  if (number < 0) {
    PUSH_NUMBER_PROMPT(EN_PROMPT_MINUS);
    number = -number;
  }

#if !defined(CPUARM)
  if (unit) {
    unit--;
    convertUnit(number, unit);
    if (IS_IMPERIAL_ENABLE()) {
      if (unit == UNIT_DIST) {
        unit = UNIT_FEET;
      }
      if (unit == UNIT_SPEED) {
    	unit = UNIT_KTS;
      }
    }
    unit++;
  }
#endif

  int8_t mode = MODE(att);
  if (mode > 0) {
#if defined(CPUARM)
    if (mode == 2) {
      number /= 10;
    }
#else
    // we assume that we are PREC1
#endif
    div_t qr = div((int)number, 10);
    if (qr.rem) {
      PLAY_NUMBER(qr.quot, 0, 0);
      PUSH_NUMBER_PROMPT(EN_PROMPT_POINT_BASE + qr.rem);
      number = -1;
    }
    else {
      number = qr.quot;
    }
  }

  int16_t tmp = number;

  if (number >= 1000) {
    PLAY_NUMBER(number / 1000, 0, 0);
    PUSH_NUMBER_PROMPT(EN_PROMPT_THOUSAND);
    number %= 1000;
    if (number == 0)
      number = -1;
  }
  if (number >= 100) {
    PUSH_NUMBER_PROMPT(EN_PROMPT_HUNDRED + (number/100)-1);
    number %= 100;
    if (number == 0)
      number = -1;
  }
  if (number >= 0) {
    PUSH_NUMBER_PROMPT(EN_PROMPT_ZERO + number);
  }

  if (unit) {
    EN_PUSH_UNIT_PROMPT(tmp, unit);
  }
}

I18N_PLAY_FUNCTION(en, playDuration, int seconds PLAY_DURATION_ATT)
{
  if (seconds == 0) {
    PLAY_NUMBER(seconds, 0, 0);
    return;
  }

  if (seconds < 0) {
    PUSH_NUMBER_PROMPT(EN_PROMPT_MINUS);
    seconds = -seconds;
  }

  uint8_t tmp = seconds / 3600;
  seconds %= 3600;
  if (tmp > 0 || IS_PLAY_TIME()) {
    PLAY_NUMBER(tmp, UNIT_HOURS, 0);
  }

  tmp = seconds / 60;
  seconds %= 60;
  if (tmp > 0) {
    PLAY_NUMBER(tmp, UNIT_MINUTES, 0);
    if (seconds > 0)
      PUSH_NUMBER_PROMPT(EN_PROMPT_AND);
  }

  if (seconds > 0) {
    PLAY_NUMBER(seconds, UNIT_SECONDS, 0);
  }
}

LANGUAGE_PACK_DECLARE_DEFAULT(en, "English");

#endif
