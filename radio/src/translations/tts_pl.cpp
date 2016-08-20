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

enum PolishPrompts {
  PL_PROMPT_NUMBERS_BASE = 0,
  PL_PROMPT_ZERO = PL_PROMPT_NUMBERS_BASE+0,    //0-99
  PL_PROMPT_STO = PL_PROMPT_NUMBERS_BASE+100,   //100,200 .. 900
  PL_PROMPT_TYSIAC = PL_PROMPT_NUMBERS_BASE+109, //1000
  PL_PROMPT_TYSIACE = PL_PROMPT_NUMBERS_BASE+110,
  PL_PROMPT_TYSIECY = PL_PROMPT_NUMBERS_BASE+111,
  PL_PROMPT_JEDNA = PL_PROMPT_NUMBERS_BASE+112,
  PL_PROMPT_JEDNO = PL_PROMPT_NUMBERS_BASE+113,
  PL_PROMPT_DWIE = PL_PROMPT_NUMBERS_BASE+114,
  PL_PROMPT_CALA = PL_PROMPT_NUMBERS_BASE+115,
  PL_PROMPT_CALE = PL_PROMPT_NUMBERS_BASE+116,
  PL_PROMPT_CALYCH = PL_PROMPT_NUMBERS_BASE+117,
  PL_PROMPT_MINUS = PL_PROMPT_NUMBERS_BASE+118,
  PL_PROMPT_DZIESIATKI_ZENSKIE=PL_PROMPT_NUMBERS_BASE+120, // 22(0122.wav),32(123.wav),42,52,62,72,82,92(129.wav) - this are special female numbers when the unit is female
};

#if defined(VOICE)

#if defined(CPUARM)
  #define PL_PUSH_UNIT_PROMPT(p, u) pl_pushUnitPrompt((p), (u), id)
#else
  #define PL_PUSH_UNIT_PROMPT(p, u) pushUnitPrompt((p), (u))
#endif

#define MESKI 0x80
#define ZENSKI 0x81
#define NIJAKI 0x82

I18N_PLAY_FUNCTION(pl, pushUnitPrompt, int16_t number, uint8_t unitprompt)
{
  unitprompt *= 4;
  if (number == 1)
    PUSH_UNIT_PROMPT(unitprompt);
  else if (number > 1 && number < 5)
    PUSH_UNIT_PROMPT(unitprompt+1);
  else {
    int test_2 =0;
    test_2 =number % 10;
    int ten=0;
    ten=(number - (number % 10))/10;
    if ((test_2 > 1 && test_2 < 5) && ten >=2)
	PUSH_UNIT_PROMPT(unitprompt+1);
    else
	PUSH_UNIT_PROMPT(unitprompt+2);
    }
}

I18N_PLAY_FUNCTION(pl, playNumber, getvalue_t number, uint8_t unit, uint8_t att)
{

  if (number < 0) {
    PUSH_NUMBER_PROMPT(PL_PROMPT_MINUS);
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
    if (qr.rem) {
      PLAY_NUMBER(qr.quot, 0, ZENSKI);
      if (qr.quot == 0)
        PUSH_NUMBER_PROMPT(PL_PROMPT_CALA);
      else
        PL_PUSH_UNIT_PROMPT(qr.quot, PL_PROMPT_CALA);
      PLAY_NUMBER(qr.rem, 0, ZENSKI);
      PUSH_UNIT_PROMPT(((unit-1)*4)+3);
      return;
    }
    else {
      number = qr.quot;
    }
  }

  int16_t tmp = number;
  
  switch(unit) {
    case 0:
      break;
// ft/s
    case 6:
// mph
    case 8:
// feet
    case 10:
//mAh
    case 14:
//flOz
    case 21:
//Hours
    case 22:
//Minutes
    case 23:
//Seconds
    case 24:
      att = ZENSKI;
      break;
    case 100:
      att = NIJAKI;
      break;
    default:
      att = MESKI;
      break;
  }

  if ((number == 1) && (att == ZENSKI)) {
    PUSH_NUMBER_PROMPT(PL_PROMPT_JEDNA);
    number = -1;
  }
  
  if ((number == 1) && (att == NIJAKI)) {
    PUSH_NUMBER_PROMPT(PL_PROMPT_JEDNO);
    number = -1;
  }
  
  if ((number == 2) && (att == ZENSKI)) {
    PUSH_NUMBER_PROMPT(PL_PROMPT_DWIE);
    number = -1;
  }
  




  if (number >= 1000) {
    if (number >= 2000) 
      PLAY_NUMBER(number / 1000, 0, 0);

    if (number >= 2000 && number < 5000)
      PUSH_NUMBER_PROMPT(PL_PROMPT_TYSIACE);
    else if (number >= 5000)
      PUSH_NUMBER_PROMPT(PL_PROMPT_TYSIECY);
    else 
      PUSH_NUMBER_PROMPT(PL_PROMPT_TYSIAC);

    number %= 1000;
    if (number == 0)
      number = -1;
  }
  if (number >= 100) {
    PUSH_NUMBER_PROMPT(PL_PROMPT_STO + (number/100)-1);
    number %= 100;
    if (number == 0)
      number = -1;
  }
  
  if (number >= 0) {
    int test_2 =0;
    test_2 =number % 10;
    int ten=0;
    ten=(number - (number % 10))/10;
    if (att == ZENSKI && test_2==2 && ten >= 2 ) {
      
      PUSH_NUMBER_PROMPT(PL_PROMPT_DZIESIATKI_ZENSKIE+ten);
    }else 
       PUSH_NUMBER_PROMPT(PL_PROMPT_ZERO+number);
  }

  if (unit) {
    PL_PUSH_UNIT_PROMPT(tmp, (unit-1)*4);
  }
}


// The whole funtion has to be changed
I18N_PLAY_FUNCTION(pl, playDuration, int seconds PLAY_DURATION_ATT)
{
  if (seconds < 0) {
    PUSH_NUMBER_PROMPT(PL_PROMPT_MINUS);
    seconds = -seconds;
  }

  uint8_t tmp = seconds / 3600;
  seconds %= 3600;
  if (tmp > 0 || IS_PLAY_TIME()) {
    PLAY_NUMBER(tmp, UNIT_HOURS, ZENSKI);
  }

  tmp = seconds / 60;
  seconds %= 60;
  if (tmp > 0) {
    PLAY_NUMBER(tmp, UNIT_MINUTES, ZENSKI);
  }

  if (seconds > 0) {
    PLAY_NUMBER(seconds, UNIT_SECONDS, ZENSKI);
  }
}

LANGUAGE_PACK_DECLARE(pl, "Polish");

#endif
