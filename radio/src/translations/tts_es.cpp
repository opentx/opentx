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

enum SpanishPrompts {
  ES_PROMPT_NUMBERS_BASE = 0,
  ES_PROMPT_ZERO = ES_PROMPT_NUMBERS_BASE+0,
  ES_PROMPT_CIEN = ES_PROMPT_NUMBERS_BASE+100,
  ES_PROMPT_CIENTO = ES_PROMPT_NUMBERS_BASE+101,
  ES_PROMPT_DOSCIENTOS = ES_PROMPT_NUMBERS_BASE+102,
  ES_PROMPT_TRESCIENTOS,
  ES_PROMPT_CUATROCIENTOS,
  ES_PROMPT_QUINIENTOS,
  ES_PROMPT_SESCIENTOS,
  ES_PROMPT_SETECIENTOS,
  ES_PROMPT_OCHOCIENTOS,
  ES_PROMPT_NUEVECIENTOS,
  ES_PROMPT_MIL = ES_PROMPT_NUMBERS_BASE+110,
  ES_PROMPT_VIRGOLA = 111,
  ES_PROMPT_UN,
  ES_PROMPT_UNA,
  ES_PROMPT_Y,
  ES_PROMPT_MENO,
  ES_PROMPT_HORA,
  ES_PROMPT_HORAS,
  ES_PROMPT_MINUTO,
  ES_PROMPT_MINUTOS,
  ES_PROMPT_SEGUNDO,
  ES_PROMPT_SEGUNDOS,
};

#if defined(VOICE)

I18N_PLAY_FUNCTION(es, playNumber, getvalue_t number, uint8_t unit, uint8_t att)
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
    PUSH_NUMBER_PROMPT(ES_PROMPT_MENO);
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
      PUSH_NUMBER_PROMPT(ES_PROMPT_VIRGOLA);
      if (mode==2 && qr.rem < 10)
        PUSH_NUMBER_PROMPT(ES_PROMPT_ZERO);
      PLAY_NUMBER(qr.rem, unit, 0);
    }
    else {
      PLAY_NUMBER(qr.quot, unit, 0);
    }
    return;
  }

  if (number >= 1000) {
    if (number >= 2000) {
      PLAY_NUMBER(number / 1000, 0, 0);
      PUSH_NUMBER_PROMPT(ES_PROMPT_MIL);
    } else {
      PUSH_NUMBER_PROMPT(ES_PROMPT_MIL);
    }
    number %= 1000;
    if (number == 0)
      number = -1;
  }
  if (number > 100) {
    PUSH_NUMBER_PROMPT(ES_PROMPT_CIENTO + (number/100) - 1);
    number %= 100;
    if (number == 0)
      number = -1;
  }
  if (number == 100) {
    PUSH_NUMBER_PROMPT(ES_PROMPT_CIEN);
    number = -1;
  }
  if (number >= 0) {
    PUSH_NUMBER_PROMPT(ES_PROMPT_ZERO+number);
  }

  if (unit) {
    PUSH_UNIT_PROMPT((unit-1)*4);
  }
}

I18N_PLAY_FUNCTION(es, playDuration, int seconds PLAY_DURATION_ATT)
{
  if (seconds < 0) {
    PUSH_NUMBER_PROMPT(ES_PROMPT_MENO);
    seconds = -seconds;
  }

  uint8_t ore = 0;
  uint8_t tmp = seconds / 3600;
  seconds %= 3600;
  if (tmp > 0 || IS_PLAY_TIME()) {
    ore = tmp;
    PLAY_NUMBER(tmp, UNIT_HOURS, 0);
  }

  tmp = seconds / 60;
  seconds %= 60;
  if (tmp > 0 || ore >0) {
    PLAY_NUMBER(tmp, UNIT_MINUTES, 0);
    }
  PUSH_NUMBER_PROMPT(ES_PROMPT_Y);
  PLAY_NUMBER(seconds, UNIT_SECONDS, 0);
}

LANGUAGE_PACK_DECLARE(es, "Espanol");

#endif
