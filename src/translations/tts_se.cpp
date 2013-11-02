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
 * - Kjell Kernen
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

enum SwedishPrompts {
  SE_PROMPT_NUMBERS_BASE = 0,
  SE_PROMPT_ZERO = SE_PROMPT_NUMBERS_BASE+0,       //02-99
  SE_PROMPT_HUNDRED = SE_PROMPT_NUMBERS_BASE+100,  //100,200 .. 900
  SE_PROMPT_THOUSAND = SE_PROMPT_NUMBERS_BASE+109, //1000
  SE_PROMPT_AND = SE_PROMPT_NUMBERS_BASE+110,
  SE_PROMPT_MINUS = SE_PROMPT_NUMBERS_BASE+111,
  SE_PROMPT_POINT = SE_PROMPT_NUMBERS_BASE+112,

  SE_PROMPT_UNITS_BASE = 115,
  SE_PROMPT_VOLTS = SE_PROMPT_UNITS_BASE+UNIT_VOLTS, //(one)volt,(two)volts
  SE_PROMPT_AMPS = SE_PROMPT_UNITS_BASE+(UNIT_AMPS*2),
  SE_PROMPT_METERS_PER_SECOND = SE_PROMPT_UNITS_BASE+(UNIT_METERS_PER_SECOND*2),
  SE_PROMPT_SPARE1 = SE_PROMPT_UNITS_BASE+(UNIT_RAW*2),
  SE_PROMPT_KMH = SE_PROMPT_UNITS_BASE+(UNIT_KMH*2),
  SE_PROMPT_METERS = SE_PROMPT_UNITS_BASE+(UNIT_METERS*2),
  SE_PROMPT_DEGREES = SE_PROMPT_UNITS_BASE+(UNIT_DEGREES*2),
  SE_PROMPT_PERCENT = SE_PROMPT_UNITS_BASE+(UNIT_PERCENT*2),
  SE_PROMPT_MILLIAMPS = SE_PROMPT_UNITS_BASE+(UNIT_MILLIAMPS*2),
  SE_PROMPT_MAH = SE_PROMPT_UNITS_BASE+(UNIT_MAH*2),
  SE_PROMPT_WATTS = SE_PROMPT_UNITS_BASE+(UNIT_WATTS*2),
  SE_PROMPT_DB = SE_PROMPT_UNITS_BASE+(UNIT_DBM*2),
  SE_PROMPT_FEET = SE_PROMPT_UNITS_BASE+(UNIT_FEET*2),
  SE_PROMPT_KTS = SE_PROMPT_UNITS_BASE+(UNIT_KTS*2),
  SE_PROMPT_HOURS = SE_PROMPT_UNITS_BASE+(UNIT_HOURS*2),
  SE_PROMPT_MINUTES = SE_PROMPT_UNITS_BASE+(UNIT_MINUTES*2),
  SE_PROMPT_SECONDS = SE_PROMPT_UNITS_BASE+(UNIT_SECONDS*2),
  SE_PROMPT_RPMS = SE_PROMPT_UNITS_BASE+(UNIT_RPMS*2),
  SE_PROMPT_G = SE_PROMPT_UNITS_BASE+(UNIT_G*2),

  SE_PROMPT_POINT_BASE = 160, //.0 - .9

};

#if defined(VOICE)

I18N_PLAY_FUNCTION(se, playNumber, getvalue_t number, uint8_t unit, uint8_t att)
{
  if (number < 0) {
    PUSH_NUMBER_PROMPT(SE_PROMPT_MINUS);
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
#if defined(CPUARM)
    if ((att & PREC1) && (unit == UNIT_FEET || (unit == UNIT_METERS && number >= 100))) {
      number /= 10;
      att -= PREC1;
    }
#endif
    unit++;
  }

  int8_t mode = MODE(att);
  if (mode > 0) {
    // we assume that we are PREC1
    div_t qr = div(number, 10);
    if (qr.rem) {
      PLAY_NUMBER(qr.quot, 0, 0);
      PUSH_NUMBER_PROMPT(SE_PROMPT_POINT_BASE + qr.rem);
      number = -1;
    }
    else {
      number = qr.quot;
    }
  }
  
  int16_t tmpNumber = number;
  
  if (number >= 1000) {
    PLAY_NUMBER(number / 1000, 0, 0);
    PUSH_NUMBER_PROMPT(SE_PROMPT_THOUSAND);
    number %= 1000;
    if (number == 0)
      number = -1;
  }
  if (number >= 100) {
    PUSH_NUMBER_PROMPT(SE_PROMPT_HUNDRED + (number/100)-1);
    number %= 100;
    if (number == 0)
      number = -1;
  }
  if (number >= 0) {
    PUSH_NUMBER_PROMPT(SE_PROMPT_ZERO + number);
  }
  
  if (unit) {
    if  (tmpNumber == 1 ) 
      PUSH_NUMBER_PROMPT(SE_PROMPT_UNITS_BASE+((unit-1)*2));   // Singular prompts
    else
      PUSH_NUMBER_PROMPT(SE_PROMPT_UNITS_BASE+((unit-1)*2)+1); // Plural prompts
  }
}

I18N_PLAY_FUNCTION(se, playDuration, int16_t seconds)
{
  if (seconds < 0) {
    PUSH_NUMBER_PROMPT(SE_PROMPT_MINUS);
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
      PUSH_NUMBER_PROMPT(SE_PROMPT_AND);
  }

  if (seconds > 0) {
    PLAY_NUMBER(seconds, UNIT_SECONDS+1 , 0);
  }
}

LANGUAGE_PACK_DECLARE(se, "Swedish");

#endif
