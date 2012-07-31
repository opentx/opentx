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
  PROMPT_NULA = PROMPT_NUMBERS_BASE+0,
  /* ... */
  PROMPT_DVACET = PROMPT_NUMBERS_BASE+20,
  PROMPT_TRICET = PROMPT_NUMBERS_BASE+21,
  /* ... */
  PROMPT_DEVADESAT = PROMPT_NUMBERS_BASE+27,
  PROMPT_JEDEN = PROMPT_NUMBERS_BASE+28,
  PROMPT_JEDNO = PROMPT_NUMBERS_BASE+29,
  PROMPT_DVE = PROMPT_NUMBERS_BASE+30,
  PROMPT_STO = PROMPT_NUMBERS_BASE+31,
  PROMPT_STA = PROMPT_NUMBERS_BASE+32,
  PROMPT_SET = PROMPT_NUMBERS_BASE+33,
  PROMPT_TISIC = PROMPT_NUMBERS_BASE+34,
  PROMPT_TISICE = PROMPT_NUMBERS_BASE+35,
  PROMPT_CELA = PROMPT_NUMBERS_BASE+36,
  PROMPT_CELE = PROMPT_NUMBERS_BASE+37,
  PROMPT_CELYCH = PROMPT_NUMBERS_BASE+38,
  PROMPT_MINUS = 39,

  PROMPT_HODINA = 41,
  PROMPT_HODINY = 42,
  PROMPT_HODIN = 43,
  PROMPT_MINUTA = 44,
  PROMPT_MINUTY = 45,
  PROMPT_MINUT = 46,
  PROMPT_SEKUNDA = 47,
  PROMPT_SEKUNDY = 48,
  PROMPT_SEKUND = 49,

  PROMPT_UNITS_BASE = 50,
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
  
  PROMPT_LABELS_BASE = 110,
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
    playNumber(qr.quot);
    pushPrompt(PROMPT_CELA);
    playNumber(qr.rem, unit);
    return;
  }

  int16_t tmp = number;
  
  //TODO: unit reconcile to MUZSKY,ZENSKY,STREDNI -> att

  if (number < 0) {
    pushPrompt(PROMPT_MINUS);
    number = -number;
  }
    
  if ((number == 1) && (att == MUZSKY)) {
    pushPrompt(PROMPT_JEDEN);
    return;
  }
  
  if ((number == 1) && (att == STREDNI)) {
    pushPrompt(PROMPT_JEDNO);
    return;
  }
  
  if ((number == 2) && ((att == ZENSKY) || (att == STREDNI))) {
    pushPrompt(PROMPT_DVE);
    return;
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
    if (number/100 != 1)
      playNumber(number/100,0,STREDNI);
    pushUnitPrompt(number/100,PROMPT_STO);
  }
  number %= 100;
  if (number == 0)
    number = -1;
  
  if (number >= 20) {
    pushPrompt(PROMPT_DVACET + (number-20)/10);
    number %= 10;
    if (number == 0)
      number = -1;
  }
  if (number >= 0) {
    pushPrompt(PROMPT_NULA+number);
  }

  if (unit) {
    pushUnitPrompt(tmp,(PROMPT_UNITS_BASE+(unit-1)*4));
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
