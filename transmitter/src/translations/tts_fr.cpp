/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
 * - Bertrand Songis <bsongis@gmail.com>
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

enum FrenchPrompts {
  FR_PROMPT_NUMBERS_BASE = 0,
  FR_PROMPT_ZERO = FR_PROMPT_NUMBERS_BASE+0,
  FR_PROMPT_CENT = FR_PROMPT_NUMBERS_BASE+100,
  FR_PROMPT_DEUX_CENT = FR_PROMPT_CENT+1,
  FR_PROMPT_MILLE = FR_PROMPT_CENT+9,

  FR_PROMPT_UNE = 110,
  FR_PROMPT_ONZE,
  FR_PROMPT_VINGT_ET_UNE,
  FR_PROMPT_TRENTE_ET_UNE,
  FR_PROMPT_QUARANTE_ET_UNE,
  FR_PROMPT_CINQUANTE_ET_UNE,
  FR_PROMPT_SOIXANTE_ET_UNE,
  FR_PROMPT_SOIXANTE_ET_ONZE,
  FR_PROMPT_QUATRE_VINGT_UNE,

  FR_PROMPT_VIRGULE = 119,
  FR_PROMPT_ET = 120,
  FR_PROMPT_MOINS = 121,

  FR_PROMPT_UNITS_BASE = 125,
  FR_PROMPT_VOLTS = FR_PROMPT_UNITS_BASE+UNIT_VOLTS,
  FR_PROMPT_AMPS = FR_PROMPT_UNITS_BASE+UNIT_AMPS,
  FR_PROMPT_METERS_PER_SECOND = FR_PROMPT_UNITS_BASE+UNIT_METERS_PER_SECOND,
  FR_PROMPT_SPARE1 = FR_PROMPT_UNITS_BASE+UNIT_RAW,
  FR_PROMPT_KMH = FR_PROMPT_UNITS_BASE+UNIT_KMH,
  FR_PROMPT_METERS = FR_PROMPT_UNITS_BASE+UNIT_METERS,
  FR_PROMPT_DEGREES = FR_PROMPT_UNITS_BASE+UNIT_DEGREES,
  FR_PROMPT_PERCENT = FR_PROMPT_UNITS_BASE+UNIT_PERCENT,
  FR_PROMPT_MILLIAMPS = FR_PROMPT_UNITS_BASE+UNIT_MILLIAMPS,
  FR_PROMPT_MAH = FR_PROMPT_UNITS_BASE+UNIT_MAH,
  FR_PROMPT_WATTS = FR_PROMPT_UNITS_BASE+UNIT_WATTS,
  FR_PROMPT_FEET = FR_PROMPT_UNITS_BASE+UNIT_FEET,
  FR_PROMPT_KTS = FR_PROMPT_UNITS_BASE+UNIT_KTS,
  FR_PROMPT_HEURE = FR_PROMPT_UNITS_BASE+UNIT_HOURS,
  FR_PROMPT_MINUTE = FR_PROMPT_UNITS_BASE+UNIT_MINUTES,
  FR_PROMPT_SECONDE = FR_PROMPT_UNITS_BASE+UNIT_SECONDS,

  FR_PROMPT_LABELS_BASE = 145,
  FR_PROMPT_TIMER1 = FR_PROMPT_LABELS_BASE+TELEM_TM1,
  FR_PROMPT_TIMER2 = FR_PROMPT_LABELS_BASE+TELEM_TM2,
  FR_PROMPT_RSSI_TX = FR_PROMPT_LABELS_BASE+TELEM_RSSI_TX,
  FR_PROMPT_RSSI_RX = FR_PROMPT_LABELS_BASE+TELEM_RSSI_RX,
  FR_PROMPT_A1 = FR_PROMPT_LABELS_BASE+TELEM_A1,
  FR_PROMPT_A2 = FR_PROMPT_LABELS_BASE+TELEM_A2,
  FR_PROMPT_ALTITUDE = FR_PROMPT_LABELS_BASE+TELEM_ALT,
  FR_PROMPT_RPM = FR_PROMPT_LABELS_BASE+TELEM_RPM,
  FR_PROMPT_ESSENCE = FR_PROMPT_LABELS_BASE+TELEM_FUEL,
  FR_PROMPT_T1 = FR_PROMPT_LABELS_BASE+TELEM_T1,
  FR_PROMPT_T2 = FR_PROMPT_LABELS_BASE+TELEM_T2,
  FR_PROMPT_VITESSE = FR_PROMPT_LABELS_BASE+TELEM_SPEED,
  FR_PROMPT_DISTANCE = FR_PROMPT_LABELS_BASE+TELEM_DIST,
  FR_PROMPT_GPSALTITUDE = FR_PROMPT_LABELS_BASE+TELEM_GPSALT,
  FR_PROMPT_ELEMENTS_LIPO = FR_PROMPT_LABELS_BASE+TELEM_CELL,
  FR_PROMPT_TOTAL_LIPO = FR_PROMPT_LABELS_BASE+TELEM_CELLS_SUM,
  FR_PROMPT_VFAS = FR_PROMPT_LABELS_BASE+TELEM_VFAS,
  FR_PROMPT_COURANT = FR_PROMPT_LABELS_BASE+TELEM_CURRENT,
  FR_PROMPT_CONSOMMATION = FR_PROMPT_LABELS_BASE+TELEM_CONSUMPTION,
  FR_PROMPT_PUISSANCE = FR_PROMPT_LABELS_BASE+TELEM_POWER,
  FR_PROMPT_ACCELx = FR_PROMPT_LABELS_BASE+TELEM_ACCx,
  FR_PROMPT_ACCELy = FR_PROMPT_LABELS_BASE+TELEM_ACCy,
  FR_PROMPT_ACCELz = FR_PROMPT_LABELS_BASE+TELEM_ACCz,
  FR_PROMPT_HDG = FR_PROMPT_LABELS_BASE+TELEM_HDG,
  FR_PROMPT_VARIO = FR_PROMPT_LABELS_BASE+TELEM_VSPD,

  FR_PROMPT_VIRGULE_BASE = 180, //,0 - ,9
};

#if defined(VOICE)

#define FEMININ 0x80

I18N_PLAY_FUNCTION(fr, playNumber, getvalue_t number, uint8_t unit, uint8_t att)
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
    PUSH_NUMBER_PROMPT(FR_PROMPT_MOINS);
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
      PUSH_NUMBER_PROMPT(FR_PROMPT_VIRGULE_BASE + qr.rem);
      number = -1;
    }
    else {
      number = qr.quot;
    }
  }

  if (number >= 1000) {
    if (number >= 2000)
      PLAY_NUMBER(number / 1000, 0, 0);
    PUSH_NUMBER_PROMPT(FR_PROMPT_MILLE);
    number %= 1000;
    if (number == 0)
      number = -1;
  }
  if (number >= 100) {
    if (number >= 200)
      PUSH_NUMBER_PROMPT(FR_PROMPT_ZERO + number/100);
    PUSH_NUMBER_PROMPT(FR_PROMPT_CENT);
    number %= 100;
    if (number == 0)
      number = -1;
  }
  if (((number % 10) == 1) && number < 90 && (att & FEMININ)) {
    PUSH_NUMBER_PROMPT(FR_PROMPT_UNE+(number/10));
  }
  else if (number >= 0) {
    PUSH_NUMBER_PROMPT(FR_PROMPT_ZERO+number);
  }

  if (unit) {
    PUSH_NUMBER_PROMPT(FR_PROMPT_UNITS_BASE+unit-1);
  }
}

I18N_PLAY_FUNCTION(fr, playDuration, int16_t seconds)
{
  if (seconds < 0) {
    PUSH_NUMBER_PROMPT(FR_PROMPT_MOINS);
    seconds = -seconds;
  }

  uint8_t tmp = seconds / 3600;
  seconds %= 3600;
  if (tmp > 0) {
    PLAY_NUMBER(tmp, 0, FEMININ);
    PUSH_NUMBER_PROMPT(FR_PROMPT_HEURE);
  }

  tmp = seconds / 60;
  seconds %= 60;
  if (tmp > 0) {
    PLAY_NUMBER(tmp, 0, FEMININ);
    PUSH_NUMBER_PROMPT(FR_PROMPT_MINUTE);
    if (seconds > 0)
      PUSH_NUMBER_PROMPT(FR_PROMPT_ET);
  }

  if (seconds > 0) {
    PLAY_NUMBER(seconds, 0, FEMININ);
    PUSH_NUMBER_PROMPT(FR_PROMPT_SECONDE);
  }
}

LANGUAGE_PACK_DECLARE(fr, "Francais");

#endif
