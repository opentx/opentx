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

#include "model_setup.h"
#include "opentx.h"
#include "libopenui.h"

#define SET_DIRTY()     storageDirty(EE_MODEL)

uint8_t findEmptyReceiverSlot()
{
  for (uint8_t slot=0; slot<NUM_RECEIVERS; slot++) {
    if (!g_model.receiverData[slot].used) {
      return slot + 1;
    }
  }
  return 0;
}


void resetModuleSettings(uint8_t module)
{
  g_model.moduleData[module].channelsStart = 0;
  g_model.moduleData[module].channelsCount = defaultModuleChannels_M8(module);
  g_model.moduleData[module].rfProtocol = 0;
  if (isModulePPM(module)) {
    SET_DEFAULT_PPM_FRAME_LENGTH(EXTERNAL_MODULE);
  }
}

class ChannelFailsafeBargraph: public Window {
  public:
    ChannelFailsafeBargraph(Window * parent, const rect_t & rect, uint8_t moduleIndex, uint8_t channel):
      Window(parent, rect),
      moduleIndex(moduleIndex),
      channel(channel)
    {
    }

    void checkEvents() override
    {
      invalidate();
    }

    void paint(BitmapBuffer * dc) override
    {
      int32_t failsafeValue = g_model.failsafeChannels[channel];
      int32_t channelValue = channelOutputs[channel];

      const int lim = (g_model.extendedLimits ? (512 * LIMIT_EXT_PERCENT / 100) : 512) * 2;

      coord_t x = 0;
      lcdDrawRect(x, 0, width(), height());
      const coord_t lenChannel = limit((uint8_t) 1, uint8_t((abs(channelValue) * width() / 2 + lim / 2) / lim),
                                       uint8_t(width() / 2));
      const coord_t lenFailsafe = limit((uint8_t) 1, uint8_t((abs(failsafeValue) * width() / 2 + lim / 2) / lim),
                                        uint8_t(width() / 2));
      x += width() / 2;
      const coord_t xChannel = (channelValue > 0) ? x : x + 1 - lenChannel;
      const coord_t xFailsafe = (failsafeValue > 0) ? x : x + 1 - lenFailsafe;
      lcdDrawSolidFilledRect(xChannel, + 2, lenChannel, (height() / 2) - 3, TEXT_COLOR);
      lcdDrawSolidFilledRect(xFailsafe, (height() / 2) + 1, lenFailsafe, (height() / 2) - 3, ALARM_COLOR);
    }

  protected:
    uint8_t moduleIndex;
    uint8_t channel;
};

class FailSafeBody : public Window {
  public:
    FailSafeBody(Window * parent, const rect_t &rect, uint8_t moduleIndex) :
      Window(parent, rect),
      moduleIndex(moduleIndex)
    {
      build();
    }

    void checkEvents() override
    {
      invalidate();
    }

    void build()
    {
      GridLayout grid;
      grid.setLabelWidth(60);
      grid.spacer(8);

      const int lim = (g_model.extendedLimits ? (512 * LIMIT_EXT_PERCENT / 100) : 512) * 2;

      for (int ch=0; ch < maxModuleChannels(moduleIndex); ch++) {
        // Channel name
        // TODO if (g_model.limitData[ch].name[0] != '\0') { <= add channel name
        new StaticText(this, grid.getLabelSlot(), getSourceString(MIXSRC_CH1 + ch));

        // Channel numeric value
        new NumberEdit(this, grid.getFieldSlot(3, 0), -lim, lim,
                       GET_DEFAULT(calcRESXto1000(g_model.failsafeChannels[ch])),
                       SET_VALUE(g_model.failsafeChannels[ch], newValue),
                       PREC1);

        // Channel bargraph
        new ChannelFailsafeBargraph(this, {150, grid.getWindowHeight(), 150, lineHeight}, moduleIndex, ch);
        grid.nextLine();
      }

      auto out2fail = new TextButton(this, grid.getLineSlot(), STR_OUTPUTS2FAILSAFE);
      out2fail->setPressHandler([=]() {
        setCustomFailsafe(moduleIndex);
        storageDirty(EE_MODEL);
        return 0;
      });

      grid.nextLine();
      setInnerHeight(grid.getWindowHeight());
    }

  protected:
    uint8_t moduleIndex;
};

class FailSafeFooter : public Window {
  public:
    FailSafeFooter(Window * parent, const rect_t &rect) :
      Window(parent, rect)
    {
    }

    void paint(BitmapBuffer * dc) override
    {
    }
};

class FailSafePage : public PageTab {
  public:
    FailSafePage(uint8_t moduleIndex) :
      PageTab("FAILSAFE", ICON_STATS_ANALOGS),
      moduleIndex(moduleIndex)
    {
    }

    void build(FormWindow * window) override
    {
      new FailSafeBody(window, {0, 0, LCD_W, window->height() - footerHeight}, moduleIndex );
      new FailSafeFooter(window, {0, window->height() - footerHeight, LCD_W, footerHeight});
    }

  protected:
    static constexpr coord_t footerHeight = 30;
    uint8_t moduleIndex;
};

FailSafeMenu::FailSafeMenu(uint8_t moduleIndex) :
  TabsGroup(ICON_MODEL)
{
  addTab(new FailSafePage(moduleIndex));
}


class ReceiverWindow : public Window {
  public:
    ReceiverWindow(Window * parent, const rect_t &rect, uint8_t receiverIndex) :
      Window(parent, rect, FORWARD_SCROLL),
      receiverIndex(receiverIndex)
    {
      update();
    }

    ~ReceiverWindow()
    {
      deleteChildren();
    }

  protected:
    uint8_t receiverIndex;

    void update()
    {
      GridLayout grid;


      new StaticText(this, grid.getLabelSlot(), "BLABLA BLABLA");
    }
};

class ModuleWindow : public Window {
  public:
    ModuleWindow(Window * parent, const rect_t &rect, uint8_t moduleIndex) :
      Window(parent, rect, FORWARD_SCROLL),
      moduleIndex(moduleIndex)
    {
      update();
    }

    ~ModuleWindow()
    {
      deleteChildren();
    }

  protected:
    uint8_t moduleIndex;
    Choice * moduleChoice = nullptr;
    FormField * lastField = nullptr; // moduleChoice is always the firstField
    TextButton * bindButton = nullptr;
    TextButton * rangeButton = nullptr;
    TextButton * registerButton = nullptr;
    Choice * failSafeChoice = nullptr;

    void addChannelRange(GridLayout &grid, uint8_t moduleIndex)
    {
      new StaticText(this, grid.getLabelSlot(true), STR_CHANNELRANGE);
      auto channelStart = new NumberEdit(this, grid.getFieldSlot(2, 0), 1,
                                         MAX_OUTPUT_CHANNELS - sentModuleChannels(moduleIndex) + 1,
                                         GET_DEFAULT(1 + g_model.moduleData[moduleIndex].channelsStart));
      auto channelEnd = new NumberEdit(this, grid.getFieldSlot(2, 1),
                                       g_model.moduleData[moduleIndex].channelsStart + minModuleChannels(moduleIndex),
                                       min<int8_t>(MAX_OUTPUT_CHANNELS, g_model.moduleData[moduleIndex].channelsStart + maxModuleChannels(moduleIndex)),
                                       GET_DEFAULT(g_model.moduleData[moduleIndex].channelsStart + 8 + g_model.moduleData[moduleIndex].channelsCount));
      channelStart->setPrefix(STR_CH);
      channelEnd->setPrefix(STR_CH);
      channelStart->setSetValueHandler([=](int32_t newValue) {
        g_model.moduleData[moduleIndex].channelsStart = newValue - 1;
        SET_DIRTY();
        channelEnd->setMin(g_model.moduleData[moduleIndex].channelsStart + minModuleChannels(moduleIndex));
        channelEnd->setMax(min<int8_t>(MAX_OUTPUT_CHANNELS, g_model.moduleData[moduleIndex].channelsStart + maxModuleChannels(moduleIndex)));
        channelEnd->invalidate();
      });
      channelEnd->setSetValueHandler([=](int32_t newValue) {
        g_model.moduleData[moduleIndex].channelsCount = newValue - g_model.moduleData[moduleIndex].channelsStart - 8;
        SET_DIRTY();
        channelStart->setMax(MAX_OUTPUT_CHANNELS - sentModuleChannels(moduleIndex) + 1);
      });
      channelEnd->enable(minModuleChannels(moduleIndex) < maxModuleChannels(moduleIndex));
    }

    void update()
    {
      GridLayout grid;

      FormField * previousField = moduleChoice ? moduleChoice->getPreviousField() : moduleChoice->getCurrentField();
      FormField * nextField = lastField ? lastField->getNextField() : nullptr;

      clear();
      FormField::clearCurrentField();

      // Module Type
      new StaticText(this, grid.getLabelSlot(true), STR_MODE);
      moduleChoice = new Choice(this, grid.getFieldSlot(2, 0), STR_MODULE_PROTOCOLS,
                                MODULE_TYPE_NONE, MODULE_TYPE_COUNT - 1,
                                GET_DEFAULT(g_model.moduleData[moduleIndex].type),
                                [=](int32_t newValue) {
                                  g_model.moduleData[moduleIndex].type = newValue;
                                  SET_DIRTY();
                                  resetModuleSettings(moduleIndex);
                                  update();
                                  moduleChoice->setFocus();
                                });
      moduleChoice->setAvailableHandler([=](int8_t moduleType) {
        return isModuleTypeAllowed(moduleIndex, moduleType);
      });
      FormField::link(previousField, moduleChoice);

      // Module parameters
      if (isModuleXJT(moduleIndex)) {
        auto xjtChoice = new Choice(this, grid.getFieldSlot(2, 1), STR_XJT_PROTOCOLS, RF_PROTO_OFF, RF_PROTO_LAST,
                                    GET_SET_DEFAULT(g_model.moduleData[moduleIndex].rfProtocol));
        xjtChoice->setAvailableHandler([](int index) {
          return index != RF_PROTO_OFF;
        });
      }

      if (isModuleDSM2(moduleIndex)) {
        new Choice(this, grid.getFieldSlot(2, 1), STR_DSM_PROTOCOLS, DSM2_PROTO_LP45, DSM2_PROTO_DSMX,
                   GET_SET_DEFAULT(g_model.moduleData[moduleIndex].rfProtocol));
      }

      if (isModuleR9M(moduleIndex)) {
        new Choice(this, grid.getFieldSlot(2, 1), TR_R9M_REGION, MODULE_SUBTYPE_R9M_FCC,
                   MODULE_SUBTYPE_R9M_EU,
                   GET_DEFAULT(g_model.moduleData[moduleIndex].subType),
                   [=](int32_t newValue) {
                     g_model.moduleData[moduleIndex].subType = newValue;
                     SET_DIRTY();
                     update();
                   });
      }
      grid.nextLine();

      // Channel Range
      if (g_model.moduleData[moduleIndex].type != MODULE_TYPE_NONE) {
        addChannelRange(grid, moduleIndex); //TODO XJT2 should only set channel count of 8/16/24
        grid.nextLine();
      }

      // PPM modules
      if (isModulePPM(moduleIndex)) {
        // PPM frame
        new StaticText(this, grid.getLabelSlot(true), STR_PPMFRAME);

        // PPM frame length
        auto edit = new NumberEdit(this, grid.getFieldSlot(2, 0), 125, 35 * 5 + 225,
                                   GET_DEFAULT(g_model.moduleData[moduleIndex].ppm.frameLength * 5 + 225),
                                   SET_VALUE(g_model.moduleData[moduleIndex].ppm.frameLength, (newValue - 225) / 5),
                                   PREC1);
        edit->setStep(5);
        edit->setSuffix(STR_MS);

        // PPM frame delay
        edit = new NumberEdit(this, grid.getFieldSlot(2, 1), 100, 800,
                              GET_DEFAULT(g_model.moduleData[moduleIndex].ppm.delay * 50 + 300),
                              SET_VALUE(g_model.moduleData[moduleIndex].ppm.delay, (newValue - 300) / 50));
        edit->setStep(50);
        edit->setSuffix("us");
        grid.nextLine();
      }

      // Receiver number
      if (isModuleNeedingReceiverNumber(moduleIndex)) {
        new StaticText(this, grid.getLabelSlot(true), STR_RECEIVER_NUM);
        new NumberEdit(this, grid.getFieldSlot(2, 0), 0, MAX_RX_NUM(moduleIndex), GET_SET_DEFAULT(g_model.header.modelId[moduleIndex]));
        grid.nextLine();
      }

      // Module parameters

      // Bind and Range buttons
      if (isModuleNeedingBindRangeButtons(moduleIndex)) {
        bindButton = new TextButton(this, grid.getFieldSlot(2, 0), STR_MODULE_BIND);
        bindButton->setPressHandler([=]() -> uint8_t {
          if (moduleSettings[moduleIndex].mode == MODULE_MODE_RANGECHECK) {
            rangeButton->check(false);
          }
          if (moduleSettings[moduleIndex].mode == MODULE_MODE_BIND) {
            bindButton->setText(STR_MODULE_BIND);
            moduleSettings[moduleIndex].mode = MODULE_MODE_NORMAL;
            return 0;
          }
          else {
            bindButton->setText(STR_MODULE_BIND);
            moduleSettings[moduleIndex].mode = MODULE_MODE_BIND;
            return 1;
          }
        });
        bindButton->setCheckHandler([=]() {
          if (moduleSettings[moduleIndex].mode != MODULE_MODE_BIND) {
            bindButton->setText(STR_MODULE_BIND);
            bindButton->check(false);
          }
        });

        rangeButton = new TextButton(this, grid.getFieldSlot(2, 1), STR_MODULE_RANGE);
        rangeButton->setPressHandler([=]() -> uint8_t {
          if (moduleSettings[moduleIndex].mode == MODULE_MODE_BIND) {
            bindButton->setText(STR_MODULE_BIND);
            bindButton->check(false);
            moduleSettings[moduleIndex].mode = MODULE_MODE_NORMAL;
          }
          if (moduleSettings[moduleIndex].mode == MODULE_MODE_RANGECHECK) {
            moduleSettings[moduleIndex].mode = MODULE_MODE_NORMAL;
            return 0;
          }
          else {
            moduleSettings[moduleIndex].mode = MODULE_MODE_RANGECHECK;
            return 1;
          }
        });

        grid.nextLine();
      }


      // Register and Range buttons
      if (isModuleNeedingRegisterRangeButtons(moduleIndex)) {
        registerButton = new TextButton(this, grid.getFieldSlot(2, 0), STR_MODULE_REGISTER);
        registerButton->setPressHandler([=]() -> uint8_t {
          if (moduleSettings[moduleIndex].mode == MODULE_MODE_RANGECHECK) {
            rangeButton->check(false);
          }
          if (moduleSettings[moduleIndex].mode == MODULE_MODE_REGISTER) {
            registerButton->setText(STR_MODULE_REGISTER);
            moduleSettings[moduleIndex].mode = MODULE_MODE_NORMAL;
            return 0;
          }
          else {
            registerButton->setText(STR_MODULE_REGISTER);
            moduleSettings[moduleIndex].mode = MODULE_MODE_REGISTER;
            return 1;
          }
        });
        registerButton->setCheckHandler([=]() {
          if (moduleSettings[moduleIndex].mode != MODULE_MODE_REGISTER) {
            registerButton->setText(STR_MODULE_REGISTER);
            registerButton->check(false);
          }
        });

        rangeButton = new TextButton(this, grid.getFieldSlot(2, 1), STR_MODULE_RANGE);
        rangeButton->setPressHandler([=]() -> uint8_t {
          if (moduleSettings[moduleIndex].mode == MODULE_MODE_REGISTER) {
            registerButton->setText(STR_MODULE_REGISTER);
            registerButton->check(false);
            moduleSettings[moduleIndex].mode = MODULE_MODE_NORMAL;
          }
          if (moduleSettings[moduleIndex].mode == MODULE_MODE_RANGECHECK) {
            moduleSettings[moduleIndex].mode = MODULE_MODE_NORMAL;
            return 0;
          }
          else {
            moduleSettings[moduleIndex].mode = MODULE_MODE_RANGECHECK;
            return 1;
          }
        });

        grid.nextLine();
      }

      // Failsafe
      if (isModuleNeedingFailsafeButton(moduleIndex)) {
        new StaticText(this, grid.getLabelSlot(true), STR_FAILSAFE);
        failSafeChoice = new Choice(this, grid.getFieldSlot(2, 0), STR_VFAILSAFE, 0, FAILSAFE_LAST,
                                    GET_DEFAULT(g_model.moduleData[moduleIndex].failsafeMode),
                                    [=](int32_t newValue) {
                                      g_model.moduleData[moduleIndex].failsafeMode = newValue;
                                      SET_DIRTY();
                                      update();
                                      failSafeChoice->setFocus();
                                    });
        if (g_model.moduleData[moduleIndex].failsafeMode == FAILSAFE_CUSTOM) {
          new TextButton(this, grid.getFieldSlot(2, 1), STR_SET,
                         [=]() -> uint8_t {
                           new FailSafeMenu(moduleIndex);
                           return 1;
                         });
        }
        grid.nextLine();
      }

      // R9M Power
      if (isModuleR9M_FCC(moduleIndex)) {
        new StaticText(this, grid.getLabelSlot(true), STR_MULTI_RFPOWER);
        new Choice(this, grid.getFieldSlot(), STR_R9M_FCC_POWER_VALUES, 0, R9M_FCC_POWER_MAX,
                   GET_SET_DEFAULT(g_model.moduleData[moduleIndex].pxx.power));
      }

      if (isModuleR9M_LBT(moduleIndex)) {
        new StaticText(this, grid.getLabelSlot(true), STR_MULTI_RFPOWER);
        new Choice(this, grid.getFieldSlot(), STR_R9M_LBT_POWER_VALUES, 0, R9M_LBT_POWER_MAX,
                   GET_DEFAULT(min<uint8_t>(g_model.moduleData[moduleIndex].pxx.power, R9M_LBT_POWER_MAX)),
                   SET_DEFAULT(g_model.moduleData[moduleIndex].pxx.power));
      }

      // Internal module receivers
      if (isModulePXX2(moduleIndex)) {
        uint8_t receiverCount = 0;
        while (receiverCount < PXX2_MAX_RECEIVERS_PER_MODULE) {
          if (g_model.moduleData[moduleIndex].pxx2.getReceiverSlot(receiverCount)) {
            grid.addWindow(new ReceiverWindow(this, {0, grid.getWindowHeight(), LCD_W, 0}, g_model.moduleData[moduleIndex].pxx2.getReceiverSlot(receiverCount)));
            receiverCount++;
          }
          else
            break;
        }
        if (receiverCount < PXX2_MAX_RECEIVERS_PER_MODULE) {
          new StaticText(this, grid.getLabelSlot(true),STR_RECEIVER);
          auto AddReceiver = new TextButton(this, grid.getFieldSlot(), STR_RXADD_BUTTON);
          AddReceiver->setPressHandler([=]() {
            uint8_t slot = findEmptyReceiverSlot();
            if (slot > 0) {
              g_model.moduleData[moduleIndex].pxx2.receivers |= (slot << (receiverCount * 3));
              --slot;
              g_model.receiverData[slot].used = 1;
              #warning "USE 32bits copy"
              g_model.receiverData[slot].channelMapping0 = (0 << 0) + (1 << 5) + (2 << 10) + (3 << 15) + (4 << 20) + (5 << 25) + ((uint64_t)6 << 30) + ((uint64_t)7 << 35) + ((uint64_t)8 << 40) + ((uint64_t)9 << 45) + ((uint64_t)10 << 50) + ((uint64_t)11 << 55);
              g_model.receiverData[slot].channelMapping1 = (12 << 0) + (13 << 5) + (14 << 10) + (15 << 15) + (16 << 20) + (17 << 25) + ((uint64_t)18 << 30) + ((uint64_t)19 << 35) + ((uint64_t)20 << 40) + ((uint64_t)21 << 45) + ((uint64_t)22 << 50) + ((uint64_t)23 << 55);
              storageDirty(EE_MODEL);
            }
            return 0;
          });
          grid.nextLine();
        }
      }

      getParent()->moveWindowsTop(top(), adjustHeight());

      lastField = FormField::getCurrentField();
      if (nextField)
        FormField::link(lastField, nextField);
    }
};

ModelSetupPage::ModelSetupPage() :
  PageTab(STR_MENUSETUP, ICON_MODEL_SETUP)
{
}

uint8_t g_moduleIdx;

void onBindMenu(const char * result)
{
  uint8_t moduleIdx = 0; // TODO (menuVerticalPosition >= ITEM_MODEL_EXTERNAL_MODULE_LABEL ? EXTERNAL_MODULE : INTERNAL_MODULE);

  if (result == STR_BINDING_1_8_TELEM_ON) {
    g_model.moduleData[moduleIdx].pxx.receiver_telem_off = false;
    g_model.moduleData[moduleIdx].pxx.receiver_channel_9_16 = false;
  }
  else if (result == STR_BINDING_1_8_TELEM_OFF) {
    g_model.moduleData[moduleIdx].pxx.receiver_telem_off = true;
    g_model.moduleData[moduleIdx].pxx.receiver_channel_9_16 = false;
  }
  else if (result == STR_BINDING_9_16_TELEM_ON) {
    g_model.moduleData[moduleIdx].pxx.receiver_telem_off = false;
    g_model.moduleData[moduleIdx].pxx.receiver_channel_9_16 = true;
  }
  else if (result == STR_BINDING_9_16_TELEM_OFF) {
    g_model.moduleData[moduleIdx].pxx.receiver_telem_off = true;
    g_model.moduleData[moduleIdx].pxx.receiver_channel_9_16 = true;
  }
  else {
    return;
  }

  moduleSettings[moduleIdx].mode  = MODULE_MODE_BIND;
}

void ModelSetupPage::build(FormWindow * window)
{
  GridLayout grid;
  grid.spacer(2);

  // Model name
  new StaticText(window, grid.getLabelSlot(), STR_MODELNAME);
  auto first = new TextEdit(window, grid.getFieldSlot(), g_model.header.name, sizeof(g_model.header.name));
  window->setFirstField(first);
  grid.nextLine();

  // Bitmap
  new StaticText(window, grid.getLabelSlot(), STR_BITMAP);
  new FileChoice(window, grid.getFieldSlot(), BITMAPS_PATH, BITMAPS_EXT, sizeof(g_model.header.bitmap) - LEN_BITMAPS_EXT,
                 [=]() {
                   return std::string(g_model.header.bitmap, ZLEN(g_model.header.bitmap));
                 },
                 [=](std::string newValue) {
                   strncpy(g_model.header.bitmap, newValue.c_str(), sizeof(g_model.header.bitmap));
                   SET_DIRTY();
                 });
  grid.nextLine();

  for (uint8_t i = 0; i < TIMERS; i++) {
    TimerData * timer = &g_model.timers[i];

    // Timer label
    char timerLabel[8];
    strAppendStringWithIndex(timerLabel, STR_TIMER, i + 1);
    new Subtitle(window, grid.getLineSlot(), timerLabel);
    grid.nextLine();

    // Timer name
    new StaticText(window, grid.getLabelSlot(true), STR_TIMER_NAME);
    new TextEdit(window, grid.getFieldSlot(), timer->name, LEN_TIMER_NAME);
    grid.nextLine();

    // Timer mode
    new StaticText(window, grid.getLabelSlot(true), STR_MODE);
    new SwitchChoice(window, grid.getFieldSlot(2, 0), SWSRC_FIRST, SWSRC_LAST, GET_SET_DEFAULT(timer->mode));
    new Choice(window, grid.getFieldSlot(2, 1), "\006""Simple""Thr.\0 ""Thr.%", 0, TMRMODE_COUNT, GET_SET_DEFAULT(timer->mode));
    grid.nextLine();

    // Timer start value
    new StaticText(window, grid.getLabelSlot(true), "Start");
    new TimeEdit(window, grid.getFieldSlot(), 0, TIMER_MAX, GET_SET_DEFAULT(timer->start));
    grid.nextLine();

    // Timer minute beep
    new StaticText(window, grid.getLabelSlot(true), STR_MINUTEBEEP);
    new CheckBox(window, grid.getFieldSlot(), GET_SET_DEFAULT(timer->minuteBeep));
    grid.nextLine();

    // Timer countdown
    new StaticText(window, grid.getLabelSlot(true), STR_BEEPCOUNTDOWN);
    new Choice(window, grid.getFieldSlot(2, 0), STR_VBEEPCOUNTDOWN, COUNTDOWN_SILENT, COUNTDOWN_COUNT - 1, GET_SET_DEFAULT(timer->countdownBeep));
    new Choice(window, grid.getFieldSlot(2, 1), STR_COUNTDOWNVALUES, 0, 3, GET_SET_WITH_OFFSET(timer->countdownStart, 2));
    grid.nextLine();

    // Timer persistent
    new StaticText(window, grid.getLabelSlot(true), STR_PERSISTENT);
    new Choice(window, grid.getFieldSlot(), STR_VPERSISTENT, 0, 2, GET_SET_DEFAULT(timer->persistent));
    grid.nextLine();
  }

  // Extended limits
  new StaticText(window, grid.getLabelSlot(), STR_ELIMITS);
  new CheckBox(window, grid.getFieldSlot(), GET_SET_DEFAULT(g_model.extendedLimits));
  grid.nextLine();

  // Extended trims
  new StaticText(window, grid.getLabelSlot(), STR_ETRIMS);
  new CheckBox(window, grid.getFieldSlot(2, 0), GET_SET_DEFAULT(g_model.extendedTrims));
  new TextButton(window, grid.getFieldSlot(2, 1), STR_RESET_BTN,
                 []() -> uint8_t {
                   START_NO_HIGHLIGHT();
                   for (auto &flightModeData : g_model.flightModeData) {
                     memclear(&flightModeData, TRIMS_ARRAY_SIZE);
                   }
                   storageDirty(EE_MODEL);
                   AUDIO_WARNING1();
                   return 0;
                 });
  grid.nextLine();

  // Display trims
  new StaticText(window, grid.getLabelSlot(), STR_DISPLAY_TRIMS);
  new Choice(window, grid.getFieldSlot(), "\006No\0   ChangeYes", 0, 2, GET_SET_DEFAULT(g_model.displayTrims));
  grid.nextLine();

  // Trim step
  new StaticText(window, grid.getLabelSlot(), STR_TRIMINC);
  new Choice(window, grid.getFieldSlot(), STR_VTRIMINC, -2, 2, GET_SET_DEFAULT(g_model.trimInc));
  grid.nextLine();

  // Throttle parameters
  {
    new Subtitle(window, grid.getLineSlot(), STR_THROTTLE_LABEL);
    grid.nextLine();

    // Throttle reversed
    new StaticText(window, grid.getLabelSlot(true), STR_THROTTLEREVERSE);
    new CheckBox(window, grid.getFieldSlot(), GET_SET_DEFAULT(g_model.throttleReversed));
    grid.nextLine();

    // Throttle source
    new StaticText(window, grid.getLabelSlot(true), STR_TTRACE);
    new SourceChoice(window, grid.getFieldSlot(), 0, MIXSRC_LAST_CH, GET_SET_DEFAULT(g_model.thrTraceSrc));
    grid.nextLine();

    // Throttle trim
    new StaticText(window, grid.getLabelSlot(true), STR_TTRIM);
    new CheckBox(window, grid.getFieldSlot(), GET_SET_DEFAULT(g_model.thrTrim));
    grid.nextLine();
  }

  // Preflight parameters
  {
    new Subtitle(window, grid.getLineSlot(), STR_PREFLIGHT);
    grid.nextLine();

    // Display checklist
    new StaticText(window, grid.getLabelSlot(true), STR_CHECKLIST);
    new CheckBox(window, grid.getFieldSlot(), GET_SET_DEFAULT(g_model.displayChecklist));
    grid.nextLine();

    // Throttle warning
    new StaticText(window, grid.getLabelSlot(true), STR_THROTTLEWARNING);
    new CheckBox(window, grid.getFieldSlot(), GET_SET_INVERTED(g_model.disableThrottleWarning));
    grid.nextLine();

    // Switches warning
    new StaticText(window, grid.getLabelSlot(true), STR_SWITCHWARNING);
    for (int i = 0; i < NUM_SWITCHES; i++) {
      char s[3];
      if (i > 0 && (i % 3) == 0)
        grid.nextLine();

      auto button = new TextButton(window, grid.getFieldSlot(3, i % 3), getSwitchWarningString(s, i), nullptr,
                                   (BF_GET(g_model.switchWarningState, 3 * i, 3) == 0 ? 0 : BUTTON_CHECKED));
      button->setPressHandler([button, i] {
        swarnstate_t newstate = BF_GET(g_model.switchWarningState, 3 * i, 3);
        if (newstate == 1 && SWITCH_CONFIG(i) != SWITCH_3POS)
          newstate = 3;
        else
          newstate = (newstate + 1) % 4;
        BF_SET(g_model.switchWarningState, newstate, 3 * i, 3);
        SET_DIRTY();
        button->setText(getSwitchWarningString(i));
        return newstate > 0;
      });
    }
    grid.nextLine();
  }

  // Center beeps
  {
    new StaticText(window, grid.getLabelSlot(), STR_BEEPCTR);
    for (int i = 0; i < NUM_STICKS + NUM_POTS + NUM_SLIDERS; i++) {
      char s[2];
      if (i > 0 && (i % 3) == 0)
        grid.nextLine();

      new TextButton(window, grid.getFieldSlot(3, i % 3), getStringAtIndex(s, STR_RETA123, i),
                     [=]() -> uint8_t {
                       BF_BIT_FLIP(g_model.beepANACenter, BF_BIT<BeepANACenter>(i));
                       SET_DIRTY();
                       return BF_SINGLE_BIT_GET<BeepANACenter>(g_model.beepANACenter, i);
                     },
                     BF_SINGLE_BIT_GET(g_model.beepANACenter, i) ? BUTTON_CHECKED : 0);
    }
    grid.nextLine();
  }

  // Global functions
  new StaticText(window, grid.getLabelSlot(), STR_USE_GLOBAL_FUNCS);
  new CheckBox(window, grid.getFieldSlot(), GET_SET_INVERTED(g_model.noGlobalFunctions));
  grid.nextLine();

  // Internal module
  {
    new Subtitle(window, grid.getLineSlot(), STR_INTERNALRF);
    grid.nextLine();
    grid.addWindow(new ModuleWindow(window, {0, grid.getWindowHeight(), LCD_W, 0}, INTERNAL_MODULE));
  }

  // External module
  {
    new Subtitle(window, grid.getLineSlot(), STR_EXTERNALRF);
    grid.nextLine();
    grid.addWindow(new ModuleWindow(window, {0, grid.getWindowHeight(), LCD_W, 0}, EXTERNAL_MODULE));
  }

  FormField::link(FormField::getCurrentField(), first);

  grid.nextLine();

  window->setInnerHeight(grid.getWindowHeight());
}

// Switch to external antenna confirmation
//  bool newAntennaSel;
//  if (warningResult) {
//    warningResult = 0;
//    g_model.moduleData[INTERNAL_MODULE].pxx.external_antenna = XJT_EXTERNAL_ANTENNA;
//  }

