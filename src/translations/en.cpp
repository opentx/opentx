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
  PROMPT_ZERO = PROMPT_NUMBERS_BASE+0,
  /* ... */
  PROMPT_TWENTY = PROMPT_NUMBERS_BASE+20,
  PROMPT_THIRTY = PROMPT_NUMBERS_BASE+21,
  /* ... */
  PROMPT_NINETY = PROMPT_NUMBERS_BASE+27,
  PROMPT_HUNDRED = PROMPT_NUMBERS_BASE+28,
  PROMPT_THOUSAND = PROMPT_NUMBERS_BASE+29,

  PROMPT_HOUR = 40,
  PROMPT_HOURS = 41,
  PROMPT_MINUTE = 42,
  PROMPT_MINUTES = 43,
  PROMPT_SECOND = 44,
  PROMPT_SECONDS = 45,

  PROMPT_AND = 47,
  PROMPT_MINUS = 48,

  PROMPT_UNITS_BASE = 50,
  PROMPT_VOLTS = PROMPT_UNITS_BASE+UNIT_VOLTS,
  PROMPT_AMPS = PROMPT_UNITS_BASE+UNIT_AMPS,
  PROMPT_METERS_PER_SECOND = PROMPT_UNITS_BASE+UNIT_METERS_PER_SECOND,
  PROMPT_SPARE1 = PROMPT_UNITS_BASE+UNIT_RAW,
  PROMPT_KMH = PROMPT_UNITS_BASE+UNIT_KMH,
  PROMPT_METERS = PROMPT_UNITS_BASE+UNIT_METERS,
  PROMPT_DEGREES = PROMPT_UNITS_BASE+UNIT_DEGREES,
  PROMPT_PERCENT = PROMPT_UNITS_BASE+UNIT_PERCENT,
  PROMPT_MILLIAMPS = PROMPT_UNITS_BASE+UNIT_MILLIAMPS,
};

#if defined(SOMO)

void playNumber(int16_t number, uint8_t unit, uint8_t att)
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
  if (number >= 1000) {
    playNumber(number / 1000);
    pushPrompt(PROMPT_THOUSAND);
    number %= 1000;
    if (number == 0)
      return;
  }
  if (number >= 100) {
    pushPrompt(PROMPT_ZERO + number/100);
    pushPrompt(PROMPT_HUNDRED);
    number %= 100;
    if (number == 0)
      return;
  }
  if (number >= 20) {
    pushPrompt(PROMPT_TWENTY + (number-20)/10);
    number %= 10;
    if (number == 0)
      return;
  }
  pushPrompt(PROMPT_ZERO+number);

  if (unit) {
    pushPrompt(PROMPT_UNITS_BASE+unit-1);
  }
}

void playDuration(int16_t seconds)
{
  if (seconds < 0) {
    pushPrompt(PROMPT_MINUS);
    seconds = -seconds;
  }

  uint8_t tmp = seconds / 3600;
  seconds %= 3600;
  if (tmp > 0) {
    playNumber(tmp);
    pushPrompt(tmp == 1 ? PROMPT_HOUR : PROMPT_HOURS);
  }

  tmp = seconds / 60;
  seconds %= 60;
  if (tmp > 0) {
    playNumber(tmp);
    pushPrompt(tmp == 1 ? PROMPT_MINUTE : PROMPT_MINUTES);
    if (seconds > 0)
      pushPrompt(PROMPT_AND);
  }

  if (seconds > 0) {
    playNumber(seconds);
    pushPrompt(tmp == 1 ? PROMPT_SECOND : PROMPT_SECONDS);
  }
}

#endif
