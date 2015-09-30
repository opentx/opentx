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

#include "../../opentx.h"

#define XPOT_DELTA 10
#define XPOT_DELAY 10 /* cycles */

void menuCommonCalib(uint8_t event)
{
  for (uint8_t i=0; i<NUM_STICKS+NUM_POTS; i++) { // get low and high vals for sticks and trims
    int16_t vt = anaIn(i);
    reusableBuffer.calib.loVals[i] = min(vt, reusableBuffer.calib.loVals[i]);
    reusableBuffer.calib.hiVals[i] = max(vt, reusableBuffer.calib.hiVals[i]);
    if (i >= POT1 && i <= POT_LAST) {
      if (IS_POT_WITHOUT_DETENT(i)) {
        reusableBuffer.calib.midVals[i] = (reusableBuffer.calib.hiVals[i] + reusableBuffer.calib.loVals[i]) / 2;
      }
    }
  }

  calibrationState = reusableBuffer.calib.state; // make sure we don't scroll while calibrating

  switch (event)
  {
    case EVT_ENTRY:
      reusableBuffer.calib.state = 0;
      break;

    case EVT_KEY_BREAK(KEY_ENTER):
      reusableBuffer.calib.state++;
      break;
  }

  switch (reusableBuffer.calib.state) {
    case 0:
      // START CALIBRATION
      if (!READ_ONLY()) {
        lcd_putsLeft(MENU_HEADER_HEIGHT+2*FH, STR_MENUTOSTART);
      }
      break;

    case 1:
      // SET MIDPOINT
      lcd_putsAtt(0*FW, MENU_HEADER_HEIGHT+FH, STR_SETMIDPOINT, INVERS);
      lcd_putsLeft(MENU_HEADER_HEIGHT+2*FH, STR_MENUWHENDONE);

      for (uint8_t i=0; i<NUM_STICKS+NUM_POTS; i++) {
        reusableBuffer.calib.loVals[i] = 15000;
        reusableBuffer.calib.hiVals[i] = -15000;
        reusableBuffer.calib.midVals[i] = anaIn(i);
      }
      break;

    case 2:
      // MOVE STICKS/POTS
      STICK_SCROLL_DISABLE();
      lcd_putsAtt(0*FW, MENU_HEADER_HEIGHT+FH, STR_MOVESTICKSPOTS, INVERS);
      lcd_putsLeft(MENU_HEADER_HEIGHT+2*FH, STR_MENUWHENDONE);

      for (uint8_t i=0; i<NUM_STICKS+NUM_POTS; i++) {
        if (abs(reusableBuffer.calib.loVals[i]-reusableBuffer.calib.hiVals[i]) > 50) {
          g_eeGeneral.calib[i].mid = reusableBuffer.calib.midVals[i];
          int16_t v = reusableBuffer.calib.midVals[i] - reusableBuffer.calib.loVals[i];
          g_eeGeneral.calib[i].spanNeg = v - v/STICK_TOLERANCE;
          v = reusableBuffer.calib.hiVals[i] - reusableBuffer.calib.midVals[i];
          g_eeGeneral.calib[i].spanPos = v - v/STICK_TOLERANCE;
        }
      }
      break;

    case 3:
      g_eeGeneral.chkSum = evalChkSum();
      eeDirty(EE_GENERAL);
      reusableBuffer.calib.state = 4;
      break;

    default:
      reusableBuffer.calib.state = 0;
      break;
  }

  doMainScreenGraphics();
}

void menuGeneralCalib(uint8_t event)
{
  check_simple(event, e_Calib, menuTabGeneral, DIM(menuTabGeneral), 0);

  if (menuEvent) {
    calibrationState = 0;
  }

  TITLE(STR_MENUCALIBRATION);
  menuCommonCalib(READ_ONLY() ? 0 : event);
}

void menuFirstCalib(uint8_t event)
{
  if (event == EVT_KEY_BREAK(KEY_EXIT) || reusableBuffer.calib.state == 4) {
    calibrationState = 0;
    chainMenu(menuMainView);
  }
  else {
    lcd_putsCenter(0*FH, MENUCALIBRATION);
    lcd_invert_line(0);
    menuCommonCalib(event);
  }
}
