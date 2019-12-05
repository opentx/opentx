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

#include "radio_spectrum_analyser.h"
#include "opentx.h"

extern uint8_t g_moduleIdx;
#define SPECTRUM_HEIGHT  200


coord_t getAverage(uint8_t number, const uint8_t * value)
{
  uint16_t sum = 0;
  for (uint8_t i = 0; i < number; i++) {
    sum += value[i];
  }
  return sum / number;
}


class SpectrumWindow : public Window
{
  public:
    SpectrumWindow(Window *parent, const rect_t &rect) :
      Window(parent, rect, REFRESH_ALWAYS)
    {
    }

    void paint(BitmapBuffer * dc) override
    {
      if (TELEMETRY_STREAMING()) {
        dc->drawText(width() / 2, height() / 2, STR_TURN_OFF_RECEIVER, CENTERED);
        return;
      }

#if defined(SIMU)
      // Geneate random data for simu
      for(coord_t x= 0; x < width(); x++) {
        uint8_t power = rand() % 80;
        reusableBuffer.spectrumAnalyser.bars[x] = power;
        reusableBuffer.spectrumAnalyser.bars[x+1] = power;
        if (power > reusableBuffer.spectrumAnalyser.max[x]) {
          reusableBuffer.spectrumAnalyser.max[x] = power;
          reusableBuffer.spectrumAnalyser.max[x+1] = power;
        }
      }
#endif

      constexpr coord_t SCALE_HEIGHT = 12;
      coord_t SCALE_TOP = height() - SCALE_HEIGHT - MENU_FOOTER_HEIGHT;
      coord_t BARGRAPH_HEIGHT = height() - SCALE_HEIGHT;

      // Draw fixed part (scale,..)
      dc->drawSolidFilledRect(0, SCALE_TOP, width(), SCALE_HEIGHT, CURVE_AXIS_COLOR);
      for (uint32_t frequency = ((reusableBuffer.spectrumAnalyser.freq - reusableBuffer.spectrumAnalyser.span / 2) / 10000000) * 10000000 + 10000000; ; frequency += 10000000) {
        int offset = frequency - (reusableBuffer.spectrumAnalyser.freq - reusableBuffer.spectrumAnalyser.span / 2);
        int x = offset / reusableBuffer.spectrumAnalyser.step;
        if (x >= LCD_W - 1)
          break;
        dc->drawVerticalLine(x, 0, height(), STASHED, CURVE_AXIS_COLOR);

        if ((frequency / 1000000) % 2 == 0) {
          dc->drawNumber(x, SCALE_TOP - 2, frequency / 1000000, FONT(XS) | CENTERED);
        }
      }

      for (uint8_t power = 20;; power += 20) {
        int y = SCALE_TOP - 1 - limit<int>(0, power << 1, SCALE_TOP);
        if (y <= 0)
          break;
        dc->drawHorizontalLine(0, y, width(), STASHED, CURVE_AXIS_COLOR);
      }

      // Draw spectrum data
      constexpr uint8_t step = 4;

      for (coord_t xv = 0; xv < width(); xv += step) {
        coord_t yv = SCALE_TOP - 1 - limit<int>(0, getAverage(step, &reusableBuffer.spectrumAnalyser.bars[xv]) << 1, SCALE_TOP);
        coord_t max_yv = SCALE_TOP - 1 - limit<int>(0, getAverage(step, &reusableBuffer.spectrumAnalyser.max[xv]) << 1, SCALE_TOP);

        // Signal bar
        dc->drawSolidFilledRect(xv, yv, step - 1, SCALE_TOP - yv, YELLOW);
        // lcdDrawSolidRect(xv, yv, step - 1, SCALE_TOP - yv, 1, TEXT_COLOR);

        // Signal max
        dc->drawSolidHorizontalLine(xv, max_yv, step - 1, BLACK);

        // Decay max values
        if (max_yv < yv) { // Those value are INVERTED (MENU_FOOTER_TOP - value)
          for (uint8_t i = 0; i < step; i++) {
            reusableBuffer.spectrumAnalyser.max[xv + i] = max<int>(0, reusableBuffer.spectrumAnalyser.max[xv + i] - 1);
          }
        }
      }
    }

  protected:

};

RadioSpectrumAnalyser::RadioSpectrumAnalyser(uint8_t moduleIdx) :
  Page(ICON_RADIO_TOOLS),
  moduleIdx(moduleIdx)
{
  buildBody(&body);
  setFocus();
  start();
}

void RadioSpectrumAnalyser::buildBody(FormWindow * window)
{
  new SpectrumWindow(window, {0, 0, LCD_W, SPECTRUM_HEIGHT});
}

void RadioSpectrumAnalyser::start()
{
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


void RadioSpectrumAnalyser::stop()
{
  new MessageDialog(STR_MODULE, STR_STOPPING);
  moduleState[moduleIdx].readModuleInformation(&reusableBuffer.moduleSetup.pxx2.moduleInformation, PXX2_HW_INFO_TX_ID, PXX2_HW_INFO_TX_ID);
  /* wait 1s to resume normal operation before leaving */
  //  watchdogSuspend(1000);
  //  RTOS_WAIT_MS(1000);
}
