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

enum CzechPrompts {
  PROMPT_NUMBERS_BASE = 0,
  PROMPT_ZERO = PROMPT_NUMBERS_BASE+0,
  /* ... */
  PROMPT_TWENTY = PROMPT_NUMBERS_BASE+20,
  PROMPT_THIRTY = PROMPT_NUMBERS_BASE+21,
  /* ... */
  PROMPT_NINETY = PROMPT_NUMBERS_BASE+27,
  PROMPT_JEDEN = PROMPT_NUMBERS_BASE+28,
  PROMPT_DVE = PROMPT_NUMBERS_BASE+29,
  PROMPT_STO = PROMPT_NUMBERS_BASE+30,
  PROMPT_STA = PROMPT_NUMBERS_BASE+31,
  PROMPT_SET = PROMPT_NUMBERS_BASE+32,
  PROMPT_TISIC = PROMPT_NUMBERS_BASE+33,
  PROMPT_TISICE = PROMPT_NUMBERS_BASE+34,
  PROMPT_CELA = PROMPT_NUMBERS_BASE+35,

  PROMPT_HODINA = 40,
  PROMPT_HODINY = 41,
  PROMPT_HODIN = 42,
  PROMPT_MINUTA = 43,
  PROMPT_MINUTY = 44,
  PROMPT_MINUT = 45,
  PROMPT_SEKUNDA = 46,
  PROMPT_SEKUNDY = 47,
  PROMPT_SEKUND = 48,

  PROMPT_MINUS = 49,

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
  PROMPT_MAH = PROMPT_UNITS_BASE+UNIT_MAH,
  PROMPT_WATTS = PROMPT_UNITS_BASE+UNIT_WATTS,
  PROMPT_FEET = PROMPT_UNITS_BASE+UNIT_FEET,
  PROMPT_KTS = PROMPT_UNITS_BASE+UNIT_KTS,
  
  PROMPT_LABELS_BASE = 70,
  PROMPT_TIMER1 = PROMPT_LABELS_BASE+TELEM_TM1,
  PROMPT_TIMER2 = PROMPT_LABELS_BASE+TELEM_TM2,
  PROMPT_A1 = PROMPT_LABELS_BASE+TELEM_A1,
  PROMPT_A2 = PROMPT_LABELS_BASE+TELEM_A2,
  PROMPT_RSSI_TX = PROMPT_LABELS_BASE+TELEM_RSSI_TX,
  PROMPT_RSSI_RX = PROMPT_LABELS_BASE+TELEM_RSSI_RX,
  PROMPT_ALTITUDE = PROMPT_LABELS_BASE+TELEM_ALT,
  PROMPT_RPM = PROMPT_LABELS_BASE+TELEM_RPM,
  PROMPT_ESSENCE = PROMPT_LABELS_BASE+TELEM_FUEL,
  PROMPT_T1 = PROMPT_LABELS_BASE+TELEM_T1,
  PROMPT_T2 = PROMPT_LABELS_BASE+TELEM_T2,
  PROMPT_VITESSE = PROMPT_LABELS_BASE+TELEM_SPEED,
  PROMPT_DISTANCE = PROMPT_LABELS_BASE+TELEM_DIST,
  PROMPT_GPSALTITUDE = PROMPT_LABELS_BASE+TELEM_GPSALT,
  PROMPT_ELEMENTS_LIPO = PROMPT_LABELS_BASE+TELEM_CELL,
  PROMPT_TOTAL_LIPO = PROMPT_LABELS_BASE+TELEM_CELLS_SUM,
  PROMPT_VFAS = PROMPT_LABELS_BASE+TELEM_VFAS,
  PROMPT_COURANT = PROMPT_LABELS_BASE+TELEM_CURRENT,
  PROMPT_CONSOMMATION = PROMPT_LABELS_BASE+TELEM_CONSUMPTION,
  PROMPT_PUISSANCE = PROMPT_LABELS_BASE+TELEM_POWER,
  PROMPT_ACCELx = PROMPT_LABELS_BASE+TELEM_ACCx,
  PROMPT_ACCELy = PROMPT_LABELS_BASE+TELEM_ACCy,
  PROMPT_ACCELz = PROMPT_LABELS_BASE+TELEM_ACCz,
  PROMPT_HDG = PROMPT_LABELS_BASE+TELEM_HDG,
  PROMPT_VARIO = PROMPT_LABELS_BASE+TELEM_VSPD,
  
  
};

#if defined(VOICE)


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

  int8_t mode = MODE(att);
  if (mode > 0) {
    div_t qr = div(number, (mode == 1 ? 10 : 100));
    playNumber(qr.quot);
    pushPrompt(PROMPT_CELA);
    playNumber(qr.rem, unit);
    return;
  }



  if (number < 0) {
    pushPrompt(PROMPT_MINUS);
    number = -number;
  }

  if (number >= 1000) {
    if (number >= 2000) 
      playNumber(number / 1000);
    if (number >= 2000 && number < 5000)
      pushPrompt(PROMPT_TISICE);
    else
      pushPrompt(PROMPT_TISIC);
    number %= 1000;
    if (number == 0)
      number = -1;
  }
  if (number >= 100) {
    if (number >= 200) {
      if (number < 300)
        pushPrompt(PROMPT_DVE);
      else
      pushPrompt(PROMPT_ZERO + number/100);
      }
      if (number >= 200 && number < 500)
        pushPrompt(PROMPT_STA);
      else if (number >= 500 && number < 1000)
      	pushPrompt(PROMPT_SET);
      else
        pushPrompt(PROMPT_STO);
    number %= 100;
    if (number == 0)
      number = -1;
  }
  if (number >= 20) {
    pushPrompt(PROMPT_TWENTY + (number-20)/10);
    number %= 10;
    if (number == 0)
      number = -1;
  }
  if (number >= 0) {
    pushPrompt(PROMPT_ZERO+number);
  }

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
    if (tmp == 1) {
      pushPrompt(PROMPT_ZERO+tmp);
      pushPrompt(PROMPT_HODINA);
    }
    else if (tmp >= 2 && tmp < 5) {
      if (tmp == 2)
        pushPrompt(PROMPT_DVE);
      else
        pushPrompt(PROMPT_ZERO+tmp);
      pushPrompt(PROMPT_HODINY);
    }
    else { 
      playNumber(tmp);
      pushPrompt(PROMPT_HODIN);
    }
  }

  tmp = seconds / 60;
  seconds %= 60;
  if (tmp > 0) {
    if (tmp == 1) {
      pushPrompt(PROMPT_ZERO+tmp);
      pushPrompt(PROMPT_MINUTA);
    }
    else if (tmp >= 2 && tmp < 5) {
      if (tmp == 2)
        pushPrompt(PROMPT_DVE);
      else
        pushPrompt(PROMPT_ZERO+tmp);
      pushPrompt(PROMPT_MINUTY);
    }
    else { 
      playNumber(tmp);
      pushPrompt(PROMPT_MINUT);
    }
  }

  if (seconds > 0) {
    if (seconds == 1) {
      pushPrompt(PROMPT_ZERO+seconds);
      pushPrompt(PROMPT_SEKUNDA);
    }
    else if (seconds >= 2 && seconds < 5) {
      if (seconds == 2)
        pushPrompt(PROMPT_DVE);
      else
        pushPrompt(PROMPT_ZERO+seconds);
      pushPrompt(PROMPT_SEKUNDY);
    }
    else { 
      playNumber(seconds);
      pushPrompt(PROMPT_SEKUND);
    }
  }
}

#endif
