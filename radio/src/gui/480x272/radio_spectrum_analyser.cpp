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

enum SpectrumFields
{
  SPECTRUM_FREQUENCY,
  SPECTRUM_SPAN,
  SPECTRUM_TRACK,
  SPECTRUM_FIELDS_MAX
};

coord_t getAverage(uint8_t number, const uint8_t * value)
{
  uint16_t sum = 0;
  for (uint8_t i = 0; i < number; i++) {
    sum += value[i];
  }
  return sum / number;
}

#if defined(INTERNAL_MODULE_MULTI)
  #define SPECTRUM_ROW  (g_moduleIdx == INTERNAL_MODULE ? READONLY_ROW : isModuleMultimodule(g_moduleIdx) ? READONLY_ROW : (uint8_t)0)
#else
  #define SPECTRUM_ROW  (isModuleMultimodule(g_moduleIdx) ? READONLY_ROW : (uint8_t)0)
#endif

bool menuRadioSpectrumAnalyser(event_t event)
{
  SUBMENU(STR_MENU_SPECTRUM_ANALYSER, ICON_RADIO_SPECTRUM_ANALYSER, SPECTRUM_FIELDS_MAX, {
    SPECTRUM_ROW,  //Freq
    SPECTRUM_ROW,  //Span
    0              //Track
  });

  if (menuEvent) {
    lcdDrawCenteredText(LCD_H / 2, STR_STOPPING);
    lcdRefresh();
    if (isModulePXX2(g_moduleIdx)) {
      moduleState[g_moduleIdx].readModuleInformation(&reusableBuffer.moduleSetup.pxx2.moduleInformation, PXX2_HW_INFO_TX_ID, PXX2_HW_INFO_TX_ID);
      resetAccessAuthenticationCount();
    }
    else if (isModuleMultimodule(g_moduleIdx)) {
      if (reusableBuffer.spectrumAnalyser.moduleOFF)
        setModuleType(INTERNAL_MODULE, MODULE_TYPE_NONE);
      else
        moduleState[g_moduleIdx].mode = MODULE_MODE_NORMAL;
    }
    /* wait 1s to resume normal operation before leaving */
    watchdogSuspend(500 /*5s*/);
    RTOS_WAIT_MS(1000);
    return false;
  }

  if (moduleState[g_moduleIdx].mode != MODULE_MODE_SPECTRUM_ANALYSER) {
    if (TELEMETRY_STREAMING()) {
      POPUP_WARNING(STR_TURN_OFF_RECEIVER);
      if (event == EVT_KEY_FIRST(KEY_EXIT)) {
        killEvents(event);
        popMenu();
      }
      return false;
    }
    memclear(&reusableBuffer.spectrumAnalyser, sizeof(reusableBuffer.spectrumAnalyser));

#if defined(INTERNAL_MODULE_MULTI)
    if (g_moduleIdx == INTERNAL_MODULE && g_model.moduleData[INTERNAL_MODULE].type == MODULE_TYPE_NONE) {
      reusableBuffer.spectrumAnalyser.moduleOFF = true;
      setModuleType(INTERNAL_MODULE, MODULE_TYPE_MULTIMODULE);
    }
#endif

    if (isModuleR9MAccess(g_moduleIdx)) {
      reusableBuffer.spectrumAnalyser.spanDefault = 20;
      reusableBuffer.spectrumAnalyser.spanMax = 40;
      reusableBuffer.spectrumAnalyser.freqDefault = 890;
      reusableBuffer.spectrumAnalyser.freqMin = 850;
      reusableBuffer.spectrumAnalyser.freqMax = 930;
    }
    else {
      if (isModuleMultimodule(g_moduleIdx))
        reusableBuffer.spectrumAnalyser.spanDefault = 80;  // 80MHz
      else
        reusableBuffer.spectrumAnalyser.spanDefault = 40;  // 40MHz
      reusableBuffer.spectrumAnalyser.spanMax = 80;
      reusableBuffer.spectrumAnalyser.freqDefault = 2440; // 2440MHz
      reusableBuffer.spectrumAnalyser.freqMin = 2400;
      reusableBuffer.spectrumAnalyser.freqMax = 2485;
    }

    reusableBuffer.spectrumAnalyser.span = reusableBuffer.spectrumAnalyser.spanDefault * 1000000;
    reusableBuffer.spectrumAnalyser.freq = reusableBuffer.spectrumAnalyser.freqDefault * 1000000;
    reusableBuffer.spectrumAnalyser.track = reusableBuffer.spectrumAnalyser.freq;
    reusableBuffer.spectrumAnalyser.step = reusableBuffer.spectrumAnalyser.span / LCD_W;
    reusableBuffer.spectrumAnalyser.dirty = true;
    moduleState[g_moduleIdx].mode = MODULE_MODE_SPECTRUM_ANALYSER;
  }

  for (uint8_t i = 0; i < SPECTRUM_FIELDS_MAX; i++) {
    LcdFlags attr = (menuVerticalPosition == i ? (s_editMode > 0 ? INVERS | BLINK : INVERS) : 0);

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

      case SPECTRUM_SPAN: {
        uint8_t span = reusableBuffer.spectrumAnalyser.span / 1000000;
        lcdDrawText(MENUS_MARGIN_LEFT + 100, MENU_FOOTER_TOP, "S:", TEXT_INVERTED_COLOR);
        lcdDrawNumber(lcdNextPos + 2, MENU_FOOTER_TOP, reusableBuffer.spectrumAnalyser.span / 1000000, attr | TEXT_INVERTED_COLOR);
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

      case SPECTRUM_TRACK: {
        uint16_t track = reusableBuffer.spectrumAnalyser.track / 1000000;
        lcdDrawText(lcdNextPos + 10, MENU_FOOTER_TOP, "T:", TEXT_INVERTED_COLOR);
        lcdDrawNumber(lcdNextPos + 2, MENU_FOOTER_TOP, reusableBuffer.spectrumAnalyser.track / 1000000, attr | TEXT_INVERTED_COLOR);
        lcdDrawText(lcdNextPos + 2, MENU_FOOTER_TOP, "MHz", TEXT_INVERTED_COLOR);
        if (attr) {
          reusableBuffer.spectrumAnalyser.track = uint32_t(
            checkIncDec(event, track, (reusableBuffer.spectrumAnalyser.freq - reusableBuffer.spectrumAnalyser.span / 2) / 1000000,
                        (reusableBuffer.spectrumAnalyser.freq + reusableBuffer.spectrumAnalyser.span / 2) / 1000000, 0)) * 1000000;
          if (checkIncDec_Ret) {
            reusableBuffer.spectrumAnalyser.dirty = true;
          }
        }
        break;
      }
    }
  }

  constexpr coord_t SCALE_HEIGHT = 12;
  constexpr coord_t SCALE_TOP = MENU_FOOTER_TOP - SCALE_HEIGHT;
  constexpr coord_t BARGRAPH_HEIGHT = SCALE_TOP - MENU_HEADER_HEIGHT;

  // Draw fixed part (scale,..)
  lcdDrawSolidFilledRect(0, SCALE_TOP, LCD_W, SCALE_HEIGHT, CURVE_AXIS_COLOR);
  for (uint32_t frequency = ((reusableBuffer.spectrumAnalyser.freq - reusableBuffer.spectrumAnalyser.span / 2) / 10000000) * 10000000 + 10000000; ; frequency += 10000000) {
    int offset = frequency - (reusableBuffer.spectrumAnalyser.freq - reusableBuffer.spectrumAnalyser.span / 2);
    int x = offset / reusableBuffer.spectrumAnalyser.step;
    if (x >= LCD_W - 1)
      break;
    lcdDrawVerticalLine(x, MENU_HEADER_HEIGHT, LCD_H - MENU_HEADER_HEIGHT - MENU_FOOTER_HEIGHT, STASHED, CURVE_AXIS_COLOR);

    if ((frequency / 1000000) % 2 == 0) {
      lcdDrawNumber(x, SCALE_TOP - 1, frequency / 1000000, TINSIZE | TEXT_COLOR | CENTERED);
    }
  }

  for (uint8_t power = 20;; power += 20) {
    int y = MENU_FOOTER_TOP - 1 - limit<int>(0, power << 1, LCD_H - MENU_HEADER_HEIGHT - MENU_FOOTER_HEIGHT);
    if (y <= MENU_HEADER_HEIGHT)
      break;
    lcdDrawHorizontalLine(0, y, LCD_W, STASHED, CURVE_AXIS_COLOR);
  }

  // Draw tracker
  int offset = reusableBuffer.spectrumAnalyser.track - (reusableBuffer.spectrumAnalyser.freq - reusableBuffer.spectrumAnalyser.span / 2);
  int x = limit<int>(0, offset / reusableBuffer.spectrumAnalyser.step, LCD_W - 1);
  lcdDrawSolidVerticalLine(x, MENU_HEADER_HEIGHT, BARGRAPH_HEIGHT, TEXT_COLOR);

  // Draw spectrum data
  constexpr uint8_t step = 4;

  for (coord_t xv = 0; xv < LCD_W; xv += step) {
    coord_t yv = SCALE_TOP - 1 - limit<int>(0, getAverage(step, &reusableBuffer.spectrumAnalyser.bars[xv]) << 1, BARGRAPH_HEIGHT);
    coord_t max_yv = SCALE_TOP - 1 - limit<int>(0, getAverage(step, &reusableBuffer.spectrumAnalyser.max[xv]) << 1, BARGRAPH_HEIGHT);

    // Signal bar
    lcdDrawSolidFilledRect(xv, yv, step - 1, SCALE_TOP - yv, TEXT_INVERTED_BGCOLOR);
    // lcdDrawSolidRect(xv, yv, step - 1, SCALE_TOP - yv, 1, TEXT_COLOR);

    // Signal max
    lcdDrawSolidHorizontalLine(xv, max_yv, step - 1, TEXT_COLOR);

    // Decay max values
    if (max_yv < yv) { // Those value are INVERTED (MENU_FOOTER_TOP - value)
      for (uint8_t i = 0; i < step; i++) {
        reusableBuffer.spectrumAnalyser.max[xv + i] = max<int>(0, reusableBuffer.spectrumAnalyser.max[xv + i] - 1);
      }
    }
  }

  return true;
}