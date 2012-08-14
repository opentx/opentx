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

enum FrenchPrompts {
  PROMPT_NUMBERS_BASE = 0,
  PROMPT_ZERO = PROMPT_NUMBERS_BASE+0,
  PROMPT_CENT = PROMPT_NUMBERS_BASE+100,
  PROMPT_DEUX_CENT = PROMPT_CENT+1,
  PROMPT_MILLE = PROMPT_CENT+9,

  PROMPT_UNE = 110,
  PROMPT_ONZE,
  PROMPT_VINGT_ET_UNE,
  PROMPT_TRENTE_ET_UNE,
  PROMPT_QUARANTE_ET_UNE,
  PROMPT_CINQUANTE_ET_UNE,
  PROMPT_SOIXANTE_ET_UNE,
  PROMPT_SOIXANTE_ET_ONZE,
  PROMPT_QUATRE_VINGT_UNE,

  PROMPT_VIRGULE = 119,
  PROMPT_ET = 120,
  PROMPT_MOINS = 121,

  PROMPT_UNITS_BASE = 125,
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
  PROMPT_HEURE = PROMPT_UNITS_BASE+UNIT_HOURS,
  PROMPT_MINUTE = PROMPT_UNITS_BASE+UNIT_MINUTES,
  PROMPT_SECONDE = PROMPT_UNITS_BASE+UNIT_SECONDS,

  PROMPT_LABELS_BASE = 145,
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

#define FEMININ 0x80

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
    if (qr.rem > 0) {
      playNumber(qr.quot);
      pushPrompt(PROMPT_VIRGULE);
      if (mode==2 && qr.rem < 10)
        pushPrompt(PROMPT_ZERO);
      playNumber(qr.rem, unit);
    }
    else {
      playNumber(qr.quot, unit);
    }
    return;
  }

  if (number < 0) {
    pushPrompt(PROMPT_MOINS);
    number = -number;
  }

  if (number >= 1000) {
    if (number >= 2000)
      playNumber(number / 1000);
    pushPrompt(PROMPT_MILLE);
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
  if (((number % 10) == 1) && number < 90 && (att & FEMININ)) {
    pushPrompt(PROMPT_UNE+(number/10));
  }
  else if (number >= 0) {
    pushPrompt(PROMPT_ZERO+number);
  }

  if (unit) {
    pushPrompt(PROMPT_UNITS_BASE+unit-1);
  }
}

void playDuration(int16_t seconds)
{
  if (seconds < 0) {
    pushPrompt(PROMPT_MOINS);
    seconds = -seconds;
  }

  uint8_t tmp = seconds / 3600;
  seconds %= 3600;
  if (tmp > 0) {
    playNumber(tmp, 0, FEMININ);
    pushPrompt(PROMPT_HEURE);
  }

  tmp = seconds / 60;
  seconds %= 60;
  if (tmp > 0) {
    playNumber(tmp, 0, FEMININ);
    pushPrompt(PROMPT_MINUTE);
    if (seconds > 0)
      pushPrompt(PROMPT_ET);
  }

  if (seconds > 0) {
    playNumber(seconds, 0, FEMININ);
    pushPrompt(PROMPT_SECONDE);
  }
}

#endif
