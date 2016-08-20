/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
 * - Andreas Weitl <andreas.weitl@gmx.de>
 * - Bertrand Songis <bsongis@gmail.com>
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

enum GermanPrompts {
  DE_PROMPT_NUMBERS_BASE = 0,
  DE_PROMPT_NULL = DE_PROMPT_NUMBERS_BASE+0,
  DE_PROMPT_HUNDERT = DE_PROMPT_NUMBERS_BASE+100,
  DE_PROMPT_TAUSEND = DE_PROMPT_NUMBERS_BASE+101,
  DE_PROMPT_COMMA = 102,
  DE_PROMPT_UND,
  DE_PROMPT_MINUS,
  DE_PROMPT_UHR,
  DE_PROMPT_MINUTE,
  DE_PROMPT_MINUTEN,
  DE_PROMPT_SECUNDE,
  DE_PROMPT_SECUNDEN,
};

#if defined(VOICE)

/* The list bellow MUST be kept in sync with /radio/util/tts_de.py */
char const * deUnitsFilenames[] = {
  "volt",
  "amp",
  "mamp",
  "knot",
  "mps",
  "fps",
  "kph",
  "mph",
  "meter",
  "foot",
  "celsius",
  "fahr",
  "percent",
  "mamph",
  "watt",
  "mwatt",
  "db",
  "rpm",
  "g",
  "degree",
  "radian",
  "ml",
  "founce",
  "hour",
  "minute",
  "second",
};

I18N_PLAY_FUNCTION(de, playNumber, getvalue_t number, uint8_t unit, uint8_t att)
{
/*  if digit >= 1000000000:
      temp_digit, digit = divmod(digit, 1000000000)
      prompts.extend(self.getNumberPrompt(temp_digit))
      prompts.append(Prompt(GUIDE_00_BILLION, dir=2))
  if digit >= 1000000:
      temp_digit, digit = divmod(digit, 1000000)
      prompts.extend(self.getNumberPrompt(temp_digit))
      prompts.append(Prompt(GUIDE_00_MILLION, dir=2))
*/

  if (number < 0) {
    PUSH_NUMBER_PROMPT(DE_PROMPT_MINUS);
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
    div_t qr = div(number, 10);
    if (qr.rem > 0) {
      PLAY_NUMBER(qr.quot, 0, 0);
      PUSH_NUMBER_PROMPT(DE_PROMPT_COMMA);
      if (mode==2 && qr.rem < 10)
        PUSH_NUMBER_PROMPT(DE_PROMPT_NULL);
      PLAY_NUMBER(qr.rem, unit, 0);
    }
    else {
      PLAY_NUMBER(qr.quot, unit, 0);
    }
    return;
  }

  if (number >= 1000) {
    if (number >= 1100) {
      PLAY_NUMBER(number / 1000, 0, 0);
      PUSH_NUMBER_PROMPT(DE_PROMPT_TAUSEND);
    } else {
      PUSH_NUMBER_PROMPT(DE_PROMPT_TAUSEND);
    }
    number %= 1000;
    if (number == 0)
      number = -1;
  }
  if (number >= 100) {
    if (number >= 200)
      PUSH_NUMBER_PROMPT(DE_PROMPT_NULL + number/100);
    PUSH_NUMBER_PROMPT(DE_PROMPT_HUNDERT);
    number %= 100;
    if (number == 0)
      number = -1;
  }
  PUSH_NUMBER_PROMPT(DE_PROMPT_NULL+number);

  if (unit) {
    PUSH_UNIT_PROMPT((char *)deUnitsFilenames[unit-1]);
  }
}

I18N_PLAY_FUNCTION(de, playDuration, int seconds PLAY_DURATION_ATT)
{
  if (seconds < 0) {
    PUSH_NUMBER_PROMPT(DE_PROMPT_MINUS);
    seconds = -seconds;
  }

  uint8_t ore = 0;
  uint8_t tmp = seconds / 3600;
  seconds %= 3600;
  if (tmp > 0 || IS_PLAY_TIME()) {
    PLAY_NUMBER(tmp, 0, 0);
    PUSH_NUMBER_PROMPT(DE_PROMPT_UHR);
  }

  tmp = seconds / 60;
  seconds %= 60;
  if (tmp > 0 || ore >0) {
    PLAY_NUMBER(tmp, 0, 0);
    if (tmp != 1) {
      PUSH_NUMBER_PROMPT(DE_PROMPT_MINUTEN);
    } else {
      PUSH_NUMBER_PROMPT(DE_PROMPT_MINUTE);
    }
    PUSH_NUMBER_PROMPT(DE_PROMPT_UND);
  }
  PLAY_NUMBER(seconds, 0, 0);
  if (seconds != 1) {
    PUSH_NUMBER_PROMPT(DE_PROMPT_SECUNDEN);
  } else {
    PUSH_NUMBER_PROMPT(DE_PROMPT_SECUNDE);
  }
}

LANGUAGE_PACK_DECLARE(de, "Deutsch");

#endif
