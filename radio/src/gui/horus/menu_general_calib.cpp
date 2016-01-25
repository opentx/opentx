/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "../../opentx.h"

#define XPOT_DELTA                     10
#define XPOT_DELAY                     10 /* cycles */
#define POT_BAR_INTERVAL               20
#define POT_BAR_BOTTOM                 200
#define STICKS_WIDTH                   90
#define STICKS_Y                       110
#define STICK_LEFT_X                   25
#define STICK_RIGHT_X                  (LCD_W-STICK_LEFT_X-STICKS_WIDTH)

void drawSticks()
{
  int16_t calibStickVert = calibratedStick[CONVERT_MODE(1)];
  if (g_model.throttleReversed && CONVERT_MODE(1) == THR_STICK)
    calibStickVert = -calibStickVert;
  drawStick(STICK_LEFT_X, STICKS_Y, calibratedStick[CONVERT_MODE(0)], calibStickVert);

  calibStickVert = calibratedStick[CONVERT_MODE(2)];
  if (g_model.throttleReversed && CONVERT_MODE(2) == THR_STICK)
    calibStickVert = -calibStickVert;
  drawStick(STICK_RIGHT_X, STICKS_Y, calibratedStick[CONVERT_MODE(3)], calibStickVert);
}

void drawPots()
{
  // The pots which are displayed in the main view
  extern void drawMainPots();
  drawMainPots();

  // The 2 main front sliders
  drawVerticalSlider(125, 120, 120, calibratedStick[7], -RESX, RESX, 40, OPTION_SLIDER_TICKS | OPTION_SLIDER_BIG_TICKS |
                                                                       OPTION_SLIDER_SQUARE_BUTTON);
  drawVerticalSlider(LCD_W-125-12, 120, 120, calibratedStick[8], -RESX, RESX, 40, OPTION_SLIDER_TICKS | OPTION_SLIDER_BIG_TICKS |
                                                                                OPTION_SLIDER_SQUARE_BUTTON);
}

bool menuCommonCalib(evt_t event)
{
  drawScreenTemplate(NULL, OPTION_MENU_NO_FOOTER);

  for (uint8_t i=0; i<NUM_STICKS+NUM_POTS; i++) { // get low and high vals for sticks and trims
    int16_t vt = anaIn(i);
    reusableBuffer.calib.loVals[i] = min(vt, reusableBuffer.calib.loVals[i]);
    reusableBuffer.calib.hiVals[i] = max(vt, reusableBuffer.calib.hiVals[i]);
    if (i >= POT1 && i <= POT_LAST) {
      if (IS_POT_WITHOUT_DETENT(i)) {
        reusableBuffer.calib.midVals[i] = (reusableBuffer.calib.hiVals[i] + reusableBuffer.calib.loVals[i]) / 2;
      }
      uint8_t idx = i - POT1;
      int count = reusableBuffer.calib.xpotsCalib[idx].stepsCount;
      if (IS_POT_MULTIPOS(i) && count <= XPOTS_MULTIPOS_COUNT) {
        if (reusableBuffer.calib.xpotsCalib[idx].lastCount == 0 || vt < reusableBuffer.calib.xpotsCalib[idx].lastPosition - XPOT_DELTA || vt > reusableBuffer.calib.xpotsCalib[idx].lastPosition + XPOT_DELTA) {
          reusableBuffer.calib.xpotsCalib[idx].lastPosition = vt;
          reusableBuffer.calib.xpotsCalib[idx].lastCount = 1;
        }
        else {
          if (reusableBuffer.calib.xpotsCalib[idx].lastCount < 255) reusableBuffer.calib.xpotsCalib[idx].lastCount++;
        }
        if (reusableBuffer.calib.xpotsCalib[idx].lastCount == XPOT_DELAY) {
          int16_t position = reusableBuffer.calib.xpotsCalib[idx].lastPosition;
          bool found = false;
          for (int j=0; j<count; j++) {
            int16_t step = reusableBuffer.calib.xpotsCalib[idx].steps[j];
            if (position >= step-XPOT_DELTA && position <= step+XPOT_DELTA) {
              found = true;
              break;
            }
          }
          if (!found) {
            if (count < XPOTS_MULTIPOS_COUNT) {
              reusableBuffer.calib.xpotsCalib[idx].steps[count] = position;
            }
            reusableBuffer.calib.xpotsCalib[idx].stepsCount += 1;
          }
        }
      }
    }
  }

  switch (event) {
    case EVT_ENTRY:
    case EVT_KEY_BREAK(KEY_EXIT):
      calibrationState = 0;
      break;

    case EVT_KEY_BREAK(KEY_ENTER):
      calibrationState++;
      break;
  }

  switch (calibrationState) {
    case 0:
      // START CALIBRATION
      if (!READ_ONLY()) {
        lcdDrawText(50, 3, STR_MENUCALIBRATION, MENU_TITLE_COLOR);
        lcdDrawText(50, 3+FH, "Press [Enter] to start", MENU_TITLE_COLOR);
      }
      break;

    case 1:
      // SET MIDPOINT
      lcdDrawText(50, 3, STR_MENUCALIBRATION, MENU_TITLE_COLOR);
      lcdDrawText(50, 3+FH, "Please center sticks and press [Enter]", MENU_TITLE_COLOR);
      for (int i=0; i<NUM_STICKS+NUM_POTS; i++) {
        reusableBuffer.calib.loVals[i] = 15000;
        reusableBuffer.calib.hiVals[i] = -15000;
        reusableBuffer.calib.midVals[i] = anaIn(i);
        if (i<NUM_XPOTS) {
          reusableBuffer.calib.xpotsCalib[i].stepsCount = 0;
          reusableBuffer.calib.xpotsCalib[i].lastCount = 0;
        }
      }
      break;

    case 2:
      // MOVE STICKS/POTS
      lcdDrawText(50, 3, STR_MENUCALIBRATION, MENU_TITLE_COLOR);
      lcdDrawText(50, 3+FH, "Move sticks, pots and sliders and press [Enter]", MENU_TITLE_COLOR);
      for (int i=0; i<NUM_STICKS+NUM_POTS; i++) {
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
      for (int i=POT1; i<=POT_LAST; i++) {
        int idx = i - POT1;
        int count = reusableBuffer.calib.xpotsCalib[idx].stepsCount;
        if (IS_POT_MULTIPOS(i)) {
          if (count > 1 && count <= XPOTS_MULTIPOS_COUNT) {
            for (int j=0; j<count; j++) {
              for (int k=j+1; k<count; k++) {
                if (reusableBuffer.calib.xpotsCalib[idx].steps[k] < reusableBuffer.calib.xpotsCalib[idx].steps[j]) {
                  SWAP(reusableBuffer.calib.xpotsCalib[idx].steps[j], reusableBuffer.calib.xpotsCalib[idx].steps[k]);
                }
              }
            }
            StepsCalibData * calib = (StepsCalibData *) &g_eeGeneral.calib[i];
            calib->count = count - 1;
            for (int j=0; j<calib->count; j++) {
              calib->steps[j] = (reusableBuffer.calib.xpotsCalib[idx].steps[j+1] + reusableBuffer.calib.xpotsCalib[idx].steps[j]) >> 5;
            }
          }
          else {
            // g_eeGeneral.potsConfig &= ~(0x03<<(2*idx));
          }
        }
      }
      g_eeGeneral.chkSum = evalChkSum();
      storageDirty(EE_GENERAL);
      calibrationState = 4;
      break;

    default:
      calibrationState = 0;
      break;
  }

  lcdDrawBitmap((LCD_W-206)/2, LCD_H-220, LBM_HORUS);
  drawSticks();
  drawPots();

  for (int i=POT1; i<=POT_LAST; i++) {
    uint8_t steps = 0;
    if (calibrationState == 2) {
      steps = reusableBuffer.calib.xpotsCalib[i-POT1].stepsCount;
    }
    else if (IS_POT_MULTIPOS(i)) {
      StepsCalibData * calib = (StepsCalibData *) &g_eeGeneral.calib[i];
      steps = calib->count + 1;
    }
    if (calibrationState != 0 && steps > 0 && steps <= XPOTS_MULTIPOS_COUNT) {
      lcdDrawNumber(LCD_W/2+3-(POT_BAR_INTERVAL*NUM_POTS/2)+(POT_BAR_INTERVAL*(i-POT1)), POT_BAR_BOTTOM+15, steps, TEXT_COLOR|TINSIZE, 0, "[", "]");
    }
  }

  return true;
}

bool menuGeneralCalib(evt_t event)
{
  if (event == EVT_ENTRY || event == EVT_ENTRY_UP) TRACE("Menu %s displayed ...", STR_MENUCALIBRATION);
  if (calibrationState == 4) {
    calibrationState = 0;
    popMenu();
    return false;
  }
  if (!check_submenu_simple(event, 0)) {
    return false;
  }

  menuVerticalPosition = -1;

  return menuCommonCalib(READ_ONLY() ? 0 : event);
}

bool menuFirstCalib(evt_t event)
{
  if (event == EVT_KEY_BREAK(KEY_EXIT) || calibrationState == 4) {
    calibrationState = 0;
    chainMenu(menuMainView);
    return false;
  }
  else {
    return menuCommonCalib(event);
  }
}
