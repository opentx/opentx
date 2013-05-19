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
  SK_PROMPT_NUMBERS_BASE = 0,
  SK_PROMPT_NULA = SK_PROMPT_NUMBERS_BASE+0,    //0-99
  SK_PROMPT_STO = SK_PROMPT_NUMBERS_BASE+100,   //100,200 .. 900
  SK_PROMPT_TISIC = SK_PROMPT_NUMBERS_BASE+109, //1000
  SK_PROMPT_DVETISIC = SK_PROMPT_NUMBERS_BASE+110,
  SK_PROMPT_JEDEN = SK_PROMPT_NUMBERS_BASE+111,
  SK_PROMPT_JEDNO = SK_PROMPT_NUMBERS_BASE+112,
  SK_PROMPT_DVE = SK_PROMPT_NUMBERS_BASE+113,
  SK_PROMPT_CELA = SK_PROMPT_NUMBERS_BASE+114,
  SK_PROMPT_CELE = SK_PROMPT_NUMBERS_BASE+115,
  SK_PROMPT_CELYCH = SK_PROMPT_NUMBERS_BASE+116,
  SK_PROMPT_MINUS = SK_PROMPT_NUMBERS_BASE+117,

  SK_PROMPT_UNITS_BASE = 118,
  SK_PROMPT_VOLTS = SK_PROMPT_UNITS_BASE+UNIT_VOLTS, //(jeden)volt,(dva)volty,(pet)voltu,(desetina)voltu
  SK_PROMPT_AMPS = SK_PROMPT_UNITS_BASE+(UNIT_AMPS*4),
  SK_PROMPT_METERS_PER_SECOND = SK_PROMPT_UNITS_BASE+(UNIT_METERS_PER_SECOND*4),
  SK_PROMPT_SPARE1 = SK_PROMPT_UNITS_BASE+(UNIT_RAW*4),
  SK_PROMPT_KMH = SK_PROMPT_UNITS_BASE+(UNIT_KMH*4),
  SK_PROMPT_METERS = SK_PROMPT_UNITS_BASE+(UNIT_METERS*4),
  SK_PROMPT_DEGREES = SK_PROMPT_UNITS_BASE+(UNIT_DEGREES*4),
  SK_PROMPT_PERCENT = SK_PROMPT_UNITS_BASE+(UNIT_PERCENT*4),
  SK_PROMPT_MILLIAMPS = SK_PROMPT_UNITS_BASE+(UNIT_MILLIAMPS*4),
  SK_PROMPT_MAH = SK_PROMPT_UNITS_BASE+(UNIT_MAH*4),
  SK_PROMPT_WATTS = SK_PROMPT_UNITS_BASE+(UNIT_WATTS*4),
  SK_PROMPT_DB = SK_PROMPT_UNITS_BASE+(UNIT_DBM*4),
  SK_PROMPT_FEET = SK_PROMPT_UNITS_BASE+(UNIT_FEET*4),
  SK_PROMPT_KTS = SK_PROMPT_UNITS_BASE+(UNIT_KTS*4),
  SK_PROMPT_HOURS = SK_PROMPT_UNITS_BASE+(UNIT_HOURS*4),
  SK_PROMPT_MINUTES = SK_PROMPT_UNITS_BASE+(UNIT_MINUTES*4),
  SK_PROMPT_SECONDS = SK_PROMPT_UNITS_BASE+(UNIT_SECONDS*4),
  SK_PROMPT_RPMS = SK_PROMPT_UNITS_BASE+(UNIT_RPMS*4),
  SK_PROMPT_G = SK_PROMPT_UNITS_BASE+(UNIT_G*4),
 
  SK_PROMPT_LABELS_BASE = 194,
  SK_PROMPT_TIMER1 = SK_PROMPT_LABELS_BASE+TELEM_TM1,
  SK_PROMPT_TIMER2 = SK_PROMPT_LABELS_BASE+TELEM_TM2,
  SK_PROMPT_RSSI_TX = SK_PROMPT_LABELS_BASE+TELEM_RSSI_TX,
  SK_PROMPT_RSSI_RX = SK_PROMPT_LABELS_BASE+TELEM_RSSI_RX,
  SK_PROMPT_A1 = SK_PROMPT_LABELS_BASE+TELEM_A1,
  SK_PROMPT_A2 = SK_PROMPT_LABELS_BASE+TELEM_A2,
  SK_PROMPT_ALTITUDE = SK_PROMPT_LABELS_BASE+TELEM_ALT,
  SK_PROMPT_RPM = SK_PROMPT_LABELS_BASE+TELEM_RPM,
  SK_PROMPT_ESSENCE = SK_PROMPT_LABELS_BASE+TELEM_FUEL,
  SK_PROMPT_T1 = SK_PROMPT_LABELS_BASE+TELEM_T1,
  SK_PROMPT_T2 = SK_PROMPT_LABELS_BASE+TELEM_T2,
  SK_PROMPT_VITESSE = SK_PROMPT_LABELS_BASE+TELEM_SPEED,
  SK_PROMPT_DISTANCE = SK_PROMPT_LABELS_BASE+TELEM_DIST,
  SK_PROMPT_GPSALTITUDE = SK_PROMPT_LABELS_BASE+TELEM_GPSALT,
  SK_PROMPT_ELEMENTS_LIPO = SK_PROMPT_LABELS_BASE+TELEM_CELL,
  SK_PROMPT_TOTAL_LIPO = SK_PROMPT_LABELS_BASE+TELEM_CELLS_SUM,
  SK_PROMPT_VFAS = SK_PROMPT_LABELS_BASE+TELEM_VFAS,
  SK_PROMPT_COURANT = SK_PROMPT_LABELS_BASE+TELEM_CURRENT,
  SK_PROMPT_CONSOMMATION = SK_PROMPT_LABELS_BASE+TELEM_CONSUMPTION,
  SK_PROMPT_PUISSANCE = SK_PROMPT_LABELS_BASE+TELEM_POWER,
  SK_PROMPT_ACCELx = SK_PROMPT_LABELS_BASE+TELEM_ACCx,
  SK_PROMPT_ACCELy = SK_PROMPT_LABELS_BASE+TELEM_ACCy,
  SK_PROMPT_ACCELz = SK_PROMPT_LABELS_BASE+TELEM_ACCz,
  SK_PROMPT_HDG = SK_PROMPT_LABELS_BASE+TELEM_HDG,
  SK_PROMPT_VARIO = SK_PROMPT_LABELS_BASE+TELEM_VSPD,
 
};

#if defined(VOICE)

#if defined(CPUARM)
  #define SK_PUSH_UNIT_PROMPT(p, u) sk_pushUnitPrompt((p), (u), id)
#else
  #define SK_PUSH_UNIT_PROMPT(p, u) pushUnitPrompt((p), (u))
#endif

#define MUZSKY 0x80
#define ZENSKY 0x81
#define STREDNI 0x82

I18N_PLAY_FUNCTION(sk, pushUnitPrompt, int16_t number, uint8_t unitprompt)
{
  if (number == 1)
    PUSH_NUMBER_PROMPT(unitprompt);
  else if (number > 1 && number < 5)
    PUSH_NUMBER_PROMPT(unitprompt+1);
  else
    PUSH_NUMBER_PROMPT(unitprompt+2);
}

I18N_PLAY_FUNCTION(sk, playNumber, getvalue_t number, uint8_t unit, uint8_t att)
{

  if (number < 0) {
    PUSH_NUMBER_PROMPT(SK_PROMPT_MINUS);
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
    // we assume that we are PREC1
    div_t qr = div(number, 10);   
      if (qr.rem) {
        PLAY_NUMBER(qr.quot, 0, ZENSKY);
        if (qr.quot == 0)
          PUSH_NUMBER_PROMPT(SK_PROMPT_CELA);
        else
          SK_PUSH_UNIT_PROMPT(qr.quot, SK_PROMPT_CELA);
        PLAY_NUMBER(qr.rem, 0, ZENSKY);
        PUSH_NUMBER_PROMPT(SK_PROMPT_UNITS_BASE+((unit-1)*4)+3);
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
    PUSH_NUMBER_PROMPT(SK_PROMPT_JEDEN);
    number = -1;
  }
  
  if ((number == 1) && (att == STREDNI)) {
    PUSH_NUMBER_PROMPT(SK_PROMPT_JEDNO);
    number = -1;
  }
  
  if ((number == 2) && ((att == ZENSKY) || (att == STREDNI))) {
    PUSH_NUMBER_PROMPT(SK_PROMPT_DVE);
    number = -1;
  }
  
  if (number >= 1000) { 
    if (number >= 3000)
      PLAY_NUMBER(number / 1000, 0, 0);     
    if (number >= 2000 && number < 3000)
      PUSH_NUMBER_PROMPT(SK_PROMPT_DVETISIC);
    else
      PUSH_NUMBER_PROMPT(SK_PROMPT_TISIC);

    number %= 1000;
    if (number == 0)
      number = -1;
  }
  if (number >= 100) {
    PUSH_NUMBER_PROMPT(SK_PROMPT_STO + (number/100)-1);
    number %= 100;
    if (number == 0)
      number = -1;
  }
  
  if (number >= 0) {
    PUSH_NUMBER_PROMPT(SK_PROMPT_NULA+number);
  }

  if (unit) {
    SK_PUSH_UNIT_PROMPT(tmp, (SK_PROMPT_UNITS_BASE+((unit-1)*4)));
  }
}

I18N_PLAY_FUNCTION(sk, playDuration, int16_t seconds)
{
  if (seconds < 0) {
    PUSH_NUMBER_PROMPT(SK_PROMPT_MINUS);
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

LANGUAGE_PACK_DECLARE(sk, "Slovak");

#endif
