/*
 * Authors (alphabetical order)
 * - Bertrand Songis <bsongis@gmail.com>
 * - Martin Hotar <mhotar@gmail.com>
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

enum CzechPrompts {
  CZ_PROMPT_NUMBERS_BASE = 0,
  CZ_PROMPT_NULA = CZ_PROMPT_NUMBERS_BASE+0,    //0-99
  CZ_PROMPT_STO = CZ_PROMPT_NUMBERS_BASE+100,   //100,200 .. 900
  CZ_PROMPT_TISIC = CZ_PROMPT_NUMBERS_BASE+109, //1000
  CZ_PROMPT_TISICE = CZ_PROMPT_NUMBERS_BASE+110,
  CZ_PROMPT_JEDEN = CZ_PROMPT_NUMBERS_BASE+111,
  CZ_PROMPT_JEDNO = CZ_PROMPT_NUMBERS_BASE+112,
  CZ_PROMPT_DVE = CZ_PROMPT_NUMBERS_BASE+113,
  CZ_PROMPT_CELA = CZ_PROMPT_NUMBERS_BASE+114,
  CZ_PROMPT_CELE = CZ_PROMPT_NUMBERS_BASE+115,
  CZ_PROMPT_CELYCH = CZ_PROMPT_NUMBERS_BASE+116,
  CZ_PROMPT_MINUS = CZ_PROMPT_NUMBERS_BASE+117,

  CZ_PROMPT_UNITS_BASE = 118,
  CZ_PROMPT_VOLTS = CZ_PROMPT_UNITS_BASE+UNIT_VOLTS, //(jeden)volt,(dva)volty,(pet)voltu,(desetina)voltu
  CZ_PROMPT_AMPS = CZ_PROMPT_UNITS_BASE+(UNIT_AMPS*4),
  CZ_PROMPT_METERS_PER_SECOND = CZ_PROMPT_UNITS_BASE+(UNIT_METERS_PER_SECOND*4),
  CZ_PROMPT_SPARE1 = CZ_PROMPT_UNITS_BASE+(UNIT_RAW*4),
  CZ_PROMPT_KMH = CZ_PROMPT_UNITS_BASE+(UNIT_KMH*4),
  CZ_PROMPT_METERS = CZ_PROMPT_UNITS_BASE+(UNIT_METERS*4),
  CZ_PROMPT_DEGREES = CZ_PROMPT_UNITS_BASE+(UNIT_DEGREES*4),
  CZ_PROMPT_PERCENT = CZ_PROMPT_UNITS_BASE+(UNIT_PERCENT*4),
  CZ_PROMPT_MILLIAMPS = CZ_PROMPT_UNITS_BASE+(UNIT_MILLIAMPS*4),
  CZ_PROMPT_MAH = CZ_PROMPT_UNITS_BASE+(UNIT_MAH*4),
  CZ_PROMPT_WATTS = CZ_PROMPT_UNITS_BASE+(UNIT_WATTS*4),
  CZ_PROMPT_DB = CZ_PROMPT_UNITS_BASE+(UNIT_DBM*4),
  CZ_PROMPT_FEET = CZ_PROMPT_UNITS_BASE+(UNIT_FEET*4),
  CZ_PROMPT_KTS = CZ_PROMPT_UNITS_BASE+(UNIT_KTS*4),
  CZ_PROMPT_HOURS = CZ_PROMPT_UNITS_BASE+(UNIT_HOURS*4),
  CZ_PROMPT_MINUTES = CZ_PROMPT_UNITS_BASE+(UNIT_MINUTES*4),
  CZ_PROMPT_SECONDS = CZ_PROMPT_UNITS_BASE+(UNIT_SECONDS*4),
  CZ_PROMPT_RPMS = CZ_PROMPT_UNITS_BASE+(UNIT_RPMS*4),
  CZ_PROMPT_G = CZ_PROMPT_UNITS_BASE+(UNIT_G*4),
 
  CZ_PROMPT_LABELS_BASE = 194,
  CZ_PROMPT_TIMER1 = CZ_PROMPT_LABELS_BASE+TELEM_TM1,
  CZ_PROMPT_TIMER2 = CZ_PROMPT_LABELS_BASE+TELEM_TM2,
  CZ_PROMPT_RSSI_TX = CZ_PROMPT_LABELS_BASE+TELEM_RSSI_TX,
  CZ_PROMPT_RSSI_RX = CZ_PROMPT_LABELS_BASE+TELEM_RSSI_RX,
  CZ_PROMPT_A1 = CZ_PROMPT_LABELS_BASE+TELEM_A1,
  CZ_PROMPT_A2 = CZ_PROMPT_LABELS_BASE+TELEM_A2,
  CZ_PROMPT_ALTITUDE = CZ_PROMPT_LABELS_BASE+TELEM_ALT,
  CZ_PROMPT_RPM = CZ_PROMPT_LABELS_BASE+TELEM_RPM,
  CZ_PROMPT_ESSENCE = CZ_PROMPT_LABELS_BASE+TELEM_FUEL,
  CZ_PROMPT_T1 = CZ_PROMPT_LABELS_BASE+TELEM_T1,
  CZ_PROMPT_T2 = CZ_PROMPT_LABELS_BASE+TELEM_T2,
  CZ_PROMPT_VITESSE = CZ_PROMPT_LABELS_BASE+TELEM_SPEED,
  CZ_PROMPT_DISTANCE = CZ_PROMPT_LABELS_BASE+TELEM_DIST,
  CZ_PROMPT_GPSALTITUDE = CZ_PROMPT_LABELS_BASE+TELEM_GPSALT,
  CZ_PROMPT_ELEMENTS_LIPO = CZ_PROMPT_LABELS_BASE+TELEM_CELL,
  CZ_PROMPT_TOTAL_LIPO = CZ_PROMPT_LABELS_BASE+TELEM_CELLS_SUM,
  CZ_PROMPT_VFAS = CZ_PROMPT_LABELS_BASE+TELEM_VFAS,
  CZ_PROMPT_COURANT = CZ_PROMPT_LABELS_BASE+TELEM_CURRENT,
  CZ_PROMPT_CONSOMMATION = CZ_PROMPT_LABELS_BASE+TELEM_CONSUMPTION,
  CZ_PROMPT_PUISSANCE = CZ_PROMPT_LABELS_BASE+TELEM_POWER,
  CZ_PROMPT_ACCELx = CZ_PROMPT_LABELS_BASE+TELEM_ACCx,
  CZ_PROMPT_ACCELy = CZ_PROMPT_LABELS_BASE+TELEM_ACCy,
  CZ_PROMPT_ACCELz = CZ_PROMPT_LABELS_BASE+TELEM_ACCz,
  CZ_PROMPT_HDG = CZ_PROMPT_LABELS_BASE+TELEM_HDG,
  CZ_PROMPT_VARIO = CZ_PROMPT_LABELS_BASE+TELEM_VSPD,
 
};

#if defined(VOICE)

#if defined(CPUARM)
  #define CZ_PUSH_UNIT_PROMPT(p, u) cz_pushUnitPrompt((p), (u), id)
#else
  #define CZ_PUSH_UNIT_PROMPT(p, u) pushUnitPrompt((p), (u))
#endif

#define MUZSKY 0x80
#define ZENSKY 0x81
#define STREDNI 0x82

I18N_PLAY_FUNCTION(cz, pushUnitPrompt, int16_t number, uint8_t unitprompt)
{
  if (number == 1)
    PUSH_NUMBER_PROMPT(unitprompt);
  else if (number > 1 && number < 5)
    PUSH_NUMBER_PROMPT(unitprompt+1);
  else
    PUSH_NUMBER_PROMPT(unitprompt+2);
}

I18N_PLAY_FUNCTION(cz, playNumber, getvalue_t number, uint8_t unit, uint8_t att)
{

  if (number < 0) {
    PUSH_NUMBER_PROMPT(CZ_PROMPT_MINUS);
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
        PLAY_NUMBER(qr.quot, 0, ZENSKY);
        if (qr.quot == 0)
          PUSH_NUMBER_PROMPT(CZ_PROMPT_CELA);
        else
          CZ_PUSH_UNIT_PROMPT(qr.quot, CZ_PROMPT_CELA);
        PLAY_NUMBER(qr.rem, 0, ZENSKY);
        PUSH_NUMBER_PROMPT(CZ_PROMPT_UNITS_BASE+((unit-1)*4)+3);
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
    PUSH_NUMBER_PROMPT(CZ_PROMPT_JEDEN);
    number = -1;
  }
  
  if ((number == 1) && (att == STREDNI)) {
    PUSH_NUMBER_PROMPT(CZ_PROMPT_JEDNO);
    number = -1;
  }
  
  if ((number == 2) && ((att == ZENSKY) || (att == STREDNI))) {
    PUSH_NUMBER_PROMPT(CZ_PROMPT_DVE);
    number = -1;
  }
  
  if (number >= 1000) {
    if (number >= 2000) 
      PLAY_NUMBER(number / 1000, 0, 0);
    if (number >= 2000 && number < 5000)
      PUSH_NUMBER_PROMPT(CZ_PROMPT_TISICE);
    else
      PUSH_NUMBER_PROMPT(CZ_PROMPT_TISIC);
    number %= 1000;
    if (number == 0)
      number = -1;
  }
  if (number >= 100) {
    PUSH_NUMBER_PROMPT(CZ_PROMPT_STO + (number/100)-1);
    number %= 100;
    if (number == 0)
      number = -1;
  }
  
  if (number >= 0) {
    PUSH_NUMBER_PROMPT(CZ_PROMPT_NULA+number);
  }

  if (unit) {
    CZ_PUSH_UNIT_PROMPT(tmp, (CZ_PROMPT_UNITS_BASE+((unit-1)*4)));
  }
}

I18N_PLAY_FUNCTION(cz, playDuration, int16_t seconds)
{
  if (seconds < 0) {
    PUSH_NUMBER_PROMPT(CZ_PROMPT_MINUS);
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

LANGUAGE_PACK_DECLARE(cz, "Czech");

#endif
