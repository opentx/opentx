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

enum PortuguesePrompts {
  PROMPT_NUMBERS_BASE = 0,
  PROMPT_ZERO = PROMPT_NUMBERS_BASE+0,
  PROMPT_CEM = PROMPT_NUMBERS_BASE+100,
  PROMPT_CENTO = PROMPT_NUMBERS_BASE+101,
  PROMPT_DUZENTOS = PROMPT_NUMBERS_BASE+102,
  PROMPT_TREZCENTOS,
  PROMPT_QUATROCENTOS,
  PROMPT_QUINHENTOS,
  PROMPT_SEISCENTOS,
  PROMPT_SETECENTOS,
  PROMPT_OITOCENTOS,
  PROMPT_NUEVECENTOS,
  PROMPT_MIL = PROMPT_NUMBERS_BASE+110,
  PROMPT_VIRGULA = 111,
  PROMPT_UMA,
  PROMPT_DUAS,
  PROMPT_E,
  PROMPT_MENOS,
  PROMPT_HORA,
  PROMPT_HORAS,
  PROMPT_MINUTO,
  PROMPT_MINUTOS,
  PROMPT_SEGUNDO,
  PROMPT_SEGUNDOS,

  PROMPT_UNITS_BASE = 122,
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

  PROMPT_LABELS_BASE = 140,
  PROMPT_TIMER1 = PROMPT_LABELS_BASE+TELEM_TM1,
  PROMPT_TIMER2 = PROMPT_LABELS_BASE+TELEM_TM2,
  PROMPT_RSSI_TX = PROMPT_LABELS_BASE+TELEM_RSSI_TX,
  PROMPT_RSSI_RX = PROMPT_LABELS_BASE+TELEM_RSSI_RX,
  PROMPT_A1 = PROMPT_LABELS_BASE+TELEM_A1,
  PROMPT_A2 = PROMPT_LABELS_BASE+TELEM_A2,
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

PLAY_FUNCTION(playNumber, int16_t number, uint8_t unit, uint8_t att)
{
  if (number < 0) {
    PUSH_PROMPT(PROMPT_MENOS);
    number = -number;
  }
  int8_t mode = MODE(att);
  if (mode > 0) {
    div_t qr = div(number, (mode == 1 ? 10 : 100));
    if (qr.rem > 0) {
      PLAY_NUMBER(qr.quot, 0, 0);
      PUSH_PROMPT(PROMPT_VIRGULA);
      if (mode==2 && qr.rem < 10)
        PUSH_PROMPT(PROMPT_ZERO);
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
      PUSH_PROMPT(PROMPT_MIL);
    } else {
      PUSH_PROMPT(PROMPT_MIL);
    }
    number %= 1000;
    if (number == 0)
      number = -1;
  }
  if (number >= 100) {
    PUSH_PROMPT(PROMPT_CENTO + number/100);
    number %= 100;
    if (number == 0)
      number = -1;
  }
  PUSH_PROMPT(PROMPT_ZERO+number);

  if (unit) {
    PUSH_PROMPT(PROMPT_UNITS_BASE+unit-1);
  }
}

PLAY_FUNCTION(playDuration, int16_t seconds)
{
  if (seconds < 0) {
    PUSH_PROMPT(PROMPT_MENOS);
    seconds = -seconds;
  }

  uint8_t ore = 0;
  uint8_t tmp = seconds / 3600;
  seconds %= 3600;
  if (tmp > 0) {
    ore=tmp;
    if (tmp > 2) {
      PLAY_NUMBER(tmp, 0, 0);
      PUSH_PROMPT(PROMPT_HORAS);
    } else if (tmp==2) {
      PUSH_PROMPT(PROMPT_DUAS);
      PUSH_PROMPT(PROMPT_HORAS);
    } else if (tmp==1) {
      PUSH_PROMPT(PROMPT_UMA);
      PUSH_PROMPT(PROMPT_HORA);
    }
  }

  tmp = seconds / 60;
  seconds %= 60;
  if (tmp > 0 || ore >0) {
    if (tmp != 1) {
      PLAY_NUMBER(tmp, 0, 0);
      PUSH_PROMPT(PROMPT_MINUTOS);
    } else {
      PUSH_PROMPT(PROMPT_NUMBERS_BASE+1);
      PUSH_PROMPT(PROMPT_MINUTO);
    }
    PUSH_PROMPT(PROMPT_E);
  }

  if (seconds != 1) {
    PLAY_NUMBER(seconds, 0, 0);
    PUSH_PROMPT(PROMPT_SEGUNDOS);
  } else {
    PUSH_PROMPT(PROMPT_NUMBERS_BASE+1);
    PUSH_PROMPT(PROMPT_SEGUNDO);
  }
}

#endif
