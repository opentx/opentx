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
 * Fixing polish tts for OpenTX 2.3.2: Rafał Michalski
 */

#include "opentx.h"

enum PolishPrompts {
  PL_PROMPT_NUMBERS_BASE = 0,
  PL_PROMPT_ZERO = PL_PROMPT_NUMBERS_BASE+0,    		// 0-99
  PL_PROMPT_STO = PL_PROMPT_NUMBERS_BASE+100,   		// 100,200 .. 900
  PL_PROMPT_TYSIAC = PL_PROMPT_NUMBERS_BASE+109, 		// 1000
  PL_PROMPT_TYSIACE = PL_PROMPT_NUMBERS_BASE+110,		// TYSIĄCE - thousands variety
  PL_PROMPT_TYSIECY = PL_PROMPT_NUMBERS_BASE+111,		// TYSIĘCY - thousands variety
  PL_PROMPT_JEDNA = PL_PROMPT_NUMBERS_BASE+112,			// JEDNA - one variety
  //PL_PROMPT_JEDNO = PL_PROMPT_NUMBERS_BASE+113,		// JEDNO - one variety
  PL_PROMPT_DWIE = PL_PROMPT_NUMBERS_BASE+114,			// DWIE - two variety
  //PL_PROMPT_CALA = PL_PROMPT_NUMBERS_BASE+115,		// CAŁA - whole
  //PL_PROMPT_CALE = PL_PROMPT_NUMBERS_BASE+116,		// CAŁE - whole
  //PL_PROMPT_CALYCH = PL_PROMPT_NUMBERS_BASE+117,		// CAŁYCH
  PL_PROMPT_MINUS = PL_PROMPT_NUMBERS_BASE+118,			// MINUS

  PL_PROMPT_PRZECINEK = PL_PROMPT_NUMBERS_BASE+119,		// PRZECINEK - comma

  PL_PROMPT_DZIESIATKI_ZENSKIE = PL_PROMPT_NUMBERS_BASE+120, 	// 22(0122.wav),32(123.wav),42,52,62,72,82,92(129.wav) - this are special female numbers when the unit is female

  PL_PROMPT_POINT_BASE = 130, 					//,0 - ,9 (0130.wav - 0139.wav)
  PL_PROMPT_UNITS_BASE = 160,
};


  #define PL_PUSH_UNIT_PROMPT(u, p) pl_pushUnitPrompt((u), (p), id)

#define MESKI 0x80
#define ZENSKI 0x81
#define NIJAKI 0x82


I18N_PLAY_FUNCTION(pl, pushUnitPrompt, uint8_t unitprompt, int16_t number)
{

    if (number == 1)			        // If value is 1
    	PUSH_UNIT_PROMPT(unitprompt, 0);
    
    else if (number > 1 && number < 5)		// If we have number from 2 to 4
    	PUSH_UNIT_PROMPT(unitprompt, 1);
    
    else if (number < 0)			// if negative number, we have to use 2 units form (for example value = 1.3)
	PUSH_UNIT_PROMPT(unitprompt, 3);

    else {
	int mod10 = 0;
	mod10 = number % 10;
	int ten=0;
	ten = (number - (number % 10))/10;

    	if ((mod10 > 1 && mod10 < 5) && ten >=2)
	    PUSH_UNIT_PROMPT(unitprompt, 1);	//For polish female tens 22,23,24,32,33,34 ...
	
    	else
	    PUSH_UNIT_PROMPT(unitprompt, 2);	//reszta
    }


}

I18N_PLAY_FUNCTION(pl, playNumber, getvalue_t number, uint8_t unit, uint8_t att)
{

  if (number < 0) {
    PUSH_NUMBER_PROMPT(PL_PROMPT_MINUS);
    number = -number;
  }


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
    if (qr.rem) {						//if there is rest from dividing
      PLAY_NUMBER(qr.quot, 0, 0);
      PUSH_NUMBER_PROMPT(PL_PROMPT_PRZECINEK);			//PUSH_NUMBER_PROMPT(PL_PROMPT_POINT_BASE + qr.rem); //Tutaj potrzeba 9 dźwięków ,0 ,1 ,2 ...
      PUSH_NUMBER_PROMPT(PL_PROMPT_NUMBERS_BASE + qr.rem);
      number = -1;
    }
    else {
      number = qr.quot;
    }
  }


  int16_t tmp = number;

  switch(unit) {
    case UNIT_RAW:		// case 0
      break;
    case UNIT_FEET_PER_SECOND: 	// case 6
    case UNIT_MPH:		// case 8
    case UNIT_FEET:		// case 10
    case UNIT_MAH:		// case 14
    case UNIT_FLOZ:		// case 21
    case UNIT_HOURS:		// case 22
    case UNIT_MINUTES:		// case 23
    case UNIT_SECONDS:		// case 24
      att = ZENSKI; 		//JEDNA, DWIE
      break;
    default:
      att = MESKI;		//JEDEN
      break;
  }

  if ((number == 1) && (att == ZENSKI)) {
    PUSH_NUMBER_PROMPT(PL_PROMPT_JEDNA);
    number = -1;
  }

  if ((number == 2) && (att == ZENSKI)) {
    PUSH_NUMBER_PROMPT(PL_PROMPT_DWIE);
    number = -1;
  }

// ==== TYSIĄCE =======
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

// === For polish female tens (22,23,23,32,33,33 ...) ===
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
// =========

  if (unit) {
    PL_PUSH_UNIT_PROMPT(unit, tmp);
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
