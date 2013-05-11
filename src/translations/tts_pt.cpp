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

enum PortuguesePrompts {
  PT_PROMPT_NUMBERS_BASE = 0,
  PT_PROMPT_ZERO = PT_PROMPT_NUMBERS_BASE+0,
  PT_PROMPT_CEM = PT_PROMPT_NUMBERS_BASE+100,
  PT_PROMPT_CENTO = PT_PROMPT_NUMBERS_BASE+101,
  PT_PROMPT_DUZENTOS = PT_PROMPT_NUMBERS_BASE+102,
  PT_PROMPT_TREZCENTOS,
  PT_PROMPT_QUATROCENTOS,
  PT_PROMPT_QUINHENTOS,
  PT_PROMPT_SEISCENTOS,
  PT_PROMPT_SETECENTOS,
  PT_PROMPT_OITOCENTOS,
  PT_PROMPT_NUEVECENTOS,
  PT_PROMPT_MIL = PT_PROMPT_NUMBERS_BASE+110,
  PT_PROMPT_VIRGULA = 111,
  PT_PROMPT_UMA,
  PT_PROMPT_DUAS,
  PT_PROMPT_E,
  PT_PROMPT_MENOS,
  PT_PROMPT_HORA,
  PT_PROMPT_HORAS,
  PT_PROMPT_MINUTO,
  PT_PROMPT_MINUTOS,
  PT_PROMPT_SEGUNDO,
  PT_PROMPT_SEGUNDOS,

  PT_PROMPT_UNITS_BASE = 122,
  PT_PROMPT_VOLTS = PT_PROMPT_UNITS_BASE+UNIT_VOLTS,
  PT_PROMPT_AMPS = PT_PROMPT_UNITS_BASE+UNIT_AMPS,
  PT_PROMPT_METERS_PER_SECOND = PT_PROMPT_UNITS_BASE+UNIT_METERS_PER_SECOND,
  PT_PROMPT_SPARE1 = PT_PROMPT_UNITS_BASE+UNIT_RAW,
  PT_PROMPT_KMH = PT_PROMPT_UNITS_BASE+UNIT_KMH,
  PT_PROMPT_METERS = PT_PROMPT_UNITS_BASE+UNIT_METERS,
  PT_PROMPT_DEGREES = PT_PROMPT_UNITS_BASE+UNIT_DEGREES,
  PT_PROMPT_PERCENT = PT_PROMPT_UNITS_BASE+UNIT_PERCENT,
  PT_PROMPT_MILLIAMPS = PT_PROMPT_UNITS_BASE+UNIT_MILLIAMPS,
  PT_PROMPT_MAH = PT_PROMPT_UNITS_BASE+UNIT_MAH,
  PT_PROMPT_WATTS = PT_PROMPT_UNITS_BASE+UNIT_WATTS,
  PT_PROMPT_FEET = PT_PROMPT_UNITS_BASE+UNIT_FEET,
  PT_PROMPT_KTS = PT_PROMPT_UNITS_BASE+UNIT_KTS,

  PT_PROMPT_LABELS_BASE = 140,
  PT_PROMPT_TIMER1 = PT_PROMPT_LABELS_BASE+TELEM_TM1,
  PT_PROMPT_TIMER2 = PT_PROMPT_LABELS_BASE+TELEM_TM2,
  PT_PROMPT_RSSI_TX = PT_PROMPT_LABELS_BASE+TELEM_RSSI_TX,
  PT_PROMPT_RSSI_RX = PT_PROMPT_LABELS_BASE+TELEM_RSSI_RX,
  PT_PROMPT_A1 = PT_PROMPT_LABELS_BASE+TELEM_A1,
  PT_PROMPT_A2 = PT_PROMPT_LABELS_BASE+TELEM_A2,
  PT_PROMPT_ALTITUDE = PT_PROMPT_LABELS_BASE+TELEM_ALT,
  PT_PROMPT_RPM = PT_PROMPT_LABELS_BASE+TELEM_RPM,
  PT_PROMPT_ESSENCE = PT_PROMPT_LABELS_BASE+TELEM_FUEL,
  PT_PROMPT_T1 = PT_PROMPT_LABELS_BASE+TELEM_T1,
  PT_PROMPT_T2 = PT_PROMPT_LABELS_BASE+TELEM_T2,
  PT_PROMPT_VITESSE = PT_PROMPT_LABELS_BASE+TELEM_SPEED,
  PT_PROMPT_DISTANCE = PT_PROMPT_LABELS_BASE+TELEM_DIST,
  PT_PROMPT_GPSALTITUDE = PT_PROMPT_LABELS_BASE+TELEM_GPSALT,
  PT_PROMPT_ELEMENTS_LIPO = PT_PROMPT_LABELS_BASE+TELEM_CELL,
  PT_PROMPT_TOTAL_LIPO = PT_PROMPT_LABELS_BASE+TELEM_CELLS_SUM,
  PT_PROMPT_VFAS = PT_PROMPT_LABELS_BASE+TELEM_VFAS,
  PT_PROMPT_COURANT = PT_PROMPT_LABELS_BASE+TELEM_CURRENT,
  PT_PROMPT_CONSOMMATION = PT_PROMPT_LABELS_BASE+TELEM_CONSUMPTION,
  PT_PROMPT_PUISSANCE = PT_PROMPT_LABELS_BASE+TELEM_POWER,
  PT_PROMPT_ACCELx = PT_PROMPT_LABELS_BASE+TELEM_ACCx,
  PT_PROMPT_ACCELy = PT_PROMPT_LABELS_BASE+TELEM_ACCy,
  PT_PROMPT_ACCELz = PT_PROMPT_LABELS_BASE+TELEM_ACCz,
  PT_PROMPT_HDG = PT_PROMPT_LABELS_BASE+TELEM_HDG,
  PT_PROMPT_VARIO = PT_PROMPT_LABELS_BASE+TELEM_VSPD,

};

#if defined(VOICE)

I18N_PLAY_FUNCTION(pt, playNumber, getvalue_t number, uint8_t unit, uint8_t att)
{
  if (number < 0) {
    PUSH_NUMBER_PROMPT(PT_PROMPT_MENOS);
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
      PUSH_NUMBER_PROMPT(PT_PROMPT_VIRGULA);
      if (mode==2 && qr.rem < 10)
        PUSH_NUMBER_PROMPT(PT_PROMPT_ZERO);
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
      PUSH_NUMBER_PROMPT(PT_PROMPT_MIL);
    } else {
      PUSH_NUMBER_PROMPT(PT_PROMPT_MIL);
    }
    number %= 1000;
    if (number == 0)
      number = -1;
  }
  if (number >= 100) {
    PUSH_NUMBER_PROMPT(PT_PROMPT_CENTO + number/100);
    number %= 100;
    if (number == 0)
      number = -1;
  }
  PUSH_NUMBER_PROMPT(PT_PROMPT_ZERO+number);

  if (unit) {
    PUSH_NUMBER_PROMPT(PT_PROMPT_UNITS_BASE+unit-1);
  }
}

I18N_PLAY_FUNCTION(pt, playDuration, int16_t seconds)
{
  if (seconds < 0) {
    PUSH_NUMBER_PROMPT(PT_PROMPT_MENOS);
    seconds = -seconds;
  }

  uint8_t ore = 0;
  uint8_t tmp = seconds / 3600;
  seconds %= 3600;
  if (tmp > 0) {
    ore=tmp;
    if (tmp > 2) {
      PLAY_NUMBER(tmp, 0, 0);
      PUSH_NUMBER_PROMPT(PT_PROMPT_HORAS);
    } else if (tmp==2) {
      PUSH_NUMBER_PROMPT(PT_PROMPT_DUAS);
      PUSH_NUMBER_PROMPT(PT_PROMPT_HORAS);
    } else if (tmp==1) {
      PUSH_NUMBER_PROMPT(PT_PROMPT_UMA);
      PUSH_NUMBER_PROMPT(PT_PROMPT_HORA);
    }
  }

  tmp = seconds / 60;
  seconds %= 60;
  if (tmp > 0 || ore >0) {
    if (tmp != 1) {
      PLAY_NUMBER(tmp, 0, 0);
      PUSH_NUMBER_PROMPT(PT_PROMPT_MINUTOS);
    } else {
      PUSH_NUMBER_PROMPT(PT_PROMPT_NUMBERS_BASE+1);
      PUSH_NUMBER_PROMPT(PT_PROMPT_MINUTO);
    }
    PUSH_NUMBER_PROMPT(PT_PROMPT_E);
  }

  if (seconds != 1) {
    PLAY_NUMBER(seconds, 0, 0);
    PUSH_NUMBER_PROMPT(PT_PROMPT_SEGUNDOS);
  } else {
    PUSH_NUMBER_PROMPT(PT_PROMPT_NUMBERS_BASE+1);
    PUSH_NUMBER_PROMPT(PT_PROMPT_SEGUNDO);
  }
}

LANGUAGE_PACK_DECLARE(pt, "portuguese");

#endif
