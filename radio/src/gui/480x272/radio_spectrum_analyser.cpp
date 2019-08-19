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

#include "opentx.h"

extern uint8_t g_moduleIdx;

enum SpectrumFields {
  SPECTRUM_FREQUENCY,
  SPECTRUM_SPAN,
  SPECTRUM_FIELDS_MAX
};

bool menuRadioSpectrumAnalyser(event_t event)
{
  SUBMENU(STR_MENU_SPECTRUM_ANALYSER, ICON_RADIO, 1, {1});

  if (menuEvent) {
    lcdDrawCenteredText(LCD_H/2, STR_STOPPING);
    lcdRefresh();
    moduleState[g_moduleIdx].readModuleInformation(&reusableBuffer.moduleSetup.pxx2.moduleInformation, PXX2_HW_INFO_TX_ID, PXX2_HW_INFO_TX_ID);
    /* wait 1s to resume normal operation before leaving */
    watchdogSuspend(1000);
    RTOS_WAIT_MS(1000);
    return false;
  }

  if (moduleState[g_moduleIdx].mode != MODULE_MODE_SPECTRUM_ANALYSER) {
    if (TELEMETRY_STREAMING()) {
      lcdDrawCenteredText(LCD_H/2, STR_TURN_OFF_RECEIVER);
      if (event == EVT_KEY_FIRST(KEY_EXIT)) {
        killEvents(event);
        popMenu();
      }
      return false;
    }

    memclear(&reusableBuffer.spectrumAnalyser, sizeof(reusableBuffer.spectrumAnalyser));

    if (isModuleR9MAccess(g_moduleIdx)) {
      reusableBuffer.spectrumAnalyser.spanDefault = 20;
      reusableBuffer.spectrumAnalyser.spanMax = 40;
      reusableBuffer.spectrumAnalyser.freqDefault = 890;
      reusableBuffer.spectrumAnalyser.freqMin = 850;
      reusableBuffer.spectrumAnalyser.freqMax = 930;
    }
    else {
      reusableBuffer.spectrumAnalyser.spanDefault = 40;  // 40MHz
      reusableBuffer.spectrumAnalyser.spanMax = 80;
      reusableBuffer.spectrumAnalyser.freqDefault = 2440; // 2440MHz
      reusableBuffer.spectrumAnalyser.freqMin = 2400;
      reusableBuffer.spectrumAnalyser.freqMax = 2485;
    }

    reusableBuffer.spectrumAnalyser.span = reusableBuffer.spectrumAnalyser.spanDefault * 1000000;
    reusableBuffer.spectrumAnalyser.freq = reusableBuffer.spectrumAnalyser.freqDefault * 1000000;
    reusableBuffer.spectrumAnalyser.step = reusableBuffer.spectrumAnalyser.span / LCD_W;
    reusableBuffer.spectrumAnalyser.dirty = true;
    moduleState[g_moduleIdx].mode = MODULE_MODE_SPECTRUM_ANALYSER;
  }

  for (uint8_t i=0; i<SPECTRUM_FIELDS_MAX; i++) {
    LcdFlags attr = (menuHorizontalPosition == i ? (s_editMode>0 ? INVERS|BLINK : INVERS) : 0);

    switch (i) {
      case SPECTRUM_FREQUENCY: {
        uint16_t frequency = reusableBuffer.spectrumAnalyser.freq / 1000000;
        lcdDrawText(MENUS_MARGIN_LEFT, MENU_FOOTER_TOP, "F:", TEXT_INVERTED_COLOR);
        lcdDrawNumber(lcdNextPos + 2, MENU_FOOTER_TOP, frequency, attr | TEXT_INVERTED_COLOR);
        lcdDrawText(lcdNextPos + 2, MENU_FOOTER_TOP, "MHz", TEXT_INVERTED_COLOR);
        if (attr) {
          reusableBuffer.spectrumAnalyser.freq = uint32_t(checkIncDec(event, frequency, reusableBuffer.spectrumAnalyser.freqMin, reusableBuffer.spectrumAnalyser.freqMax, 0)) * 1000000;
          if (checkIncDec_Ret) {
            reusableBuffer.spectrumAnalyser.dirty = true;
          }
        }
        break;
      }

      case SPECTRUM_SPAN:
        uint8_t span = reusableBuffer.spectrumAnalyser.span / 1000000;
        lcdDrawText(MENUS_MARGIN_LEFT + 100, MENU_FOOTER_TOP, "S:", TEXT_INVERTED_COLOR);
        lcdDrawNumber(lcdNextPos + 2, MENU_FOOTER_TOP, reusableBuffer.spectrumAnalyser.span/1000000, attr | TEXT_INVERTED_COLOR);
        lcdDrawText(lcdNextPos + 2, MENU_FOOTER_TOP, "MHz", TEXT_INVERTED_COLOR);
        if (attr) {
          reusableBuffer.spectrumAnalyser.span = checkIncDec(event, span, 1, reusableBuffer.spectrumAnalyser.spanMax, 0) * 1000000;
          if (checkIncDec_Ret) {
            reusableBuffer.spectrumAnalyser.step = reusableBuffer.spectrumAnalyser.span / LCD_W;
            reusableBuffer.spectrumAnalyser.dirty = true;
          }
        }
        break;
    }
  }

  for (uint32_t frequency = ((reusableBuffer.spectrumAnalyser.freq - reusableBuffer.spectrumAnalyser.span / 2) / 10000000) * 10000000; frequency < reusableBuffer.spectrumAnalyser.freq + reusableBuffer.spectrumAnalyser.span / 2; frequency += 10000000) {
    int32_t offset = frequency - (reusableBuffer.spectrumAnalyser.freq - reusableBuffer.spectrumAnalyser.span / 2);
    uint32_t x = offset / reusableBuffer.spectrumAnalyser.step;
    if (x > 0 && x < LCD_W - 1)
      lcdDrawVerticalLine(x, MENU_HEADER_HEIGHT, LCD_H - MENU_HEADER_HEIGHT - MENU_FOOTER_HEIGHT, STASHED, CURVE_AXIS_COLOR);
  }

  for (coord_t y = MENU_HEADER_HEIGHT + (MENU_FOOTER_TOP - MENU_HEADER_HEIGHT) / 8; y < MENU_FOOTER_TOP; y += (MENU_FOOTER_TOP - MENU_HEADER_HEIGHT) / 8) {
    lcdDrawHorizontalLine(0, y, LCD_W, STASHED, CURVE_AXIS_COLOR);
  }

  coord_t peak_y = LCD_H;
  coord_t peak_x = 0;

  coord_t prev_yv = (coord_t)-1;
  for (coord_t xv=0; xv<LCD_W; xv++) {
    coord_t yv = MENU_FOOTER_TOP - limit<int>(0, reusableBuffer.spectrumAnalyser.bars[xv] << 1, LCD_H - MENU_HEADER_HEIGHT - MENU_FOOTER_HEIGHT);
    if (prev_yv != (coord_t)-1) {
      if (yv < peak_y) {
        peak_x = xv;
        peak_y = yv;
      }
      if (prev_yv < yv) {
        for (int y=prev_yv; y<=yv; y+=1) {
          lcdDrawPoint(xv, y, TEXT_COLOR);
        }
      }
      else {
        for (int y=yv; y<=prev_yv; y+=1) {
          lcdDrawPoint(xv, y, TEXT_COLOR);
        }
      }
    }
    prev_yv = yv;
  }

  prev_yv = (coord_t)-1;
  for (coord_t xv=0; xv<LCD_W; xv++) {
    coord_t yv = MENU_FOOTER_TOP - limit<int>(0, reusableBuffer.spectrumAnalyser.max[xv] << 1, LCD_H - MENU_HEADER_HEIGHT - MENU_FOOTER_HEIGHT);
    if (prev_yv != (coord_t)-1) {
      if (prev_yv < yv) {
        for (int y=prev_yv; y<=yv; y+=1) {
          lcdDrawPoint(xv, y, TEXT_INVERTED_BGCOLOR);
        }
      }
      else {
        for (int y=yv; y<=prev_yv; y+=1) {
          lcdDrawPoint(xv, y, TEXT_INVERTED_BGCOLOR);
        }
      }
    }
    prev_yv = yv;
  }

  coord_t y = max<coord_t>(MENU_HEADER_HEIGHT + 1, peak_y - FH);
  lcdDrawNumber(limit<coord_t>(20, peak_x, LCD_W - 20), y, ((reusableBuffer.spectrumAnalyser.freq - reusableBuffer.spectrumAnalyser.span / 2) + peak_x * (reusableBuffer.spectrumAnalyser.span / LCD_W)) / 1000000, TINSIZE | CENTERED);
  lcdDrawText(lcdNextPos, y, "M", TINSIZE);

  return true;
}
