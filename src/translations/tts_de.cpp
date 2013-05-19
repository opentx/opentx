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

#include "../opentx.h"

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

  DE_PROMPT_UNITS_BASE = 110,
  DE_PROMPT_VOLTS = DE_PROMPT_UNITS_BASE+UNIT_VOLTS,
  DE_PROMPT_AMPS = DE_PROMPT_UNITS_BASE+UNIT_AMPS,
  DE_PROMPT_METERS_PER_SECOND = DE_PROMPT_UNITS_BASE+UNIT_METERS_PER_SECOND,
  DE_PROMPT_SPARE1 = DE_PROMPT_UNITS_BASE+UNIT_RAW,
  DE_PROMPT_KMH = DE_PROMPT_UNITS_BASE+UNIT_KMH,
  DE_PROMPT_METERS = DE_PROMPT_UNITS_BASE+UNIT_METERS,
  DE_PROMPT_DEGREES = DE_PROMPT_UNITS_BASE+UNIT_DEGREES,
  DE_PROMPT_PERCENT = DE_PROMPT_UNITS_BASE+UNIT_PERCENT,
  DE_PROMPT_MILLIAMPS = DE_PROMPT_UNITS_BASE+UNIT_MILLIAMPS,
  DE_PROMPT_MAH = DE_PROMPT_UNITS_BASE+UNIT_MAH,
  DE_PROMPT_WATTS = DE_PROMPT_UNITS_BASE+UNIT_WATTS,
  DE_PROMPT_FEET = DE_PROMPT_UNITS_BASE+UNIT_FEET,
  DE_PROMPT_KTS = DE_PROMPT_UNITS_BASE+UNIT_KTS,
  DE_PROMPT_HOURS = DE_PROMPT_UNITS_BASE+UNIT_HOURS,
  DE_PROMPT_MINUTES = DE_PROMPT_UNITS_BASE+UNIT_MINUTES,
  DE_PROMPT_SECONDS = DE_PROMPT_UNITS_BASE+UNIT_SECONDS,
  DE_PROMPT_RPMS = DE_PROMPT_UNITS_BASE+UNIT_RPMS,
  DE_PROMPT_G= DE_PROMPT_UNITS_BASE+UNIT_G,

  DE_PROMPT_LABELS_BASE = 128,
  DE_PROMPT_TIMER1 = DE_PROMPT_LABELS_BASE+TELEM_TM1,
  DE_PROMPT_TIMER2 = DE_PROMPT_LABELS_BASE+TELEM_TM2,
  DE_PROMPT_RSSI_TX = DE_PROMPT_LABELS_BASE+TELEM_RSSI_TX,
  DE_PROMPT_RSSI_RX = DE_PROMPT_LABELS_BASE+TELEM_RSSI_RX,
  DE_PROMPT_A1 = DE_PROMPT_LABELS_BASE+TELEM_A1,
  DE_PROMPT_A2 = DE_PROMPT_LABELS_BASE+TELEM_A2,
  DE_PROMPT_ALTITUDE = DE_PROMPT_LABELS_BASE+TELEM_ALT,
  DE_PROMPT_RPM = DE_PROMPT_LABELS_BASE+TELEM_RPM,
  DE_PROMPT_ESSENCE = DE_PROMPT_LABELS_BASE+TELEM_FUEL,
  DE_PROMPT_T1 = DE_PROMPT_LABELS_BASE+TELEM_T1,
  DE_PROMPT_T2 = DE_PROMPT_LABELS_BASE+TELEM_T2,
  DE_PROMPT_VITESSE = DE_PROMPT_LABELS_BASE+TELEM_SPEED,
  DE_PROMPT_DISTANCE = DE_PROMPT_LABELS_BASE+TELEM_DIST,
  DE_PROMPT_GPSALTITUDE = DE_PROMPT_LABELS_BASE+TELEM_GPSALT,
  DE_PROMPT_ELEMENTS_LIPO = DE_PROMPT_LABELS_BASE+TELEM_CELL,
  DE_PROMPT_TOTAL_LIPO = DE_PROMPT_LABELS_BASE+TELEM_CELLS_SUM,
  DE_PROMPT_VFAS = DE_PROMPT_LABELS_BASE+TELEM_VFAS,
  DE_PROMPT_COURANT = DE_PROMPT_LABELS_BASE+TELEM_CURRENT,
  DE_PROMPT_CONSOMMATION = DE_PROMPT_LABELS_BASE+TELEM_CONSUMPTION,
  DE_PROMPT_PUISSANCE = DE_PROMPT_LABELS_BASE+TELEM_POWER,
  DE_PROMPT_ACCELx = DE_PROMPT_LABELS_BASE+TELEM_ACCx,
  DE_PROMPT_ACCELy = DE_PROMPT_LABELS_BASE+TELEM_ACCy,
  DE_PROMPT_ACCELz = DE_PROMPT_LABELS_BASE+TELEM_ACCz,
  DE_PROMPT_HDG = DE_PROMPT_LABELS_BASE+TELEM_HDG,
  DE_PROMPT_VARIO = DE_PROMPT_LABELS_BASE+TELEM_VSPD,

};

#if defined(VOICE)

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

  if (unit) {
    unit--;
    convertUnit(number, unit);
    if (IS_IMPERIAL_ENABLE()) {
      if (unit == UNIT_METERS) {
        unit = UNIT_FEET;
      }
    }
    unit++;
  }

  int8_t mode = MODE(att);
  if (mode > 0) {
    div_t qr = div(number, (mode == 1 ? 10 : 100));
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
    PUSH_NUMBER_PROMPT(DE_PROMPT_UNITS_BASE+unit-1);
  }
}

I18N_PLAY_FUNCTION(de, playDuration, int16_t seconds)
{
  if (seconds < 0) {
    PUSH_NUMBER_PROMPT(DE_PROMPT_MINUS);
    seconds = -seconds;
  }

  uint8_t ore = 0;
  uint8_t tmp = seconds / 3600;
  seconds %= 3600;
  if (tmp > 0) {
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

LANGUAGE_PACK_DECLARE(de, "German");

#endif
