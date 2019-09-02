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

class ChannelFailsafeBargraph: public Window {
  public:
    ChannelFailsafeBargraph(Window * parent, const rect_t & rect, uint8_t moduleIdx, uint8_t channel):
      Window(parent, rect),
      moduleIdx(moduleIdx),
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
    uint8_t moduleIdx;
    uint8_t channel;
};

class FailSafeBody : public Window {
  public:
    FailSafeBody(Window * parent, const rect_t &rect, uint8_t moduleIdx) :
      Window(parent, rect),
      moduleIdx(moduleIdx)
    {
      build();
    }

    void checkEvents() override
    {
      invalidate();
    }

    void build()
    {
      FormGridLayout grid;
      grid.setLabelWidth(60);
      grid.spacer(8);

      const int lim = (g_model.extendedLimits ? (512 * LIMIT_EXT_PERCENT / 100) : 512) * 2;

      for (int ch=0; ch < maxModuleChannels(moduleIdx); ch++) {
        // Channel name
        // TODO if (g_model.limitData[ch].name[0] != '\0') { <= add channel name
        new StaticText(this, grid.getLabelSlot(), getSourceString(MIXSRC_CH1 + ch));

        // Channel numeric value
        new NumberEdit(this, grid.getFieldSlot(3, 0), -lim, lim,
                       GET_DEFAULT(calcRESXto1000(g_model.failsafeChannels[ch])),
                       SET_VALUE(g_model.failsafeChannels[ch], newValue),
                       PREC1);

        // Channel bargraph
        new ChannelFailsafeBargraph(this, {150, grid.getWindowHeight(), 150, PAGE_LINE_HEIGHT}, moduleIdx, ch);
        grid.nextLine();
      }

      auto out2fail = new TextButton(this, grid.getLineSlot(), STR_CHANNELS2FAILSAFE);
      out2fail->setPressHandler([=]() {
        setCustomFailsafe(moduleIdx);
        storageDirty(EE_MODEL);
        return 0;
      });

      grid.nextLine();
      setInnerHeight(grid.getWindowHeight());
    }

  protected:
    uint8_t moduleIdx;
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
    FailSafePage(uint8_t moduleIdx) :
      PageTab("FAILSAFE", ICON_STATS_ANALOGS),
      moduleIdx(moduleIdx)
    {
    }

    void build(FormWindow * window) override
    {
      new FailSafeBody(window, {0, 0, LCD_W, window->height() - footerHeight}, moduleIdx );
      new FailSafeFooter(window, {0, window->height() - footerHeight, LCD_W, footerHeight});
    }

  protected:
    static constexpr coord_t footerHeight = 30;
    uint8_t moduleIdx;
};

FailSafeMenu::FailSafeMenu(uint8_t moduleIdx) :
  TabsGroup(ICON_MODEL)
{
  addTab(new FailSafePage(moduleIdx));
}

class RegisterDialog: public Dialog {
  public:
    RegisterDialog(uint8_t moduleIdx):
      Dialog(STR_REGISTER, {50, 73, LCD_W - 100, LCD_H - 146}),
      moduleIdx(moduleIdx)
    {
      FormGridLayout grid(width());
      grid.setLabelWidth(150);
      grid.spacer(PAGE_LINE_HEIGHT + 8);

      // Register ID
      new StaticText(this, grid.getLabelSlot(), STR_REG_ID);
      auto edit = new TextEdit(this, grid.getFieldSlot(), g_model.modelRegistrationID, sizeof(g_model.modelRegistrationID));
      grid.nextLine();

      // UID
      new StaticText(this, grid.getLabelSlot(), "UID");
      uid = new NumberEdit(this, grid.getFieldSlot(), 0, 2, GET_SET_DEFAULT(reusableBuffer.moduleSetup.pxx2.registerLoopIndex));
      grid.nextLine();

      // RX name
      new StaticText(this, grid.getLabelSlot(), STR_RX_NAME);
      waiting = new StaticText(this, grid.getFieldSlot(), STR_WAITING);
      grid.nextLine();
      grid.spacer(6);

      // Buttons
      exitButton = new TextButton(this, grid.getLabelSlot(), "EXIT",
                                   [=]() -> int8_t {
                                       this->deleteLater();
                                       return 0;
                                   });
      exitButton->setFocus();
      FormField::link(exitButton, edit);

      start();

      setCloseHandler([=]() {
          moduleState[moduleIdx].mode = MODULE_MODE_NORMAL;
      });
    }

    void start()
    {
      memclear(&reusableBuffer.moduleSetup.pxx2, sizeof(reusableBuffer.moduleSetup.pxx2));
      moduleState[moduleIdx].mode = MODULE_MODE_REGISTER;
    }

    void checkEvents() override
    {
      if (!rxName && reusableBuffer.moduleSetup.pxx2.registerStep >= REGISTER_RX_NAME_RECEIVED) {
        rect_t rect = waiting->getRect();
        waiting->deleteLater();
        FormField::clearCurrentField();
        rxName = new TextEdit(this, rect, reusableBuffer.moduleSetup.pxx2.registerRxName, PXX2_LEN_RX_NAME);
        rect = exitButton->getRect();
        auto okButton = new TextButton(this, rect, "OK",
                                    [=]() -> int8_t {
                                        reusableBuffer.moduleSetup.pxx2.registerStep = REGISTER_RX_NAME_SELECTED;
                                        return 0;
                                    });
        exitButton->setLeft(left() + rect.w + 10);
        FormField::link(uid, rxName);
        FormField::link(rxName, okButton);
        FormField::link(okButton, exitButton);
        okButton->setFocus();
      }
      else if (reusableBuffer.moduleSetup.pxx2.registerStep == REGISTER_OK) {
        deleteLater();
        POPUP_INFORMATION(STR_REG_OK);
      }

      Dialog::checkEvents();
    }

  protected:
    uint8_t moduleIdx;
    NumberEdit * uid;
    StaticText * waiting;
    TextEdit * rxName = nullptr;
    TextButton * exitButton;
};

class BindWaitDialog: public Dialog {
  public:
    BindWaitDialog(uint8_t moduleIdx, uint8_t receiverIdx):
            Dialog(STR_BIND, {50, 73, LCD_W - 100, LCD_H - 146}),
            moduleIdx(moduleIdx),
            receiverIdx(receiverIdx)
    {
      new StaticText(this, {0, height() / 2, width(), PAGE_LINE_HEIGHT}, STR_WAITING_FOR_RX, CENTERED);
    }

    void checkEvents() override;

#if defined(HARDWARE_KEYS)
    void onKeyEvent(event_t event) override
    {
      TRACE_WINDOWS("%s received event 0x%X", getWindowDebugString().c_str(), event);

      if (event == EVT_KEY_BREAK(KEY_EXIT)) {
        moduleState[moduleIdx].mode = MODULE_MODE_NORMAL;
        deleteLater();
      }
    }
#endif

  protected:
    uint8_t moduleIdx;
    uint8_t receiverIdx;
};

class BindRxChoiceMenu: public Menu {
  public:
    BindRxChoiceMenu(uint8_t moduleIdx, uint8_t receiverIdx):
      Menu(),
      moduleIdx(moduleIdx),
      receiverIdx(receiverIdx)
    {
      uint8_t receiversCount = min<uint8_t>(reusableBuffer.moduleSetup.bindInformation.candidateReceiversCount, PXX2_MAX_RECEIVERS_PER_MODULE);
      for (uint8_t i = 0; i < receiversCount; i++) {
        const char * receiverName = reusableBuffer.moduleSetup.bindInformation.candidateReceiversNames[i];
        addLine(receiverName, [=]() {
            reusableBuffer.moduleSetup.bindInformation.selectedReceiverIndex = i;
            if (isModuleR9MAccess(moduleIdx) && reusableBuffer.moduleSetup.pxx2.moduleInformation.information.variant == PXX2_VARIANT_EU) {
              reusableBuffer.moduleSetup.bindInformation.step = BIND_RX_NAME_SELECTED;
//              POPUP_MENU_ADD_ITEM(STR_16CH_WITH_TELEMETRY);
//              POPUP_MENU_ADD_ITEM(STR_16CH_WITHOUT_TELEMETRY);
//              POPUP_MENU_START(onPXX2R9MBindModeMenu);
            }
            else if (isModuleR9MAccess(moduleIdx) && reusableBuffer.moduleSetup.pxx2.moduleInformation.information.variant == PXX2_VARIANT_FLEX) {
              reusableBuffer.moduleSetup.bindInformation.step = BIND_RX_NAME_SELECTED;
//              POPUP_MENU_ADD_ITEM(STR_FLEX_868);
//              POPUP_MENU_ADD_ITEM(STR_FLEX_915);
//              POPUP_MENU_START(onPXX2R9MBindModeMenu);
            }
            else {
#if defined(SIMU)
              memcpy(g_model.moduleData[moduleIdx].pxx2.receiverName[receiverIdx], receiverName, PXX2_LEN_RX_NAME);
              storageDirty(EE_MODEL);
              reusableBuffer.moduleSetup.bindInformation.step = BIND_OK;
              new MessageDialog(STR_BIND, STR_BIND_OK);
#else
              reusableBuffer.moduleSetup.bindInformation.step = BIND_START;
              new BindWaitDialog(moduleIdx, receiverIdx);
#endif
            }
        });
      }

      setCancelHandler([=]() {
          moduleState[moduleIdx].mode = MODULE_MODE_NORMAL;
      });
    }

  protected:
    uint8_t moduleIdx;
    uint8_t receiverIdx;
};

void BindWaitDialog::checkEvents()
{
  if (moduleState[moduleIdx].mode == MODULE_MODE_NORMAL) {
    removePXX2ReceiverIfEmpty(moduleIdx, receiverIdx);
    deleteLater();
    if (reusableBuffer.moduleSetup.bindInformation.step == BIND_OK)
      POPUP_INFORMATION(STR_BIND_OK);
    return;
  }

  if (reusableBuffer.moduleSetup.bindInformation.step == BIND_INIT && reusableBuffer.moduleSetup.bindInformation.candidateReceiversCount > 0) {
    deleteLater();
    new BindRxChoiceMenu(moduleIdx, receiverIdx);
    return;
  }

  Dialog::checkEvents();
}

class ReceiverButton: public TextButton {
  public:
    ReceiverButton(Window * parent, rect_t rect, uint8_t moduleIdx, uint8_t receiverIdx):
      TextButton(parent, rect, STR_BIND, [=]() {
          if (g_model.moduleData[moduleIdx].pxx2.receiverName[receiverIdx][0] == '\0') {
            startBind();
          }
          else {
            auto menu = new Menu();
            menu->addLine(STR_BIND, [=]() {
                startBind();
                return 0;
            });
            menu->addLine(STR_OPTIONS, [=]() {
                memclear(&reusableBuffer.hardwareAndSettings, sizeof(reusableBuffer.hardwareAndSettings));
                reusableBuffer.hardwareAndSettings.receiverSettings.receiverId = receiverIdx;
                // g_moduleIdx = moduleIdx;
                // pushMenu(menuModelReceiverOptions);
                return 0;
            });
            menu->addLine(STR_SHARE, [=]() {
                reusableBuffer.moduleSetup.pxx2.shareReceiverIndex = receiverIdx;
                moduleState[moduleIdx].mode = MODULE_MODE_SHARE;
                return 0;
            });
            menu->addLine(STR_DELETE, [=]() {
                memclear(&reusableBuffer.moduleSetup.pxx2, sizeof(reusableBuffer.moduleSetup.pxx2));
                reusableBuffer.moduleSetup.pxx2.resetReceiverIndex = receiverIdx;
                reusableBuffer.moduleSetup.pxx2.resetReceiverFlags = 0x01;
                new ConfirmDialog(STR_RECEIVER, STR_RECEIVER_DELETE, [=]() {
                    moduleState[moduleIdx].mode = MODULE_MODE_RESET;
                    removePXX2Receiver(moduleIdx, receiverIdx);
                });
                return 0;
            });
            menu->addLine(STR_RESET, [=]() {
                memclear(&reusableBuffer.moduleSetup.pxx2, sizeof(reusableBuffer.moduleSetup.pxx2));
                reusableBuffer.moduleSetup.pxx2.resetReceiverIndex = receiverIdx;
                reusableBuffer.moduleSetup.pxx2.resetReceiverFlags = 0xFF;
                new ConfirmDialog(STR_RECEIVER, STR_RECEIVER_DELETE, [=]() {
                    moduleState[moduleIdx].mode = MODULE_MODE_RESET;
                    removePXX2Receiver(moduleIdx, receiverIdx);
                });
                return 0;
            });
            menu->setCloseHandler([=]() {
                removePXX2ReceiverIfEmpty(moduleIdx, receiverIdx);
            });
          }
          return 0;
      }),
      moduleIdx(moduleIdx),
      receiverIdx(receiverIdx)
    {
    }

    void startBind()
    {
      memclear(&reusableBuffer.moduleSetup.bindInformation, sizeof(BindInformation));
      reusableBuffer.moduleSetup.bindInformation.rxUid = receiverIdx;
      if (isModuleR9MAccess(moduleIdx)) {
#if defined(SIMU)
        reusableBuffer.moduleSetup.pxx2.moduleInformation.information.modelID = 1;
        reusableBuffer.moduleSetup.pxx2.moduleInformation.information.variant = 2;
#else
        moduleState[moduleIdx].readModuleInformation(&reusableBuffer.moduleSetup.pxx2.moduleInformation, PXX2_HW_INFO_TX_ID, PXX2_HW_INFO_TX_ID);
#endif
      }
      else {
        moduleState[moduleIdx].startBind(&reusableBuffer.moduleSetup.bindInformation);
      }

      new BindWaitDialog(moduleIdx, receiverIdx);
    }

    void checkEvents() override
    {
      if (g_model.moduleData[moduleIdx].pxx2.receiverName[receiverIdx][0] != '\0') {
        char receiverName[PXX2_LEN_RX_NAME + 1];
        memset(receiverName, 0, sizeof(receiverName));
        strncpy(receiverName, g_model.moduleData[moduleIdx].pxx2.receiverName[receiverIdx],
                effectiveLen(g_model.moduleData[moduleIdx].pxx2.receiverName[receiverIdx], PXX2_LEN_RX_NAME));
        setText(receiverName);
      }
      else {
        setText(STR_BIND);
      }

      TextButton::checkEvents();
    }

  protected:
    uint8_t moduleIdx;
    uint8_t receiverIdx;
};

class ModuleWindow : public Window {
  public:
    ModuleWindow(Window * parent, const rect_t &rect, uint8_t moduleIdx) :
      Window(parent, rect, FORWARD_SCROLL),
      moduleIdx(moduleIdx)
    {
      update();
    }

    ~ModuleWindow()
    {
      deleteChildren();
    }

  protected:
    uint8_t moduleIdx;
    Choice * moduleChoice = nullptr;
    Choice * rfChoice = nullptr;
    FormField * lastField = nullptr; // moduleChoice is always the firstField
    TextButton * bindButton = nullptr;
    TextButton * rangeButton = nullptr;
    TextButton * registerButton = nullptr;
    Choice * failSafeChoice = nullptr;

    void addChannelRange(FormGridLayout &grid, uint8_t moduleIdx)
    {
      new StaticText(this, grid.getLabelSlot(true), STR_CHANNELRANGE);
      auto channelStart = new NumberEdit(this, grid.getFieldSlot(2, 0), 1,
                                         MAX_OUTPUT_CHANNELS - sentModuleChannels(moduleIdx) + 1,
                                         GET_DEFAULT(1 + g_model.moduleData[moduleIdx].channelsStart));
      auto channelEnd = new NumberEdit(this, grid.getFieldSlot(2, 1),
                                       g_model.moduleData[moduleIdx].channelsStart + minModuleChannels(moduleIdx),
                                       min<int8_t>(MAX_OUTPUT_CHANNELS, g_model.moduleData[moduleIdx].channelsStart + maxModuleChannels(moduleIdx)),
                                       GET_DEFAULT(g_model.moduleData[moduleIdx].channelsStart + 8 + g_model.moduleData[moduleIdx].channelsCount));
      if (isModulePXX2(moduleIdx)) {
        channelEnd->setAvailableHandler([=](int value) {
            return isPxx2IsrmChannelsCountAllowed(value - 8);
        });
      }
      channelStart->setPrefix(STR_CH);
      channelEnd->setPrefix(STR_CH);
      channelStart->setSetValueHandler([=](int32_t newValue) {
        g_model.moduleData[moduleIdx].channelsStart = newValue - 1;
        SET_DIRTY();
        channelEnd->setMin(g_model.moduleData[moduleIdx].channelsStart + minModuleChannels(moduleIdx));
        channelEnd->setMax(min<int8_t>(MAX_OUTPUT_CHANNELS, g_model.moduleData[moduleIdx].channelsStart + maxModuleChannels(moduleIdx)));
        channelEnd->invalidate();
      });
      channelEnd->setSetValueHandler([=](int32_t newValue) {
        g_model.moduleData[moduleIdx].channelsCount = newValue - g_model.moduleData[moduleIdx].channelsStart - 8;
        SET_DIRTY();
        channelStart->setMax(MAX_OUTPUT_CHANNELS - sentModuleChannels(moduleIdx) + 1);
      });
      channelEnd->enable(minModuleChannels(moduleIdx) < maxModuleChannels(moduleIdx));
    }

    void update()
    {
      FormGridLayout grid;
      grid.setLabelWidth(175);

      FormField * previousField = moduleChoice ? moduleChoice->getPreviousField() : moduleChoice->getCurrentField();
      FormField * nextField = lastField ? lastField->getNextField() : nullptr;

      clear();
      FormField::clearCurrentField();

      // Module Type
      new StaticText(this, grid.getLabelSlot(true), STR_MODE);
      moduleChoice = new Choice(this, grid.getFieldSlot(), STR_INTERNAL_MODULE_PROTOCOLS,
                                MODULE_TYPE_NONE, MODULE_TYPE_COUNT - 1,
                                GET_DEFAULT(g_model.moduleData[moduleIdx].type),
                                [=](int32_t newValue) {
                                  g_model.moduleData[moduleIdx].type = newValue;
                                  SET_DIRTY();
                                  // TODO resetModuleSettings(moduleIdx);
                                  update();
                                  moduleChoice->setFocus();
                                });
      moduleChoice->setAvailableHandler([=](int8_t moduleType) {
          return moduleIdx == INTERNAL_MODULE ? isInternalModuleAvailable(moduleType) : isExternalModuleAvailable(moduleType);
      });
      FormField::link(previousField, moduleChoice);

      // Module parameters
      if (isModuleXJT(moduleIdx)) {
        auto xjtChoice = new Choice(this, grid.getFieldSlot(2, 1), STR_XJT_ACCST_RF_PROTOCOLS, MODULE_SUBTYPE_PXX1_OFF, MODULE_SUBTYPE_PXX1_ACCST_LR12,
                                    GET_SET_DEFAULT(g_model.moduleData[moduleIdx].rfProtocol));
        xjtChoice->setAvailableHandler([](int index) {
          return index != MODULE_SUBTYPE_PXX1_OFF;
        });
      }
      else if (isModuleDSM2(moduleIdx)) {
        new Choice(this, grid.getFieldSlot(2, 1), STR_DSM_PROTOCOLS, DSM2_PROTO_LP45, DSM2_PROTO_DSMX,
                   GET_SET_DEFAULT(g_model.moduleData[moduleIdx].rfProtocol));
      }
      else if (isModuleR9M(moduleIdx)) {
        rfChoice = new Choice(this, grid.getFieldSlot(2, 1), STR_R9M_REGION, MODULE_SUBTYPE_R9M_FCC, MODULE_SUBTYPE_R9M_EU,
                              GET_DEFAULT(g_model.moduleData[moduleIdx].subType),
                              [=](int32_t newValue) {
                                  g_model.moduleData[moduleIdx].subType = newValue;
                                  SET_DIRTY();
                                  update();
                                  rfChoice->setFocus();
                              });
      }
      else if (isModulePXX2(moduleIdx)) {
        rfChoice = new Choice(this, grid.getFieldSlot(2, 1), STR_ISRM_RF_PROTOCOLS, 0, MODULE_SUBTYPE_ISRM_PXX2_ACCST_LR12,
                              GET_DEFAULT(g_model.moduleData[moduleIdx].subType),
                              [=](int32_t newValue) {
                                  g_model.moduleData[moduleIdx].subType = newValue;
                                  SET_DIRTY();
                                  update();
                                  rfChoice->setFocus();
                              });
      }
#if defined(MULTIMODULE)
      else if (isModuleMultimodule(moduleIdx)) {
        grid.nextLine();
        new StaticText(this, grid.getLabelSlot(true), STR_RF_PROTOCOL);

        // Multi type (CUSTOM, brand A, brand B,...)
        int multiRfProto = g_model.moduleData[moduleIdx].multi.customProto == 1 ? MODULE_SUBTYPE_MULTI_CUSTOM : g_model.moduleData[moduleIdx].getMultiProtocol(false);
        rfChoice = new Choice(this, grid.getFieldSlot(g_model.moduleData[moduleIdx].multi.customProto ? 3 : 2, 0), STR_MULTI_PROTOCOLS, MODULE_SUBTYPE_MULTI_FIRST, MODULE_SUBTYPE_MULTI_LAST,
                              GET_DEFAULT(multiRfProto),
                              [=](int32_t newValue) {
                                g_model.moduleData[moduleIdx].multi.customProto = (newValue == MODULE_SUBTYPE_MULTI_CUSTOM);
                                if (!g_model.moduleData[moduleIdx].multi.customProto)
                                  g_model.moduleData[moduleIdx].setMultiProtocol(newValue);
                                g_model.moduleData[moduleIdx].subType = 0;
                                // Sensible default for DSM2 (same as for ppm): 7ch@22ms + Autodetect settings enabled
                                if (g_model.moduleData[moduleIdx].getMultiProtocol(true) == MODULE_SUBTYPE_MULTI_DSM2) {
                                  g_model.moduleData[moduleIdx].multi.autoBindMode = 1;
                                }
                                else {
                                  g_model.moduleData[moduleIdx].multi.autoBindMode = 0;
                                }
                                g_model.moduleData[moduleIdx].multi.optionValue = 0;
                                SET_DIRTY();
                                update();
                                rfChoice->setFocus();
                              });

        if (g_model.moduleData[moduleIdx].multi.customProto) {
          // Proto column 1
          new NumberEdit(this, grid.getFieldSlot(3, 1), 0, 63,
                         GET_DEFAULT(g_model.moduleData[moduleIdx].getMultiProtocol(false)),
                         [=](int32_t newValue) {
                           g_model.moduleData[moduleIdx].setMultiProtocol(newValue);
                           SET_DIRTY();
                         });

          // Proto column 2
          new NumberEdit(this, grid.getFieldSlot(3, 2), 0, 7, GET_SET_DEFAULT(g_model.moduleData[moduleIdx].subType));
        }
        else {
          // Subtype (D16, DSMX,...)
          const mm_protocol_definition * pdef = getMultiProtocolDefinition(g_model.moduleData[moduleIdx].getMultiProtocol(false));
          if (pdef->maxSubtype > 0)
            new Choice(this, grid.getFieldSlot(2, 1), pdef->subTypeString, 0, pdef->maxSubtype,GET_SET_DEFAULT(g_model.moduleData[moduleIdx].subType));
        }
        grid.nextLine();

        // Multimodule status
        new StaticText(this, grid.getLabelSlot(true), STR_MODULE_STATUS);
        char statusText[64];
        multiModuleStatus.getStatusString(statusText);
        new StaticText(this, grid.getFieldSlot(), statusText);

        // Multimodule sync
        if(multiSyncStatus.isValid()) {
          grid.nextLine();
          new StaticText(this, grid.getLabelSlot(true), STR_MODULE_SYNC);
          multiSyncStatus.getRefreshString(statusText);
          new StaticText(this, grid.getFieldSlot(), statusText);
        }

        // Multi optional feature row
        const uint8_t multi_proto = g_model.moduleData[moduleIdx].getMultiProtocol(true);
        const mm_protocol_definition *pdef = getMultiProtocolDefinition(multi_proto);
        if (pdef->optionsstr) {
          grid.nextLine();
          new StaticText(this, grid.getLabelSlot(true), pdef->optionsstr);
          if (multi_proto == MODULE_SUBTYPE_MULTI_FS_AFHDS2A) {
            auto edit = new NumberEdit(this, grid.getFieldSlot(2,0), 50, 400,
                           GET_DEFAULT(50 + 5 * g_model.moduleData[moduleIdx].multi.optionValue),
                           SET_VALUE(g_model.moduleData[moduleIdx].multi.optionValue, (newValue- 50) / 5));
            edit->setStep(5);
          }
          else if (multi_proto == MODULE_SUBTYPE_MULTI_OLRS) {
            new NumberEdit(this, grid.getFieldSlot(2,0), -1, 7, GET_SET_DEFAULT(g_model.moduleData[moduleIdx].multi.optionValue));
          }
          else {
            new NumberEdit(this, grid.getFieldSlot(2,0), -128, 127, GET_SET_DEFAULT(g_model.moduleData[moduleIdx].multi.optionValue));
          }
        }

        // Bind on power up
        grid.nextLine();
        new StaticText(this, grid.getLabelSlot(true),g_model.moduleData[moduleIdx].getMultiProtocol(true) == MODULE_SUBTYPE_MULTI_DSM2 ? STR_MULTI_DSM_AUTODTECT : STR_MULTI_AUTOBIND);
        new CheckBox(this, grid.getFieldSlot(), GET_SET_DEFAULT(g_model.moduleData[moduleIdx].multi.autoBindMode));

        // Low power mode
        grid.nextLine();
        new StaticText(this, grid.getLabelSlot(true), STR_MULTI_LOWPOWER);
        new CheckBox(this, grid.getFieldSlot(), GET_SET_DEFAULT(g_model.moduleData[moduleIdx].multi.lowPowerMode));
      }
#endif
#if defined (PCBNV14)
      else if (isModuleFlysky(moduleIdx)) {
        grid.nextLine();
        rfChoice = new Choice(this, grid.getFieldSlot(), STR_FLYSKY_PROTOCOLS, 0, 3,
                   GET_DEFAULT(g_model.moduleData[moduleIdx].flysky.mode),
                   [=](int32_t newValue) -> void {
                     g_model.moduleData[moduleIdx].flysky.mode = newValue;
                     SET_DIRTY();
                     //TODO moduleFlagBackNormal(moduleIdx);
                     //TODO onFlySkyReceiverSetPulse(INTERNAL_MODULE, newValue);
                   });


        grid.nextLine();
        new StaticText(this, grid.getLabelSlot(true), STR_FLYSKY_TELEMETRY);
        new CheckBox(this, grid.getFieldSlot(), GET_SET_DEFAULT(g_model.rssiAlarms.flysky_telemetry));
      }
#endif
      grid.nextLine();

      // Channel Range
      if (g_model.moduleData[moduleIdx].type != MODULE_TYPE_NONE) {
        addChannelRange(grid, moduleIdx); //TODO XJT2 should only set channel count of 8/16/24
        grid.nextLine();
      }

      // PPM modules
      if (isModulePPM(moduleIdx)) {
        // PPM frame
        new StaticText(this, grid.getLabelSlot(true), STR_PPMFRAME);

        // PPM frame length
        auto edit = new NumberEdit(this, grid.getFieldSlot(2, 0), 125, 35 * 5 + 225,
                                   GET_DEFAULT(g_model.moduleData[moduleIdx].ppm.frameLength * 5 + 225),
                                   SET_VALUE(g_model.moduleData[moduleIdx].ppm.frameLength, (newValue - 225) / 5),
                                   PREC1);
        edit->setStep(5);
        edit->setSuffix(STR_MS);

        // PPM frame delay
        edit = new NumberEdit(this, grid.getFieldSlot(2, 1), 100, 800,
                              GET_DEFAULT(g_model.moduleData[moduleIdx].ppm.delay * 50 + 300),
                              SET_VALUE(g_model.moduleData[moduleIdx].ppm.delay, (newValue - 300) / 50));
        edit->setStep(50);
        edit->setSuffix("us");
        grid.nextLine();
      }

      // Module parameters

      // Bind and Range buttons
      if (!isModuleRFAccess(moduleIdx) && isModuleBindRangeAvailable(moduleIdx)) {
        uint8_t thirdColumn = 0;
        new StaticText(this, grid.getLabelSlot(true), STR_RECEIVER);

        // Model index
        if (isModuleModelIndexAvailable(moduleIdx)) {
          thirdColumn++;
          new NumberEdit(this, grid.getFieldSlot(3, 0), 0, getMaxRxNum(moduleIdx), GET_SET_DEFAULT(g_model.header.modelId[moduleIdx]));
        }

        bindButton = new TextButton(this, grid.getFieldSlot(2+thirdColumn, 0+thirdColumn), STR_MODULE_BIND);
        bindButton->setPressHandler([=]() -> uint8_t {
          if (moduleState[moduleIdx].mode == MODULE_MODE_RANGECHECK) {
            rangeButton->check(false);
          }
          if (moduleState[moduleIdx].mode == MODULE_MODE_BIND) {
            bindButton->setText(STR_MODULE_BIND);
            moduleState[moduleIdx].mode = MODULE_MODE_NORMAL;
            return 0;
          }
          else {
            bindButton->setText(STR_MODULE_BIND);
            moduleState[moduleIdx].mode = MODULE_MODE_BIND;
            return 1;
          }
        });
        bindButton->setCheckHandler([=]() {
          if (moduleState[moduleIdx].mode != MODULE_MODE_BIND) {
            bindButton->setText(STR_MODULE_BIND);
            bindButton->check(false);
          }
        });

        rangeButton = new TextButton(this, grid.getFieldSlot(2+thirdColumn, 1+thirdColumn), STR_MODULE_RANGE);
        rangeButton->setPressHandler([=]() -> uint8_t {
          if (moduleState[moduleIdx].mode == MODULE_MODE_BIND) {
            bindButton->setText(STR_MODULE_BIND);
            bindButton->check(false);
            moduleState[moduleIdx].mode = MODULE_MODE_NORMAL;
          }
          if (moduleState[moduleIdx].mode == MODULE_MODE_RANGECHECK) {
            moduleState[moduleIdx].mode = MODULE_MODE_NORMAL;
            return 0;
          }
          else {
            moduleState[moduleIdx].mode = MODULE_MODE_RANGECHECK;
            return 1;
          }
        });

        grid.nextLine();
      }

      // Failsafe
      if (isModuleFailsafeAvailable(moduleIdx)) {
        new StaticText(this, grid.getLabelSlot(true), STR_FAILSAFE);
        failSafeChoice = new Choice(this, grid.getFieldSlot(2, 0), STR_VFAILSAFE, 0, FAILSAFE_LAST,
                                    GET_DEFAULT(g_model.moduleData[moduleIdx].failsafeMode),
                                    [=](int32_t newValue) {
                                      g_model.moduleData[moduleIdx].failsafeMode = newValue;
                                      SET_DIRTY();
                                      update();
                                      failSafeChoice->setFocus();
                                    });
        if (g_model.moduleData[moduleIdx].failsafeMode == FAILSAFE_CUSTOM) {
          new TextButton(this, grid.getFieldSlot(2, 1), STR_SET,
                         [=]() -> uint8_t {
                           new FailSafeMenu(moduleIdx);
                           return 1;
                         });
        }
        grid.nextLine();
      }

      // Register and Range buttons
      if (isModuleRFAccess(moduleIdx)) {
        new StaticText(this, grid.getLabelSlot(true), STR_MODULE);
        registerButton = new TextButton(this, grid.getFieldSlot(2, 0), STR_REGISTER);
        registerButton->setPressHandler([=]() -> uint8_t {
            new RegisterDialog(moduleIdx);
            return 0;
        });

        rangeButton = new TextButton(this, grid.getFieldSlot(2, 1), STR_MODULE_RANGE);
        rangeButton->setPressHandler([=]() -> uint8_t {
            if (moduleState[moduleIdx].mode == MODULE_MODE_RANGECHECK) {
              moduleState[moduleIdx].mode = MODULE_MODE_NORMAL;
              return 0;
            }
            else {
              moduleState[moduleIdx].mode = MODULE_MODE_RANGECHECK;
              return 1;
            }
        });

        grid.nextLine();
      }

      // R9M Power
      if (isModuleR9M_FCC(moduleIdx)) {
        new StaticText(this, grid.getLabelSlot(true), STR_RFPOWER);
        new Choice(this, grid.getFieldSlot(), STR_R9M_FCC_POWER_VALUES, 0, R9M_FCC_POWER_MAX,
                   GET_SET_DEFAULT(g_model.moduleData[moduleIdx].pxx.power));
      }

      if (isModuleR9M_LBT(moduleIdx)) {
        new StaticText(this, grid.getLabelSlot(true), STR_RFPOWER);
        new Choice(this, grid.getFieldSlot(), STR_R9M_LBT_POWER_VALUES, 0, R9M_LBT_POWER_MAX,
                   GET_DEFAULT(min<uint8_t>(g_model.moduleData[moduleIdx].pxx.power, R9M_LBT_POWER_MAX)),
                   SET_DEFAULT(g_model.moduleData[moduleIdx].pxx.power));
      }

      // Receivers
      if (isModulePXX2(moduleIdx)) {
        for (uint8_t receiverIdx = 0; receiverIdx < PXX2_MAX_RECEIVERS_PER_MODULE; receiverIdx++) {
          char label[] = TR_RECEIVER " X";
          label[sizeof(label) - 2] = '1' + receiverIdx;
          new StaticText(this, grid.getLabelSlot(true), label);
          new ReceiverButton(this, grid.getFieldSlot(2, 0), moduleIdx, receiverIdx);
          grid.nextLine();
        }
      }

      getParent()->moveWindowsTop(top(), adjustHeight());
      getParent()->invalidate(); // TODO should be automatically done

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

  moduleState[moduleIdx].mode  = MODULE_MODE_BIND;
}

void ModelSetupPage::build(FormWindow * window)
{
  FormGridLayout grid;
  grid.spacer(PAGE_PADDING);

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

    auto group = new FormGroup(window, grid.getFieldSlot(), BORDER_FOCUS_ONLY | PAINT_CHILDREN_FIRST);
    GridLayout timerGrid(group);

    // Timer name
    new StaticText(window, grid.getLabelSlot(true), STR_NAME);
    grid.nextLine();
    auto nameEdit = new TextEdit(group, timerGrid.getSlot(), timer->name, LEN_TIMER_NAME);
    group->setFirstField(nameEdit);
    timerGrid.nextLine();

    // Timer mode
    new StaticText(window, grid.getLabelSlot(true), STR_MODE);
    grid.nextLine();
    new Choice(group, timerGrid.getSlot(2, 0), "\011""OFF\0     ""ON\0      ""Start\0   ""Thr\0     ""Thr%\0    ""Thr Start", 0, TMRMODE_MAX, GET_SET_DEFAULT(timer->mode));
    new SwitchChoice(group, timerGrid.getSlot(2, 1), SWSRC_FIRST, SWSRC_LAST, GET_SET_DEFAULT(timer->swtch));
    timerGrid.nextLine();

    // Timer start value
    new StaticText(window, grid.getLabelSlot(true), "Start");
    grid.nextLine();
    new TimeEdit(group, timerGrid.getSlot(), 0, TIMER_MAX, GET_SET_DEFAULT(timer->start));
    timerGrid.nextLine();

    // Timer minute beep
    new StaticText(window, grid.getLabelSlot(true), STR_MINUTEBEEP);
    grid.nextLine();
    new CheckBox(group, timerGrid.getSlot(), GET_SET_DEFAULT(timer->minuteBeep));
    timerGrid.nextLine();

    // Timer countdown
    new StaticText(window, grid.getLabelSlot(true), STR_BEEPCOUNTDOWN);
    grid.nextLine();
    new Choice(group, timerGrid.getSlot(2, 0), STR_VBEEPCOUNTDOWN, COUNTDOWN_SILENT, COUNTDOWN_COUNT - 1, GET_SET_DEFAULT(timer->countdownBeep));
    new Choice(group, timerGrid.getSlot(2, 1), STR_COUNTDOWNVALUES, 0, 3, GET_SET_WITH_OFFSET(timer->countdownStart, 2));
    timerGrid.nextLine();

    // Timer persistent
    new StaticText(window, grid.getLabelSlot(true), STR_PERSISTENT);
    grid.nextLine();
    auto persistentChoice = new Choice(group, timerGrid.getSlot(), STR_VPERSISTENT, 0, 2, GET_SET_DEFAULT(timer->persistent));
    group->setLastField(persistentChoice);
    timerGrid.nextLine();

    coord_t height = timerGrid.getWindowHeight() - 1;
    group->setHeight(height);
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
    auto group = new FormGroup(window, grid.getFieldSlot(), BORDER_FOCUS_ONLY | PAINT_CHILDREN_FIRST);
    GridLayout switchesGrid(group);
    for (int i = 0; i < NUM_SWITCHES; i++) {
      char s[SWITCH_WARNING_STR_SIZE];
      if (i > 0 && (i % 3) == 0)
        switchesGrid.nextLine();
      auto button = new TextButton(group, switchesGrid.getSlot(3, i % 3), getSwitchWarningString(s, i), nullptr,
                                   (bfGet(g_model.switchWarningState, 3 * i, 3) == 0 ? 0 : BUTTON_CHECKED));
      button->setPressHandler([button, i] {
          swarnstate_t newstate = bfGet(g_model.switchWarningState, 3 * i, 3);
          if (newstate == 1 && SWITCH_CONFIG(i) != SWITCH_3POS)
            newstate = 3;
          else
            newstate = (newstate + 1) % 4;
          g_model.switchWarningState = bfSet(g_model.switchWarningState, newstate, 3 * i, 3);
          SET_DIRTY();
          button->setText(getSwitchWarningString(i));
          return newstate > 0;
      });
      if (i == 0)
        group->setFirstField(button);
      else if (i == NUM_SWITCHES - 1)
        group->setLastField(button);
    }
    grid.addWindow(group);
  }

  // Center beeps
  {
    new StaticText(window, grid.getLabelSlot(false), STR_BEEPCTR);
    auto group = new FormGroup(window, grid.getFieldSlot(), BORDER_FOCUS_ONLY | PAINT_CHILDREN_FIRST);
    GridLayout centerGrid(group);
    for (int i = 0; i < NUM_STICKS + NUM_POTS + NUM_SLIDERS; i++) {
      char s[2];
      if (i > 0 && (i % 6) == 0)
        centerGrid.nextLine();

      auto button = new TextButton(group, centerGrid.getSlot(6, i % 6), getStringAtIndex(s, STR_RETA123, i),
                                   [=]() -> uint8_t {
                                       BFBIT_FLIP(g_model.beepANACenter, bfBit<BeepANACenter>(i));
                                       SET_DIRTY();
                                       return bfSingleBitGet<BeepANACenter>(g_model.beepANACenter, i);
                                   },
                                   bfSingleBitGet(g_model.beepANACenter, i) ? BUTTON_CHECKED : 0);
      if (i == 0)
        group->setFirstField(button);
      else if (i == NUM_STICKS + NUM_POTS + NUM_SLIDERS - 1)
        group->setLastField(button);
    }
    grid.addWindow(group);
  }

  // Global functions
  new StaticText(window, grid.getLabelSlot(), STR_USE_GLOBAL_FUNCS);
  new CheckBox(window, grid.getFieldSlot(), GET_SET_INVERTED(g_model.noGlobalFunctions));
  grid.nextLine();

  // Internal module
  {
    new Subtitle(window, grid.getLineSlot(), TR_INTERNALRF);
    grid.nextLine();
    grid.addWindow(new ModuleWindow(window, {0, grid.getWindowHeight(), LCD_W, 0}, INTERNAL_MODULE));
  }

  // External module
  {
    new Subtitle(window, grid.getLineSlot(), TR_EXTERNALRF);
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

