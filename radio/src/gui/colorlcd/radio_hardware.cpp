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

#include "radio_hardware.h"
#include "radio_calibration.h"
#include "opentx.h"

#define SET_DIRTY() storageDirty(EE_GENERAL)

#if defined(PCBHORUS)
#define SWITCH_TYPE_MAX(sw)            ((MIXSRC_SF-MIXSRC_FIRST_SWITCH == sw || MIXSRC_SH-MIXSRC_FIRST_SWITCH == sw) ? SWITCH_2POS : SWITCH_3POS)
#else
#define SWITCH_TYPE_MAX(sw)            (SWITCH_3POS)
#endif

class RTCBattValue: public StaticText
{
  public:
    RTCBattValue(Window * parent, const rect_t & rect):
      StaticText(parent, rect)
    {
    }

    void checkEvents() override
    {
      div_t qr = div(getRTCBatteryVoltage(), 100);
      char * tmp;
      tmp = strAppendUnsigned(reusableBuffer.hardwareAndSettings.msg, qr.quot);
      tmp = strAppend(tmp, ".", 1);
      tmp = strAppendUnsigned(tmp, qr.rem, 2);
      tmp = strAppend(tmp, " V", 2);
      if (text != reusableBuffer.hardwareAndSettings.msg) {
        setText(reusableBuffer.hardwareAndSettings.msg);
        invalidate();
      }
    }

  protected:
};

class SwitchDynamicLabel : public StaticText {
  public:
    SwitchDynamicLabel(Window * parent, const rect_t & rect, uint8_t index):
      StaticText(parent, rect),
      index(index)
    {
      update();
    }

    std::string label()
    {
      return TEXT_AT_INDEX(STR_VSRCRAW, (index + MIXSRC_FIRST_SWITCH - MIXSRC_Rud + 1)) + std::string(&"\300-\301"[lastpos], 1);
    }

    void update()
    {
      uint8_t newpos = position();
      if (newpos != lastpos) {
        lastpos = newpos;
        setText(label());
      }
    }

    uint8_t position()
    {
      auto value = getValue(MIXSRC_FIRST_SWITCH + index);
      if (value > 0)
        return 2;
      else if (value < 0)
        return 0;
      else
        return 1;
    }

    void checkEvents() override
    {
      update();
    }

  protected:
    uint8_t index;
    uint8_t lastpos = 0xff;
};

RadioHardwarePage::RadioHardwarePage():
  PageTab(STR_HARDWARE, ICON_RADIO_HARDWARE)
{
}

void RadioHardwarePage::build(FormWindow * window)
{
  FormGridLayout grid;
#if LCD_W > LCD_H
  grid.setLabelWidth(130);
#else
  grid.setLabelWidth(100);
#endif
  grid.spacer(6);

  // Calibration
  new StaticText(window, grid.getLabelSlot(), STR_INPUTS, 0, FONT(BOLD));
  auto calib = new TextButton(window, grid.getFieldSlot(), STR_CALIBRATION);
  calib->setPressHandler([=]() -> uint8_t {
      auto calibrationPage = new RadioCalibrationPage();
      calibrationPage->setCloseHandler([=]() {
          calib->setFocus();
      });
      return 0;
  });
  grid.nextLine();

  // Sticks
  new Subtitle(window, grid.getLineSlot(), STR_STICKS);
  grid.nextLine();
  for (int i = 0; i < NUM_STICKS; i++) {
    new StaticText(window, grid.getLabelSlot(true), TEXT_AT_INDEX(STR_VSRCRAW, (i + 1)));
    new TextEdit(window, grid.getFieldSlot(2,0), g_eeGeneral.anaNames[i], LEN_ANA_NAME);
    grid.nextLine();
  }

  // Pots
  new Subtitle(window, grid.getLineSlot(), STR_POTS);
  grid.nextLine();
  for (int i = 0; i < NUM_POTS; i++) {
    new StaticText(window, grid.getLabelSlot(true), TEXT_AT_INDEX(STR_VSRCRAW, (i + NUM_STICKS + 1)));
    new TextEdit(window, grid.getFieldSlot(2,0), g_eeGeneral.anaNames[i + NUM_STICKS], LEN_ANA_NAME);
    new Choice(window, grid.getFieldSlot(2,1), STR_POTTYPES, POT_NONE, POT_WITHOUT_DETENT, GET_SET_BF(g_eeGeneral.potsConfig, 2 * i, 2));
    grid.nextLine();
  }

  // Switches
  new Subtitle(window, grid.getLineSlot(), STR_SWITCHES);
  grid.nextLine();
  for (int i = 0; i < NUM_SWITCHES; i++) {
    new SwitchDynamicLabel(window, grid.getLabelSlot(true), i);
    new TextEdit(window, grid.getFieldSlot(2, 0), g_eeGeneral.switchNames[i], LEN_SWITCH_NAME);
    new Choice(window, grid.getFieldSlot(2, 1), STR_SWTYPES, SWITCH_NONE, SWITCH_TYPE_MAX(i),
               [=]() -> int {
                   return SWITCH_CONFIG(i);
               },
               [=](int newValue) {
                   swconfig_t mask = (swconfig_t) 0x03 << (2 * i);
                   g_eeGeneral.switchConfig = (g_eeGeneral.switchConfig & ~mask) | ((swconfig_t(newValue) & 0x03) << (2 * i));
                   SET_DIRTY();
               });
    grid.nextLine();
  }
#if LCD_W > LCD_H
  grid.setLabelWidth(180);
#else
  grid.setLabelWidth(130);
#endif

  // Bat calibration
  new StaticText(window, grid.getLabelSlot(), STR_BATT_CALIB);
  auto batCal = new NumberEdit(window, grid.getFieldSlot(), -127, 127, GET_SET_DEFAULT(g_eeGeneral.txVoltageCalibration));
  batCal->setDisplayHandler([](BitmapBuffer * dc, LcdFlags flags, int32_t value) {
      dc->drawNumber(2, 0, getBatteryVoltage(), flags | PREC2, 0, nullptr, "V");
  });
  batCal->setWindowFlags(REFRESH_ALWAYS);
  grid.nextLine();

  // RTC Batt display
  new StaticText(window, grid.getLabelSlot(), STR_RTC_BATT);

  new RTCBattValue(window, grid.getFieldSlot());
  grid.nextLine();

  // RTC Batt check enable
  new StaticText(window, grid.getLabelSlot(), STR_RTC_CHECK);
  new CheckBox(window, grid.getFieldSlot(), GET_SET_INVERTED(g_eeGeneral.disableRtcWarning ));
  grid.nextLine();

#if defined(CROSSFIRE) && SPORT_MAX_BAUDRATE < 400000
  // Max baud for external modules
  new StaticText(window, grid.getLabelSlot(), STR_MAXBAUDRATE);
  new Choice(window, grid.getFieldSlot(1,0), STR_CRSF_BAUDRATE, 0, 1, GET_SET_DEFAULT(g_eeGeneral.telemetryBaudrate));
  grid.nextLine();
#endif

  // ADC filter
  new StaticText(window, grid.getLabelSlot(), STR_JITTER_FILTER);
  new CheckBox(window, grid.getFieldSlot(), GET_SET_INVERTED(g_eeGeneral.jitterFilter));
  grid.nextLine();

  // Debugs
  new StaticText(window, grid.getLabelSlot(), STR_DEBUG, 0, FONT(BOLD));
  auto analogs = new TextButton(window, grid.getFieldSlot(2, 0), STR_ANALOGS_BTN);
  analogs->setPressHandler([=]() -> uint8_t {
      //TODO
  });
  auto keys = new TextButton(window, grid.getFieldSlot(2, 1), STR_KEYS_BTN);
  keys->setPressHandler([=]() -> uint8_t {
      //TODO
  });
  grid.nextLine();

  window->setInnerHeight(grid.getWindowHeight());
}
