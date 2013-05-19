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

#include "../opentx.h"

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

  ES_PROMPT_UNITS_BASE = 122,
  ES_PROMPT_VOLTS = ES_PROMPT_UNITS_BASE+UNIT_VOLTS,
  ES_PROMPT_AMPS = ES_PROMPT_UNITS_BASE+UNIT_AMPS,
  ES_PROMPT_METERS_PER_SECOND = ES_PROMPT_UNITS_BASE+UNIT_METERS_PER_SECOND,
  ES_PROMPT_SPARE1 = ES_PROMPT_UNITS_BASE+UNIT_RAW,
  ES_PROMPT_KMH = ES_PROMPT_UNITS_BASE+UNIT_KMH,
  ES_PROMPT_METERS = ES_PROMPT_UNITS_BASE+UNIT_METERS,
  ES_PROMPT_DEGREES = ES_PROMPT_UNITS_BASE+UNIT_DEGREES,
  ES_PROMPT_PERCENT = ES_PROMPT_UNITS_BASE+UNIT_PERCENT,
  ES_PROMPT_MILLIAMPS = ES_PROMPT_UNITS_BASE+UNIT_MILLIAMPS,
  ES_PROMPT_MAH = ES_PROMPT_UNITS_BASE+UNIT_MAH,
  ES_PROMPT_WATTS = ES_PROMPT_UNITS_BASE+UNIT_WATTS,
  ES_PROMPT_FEET = ES_PROMPT_UNITS_BASE+UNIT_FEET,
  ES_PROMPT_KTS = ES_PROMPT_UNITS_BASE+UNIT_KTS,
  ES_PROMPT_HOURS = ES_PROMPT_UNITS_BASE+UNIT_HOURS,
  ES_PROMPT_MINUTES = ES_PROMPT_UNITS_BASE+UNIT_MINUTES,
  ES_PROMPT_SECONDS = ES_PROMPT_UNITS_BASE+UNIT_SECONDS,
  ES_PROMPT_RPMS = ES_PROMPT_UNITS_BASE+UNIT_RPMS,
  ES_PROMPT_G= ES_PROMPT_UNITS_BASE+UNIT_G,

  ES_PROMPT_LABELS_BASE = 139,
  ES_PROMPT_TIMER1 = ES_PROMPT_LABELS_BASE+TELEM_TM1,
  ES_PROMPT_TIMER2 = ES_PROMPT_LABELS_BASE+TELEM_TM2,
  ES_PROMPT_RSSI_TX = ES_PROMPT_LABELS_BASE+TELEM_RSSI_TX,
  ES_PROMPT_RSSI_RX = ES_PROMPT_LABELS_BASE+TELEM_RSSI_RX,
  ES_PROMPT_A1 = ES_PROMPT_LABELS_BASE+TELEM_A1,
  ES_PROMPT_A2 = ES_PROMPT_LABELS_BASE+TELEM_A2,
  ES_PROMPT_ALTITUDE = ES_PROMPT_LABELS_BASE+TELEM_ALT,
  ES_PROMPT_RPM = ES_PROMPT_LABELS_BASE+TELEM_RPM,
  ES_PROMPT_ESSENCE = ES_PROMPT_LABELS_BASE+TELEM_FUEL,
  ES_PROMPT_T1 = ES_PROMPT_LABELS_BASE+TELEM_T1,
  ES_PROMPT_T2 = ES_PROMPT_LABELS_BASE+TELEM_T2,
  ES_PROMPT_VITESSE = ES_PROMPT_LABELS_BASE+TELEM_SPEED,
  ES_PROMPT_DISTANCE = ES_PROMPT_LABELS_BASE+TELEM_DIST,
  ES_PROMPT_GPSALTITUDE = ES_PROMPT_LABELS_BASE+TELEM_GPSALT,
  ES_PROMPT_ELEMENTS_LIPO = ES_PROMPT_LABELS_BASE+TELEM_CELL,
  ES_PROMPT_TOTAL_LIPO = ES_PROMPT_LABELS_BASE+TELEM_CELLS_SUM,
  ES_PROMPT_VFAS = ES_PROMPT_LABELS_BASE+TELEM_VFAS,
  ES_PROMPT_COURANT = ES_PROMPT_LABELS_BASE+TELEM_CURRENT,
  ES_PROMPT_CONSOMMATION = ES_PROMPT_LABELS_BASE+TELEM_CONSUMPTION,
  ES_PROMPT_PUISSANCE = ES_PROMPT_LABELS_BASE+TELEM_POWER,
  ES_PROMPT_ACCELx = ES_PROMPT_LABELS_BASE+TELEM_ACCx,
  ES_PROMPT_ACCELy = ES_PROMPT_LABELS_BASE+TELEM_ACCy,
  ES_PROMPT_ACCELz = ES_PROMPT_LABELS_BASE+TELEM_ACCz,
  ES_PROMPT_HDG = ES_PROMPT_LABELS_BASE+TELEM_HDG,
  ES_PROMPT_VARIO = ES_PROMPT_LABELS_BASE+TELEM_VSPD,

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
  if (number >= 100) {
    PUSH_NUMBER_PROMPT(ES_PROMPT_CIENTO + number/100);
    number %= 100;
    if (number == 0)
      number = -1;
  }
  PUSH_NUMBER_PROMPT(ES_PROMPT_ZERO+number);

  if (unit) {
    PUSH_NUMBER_PROMPT(ES_PROMPT_UNITS_BASE+unit-1);
  }
}

I18N_PLAY_FUNCTION(es, playDuration, int16_t seconds)
{
  if (seconds < 0) {
    PUSH_NUMBER_PROMPT(ES_PROMPT_MENO);
    seconds = -seconds;
  }

  uint8_t ore = 0;
  uint8_t tmp = seconds / 3600;
  seconds %= 3600;
  if (tmp > 0) {
    ore=tmp;
    if (tmp > 1) {
      PLAY_NUMBER(tmp, 0, 0);
      PUSH_NUMBER_PROMPT(ES_PROMPT_HORAS);
    } else {
      PUSH_NUMBER_PROMPT(ES_PROMPT_UNA);
      PUSH_NUMBER_PROMPT(ES_PROMPT_HORA);
    }
  }

  tmp = seconds / 60;
  seconds %= 60;
  if (tmp > 0 || ore >0) {
    if (tmp != 1) {
      PLAY_NUMBER(tmp, 0, 0);
      PUSH_NUMBER_PROMPT(ES_PROMPT_MINUTOS);
    } else {
      PUSH_NUMBER_PROMPT(ES_PROMPT_UN);
      PUSH_NUMBER_PROMPT(ES_PROMPT_MINUTO);
    }
    PUSH_NUMBER_PROMPT(ES_PROMPT_Y);
  }

  if (seconds != 1) {
    PLAY_NUMBER(seconds, 0, 0);
    PUSH_NUMBER_PROMPT(ES_PROMPT_SEGUNDOS);
  } else {
    PUSH_NUMBER_PROMPT(ES_PROMPT_UN);
    PUSH_NUMBER_PROMPT(ES_PROMPT_SEGUNDO);
  }
}

LANGUAGE_PACK_DECLARE(es, "Espanol");

#endif
