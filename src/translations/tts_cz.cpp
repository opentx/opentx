/*
 * Authors (alphabetical order)
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Jean-Pierre Parisy
 * - Karl Szmutny <shadow@privy.de>
 * - Martin Hotar <mhotar@gmail.com>
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

enum CzechPrompts {
  PROMPT_NUMBERS_BASE = 0,
  PROMPT_NULA = PROMPT_NUMBERS_BASE+0,    //0-99
  PROMPT_STO = PROMPT_NUMBERS_BASE+100,   //100,200 .. 900
  PROMPT_TISIC = PROMPT_NUMBERS_BASE+109, //1000
  PROMPT_TISICE = PROMPT_NUMBERS_BASE+110,
  PROMPT_JEDEN = PROMPT_NUMBERS_BASE+111,
  PROMPT_JEDNO = PROMPT_NUMBERS_BASE+112,
  PROMPT_DVE = PROMPT_NUMBERS_BASE+113,
  PROMPT_CELA = PROMPT_NUMBERS_BASE+114,
  PROMPT_CELE = PROMPT_NUMBERS_BASE+115,
  PROMPT_CELYCH = PROMPT_NUMBERS_BASE+116,
  PROMPT_MINUS = PROMPT_NUMBERS_BASE+117,

  PROMPT_HODINA = 118,
  PROMPT_HODINY = 119,
  PROMPT_HODIN = 120,
  PROMPT_MINUTA = 121,
  PROMPT_MINUTY = 122,
  PROMPT_MINUT = 123,
  PROMPT_SEKUNDA = 124,
  PROMPT_SEKUNDY = 125,
  PROMPT_SEKUND = 126,

  PROMPT_UNITS_BASE = 130,
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
  PROMPT_FEET = PROMPT_UNITS_BASE+(UNIT_FEET*4),
  PROMPT_KTS = PROMPT_UNITS_BASE+(UNIT_KTS*4),
  
  PROMPT_LABELS_BASE = 190,
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

#define MUZSKY 0x80
#define ZENSKY 0x81
#define STREDNI 0x82

void pushUnitPrompt(int16_t number, uint8_t unitprompt)
{
  if (number == 1)
    pushPrompt(unitprompt);
  if (number > 1 && number < 5)
    pushPrompt(unitprompt+1);
  if (number > 4)
    pushPrompt(unitprompt+2);
}

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
      if (qr.rem) {
        playNumber(qr.quot);
        pushUnitPrompt(qr.quot,PROMPT_CELA);
        if (mode != 1 && qr.rem < 10)
          pushPrompt (PROMPT_NULA);
        playNumber(qr.rem,0,ZENSKY);
        pushPrompt(PROMPT_UNITS_BASE+((unit-1)*4)+3);
      }
      else
        playNumber(qr.quot, unit);
    return;
  }

  int16_t tmp = number;
  
  switch(unit) {
    case 0:
      break;
    case 4:
      att = ZENSKY;
    case 8:
      att = STREDNI;
    case 10:
    case 11:
    case 12:
      att = ZENSKY;
    default:
      att = MUZSKY;
  }
  
  if (number < 0) {
    pushPrompt(PROMPT_MINUS);
    number = -number;
  }
    
  if ((number == 1) && (att == MUZSKY)) {
    pushPrompt(PROMPT_JEDEN);
    number = -1;
  }
  
  if ((number == 1) && (att == STREDNI)) {
    pushPrompt(PROMPT_JEDNO);
    number = -1;
  }
  
  if ((number == 2) && ((att == ZENSKY) || (att == STREDNI))) {
    pushPrompt(PROMPT_DVE);
    number = -1;
  }
  
  if (number >= 1000) {
    if (number >= 2000) 
      playNumber(number / 1000);
    if (number >= 2000 && number < 5000)
      pushPrompt(PROMPT_TISICE);
    else
      pushPrompt(PROMPT_TISIC);
    number %= 1000;
    if (number == 0)
      number = -1;
  }
  if (number >= 100) {
    pushPrompt(PROMPT_STO + (number/100)-1);
    number %= 100;
    if (number == 0)
      number = -1;
  }
  
  if (number >= 0) {
    pushPrompt(PROMPT_NULA+number);
  }

  if (unit) {
    pushUnitPrompt(tmp,(PROMPT_UNITS_BASE+((unit-1)*4)));
  }
}

void playDuration(int16_t seconds)
{
  if (seconds < 0) {
    pushPrompt(PROMPT_MINUS);
    seconds = -seconds;
  }

  uint8_t tmp = seconds / 3600;
  seconds %= 3600;
  if (tmp > 0) {
    playNumber(tmp,0,ZENSKY);
    pushUnitPrompt(tmp,PROMPT_HODINA);
  }

  tmp = seconds / 60;
  seconds %= 60;
  if (tmp > 0) {
    playNumber(tmp,0,ZENSKY);
    pushUnitPrompt(tmp,PROMPT_MINUTA);
  }

  if (seconds > 0) {
    playNumber(seconds,0,ZENSKY);
    pushUnitPrompt(seconds,PROMPT_SEKUNDA);
  }
}

#endif
