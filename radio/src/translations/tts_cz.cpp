/*
 * Authors (alphabetical order)
 * - Bertrand Songis <bsongis@gmail.com>
 * - Martin Hotar <mhotar@gmail.com>
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

enum CzechPrompts {
  CZ_PROMPT_NUMBERS_BASE = 0,
  CZ_PROMPT_NULA = CZ_PROMPT_NUMBERS_BASE+0,    //0-99
  CZ_PROMPT_STO = CZ_PROMPT_NUMBERS_BASE+100,   //100,200 .. 900
  CZ_PROMPT_TISIC = CZ_PROMPT_NUMBERS_BASE+109, //1000
  CZ_PROMPT_TISICE = CZ_PROMPT_NUMBERS_BASE+110,
  CZ_PROMPT_JEDEN = CZ_PROMPT_NUMBERS_BASE+111,
  CZ_PROMPT_JEDNO = CZ_PROMPT_NUMBERS_BASE+112,
  CZ_PROMPT_DVE = CZ_PROMPT_NUMBERS_BASE+113,
  CZ_PROMPT_CELA = CZ_PROMPT_NUMBERS_BASE+114,
  CZ_PROMPT_CELE = CZ_PROMPT_NUMBERS_BASE+115,
  CZ_PROMPT_CELYCH = CZ_PROMPT_NUMBERS_BASE+116,
  CZ_PROMPT_MINUS = CZ_PROMPT_NUMBERS_BASE+117,

  CZ_PROMPT_UNITS_BASE = 118, //(jeden)volt,(dva)volty,(pet)voltu,(desetina)voltu

};

#if defined(VOICE)

#if defined(CPUARM)
  #define CZ_PUSH_UNIT_PROMPT(p, u) cz_pushUnitPrompt((p), (u), id)
#else
  #define CZ_PUSH_UNIT_PROMPT(p, u) pushUnitPrompt((p), (u))
#endif

#define MUZSKY 0x80
#define ZENSKY 0x81
#define STREDNI 0x82

I18N_PLAY_FUNCTION(cz, pushUnitPrompt, int16_t number, uint8_t unitprompt)
{
#if defined(CPUARM)
  if (number == 1)
    PUSH_UNIT_PROMPT(unitprompt, 0);
  else if (number > 1 && number < 5)
    PUSH_UNIT_PROMPT(unitprompt, 1);
  else
    PUSH_UNIT_PROMPT(unitprompt, 2);
#else
  unitprompt = CZ_PROMPT_UNITS_BASE+((unitprompt-1)*4);
  if (number == 1)
    PUSH_NUMBER_PROMPT(unitprompt);
  else if (number > 1 && number < 5)
    PUSH_NUMBER_PROMPT(unitprompt+1);
  else
    PUSH_NUMBER_PROMPT(unitprompt+2);
#endif
}

I18N_PLAY_FUNCTION(cz, playNumber, getvalue_t number, uint8_t unit, uint8_t att)
{

  if (number < 0) {
    PUSH_NUMBER_PROMPT(CZ_PROMPT_MINUS);
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
        PLAY_NUMBER(qr.quot, 0, ZENSKY);
        if (qr.quot == 0)
          PUSH_NUMBER_PROMPT(CZ_PROMPT_CELA);
        else
          CZ_PUSH_UNIT_PROMPT(qr.quot, CZ_PROMPT_CELA);
        PLAY_NUMBER(qr.rem, 0, ZENSKY);
        PUSH_NUMBER_PROMPT(CZ_PROMPT_UNITS_BASE+((unit-1)*4)+3);
        return;
      }
      else
        number = qr.quot;
  }

  int16_t tmp = number;
#if defined(CPUARM) 
  switch(unit) {
    case 0:
      break;
    case 6:
    case 8:
    case 10:
    case 14:
    case 18:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
      att = ZENSKY;
      break;
    case 13:
    case 19:
      att = STREDNI;
      break;
    default:
      att = MUZSKY;
      break;
  }
#else
  switch(unit) {
    case 0:
      break;
    case 4:
    case 10:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
      att = ZENSKY;
      break;
    case 8:
    case 19:
      att = STREDNI;
      break;
    default:
      att = MUZSKY;
      break;
  }
#endif

  if ((number == 1) && (att == MUZSKY)) {
    PUSH_NUMBER_PROMPT(CZ_PROMPT_JEDEN);
    number = -1;
  }
  
  if ((number == 1) && (att == STREDNI)) {
    PUSH_NUMBER_PROMPT(CZ_PROMPT_JEDNO);
    number = -1;
  }
  
  if ((number == 2) && ((att == ZENSKY) || (att == STREDNI))) {
    PUSH_NUMBER_PROMPT(CZ_PROMPT_DVE);
    number = -1;
  }
  
  if (number >= 1000) {
    if (number >= 2000) 
      PLAY_NUMBER(number / 1000, 0, 0);
    if (number >= 2000 && number < 5000)
      PUSH_NUMBER_PROMPT(CZ_PROMPT_TISICE);
    else
      PUSH_NUMBER_PROMPT(CZ_PROMPT_TISIC);
    number %= 1000;
    if (number == 0)
      number = -1;
  }
  if (number >= 100) {
    PUSH_NUMBER_PROMPT(CZ_PROMPT_STO + (number/100)-1);
    number %= 100;
    if (number == 0)
      number = -1;
  }
  
  if (number >= 0) {
    PUSH_NUMBER_PROMPT(CZ_PROMPT_NULA+number);
  }

  if (unit) {
    CZ_PUSH_UNIT_PROMPT(tmp, unit);
  }
}

I18N_PLAY_FUNCTION(cz, playDuration, int seconds PLAY_DURATION_ATT)
{
  if (seconds < 0) {
    PUSH_NUMBER_PROMPT(CZ_PROMPT_MINUS);
    seconds = -seconds;
  }

  uint8_t tmp = seconds / 3600;
  seconds %= 3600;
  if (tmp > 0 || IS_PLAY_TIME()) {
    PLAY_NUMBER(tmp, UNIT_HOURS, ZENSKY);
  }

  tmp = seconds / 60;
  seconds %= 60;
  if (tmp > 0) {
    PLAY_NUMBER(tmp, UNIT_MINUTES, ZENSKY);
  }

  if (seconds > 0) {
    PLAY_NUMBER(seconds, UNIT_SECONDS, ZENSKY);
  }
}

LANGUAGE_PACK_DECLARE(cz, "Czech");

#endif
