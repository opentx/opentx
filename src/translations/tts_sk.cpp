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
 * - Martin Hotar
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

enum SlovakPrompts {
  PROMPT_NUMBERS_BASE = 0,
  PROMPT_NULA = PROMPT_NUMBERS_BASE+0,    //0-99
  PROMPT_STO = PROMPT_NUMBERS_BASE+100,   //100,200 .. 900
  PROMPT_TISIC = PROMPT_NUMBERS_BASE+109, //1000
  PROMPT_DVETISIC = PROMPT_NUMBERS_BASE+110,
  PROMPT_JEDEN = PROMPT_NUMBERS_BASE+111,
  PROMPT_JEDNO = PROMPT_NUMBERS_BASE+112,
  PROMPT_DVE = PROMPT_NUMBERS_BASE+113,
  PROMPT_CELA = PROMPT_NUMBERS_BASE+114,
  PROMPT_CELE = PROMPT_NUMBERS_BASE+115,
  PROMPT_CELYCH = PROMPT_NUMBERS_BASE+116,
  PROMPT_MINUS = PROMPT_NUMBERS_BASE+117,

  PROMPT_UNITS_BASE = 118,
  PROMPT_VOLTS = PROMPT_UNITS_BASE+UNIT_VOLTS, //(jeden)volt,(dva)volty,(pet)voltu,(desetina)voltu
  PROMPT_AMPS = PROMPT_UNITS_BASE+(UNIT_AMPS*4),
  PROMPT_METERS_PER_SECOND = PROMPT_UNITS_BASE+(UNIT_METERS_PER_SECOND*4),
  PROMPT_SPARE1 = PROMPT_UNITS_BASE+(UNIT_RAW*4),
  PROMPT_KMH = PROMPT_UNITS_BASE+(UNIT_KMH*4),
  PROMPT_METERS = PROMPT_UNITS_BASE+(UNIT_METERS*4),
  PROMPT_DEGREES = PROMPT_UNITS_BASE+(UNIT_DEGREES*4),
  PROMPT_PERCENT = PROMPT_UNITS_BASE+(UNIT_PERCENT*4),
  PROMPT_MILLIAMPS = PROMPT_UNITS_BASE+(UNIT_MILLIAMPS*4),
  PROMPT_MAH = PROMPT_UNITS_BASE+(UNIT_MAH*4),
  PROMPT_WATTS = PROMPT_UNITS_BASE+(UNIT_WATTS*4),
  PROMPT_DB = PROMPT_UNITS_BASE+(UNIT_DBM*4),
  PROMPT_FEET = PROMPT_UNITS_BASE+(UNIT_FEET*4),
  PROMPT_KTS = PROMPT_UNITS_BASE+(UNIT_KTS*4),
  PROMPT_HOURS = PROMPT_UNITS_BASE+(UNIT_HOURS*4),
  PROMPT_MINUTES = PROMPT_UNITS_BASE+(UNIT_MINUTES*4),
  PROMPT_SECONDS = PROMPT_UNITS_BASE+(UNIT_SECONDS*4),
  PROMPT_RPMS = PROMPT_UNITS_BASE+(UNIT_RPMS*4),
  PROMPT_G = PROMPT_UNITS_BASE+(UNIT_G*4),
 
  PROMPT_LABELS_BASE = 194,
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

#if defined(CPUARM)
#define PUSH_UNIT_PROMPT(p, u) pushUnitPrompt((p), (u), id)
#else
#define PUSH_UNIT_PROMPT(p, u) pushUnitPrompt((p), (u))
#endif

#define MUZSKY 0x80
#define ZENSKY 0x81
#define STREDNI 0x82

PLAY_FUNCTION(pushUnitPrompt, int16_t number, uint8_t unitprompt)
{
  if (number == 1)
    PUSH_NUMBER_PROMPT(unitprompt);
  else if (number > 1 && number < 5)
    PUSH_NUMBER_PROMPT(unitprompt+1);
  else
    PUSH_NUMBER_PROMPT(unitprompt+2);
}

PLAY_FUNCTION(playNumber, getvalue_t number, uint8_t unit, uint8_t att)
{

  if (number < 0) {
    PUSH_NUMBER_PROMPT(PROMPT_MINUS);
    number = -number;
  }

  convertUnit(number, unit);

  int8_t mode = MODE(att);
  if (mode > 0) {
    // we assume that we are PREC1
    div_t qr = div(number, 10);   
      if (qr.rem) {
        PLAY_NUMBER(qr.quot, 0, ZENSKY);
        if (qr.quot == 0)
          PUSH_NUMBER_PROMPT(PROMPT_CELA);
        else
          PUSH_UNIT_PROMPT(qr.quot, PROMPT_CELA);
        PLAY_NUMBER(qr.rem, 0, ZENSKY);
        PUSH_NUMBER_PROMPT(PROMPT_UNITS_BASE+((unit-1)*4)+3);
        return;
      }
      else
        number = qr.quot;
  }

  int16_t tmp = number;
  
  switch(unit) {
    case 0:
      break;
    case 4:
    case 10:
    case 13:
    case 15:
    case 16:
    case 17:
    case 18:
      att = ZENSKY;
      break;
    case 8:
    case 19:
      att = STREDNI;
      break;
    default:
      att = MUZSKY;
      break;
  }

  if ((number == 1) && (att == MUZSKY)) {
    PUSH_NUMBER_PROMPT(PROMPT_JEDEN);
    number = -1;
  }
  
  if ((number == 1) && (att == STREDNI)) {
    PUSH_NUMBER_PROMPT(PROMPT_JEDNO);
    number = -1;
  }
  
  if ((number == 2) && ((att == ZENSKY) || (att == STREDNI))) {
    PUSH_NUMBER_PROMPT(PROMPT_DVE);
    number = -1;
  }
  
  if (number >= 1000) { 
    if (number >= 3000)
      PLAY_NUMBER(number / 1000, 0, 0);     
    if (number >= 2000 && number < 3000)
      PUSH_NUMBER_PROMPT(PROMPT_DVETISIC);
    else
      PUSH_NUMBER_PROMPT(PROMPT_TISIC);

    number %= 1000;
    if (number == 0)
      number = -1;
  }
  if (number >= 100) {
    PUSH_NUMBER_PROMPT(PROMPT_STO + (number/100)-1);
    number %= 100;
    if (number == 0)
      number = -1;
  }
  
  if (number >= 0) {
    PUSH_NUMBER_PROMPT(PROMPT_NULA+number);
  }

  if (unit) {
    PUSH_UNIT_PROMPT(tmp, (PROMPT_UNITS_BASE+((unit-1)*4)));
  }
}

PLAY_FUNCTION(playDuration, int16_t seconds)
{
  if (seconds < 0) {
    PUSH_NUMBER_PROMPT(PROMPT_MINUS);
    seconds = -seconds;
  }

  uint8_t tmp = seconds / 3600;
  seconds %= 3600;
  if (tmp > 0) {
    PLAY_NUMBER(tmp, UNIT_HOURS+1, ZENSKY);
  }

  tmp = seconds / 60;
  seconds %= 60;
  if (tmp > 0) {
    PLAY_NUMBER(tmp, UNIT_MINUTES+1, ZENSKY);
  }

  if (seconds > 0) {
    PLAY_NUMBER(seconds, UNIT_SECONDS+1, ZENSKY);
  }
}

#endif
