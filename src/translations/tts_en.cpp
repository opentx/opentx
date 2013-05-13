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

#include "../opentx.h"

enum EnglishPrompts {
  EN_PROMPT_NUMBERS_BASE = 0,
  EN_PROMPT_ZERO = EN_PROMPT_NUMBERS_BASE+0,       //02-99
  EN_PROMPT_HUNDRED = EN_PROMPT_NUMBERS_BASE+100,  //100,200 .. 900
  EN_PROMPT_THOUSAND = EN_PROMPT_NUMBERS_BASE+109, //1000
  EN_PROMPT_AND = EN_PROMPT_NUMBERS_BASE+110,
  EN_PROMPT_MINUS = EN_PROMPT_NUMBERS_BASE+111,
  EN_PROMPT_POINT = EN_PROMPT_NUMBERS_BASE+112,

  EN_PROMPT_UNITS_BASE = 115,
  EN_PROMPT_VOLTS = EN_PROMPT_UNITS_BASE+UNIT_VOLTS, //(one)volt,(two)volts
  EN_PROMPT_AMPS = EN_PROMPT_UNITS_BASE+(UNIT_AMPS*2),
  EN_PROMPT_METERS_PER_SECOND = EN_PROMPT_UNITS_BASE+(UNIT_METERS_PER_SECOND*2),
  EN_PROMPT_SPARE1 = EN_PROMPT_UNITS_BASE+(UNIT_RAW*2),
  EN_PROMPT_KMH = EN_PROMPT_UNITS_BASE+(UNIT_KMH*2),
  EN_PROMPT_METERS = EN_PROMPT_UNITS_BASE+(UNIT_METERS*2),
  EN_PROMPT_DEGREES = EN_PROMPT_UNITS_BASE+(UNIT_DEGREES*2),
  EN_PROMPT_PERCENT = EN_PROMPT_UNITS_BASE+(UNIT_PERCENT*2),
  EN_PROMPT_MILLIAMPS = EN_PROMPT_UNITS_BASE+(UNIT_MILLIAMPS*2),
  EN_PROMPT_MAH = EN_PROMPT_UNITS_BASE+(UNIT_MAH*2),
  EN_PROMPT_WATTS = EN_PROMPT_UNITS_BASE+(UNIT_WATTS*2),
  EN_PROMPT_DB = EN_PROMPT_UNITS_BASE+(UNIT_DBM*2),
  EN_PROMPT_FEET = EN_PROMPT_UNITS_BASE+(UNIT_FEET*2),
  EN_PROMPT_KTS = EN_PROMPT_UNITS_BASE+(UNIT_KTS*2),
  EN_PROMPT_HOURS = EN_PROMPT_UNITS_BASE+(UNIT_HOURS*2),
  EN_PROMPT_MINUTES = EN_PROMPT_UNITS_BASE+(UNIT_MINUTES*2),
  EN_PROMPT_SECONDS = EN_PROMPT_UNITS_BASE+(UNIT_SECONDS*2),
  EN_PROMPT_RPMS = EN_PROMPT_UNITS_BASE+(UNIT_RPMS*2),
  EN_PROMPT_G = EN_PROMPT_UNITS_BASE+(UNIT_G*2),

  EN_PROMPT_POINT_BASE = 160, //.0 - .9

};

#if defined(VOICE)

#if defined(CPUARM)
  #define EN_PUSH_UNIT_PROMPT(p, u) en_pushUnitPrompt((p), (u), id)
#else
  #define EN_PUSH_UNIT_PROMPT(p, u) pushUnitPrompt((p), (u))
#endif

I18N_PLAY_FUNCTION(en, pushUnitPrompt, int16_t number, uint8_t unitprompt)
{
  if (number == 1)
    PUSH_NUMBER_PROMPT(unitprompt);
  else
    PUSH_NUMBER_PROMPT(unitprompt+1);
}

I18N_PLAY_FUNCTION(en, playNumber, getvalue_t number, uint8_t unit, uint8_t att)
{
  if (number < 0) {
    PUSH_NUMBER_PROMPT(EN_PROMPT_MINUS);
    number = -number;
  }

  if (unit) {
    unit--;
    convertUnit(number, unit);
    if (IS_IMPERIAL_ENABLE()) {
      if (unit == UNIT_METERS) {
        unit = UNIT_FEET;
      }
    }
    unit++;
  }

  int8_t mode = MODE(att);
  if (mode > 0) {
    // we assume that we are PREC1
    div_t qr = div(number, 10);
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
    EN_PUSH_UNIT_PROMPT(tmp, (EN_PROMPT_UNITS_BASE+((unit-1)*2)));
  }
}

I18N_PLAY_FUNCTION(en, playDuration, int16_t seconds)
{
  if (seconds < 0) {
    PUSH_NUMBER_PROMPT(EN_PROMPT_MINUS);
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
      PUSH_NUMBER_PROMPT(EN_PROMPT_AND);
  }

  if (seconds > 0) {
    PLAY_NUMBER(seconds, UNIT_SECONDS+1 , 0);
  }
}

LANGUAGE_PACK_DECLARE_DEFAULT(en, "english");

#endif
