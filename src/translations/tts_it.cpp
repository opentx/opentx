more/*
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

enum ItalianPrompts {
  PROMPT_NUMBERS_BASE = 0,
  PROMPT_ZERO = PROMPT_NUMBERS_BASE+0,
  PROMPT_CENT = PROMPT_NUMBERS_BASE+100,
  PROMPT_MILA = PROMPT_NUMBERS_BASE+101,
  PROMPT_MILLE = PROMPT_NUMBERS_BASE+102,
  PROMPT_UN = 103,
  PROMPT_E = 104,
  PROMPT_MENO = 105,
  PROMPT_ORA = 106,
  PROMPT_ORE = 107,
  PROMPT_MINUTO = 108,
  PROMPT_MINUTI = 109,
  PROMPT_SECONDO = 110,
  PROMPT_SECONDII = 111,

  PROMPT_UNITS_BASE = 112,
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

  PROMPT_LABELS_BASE = 126,
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

#if defined(SOMO) || defined(PCBARM)

void playNumber(int16_t number, uint8_t unit)
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
    pushPrompt(PROMPT_MENO);
    number = -number;
  }

  if (number >= 1000) {
    if (number >= 2000) {
      playNumber(number / 1000);
      pushPrompt(PROMPT_MILA);
    } else {
      pushPrompt(PROMPT_MILLE);
    }
    number %= 1000;
    if (number == 0)
      number = -1;
  }
  if (number >= 100) {
    if (number >= 200)
      pushPrompt(PROMPT_ZERO + number/100);
    pushPrompt(PROMPT_CENT);
    number %= 100;
    if (number == 0)
      number = -1;
  }
  pushPrompt(PROMPT_ZERO+number);

  if (unit) {
    pushPrompt(PROMPT_UNITS_BASE+unit-1);
  }
}

void playDuration(int16_t seconds)
{
  if (seconds < 0) {
    pushPrompt(PROMPT_MENO);
    seconds = -seconds;
  }

  uint8_t ore;
  uint8_t tmp = seconds / 3600;
  seconds %= 3600;
  if (tmp > 0) {
    ore=tmp;
    if (tmp > 1) {
      playNumber(tmp, 0);
      pushPrompt(PROMPT_ORE);
    } else {
      pushPrompt(PROMPT_UN);
      pushPrompt(PROMPT_ORA);
    }
  }

  tmp = seconds / 60;
  seconds %= 60;
  if (tmp > 0 || ore >0) {
    if (tmp != 1) {
      playNumber(tmp, 0);
      pushPrompt(PROMPT_MINUTI);
    } else {
      pushPrompt(PROMPT_UN);
      pushPrompt(PROMPT_MINUTO);
    }
    pushPrompt(PROMPT_E);
  }

  if (seconds != 1) {
    playNumber(seconds, 0);
    pushPrompt(PROMPT_SECONDI);
  } else {
    pushPrompt(PROMPT_UN);
    pushPrompt(PROMPT_SECONDO);
  }
}

#endif
