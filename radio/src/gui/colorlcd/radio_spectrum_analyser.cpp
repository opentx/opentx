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

#define SPECTRUM_HEIGHT  200

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

      coord_t peak_y = 1;
      coord_t peak_x = 0;
      for (coord_t i = 0; i < LCD_W; i++) {
        coord_t h = min<coord_t >(reusableBuffer.spectrumAnalyser.bars[i], SPECTRUM_HEIGHT);
        if (h > peak_y) {
          peak_x = i;
          peak_y = h;
        }
        dc->drawVerticalLine(i, SPECTRUM_HEIGHT - h, h, SOLID, 0);
      }

      coord_t y = max<coord_t>(FH, SPECTRUM_HEIGHT - peak_y - FH);
      drawNumber(dc, min<coord_t>(100, peak_x), y, ((reusableBuffer.spectrumAnalyser.freq - reusableBuffer.spectrumAnalyser.span / 2) + peak_x * (reusableBuffer.spectrumAnalyser.span / 128)) / 1000000, TINSIZE);
      dc->drawText(lcdNextPos, y, "M", TINSIZE);
    }

  protected:

};

RadioSpectrumAnalyser::RadioSpectrumAnalyser(uint8_t moduleIdx) :
  Page(ICON_RADIO_TOOLS),
  moduleIdx(moduleIdx)
{
  buildBody(&body);

  start();
}

void RadioSpectrumAnalyser::buildBody(FormWindow * window)
{
  new SpectrumWindow(window, {0, 0, LCD_W, SPECTRUM_HEIGHT});
}

void RadioSpectrumAnalyser::start()
{
  if (isModuleR9MAccess(moduleIdx)) {
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

  if (moduleState[moduleIdx].mode != MODULE_MODE_SPECTRUM_ANALYSER) {
    memclear(reusableBuffer.spectrumAnalyser.bars, sizeof(reusableBuffer.spectrumAnalyser.bars));
    reusableBuffer.spectrumAnalyser.span = reusableBuffer.spectrumAnalyser.spanDefault * 1000000;
    reusableBuffer.spectrumAnalyser.freq = reusableBuffer.spectrumAnalyser.freqDefault * 1000000;
    reusableBuffer.spectrumAnalyser.step = reusableBuffer.spectrumAnalyser.span / LCD_W;
    reusableBuffer.spectrumAnalyser.dirty = true;
    moduleState[moduleIdx].mode = MODULE_MODE_SPECTRUM_ANALYSER;
  }
}

void RadioSpectrumAnalyser::stop()
{
  new MessageDialog(STR_MODULE, STR_STOPPING);
  moduleState[moduleIdx].readModuleInformation(&reusableBuffer.moduleSetup.pxx2.moduleInformation, PXX2_HW_INFO_TX_ID, PXX2_HW_INFO_TX_ID);
  /* wait 1s to resume normal operation before leaving */
  //  watchdogSuspend(1000);
  //  RTOS_WAIT_MS(1000);
}
