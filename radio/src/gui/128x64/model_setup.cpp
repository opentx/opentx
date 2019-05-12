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

#include <opentx.h>

// TODO find why we need this (for REGISTER at least)
#if defined(PCBXLITE)
  #define EVT_BUTTON_PRESSED() EVT_KEY_FIRST(KEY_ENTER)
#else
  #define EVT_BUTTON_PRESSED() EVT_KEY_BREAK(KEY_ENTER)
#endif

uint8_t g_moduleIdx;

#if defined(PCBTARANIS)
uint8_t getSwitchWarningsCount()
{
  int count = 0;
  for (int i=0; i<NUM_SWITCHES; ++i) {
    if (SWITCH_WARNING_ALLOWED(i)) {
      ++count;
    }
  }
  return count;
}
#endif

enum MenuModelSetupItems {
  ITEM_MODEL_NAME,
  ITEM_MODEL_TIMER1,
  ITEM_MODEL_TIMER1_NAME,
  ITEM_MODEL_TIMER1_PERSISTENT,
  ITEM_MODEL_TIMER1_MINUTE_BEEP,
  ITEM_MODEL_TIMER1_COUNTDOWN_BEEP,
  ITEM_MODEL_TIMER2,
  ITEM_MODEL_TIMER2_NAME,
  ITEM_MODEL_TIMER2_PERSISTENT,
  ITEM_MODEL_TIMER2_MINUTE_BEEP,
  ITEM_MODEL_TIMER2_COUNTDOWN_BEEP,
  ITEM_MODEL_TIMER3,
  ITEM_MODEL_TIMER3_NAME,
  ITEM_MODEL_TIMER3_PERSISTENT,
  ITEM_MODEL_TIMER3_MINUTE_BEEP,
  ITEM_MODEL_TIMER3_COUNTDOWN_BEEP,
  ITEM_MODEL_EXTENDED_LIMITS,
  ITEM_MODEL_EXTENDED_TRIMS,
  ITEM_MODEL_DISPLAY_TRIMS,
  ITEM_MODEL_TRIM_INC,
  ITEM_MODEL_THROTTLE_REVERSED,
  ITEM_MODEL_THROTTLE_TRACE,
  ITEM_MODEL_THROTTLE_TRIM,
  ITEM_MODEL_PREFLIGHT_LABEL,
  ITEM_MODEL_CHECKLIST_DISPLAY,
  ITEM_MODEL_THROTTLE_WARNING,
  ITEM_MODEL_SWITCHES_WARNING,
#if defined(PCBTARANIS)
  ITEM_MODEL_SWITCHES_WARNING2,
  ITEM_MODEL_POTS_WARNING,
#endif
  ITEM_MODEL_BEEP_CENTER,
  ITEM_MODEL_USE_GLOBAL_FUNCTIONS,
  ITEM_MODEL_REGISTRATION_ID,
#if defined(PCBTARANIS)
  ITEM_MODEL_INTERNAL_MODULE_LABEL,
  ITEM_MODEL_INTERNAL_MODULE_MODE,
  ITEM_MODEL_INTERNAL_MODULE_CHANNELS,
  ITEM_MODEL_INTERNAL_MODULE_NPXX2_BIND,
  ITEM_MODEL_INTERNAL_MODULE_PXX2_MODEL_NUM,
#if defined(EXTERNAL_ANTENNA)
  ITEM_MODEL_INTERNAL_MODULE_ANTENNA,
#endif
  ITEM_MODEL_INTERNAL_MODULE_FAILSAFE,
  ITEM_MODEL_INTERNAL_MODULE_PXX2_REGISTER_RANGE,
  ITEM_MODEL_INTERNAL_MODULE_PXX2_OPTIONS,
  ITEM_MODEL_INTERNAL_MODULE_PXX2_RECEIVER_1,
  ITEM_MODEL_INTERNAL_MODULE_PXX2_RECEIVER_2,
  ITEM_MODEL_INTERNAL_MODULE_PXX2_RECEIVER_3,
#endif
  ITEM_MODEL_EXTERNAL_MODULE_LABEL,
  ITEM_MODEL_EXTERNAL_MODULE_MODE,
#if defined(MULTIMODULE)
  ITEM_MODEL_EXTERNAL_MODULE_SUBTYPE,
  ITEM_MODEL_EXTERNAL_MODULE_STATUS,
  ITEM_MODEL_EXTERNAL_MODULE_SYNCSTATUS,
#endif
  ITEM_MODEL_EXTERNAL_MODULE_CHANNELS,
  ITEM_MODEL_EXTERNAL_MODULE_NPXX2_BIND,
  ITEM_MODEL_EXTERNAL_MODULE_PXX2_MODEL_NUM,
#if defined(PCBSKY9X) && defined(REVX)
  ITEM_MODEL_EXTERNAL_MODULE_OUTPUT_TYPE,
#endif
  ITEM_MODEL_EXTERNAL_MODULE_OPTIONS,
#if defined(MULTIMODULE)
  ITEM_MODEL_EXTERNAL_MODULE_AUTOBIND,
#endif
  ITEM_MODEL_EXTERNAL_MODULE_POWER,
  ITEM_MODEL_EXTERNAL_MODULE_FAILSAFE,
  ITEM_MODEL_EXTERNAL_MODULE_PXX2_REGISTER_RANGE,
  ITEM_MODEL_EXTERNAL_MODULE_PXX2_OPTIONS,
  ITEM_MODEL_EXTERNAL_MODULE_PXX2_RECEIVER_1,
  ITEM_MODEL_EXTERNAL_MODULE_PXX2_RECEIVER_2,
  ITEM_MODEL_EXTERNAL_MODULE_PXX2_RECEIVER_3,
#if defined(PCBSKY9X) && !defined(REVA)
  ITEM_MODEL_EXTRA_MODULE_LABEL,
  ITEM_MODEL_EXTRA_MODULE_CHANNELS,
  ITEM_MODEL_EXTRA_MODULE_BIND,
#endif
#if defined(PCBTARANIS)
  ITEM_MODEL_TRAINER_LABEL,
  ITEM_MODEL_TRAINER_MODE,
#endif
#if defined(PCBX7) || defined(PCBX9LITE)
  #if defined(BLUETOOTH)
    ITEM_MODEL_TRAINER_BLUETOOTH,
  #endif
  ITEM_MODEL_TRAINER_CHANNELS,
  ITEM_MODEL_TRAINER_PARAMS,
#elif defined(PCBXLITE)
  ITEM_MODEL_TRAINER_BLUETOOTH,
  ITEM_MODEL_TRAINER_CHANNELS,
  ITEM_MODEL_TRAINER_PARAMS,
#endif
  ITEM_MODEL_SETUP_MAX
};

#if defined(PCBSKY9X)
  #define FIELD_PROTOCOL_MAX             2
#else
  #define FIELD_PROTOCOL_MAX             1
#endif

#define MODEL_SETUP_2ND_COLUMN           (LCD_W-11*FW)
#define MODEL_SETUP_BIND_OFS             2*FW+1
#define MODEL_SETUP_RANGE_OFS            4*FW+3
#define MODEL_SETUP_SET_FAILSAFE_OFS     7*FW-2

#define IF_PXX2_MODULE(module, xxx)      (isModulePXX2(module) ? (uint8_t)(xxx) : HIDDEN_ROW)
#define IF_NOT_PXX2_MODULE(module, xxx)  (isModulePXX2(module) ? HIDDEN_ROW : (uint8_t)(xxx))

#if defined(PCBTARANIS)
  #define CURRENT_MODULE_EDITED(k)        (k >= ITEM_MODEL_EXTERNAL_MODULE_LABEL ? EXTERNAL_MODULE : INTERNAL_MODULE)
  #define CURRENT_RECEIVER_EDITED(k)      (k - (k >= ITEM_MODEL_EXTERNAL_MODULE_LABEL ? ITEM_MODEL_EXTERNAL_MODULE_PXX2_RECEIVER_1 : ITEM_MODEL_INTERNAL_MODULE_PXX2_RECEIVER_1))
#elif defined(PCBSKY9X) && !defined(REVA)
  #define CURRENT_MODULE_EDITED(k)       (k >= ITEM_MODEL_EXTRA_MODULE_LABEL ? EXTRA_MODULE : EXTERNAL_MODULE)
#else
  #define CURRENT_MODULE_EDITED(k)       (EXTERNAL_MODULE)
#endif

#if defined(PCBXLITE)
  #define SW_WARN_ROWS                    uint8_t(NAVIGATION_LINE_BY_LINE|getSwitchWarningsCount()), uint8_t(getSwitchWarningsCount() > 4 ? TITLE_ROW : HIDDEN_ROW) // X-Lite needs an additional column for full line selection (<])
#else
  #define SW_WARN_ROWS                    uint8_t(NAVIGATION_LINE_BY_LINE|(getSwitchWarningsCount()-1)), uint8_t(getSwitchWarningsCount() > 5 ? TITLE_ROW : HIDDEN_ROW)
#endif

#define INTERNAL_MODULE_MODE_ROWS       (isModuleXJT(INTERNAL_MODULE) ? (uint8_t)1 : (uint8_t)0) // Module type + RF protocols
#define IF_INTERNAL_MODULE_ON(x)       (IS_INTERNAL_MODULE_ENABLED()? (uint8_t)(x) : HIDDEN_ROW)
#define IF_EXTERNAL_MODULE_ON(x)       (IS_EXTERNAL_MODULE_ENABLED()? (uint8_t)(x) : HIDDEN_ROW)
#define EXTERNAL_MODULE_BIND_ROWS()    ((isModuleXJT(EXTERNAL_MODULE) && IS_D8_RX(EXTERNAL_MODULE)) || isModuleSBUS(EXTERNAL_MODULE)) ? (uint8_t)1 : (isModulePPM(EXTERNAL_MODULE) || isModulePXX(EXTERNAL_MODULE) || isModuleDSM2(EXTERNAL_MODULE) || isModuleMultimodule(EXTERNAL_MODULE)) ? (uint8_t)2 : HIDDEN_ROW

#if defined(PCBSKY9X) && defined(REVX)
  #define OUTPUT_TYPE_ROWS()             (isModulePPM(EXTERNAL_MODULE) ? (uint8_t)0 : HIDDEN_ROW) ,
#elif defined(PCBSKY9X)
  #define OUTPUT_TYPE_ROWS()
#endif

#define PORT_CHANNELS_ROWS(x)          (x==EXTERNAL_MODULE ? EXTERNAL_MODULE_CHANNELS_ROWS : 0)

#define EXTERNAL_MODULE_MODE_ROWS      (isModuleXJT(EXTERNAL_MODULE) || isModuleR9M(EXTERNAL_MODULE) || isModuleDSM2(EXTERNAL_MODULE) || isModuleMultimodule(EXTERNAL_MODULE)) ? (uint8_t)1 : (uint8_t)0

#define CURSOR_ON_CELL                 (true)
#define MODEL_SETUP_MAX_LINES          (HEADER_LINE+ITEM_MODEL_SETUP_MAX)
#define POT_WARN_ITEMS()               ((g_model.potsWarnMode) ? (uint8_t)(NUM_POTS+NUM_SLIDERS) : (uint8_t)0)
#define TIMER_ROWS                     2, 0, 0, 0, 0

#if defined(PCBSKY9X) && !defined(REVA)
  #define EXTRA_MODULE_ROWS              LABEL(ExtraModule), 1, 2,
#else
  #define EXTRA_MODULE_ROWS
#endif

#if defined(PCBX7) || defined(PCBX9LITE)
  #define ANTENNA_ROW
  #if defined(BLUETOOTH)
    #define TRAINER_BLUETOOTH_M_ROW      ((bluetooth.distantAddr[0] == '\0' || bluetooth.state == BLUETOOTH_STATE_CONNECTED) ? (uint8_t)0 : (uint8_t)1)
    #define TRAINER_BLUETOOTH_S_ROW      (bluetooth.distantAddr[0] == '\0' ? HIDDEN_ROW : LABEL())
    #define TRAINER_BLUETOOTH_ROW        (g_model.trainerData.mode == TRAINER_MODE_MASTER_BLUETOOTH ? TRAINER_BLUETOOTH_M_ROW : (g_model.trainerData.mode == TRAINER_MODE_SLAVE_BLUETOOTH ? TRAINER_BLUETOOTH_S_ROW : HIDDEN_ROW)),
  #else
    #define TRAINER_BLUETOOTH_ROW
  #endif
  #define TRAINER_CHANNELS_ROW           (IS_SLAVE_TRAINER() ? (uint8_t)1 : HIDDEN_ROW)
  #define TRAINER_PARAMS_ROW             (IS_SLAVE_TRAINER() ? (uint8_t)2 : HIDDEN_ROW)
  #define TRAINER_ROWS                   LABEL(Trainer), 0, TRAINER_BLUETOOTH_ROW TRAINER_CHANNELS_ROW, TRAINER_PARAMS_ROW
#elif defined(PCBXLITES)
  #define ANTENNA_ROW                    IF_NOT_PXX2_MODULE(INTERNAL_MODULE, IF_INTERNAL_MODULE_ON(0)),
  #define IF_BT_TRAINER_ON(x)            (g_eeGeneral.bluetoothMode == BLUETOOTH_TRAINER ? (uint8_t)(x) : HIDDEN_ROW)
  #define TRAINER_BLUETOOTH_M_ROW        ((bluetooth.distantAddr[0] == '\0' || bluetooth.state == BLUETOOTH_STATE_CONNECTED) ? (uint8_t)0 : (uint8_t)1)
  #define TRAINER_BLUETOOTH_S_ROW        (bluetooth.distantAddr[0] == '\0' ? HIDDEN_ROW : LABEL())
  #define TRAINER_BLUETOOTH_ROW          (g_model.trainerData.mode == TRAINER_MODE_MASTER_BLUETOOTH ? TRAINER_BLUETOOTH_M_ROW : (g_model.trainerData.mode == TRAINER_MODE_SLAVE_BLUETOOTH ? TRAINER_BLUETOOTH_S_ROW : HIDDEN_ROW))
  #define TRAINER_CHANNELS_ROW           (IS_SLAVE_TRAINER() ? (uint8_t)1 : HIDDEN_ROW)
  #define TRAINER_PARAMS_ROW             (IS_SLAVE_TRAINER() ? (uint8_t)2 : HIDDEN_ROW)
  #define TRAINER_ROWS                   LABEL(Trainer), 0, IF_BT_TRAINER_ON(TRAINER_BLUETOOTH_ROW), TRAINER_CHANNELS_ROW, TRAINER_PARAMS_ROW
#elif defined(PCBXLITE)
  #define ANTENNA_ROW                    IF_NOT_PXX2_MODULE(INTERNAL_MODULE, IF_INTERNAL_MODULE_ON(0)),
  #define IF_BT_TRAINER_ON(x)            (g_eeGeneral.bluetoothMode == BLUETOOTH_TRAINER ? (uint8_t)(x) : HIDDEN_ROW)
  #define TRAINER_BLUETOOTH_M_ROW        ((bluetooth.distantAddr[0] == '\0' || bluetooth.state == BLUETOOTH_STATE_CONNECTED) ? (uint8_t)0 : (uint8_t)1)
  #define TRAINER_BLUETOOTH_S_ROW        (bluetooth.distantAddr[0] == '\0' ? HIDDEN_ROW : LABEL())
  #define TRAINER_BLUETOOTH_ROW          (g_model.trainerData.mode == TRAINER_MODE_MASTER_BLUETOOTH ? TRAINER_BLUETOOTH_M_ROW : (g_model.trainerData.mode == TRAINER_MODE_SLAVE_BLUETOOTH ? TRAINER_BLUETOOTH_S_ROW : HIDDEN_ROW))
  #define TRAINER_CHANNELS_ROW           (IS_SLAVE_TRAINER() ? (uint8_t)1 : HIDDEN_ROW)
  #define TRAINER_ROWS                   IF_BT_TRAINER_ON(LABEL(Trainer)), IF_BT_TRAINER_ON(0), IF_BT_TRAINER_ON(TRAINER_BLUETOOTH_ROW), IF_BT_TRAINER_ON(TRAINER_CHANNELS_ROW)
#else
  #define TRAINER_ROWS
#endif

#if defined(PXX2)
bool isPXX2ReceiverEmpty(uint8_t moduleIdx, uint8_t receiverIdx)
{
  return is_memclear(g_model.moduleData[moduleIdx].pxx2.receiverName[receiverIdx], PXX2_LEN_RX_NAME);
}

void removePXX2Receiver(uint8_t moduleIdx, uint8_t receiverIdx)
{
  memclear(g_model.moduleData[moduleIdx].pxx2.receiverName[receiverIdx], PXX2_LEN_RX_NAME);
  g_model.moduleData[moduleIdx].pxx2.receivers &= ~(1 << receiverIdx);
  storageDirty(EE_MODEL);
}

void removePXX2ReceiverIfEmpty(uint8_t moduleIdx, uint8_t receiverIdx)
{
  if (isPXX2ReceiverEmpty(moduleIdx, receiverIdx)) {
    removePXX2Receiver(moduleIdx, receiverIdx);
  }
}

void onPXX2R9MBindModeMenu(const char * result)
{
  if (result == STR_8CH_WITH_TELEMETRY) {
    reusableBuffer.moduleSetup.bindInformation.lbtMode = 0;
  }
  else if (result == STR_16CH_WITH_TELEMETRY) {
    reusableBuffer.moduleSetup.bindInformation.lbtMode = 1;
  }
  else if (result == STR_16CH_WITHOUT_TELEMETRY) {
    reusableBuffer.moduleSetup.bindInformation.lbtMode = 2;
  }
  else if (result == STR_FLEX_868) {
    reusableBuffer.moduleSetup.bindInformation.flexMode = 0;
  }
  else if (result == STR_FLEX_915) {
    reusableBuffer.moduleSetup.bindInformation.flexMode = 1;
  }
  else {
    // the user pressed [Exit]
    uint8_t moduleIdx = CURRENT_MODULE_EDITED(menuVerticalPosition - HEADER_LINE);
    uint8_t receiverIdx = CURRENT_RECEIVER_EDITED(menuVerticalPosition - HEADER_LINE);
    moduleState[moduleIdx].mode = MODULE_MODE_NORMAL;
    removePXX2ReceiverIfEmpty(moduleIdx, receiverIdx);
    return;
  }

#if defined(SIMU)
  uint8_t moduleIdx = CURRENT_MODULE_EDITED(menuVerticalPosition - HEADER_LINE);
  uint8_t receiverIdx = CURRENT_RECEIVER_EDITED(menuVerticalPosition - HEADER_LINE);
  memcpy(g_model.moduleData[moduleIdx].pxx2.receiverName[receiverIdx], reusableBuffer.moduleSetup.bindInformation.candidateReceiversNames[reusableBuffer.moduleSetup.bindInformation.selectedReceiverIndex], PXX2_LEN_RX_NAME);
  storageDirty(EE_MODEL);
  moduleState[moduleIdx].mode = MODULE_MODE_NORMAL;
  reusableBuffer.moduleSetup.bindInformation.step = BIND_OK;
  POPUP_INFORMATION(STR_BIND_OK);
#else
  reusableBuffer.moduleSetup.bindInformation.step = BIND_START;
#endif
}

void onPXX2BindMenu(const char * result)
{
  if (result != STR_EXIT) {
    uint8_t moduleIdx = CURRENT_MODULE_EDITED(menuVerticalPosition - HEADER_LINE);
    reusableBuffer.moduleSetup.bindInformation.selectedReceiverIndex = (result - reusableBuffer.moduleSetup.bindInformation.candidateReceiversNames[0]) / sizeof(reusableBuffer.moduleSetup.bindInformation.candidateReceiversNames[0]);
    if (isModuleR9M2(moduleIdx) && reusableBuffer.moduleSetup.pxx2.moduleInformation.information.variant == PXX2_VARIANT_EU) {
      reusableBuffer.moduleSetup.bindInformation.step = BIND_RX_NAME_SELECTED;
      POPUP_MENU_ADD_ITEM(STR_8CH_WITH_TELEMETRY);
      POPUP_MENU_ADD_ITEM(STR_16CH_WITH_TELEMETRY);
      POPUP_MENU_ADD_ITEM(STR_16CH_WITHOUT_TELEMETRY);
      POPUP_MENU_START(onPXX2R9MBindModeMenu);
    }
    else if (isModuleR9M2(moduleIdx) && reusableBuffer.moduleSetup.pxx2.moduleInformation.information.variant == PXX2_VARIANT_FLEX) {
      reusableBuffer.moduleSetup.bindInformation.step = BIND_RX_NAME_SELECTED;
      POPUP_MENU_ADD_ITEM(STR_FLEX_868);
      POPUP_MENU_ADD_ITEM(STR_FLEX_915);
      POPUP_MENU_START(onPXX2R9MBindModeMenu);
    }
    else {
#if defined(SIMU)
      uint8_t receiverIdx = CURRENT_RECEIVER_EDITED(menuVerticalPosition - HEADER_LINE);
      memcpy(g_model.moduleData[moduleIdx].pxx2.receiverName[receiverIdx], result, PXX2_LEN_RX_NAME);
      storageDirty(EE_MODEL);
      moduleState[moduleIdx].mode = MODULE_MODE_NORMAL;
      reusableBuffer.moduleSetup.bindInformation.step = BIND_OK;
      POPUP_INFORMATION(STR_BIND_OK);
#else
      reusableBuffer.moduleSetup.bindInformation.step = BIND_START;
#endif
    }
  }
  else {
    // the user pressed [Exit]
    uint8_t moduleIdx = CURRENT_MODULE_EDITED(menuVerticalPosition - HEADER_LINE);
    uint8_t receiverIdx = CURRENT_RECEIVER_EDITED(menuVerticalPosition - HEADER_LINE);
    moduleState[moduleIdx].mode = MODULE_MODE_NORMAL;
    removePXX2ReceiverIfEmpty(moduleIdx, receiverIdx);
  }
}

void onResetReceiverConfirm(const char * result)
{
  if (result == STR_OK) {
    uint8_t moduleIdx = CURRENT_MODULE_EDITED(menuVerticalPosition - HEADER_LINE);
    uint8_t receiverIdx = CURRENT_RECEIVER_EDITED(menuVerticalPosition - HEADER_LINE);
    moduleState[moduleIdx].mode = MODULE_MODE_RESET;
    removePXX2Receiver(moduleIdx, receiverIdx);
  }
}

void onPXX2ReceiverMenu(const char * result)
{
  uint8_t moduleIdx = CURRENT_MODULE_EDITED(menuVerticalPosition - HEADER_LINE);
  uint8_t receiverIdx = CURRENT_RECEIVER_EDITED(menuVerticalPosition - HEADER_LINE);

  if (result == STR_OPTIONS) {
    memclear(&reusableBuffer.hardwareAndSettings, sizeof(reusableBuffer.hardwareAndSettings));
    reusableBuffer.hardwareAndSettings.receiverSettings.receiverId = receiverIdx;
    g_moduleIdx = moduleIdx;
    pushMenu(menuModelReceiverOptions);
  }
  else if (result == STR_BIND) {
    memclear(&reusableBuffer.moduleSetup.bindInformation, sizeof(BindInformation));
    reusableBuffer.moduleSetup.bindInformation.rxUid = receiverIdx;
    if (isModuleR9M2(moduleIdx)) {
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
    s_editMode = 1;
  }
  else if (result == STR_SHARE) {
    reusableBuffer.moduleSetup.pxx2.shareReceiverIndex = receiverIdx;
    moduleState[moduleIdx].mode = MODULE_MODE_SHARE;
    s_editMode = 1;
  }
  else if (result == STR_DELETE || result == STR_RESET) {
    memclear(&reusableBuffer.moduleSetup.pxx2, sizeof(reusableBuffer.moduleSetup.pxx2));
    reusableBuffer.moduleSetup.pxx2.resetReceiverIndex = receiverIdx;
    reusableBuffer.moduleSetup.pxx2.resetReceiverFlags = (result == STR_RESET ? 0xFF : 0x01);
    POPUP_CONFIRMATION(result == STR_RESET ? STR_RECEIVER_RESET : STR_RECEIVER_DELETE, onResetReceiverConfirm);
  }
  else {
    removePXX2ReceiverIfEmpty(moduleIdx, receiverIdx);
  }
}
#endif

void onBindMenu(const char * result)
{
  uint8_t moduleIdx = CURRENT_MODULE_EDITED(menuVerticalPosition - HEADER_LINE);

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

  moduleState[moduleIdx].mode = MODULE_MODE_BIND;
}

enum PopupRegisterItems {
  ITEM_REGISTER_PASSWORD,
  ITEM_REGISTER_MODULE_INDEX,
  ITEM_REGISTER_RECEIVER_NAME,
  ITEM_REGISTER_BUTTONS
};

void runPopupRegister(event_t event)
{
  uint8_t backupVerticalPosition = menuVerticalPosition;
  uint8_t backupHorizontalPosition = menuHorizontalPosition;
  uint8_t backupVerticalOffset = menuVerticalOffset;
  int8_t backupEditMode = s_editMode;

  menuVerticalPosition = reusableBuffer.moduleSetup.pxx2.registerPopupVerticalPosition;
  menuHorizontalPosition = reusableBuffer.moduleSetup.pxx2.registerPopupHorizontalPosition;
  s_editMode = reusableBuffer.moduleSetup.pxx2.registerPopupEditMode;

  switch (event) {
    case EVT_KEY_BREAK(KEY_ENTER):
      if (menuVerticalPosition != ITEM_REGISTER_BUTTONS) {
        break;
      }
      else if (reusableBuffer.moduleSetup.pxx2.registerStep >= REGISTER_RX_NAME_RECEIVED && menuHorizontalPosition == 0) {
        // [Enter] pressed
        reusableBuffer.moduleSetup.pxx2.registerStep = REGISTER_RX_NAME_SELECTED;
        backupEditMode = EDIT_MODIFY_FIELD; // so that the [Register] button blinks and the REGISTER process can continue
      }
      // no break

    case EVT_KEY_LONG(KEY_EXIT):
      s_editMode = 0;
      // no break;

    case EVT_KEY_BREAK(KEY_EXIT):
      if (s_editMode <= 0) {
        warningText = nullptr;
      }
      break;
  }

  if (warningText) {
    const uint8_t dialogRows[] = { 0, 0, uint8_t(reusableBuffer.moduleSetup.pxx2.registerStep < REGISTER_RX_NAME_RECEIVED ? READONLY_ROW : 0), uint8_t(reusableBuffer.moduleSetup.pxx2.registerStep < REGISTER_RX_NAME_RECEIVED ? 0 : 1)};
    check(event, 0, nullptr, 0, dialogRows, 3, 4 - HEADER_LINE); // TODO add a comment for 3 - HEADER_LINE once understood

    drawMessageBox(warningText);

    // registration password
    lcdDrawText(WARNING_LINE_X, WARNING_LINE_Y - 4, STR_REG_ID);
    editName(WARNING_LINE_X + 8*FW, WARNING_LINE_Y - 4, g_model.modelRegistrationID, PXX2_LEN_REGISTRATION_ID, event, menuVerticalPosition == ITEM_REGISTER_PASSWORD);

    // loop index (will be removed in future)
    lcdDrawText(WARNING_LINE_X, WARNING_LINE_Y - 4 + FH, "UID");
    lcdDrawNumber(WARNING_LINE_X + 8*FW, WARNING_LINE_Y - 4 + FH, reusableBuffer.moduleSetup.pxx2.registerLoopIndex, menuVerticalPosition == ITEM_REGISTER_MODULE_INDEX ? (s_editMode ? INVERS + BLINK : INVERS) : 0);
    if (menuVerticalPosition == ITEM_REGISTER_MODULE_INDEX && s_editMode) {
      CHECK_INCDEC_MODELVAR_ZERO(event, reusableBuffer.moduleSetup.pxx2.registerLoopIndex, 2);
    }

    // RX name
    if (reusableBuffer.moduleSetup.pxx2.registerStep < REGISTER_RX_NAME_RECEIVED) {
      lcdDrawText(WARNING_LINE_X, WARNING_LINE_Y - 4 + 2 * FH, STR_WAITING);
      lcdDrawText(WARNING_LINE_X, WARNING_LINE_Y - 2 + 3 * FH, TR_EXIT, menuVerticalPosition == ITEM_REGISTER_BUTTONS ? INVERS : 0);
    }
    else {
      lcdDrawText(WARNING_LINE_X, WARNING_LINE_Y - 4 + 2 * FH, STR_RX_NAME);
      editName(WARNING_LINE_X + 8*FW, WARNING_LINE_Y - 4 + 2 * FH, reusableBuffer.moduleSetup.pxx2.registerRxName, PXX2_LEN_RX_NAME, event, menuVerticalPosition == ITEM_REGISTER_RECEIVER_NAME);
      lcdDrawText(WARNING_LINE_X, WARNING_LINE_Y - 2 + 3 * FH, TR_ENTER, menuVerticalPosition == ITEM_REGISTER_BUTTONS && menuHorizontalPosition == 0 ? INVERS : 0);
      lcdDrawText(WARNING_LINE_X + 8*FW, WARNING_LINE_Y - 2 + 3 * FH, TR_EXIT, menuVerticalPosition == ITEM_REGISTER_BUTTONS && menuHorizontalPosition == 1 ? INVERS : 0);
    }

    reusableBuffer.moduleSetup.pxx2.registerPopupVerticalPosition = menuVerticalPosition;
    reusableBuffer.moduleSetup.pxx2.registerPopupHorizontalPosition = menuHorizontalPosition;
    reusableBuffer.moduleSetup.pxx2.registerPopupEditMode = s_editMode;
  }

  menuVerticalPosition = backupVerticalPosition;
  menuHorizontalPosition = backupHorizontalPosition;
  menuVerticalOffset = backupVerticalOffset;
  s_editMode = backupEditMode;
}

void startRegisterDialog(uint8_t module)
{
  memclear(&reusableBuffer.moduleSetup.pxx2, sizeof(reusableBuffer.moduleSetup.pxx2));
  reusableBuffer.moduleSetup.pxx2.registerPopupVerticalPosition = ITEM_REGISTER_BUTTONS;
  moduleState[module].mode = MODULE_MODE_REGISTER;
  s_editMode = 0;
  killAllEvents();
  POPUP_INPUT("", runPopupRegister);
}

#if defined(BLUETOOTH)
void onBluetoothConnectMenu(const char * result)
{
  if (result != STR_EXIT) {
    uint8_t index = (result - reusableBuffer.moduleSetup.bt.devices[0]) / sizeof(reusableBuffer.moduleSetup.bt.devices[0]);
    strncpy(bluetooth.distantAddr, reusableBuffer.moduleSetup.bt.devices[index], LEN_BLUETOOTH_ADDR);
    bluetooth.state = BLUETOOTH_STATE_BIND_REQUESTED;
  }
  else {
    reusableBuffer.moduleSetup.bt.devicesCount = 0;
    bluetooth.state = BLUETOOTH_STATE_DISCOVER_END;
  }
}
#endif

void menuModelSetup(event_t event)
{
#if defined(EXTERNAL_ANTENNA)
  // Switch to external antenna confirmation
  if (warningResult) {
    warningResult = 0;
    g_model.moduleData[INTERNAL_MODULE].pxx.external_antenna = XJT_EXTERNAL_ANTENNA;
  }
#endif

  int8_t old_editMode = s_editMode;

#if defined(PCBTARANIS)
  MENU_TAB({
    HEADER_LINE_COLUMNS
    0,
    TIMER_ROWS,
    TIMER_ROWS,
    TIMER_ROWS,
    0, // Extended limits
    1, // Extended trims
    0, // Show trims
    0, // Trims step
    0, // Throttle reverse
    0, // Throttle trace source
    0, // Throttle trim

    LABEL(PreflightCheck),
      0, // Checklist
      0, // Throttle warning
      SW_WARN_ROWS, // Switch warning
      POT_WARN_ITEMS(), // Pot warning

    NUM_STICKS + NUM_POTS + NUM_SLIDERS - 1, // Center beeps
    0, // Global functions

    uint8_t((isDefaultModelRegistrationID() || (warningText && popupFunc == runPopupRegister)) ? HIDDEN_ROW : READONLY_ROW), // Registration ID

    LABEL(InternalModule),
      INTERNAL_MODULE_MODE_ROWS,                                   // module mode (PXX(2) / None)
      INTERNAL_MODULE_CHANNELS_ROWS,                               // Channels min and count
      IF_NOT_PXX2_MODULE(INTERNAL_MODULE, IF_INTERNAL_MODULE_ON(HAS_RF_PROTOCOL_MODELINDEX(g_model.moduleData[INTERNAL_MODULE].rfProtocol) ? (uint8_t)2 : (uint8_t)1)),
      IF_PXX2_MODULE(INTERNAL_MODULE, 0),                          // RxNum
      ANTENNA_ROW
      IF_INTERNAL_MODULE_ON(FAILSAFE_ROWS(INTERNAL_MODULE)),       // Failsafe
      IF_PXX2_MODULE(INTERNAL_MODULE, 1),                          // Range check and Register buttons
      IF_PXX2_MODULE(INTERNAL_MODULE, 0),                          // Module options
      IF_PXX2_MODULE(INTERNAL_MODULE, 0),                          // Receiver 1
      IF_PXX2_MODULE(INTERNAL_MODULE, 0),                          // Receiver 2
      IF_PXX2_MODULE(INTERNAL_MODULE, 0),                          // Receiver 3

    LABEL(ExternalModule),
      EXTERNAL_MODULE_MODE_ROWS,
      MULTIMODULE_SUBTYPE_ROWS(EXTERNAL_MODULE)
      MULTIMODULE_STATUS_ROWS
      EXTERNAL_MODULE_CHANNELS_ROWS,
      IF_NOT_PXX2_MODULE(EXTERNAL_MODULE, EXTERNAL_MODULE_BIND_ROWS()),      // line reused for PPM: PPM settings
      IF_PXX2_MODULE(EXTERNAL_MODULE, 0),                          // RxNum
      IF_NOT_PXX2_MODULE(EXTERNAL_MODULE, EXTERNAL_MODULE_OPTION_ROW),
      MULTIMODULE_MODULE_ROWS
      EXTERNAL_MODULE_POWER_ROW,
      FAILSAFE_ROWS(EXTERNAL_MODULE),
      IF_PXX2_MODULE(EXTERNAL_MODULE, 1),                          // Range check and Register buttons
      IF_PXX2_MODULE(EXTERNAL_MODULE, 0),                          // Module options
      IF_PXX2_MODULE(EXTERNAL_MODULE, 0),                          // Receiver 1
      IF_PXX2_MODULE(EXTERNAL_MODULE, 0),                          // Receiver 2
      IF_PXX2_MODULE(EXTERNAL_MODULE, 0),                          // Receiver 3
    TRAINER_ROWS
  });
#else
  MENU_TAB({ HEADER_LINE_COLUMNS 0, TIMER_ROWS, TIMER_ROWS, TIMER_ROWS, 0, 1, 0, 0, 0, 0, 0, LABEL(PreflightCheck), 0, 0, NUM_SWITCHES-1, NUM_STICKS+NUM_POTS+NUM_SLIDERS-1, 0,
    LABEL(ExternalModule),
    EXTERNAL_MODULE_MODE_ROWS,
    MULTIMODULE_SUBTYPE_ROWS(EXTERNAL_MODULE)
    MULTIMODULE_STATUS_ROWS
    EXTERNAL_MODULE_CHANNELS_ROWS,
    EXTERNAL_MODULE_BIND_ROWS(),
    OUTPUT_TYPE_ROWS()
    EXTERNAL_MODULE_OPTION_ROW,
    MULTIMODULE_MODULE_ROWS
    EXTERNAL_MODULE_POWER_ROW,
    FAILSAFE_ROWS(EXTERNAL_MODULE),
    EXTRA_MODULE_ROWS
    TRAINER_ROWS
  });
#endif

  MENU_CHECK(menuTabModel, MENU_MODEL_SETUP, HEADER_LINE+MODEL_SETUP_MAX_LINES);

#if (defined(DSM2) || defined(PXX))
  if (menuEvent) {
    moduleState[0].mode = 0;
#if NUM_MODULES > 1
    moduleState[1].mode = 0;
#endif
  }
#endif

  TITLE(STR_MENUSETUP);

  if (event == EVT_ENTRY) {
    reusableBuffer.moduleSetup.r9mPower = g_model.moduleData[EXTERNAL_MODULE].pxx.power;
  }

  uint8_t sub = menuVerticalPosition - HEADER_LINE;
  int8_t editMode = s_editMode;

  for (uint8_t i=0; i<NUM_BODY_LINES; ++i) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    uint8_t k = i + menuVerticalOffset;
    for (int j=0; j<=k; j++) {
      if (mstate_tab[j+HEADER_LINE] == HIDDEN_ROW) {
        if (++k >= (int)DIM(mstate_tab)) {
          return;
        }
      }
    }

    LcdFlags blink = ((editMode>0) ? BLINK|INVERS : INVERS);
    LcdFlags attr = (sub == k ? blink : 0);

    switch (k) {
      case ITEM_MODEL_NAME:
        editSingleName(MODEL_SETUP_2ND_COLUMN, y, STR_MODELNAME, g_model.header.name, sizeof(g_model.header.name), event, attr);
        memcpy(modelHeaders[g_eeGeneral.currModel].name, g_model.header.name, sizeof(g_model.header.name));
        break;

      case ITEM_MODEL_TIMER1:
      case ITEM_MODEL_TIMER2:
      case ITEM_MODEL_TIMER3:
      {
        unsigned int timerIdx = (k>=ITEM_MODEL_TIMER3 ? 2 : (k>=ITEM_MODEL_TIMER2 ? 1 : 0));
        TimerData * timer = &g_model.timers[timerIdx];
        drawStringWithIndex(0*FW, y, STR_TIMER, timerIdx+1);
        drawTimerMode(MODEL_SETUP_2ND_COLUMN, y, timer->mode, menuHorizontalPosition==0 ? attr : 0);
        drawTimer(MODEL_SETUP_2ND_COLUMN+5*FW-2+5*FWNUM+1, y, timer->start, RIGHT | (menuHorizontalPosition==1 ? attr : 0), menuHorizontalPosition==2 ? attr : 0);
        if (attr && editMode > 0) {
          div_t qr = div(timer->start, 60);
          switch (menuHorizontalPosition) {
            case 0:
            {
              int8_t timerMode = timer->mode;
              if (timerMode < 0) timerMode -= TMRMODE_COUNT-1;
              CHECK_INCDEC_MODELVAR_CHECK(event, timerMode, -TMRMODE_COUNT-SWSRC_LAST+1, TMRMODE_COUNT+SWSRC_LAST-1, isSwitchAvailableInTimers);
              if (timerMode < 0) timerMode += TMRMODE_COUNT-1;
              timer->mode = timerMode;
#if defined(AUTOSWITCH)
              if (s_editMode>0) {
                int8_t val = timer->mode - (TMRMODE_COUNT-1);
                int8_t switchVal = checkIncDecMovedSwitch(val);
                if (val != switchVal) {
                  timer->mode = switchVal + (TMRMODE_COUNT-1);
                  storageDirty(EE_MODEL);
                }
              }
#endif
              break;
            }
            case 1:
              CHECK_INCDEC_MODELVAR_ZERO(event, qr.quot, 539); // 8:59
              timer->start = qr.rem + qr.quot*60;
              break;
            case 2:
              qr.rem -= checkIncDecModel(event, qr.rem+2, 1, 62)-2;
              timer->start -= qr.rem ;
              if ((int16_t)timer->start < 0) timer->start=0;
              if ((int16_t)timer->start > 5999) timer->start=32399; // 8:59:59
              break;
          }
        }
        break;
      }

      case ITEM_MODEL_TIMER1_NAME:
      case ITEM_MODEL_TIMER2_NAME:
      case ITEM_MODEL_TIMER3_NAME:
      {
        TimerData * timer = &g_model.timers[k>=ITEM_MODEL_TIMER3 ? 2 : (k>=ITEM_MODEL_TIMER2 ? 1 : 0)];
        editSingleName(MODEL_SETUP_2ND_COLUMN, y, STR_TIMER_NAME, timer->name, sizeof(timer->name), event, attr);
        break;
      }

      case ITEM_MODEL_TIMER1_MINUTE_BEEP:
      case ITEM_MODEL_TIMER2_MINUTE_BEEP:
      case ITEM_MODEL_TIMER3_MINUTE_BEEP:
      {
        TimerData * timer = &g_model.timers[k>=ITEM_MODEL_TIMER3 ? 2 : (k>=ITEM_MODEL_TIMER2 ? 1 : 0)];
        timer->minuteBeep = editCheckBox(timer->minuteBeep, MODEL_SETUP_2ND_COLUMN, y, STR_MINUTEBEEP, attr, event);
        break;
      }

      case ITEM_MODEL_TIMER1_COUNTDOWN_BEEP:
      case ITEM_MODEL_TIMER2_COUNTDOWN_BEEP:
      case ITEM_MODEL_TIMER3_COUNTDOWN_BEEP:
      {
        TimerData * timer = &g_model.timers[k>=ITEM_MODEL_TIMER3 ? 2 : (k>=ITEM_MODEL_TIMER2 ? 1 : 0)];
        timer->countdownBeep = editChoice(MODEL_SETUP_2ND_COLUMN, y, STR_BEEPCOUNTDOWN, STR_VBEEPCOUNTDOWN, timer->countdownBeep, COUNTDOWN_SILENT, COUNTDOWN_COUNT-1, attr, event);
        break;
      }

      case ITEM_MODEL_TIMER1_PERSISTENT:
      case ITEM_MODEL_TIMER2_PERSISTENT:
      case ITEM_MODEL_TIMER3_PERSISTENT:
      {
        TimerData * timer = &g_model.timers[k>=ITEM_MODEL_TIMER3 ? 2 : (k>=ITEM_MODEL_TIMER2 ? 1 : 0)];
        timer->persistent = editChoice(MODEL_SETUP_2ND_COLUMN, y, STR_PERSISTENT, STR_VPERSISTENT, timer->persistent, 0, 2, attr, event);
        break;
      }

      case ITEM_MODEL_EXTENDED_LIMITS:
        ON_OFF_MENU_ITEM(g_model.extendedLimits, MODEL_SETUP_2ND_COLUMN, y, STR_ELIMITS, attr, event);
        break;

      case ITEM_MODEL_EXTENDED_TRIMS:
        ON_OFF_MENU_ITEM(g_model.extendedTrims, MODEL_SETUP_2ND_COLUMN, y, STR_ETRIMS, menuHorizontalPosition<=0 ? attr : 0, event==EVT_KEY_BREAK(KEY_ENTER) ? event : 0);
        lcdDrawText(MODEL_SETUP_2ND_COLUMN+4*FW, y, STR_RESET_BTN, (menuHorizontalPosition>0  && !NO_HIGHLIGHT()) ? attr : 0);
        if (attr && menuHorizontalPosition>0) {
          s_editMode = 0;
          if (event==EVT_KEY_LONG(KEY_ENTER)) {
            START_NO_HIGHLIGHT();
            for (uint8_t i=0; i<MAX_FLIGHT_MODES; i++) {
              memclear(&g_model.flightModeData[i], TRIMS_ARRAY_SIZE);
            }
            storageDirty(EE_MODEL);
            AUDIO_WARNING1();
          }
        }
        break;

      case ITEM_MODEL_DISPLAY_TRIMS:
        g_model.displayTrims = editChoice(MODEL_SETUP_2ND_COLUMN, y, STR_DISPLAY_TRIMS, STR_VDISPLAYTRIMS, g_model.displayTrims, 0, 2, attr, event);
        break;

      case ITEM_MODEL_TRIM_INC:
        g_model.trimInc = editChoice(MODEL_SETUP_2ND_COLUMN, y, STR_TRIMINC, STR_VTRIMINC, g_model.trimInc, -2, 2, attr, event);
        break;

      case ITEM_MODEL_THROTTLE_REVERSED:
        ON_OFF_MENU_ITEM(g_model.throttleReversed, MODEL_SETUP_2ND_COLUMN, y, STR_THROTTLEREVERSE, attr, event ) ;
        break;

      case ITEM_MODEL_THROTTLE_TRACE:
      {
        lcdDrawTextAlignedLeft(y, STR_TTRACE);
        if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, g_model.thrTraceSrc, NUM_POTS+NUM_SLIDERS+MAX_OUTPUT_CHANNELS);
        uint8_t idx = g_model.thrTraceSrc + MIXSRC_Thr;
        if (idx > MIXSRC_Thr)
          idx += 1;
        if (idx >= MIXSRC_FIRST_POT+NUM_POTS+NUM_SLIDERS)
          idx += MIXSRC_CH1 - MIXSRC_FIRST_POT - NUM_POTS - NUM_SLIDERS;
        drawSource(MODEL_SETUP_2ND_COLUMN, y, idx, attr);
        break;
      }

      case ITEM_MODEL_THROTTLE_TRIM:
        ON_OFF_MENU_ITEM(g_model.thrTrim, MODEL_SETUP_2ND_COLUMN, y, STR_TTRIM, attr, event);
        break;

      case ITEM_MODEL_PREFLIGHT_LABEL:
        lcdDrawTextAlignedLeft(y, STR_PREFLIGHT);
        break;

      case ITEM_MODEL_CHECKLIST_DISPLAY:
        ON_OFF_MENU_ITEM(g_model.displayChecklist, MODEL_SETUP_2ND_COLUMN, y, STR_CHECKLIST, attr, event);
        break;

      case ITEM_MODEL_THROTTLE_WARNING:
        g_model.disableThrottleWarning = !editCheckBox(!g_model.disableThrottleWarning, MODEL_SETUP_2ND_COLUMN, y, STR_THROTTLEWARNING, attr, event);
        break;

#if defined(PCBTARANIS)
      case ITEM_MODEL_SWITCHES_WARNING2:
        if (i==0) {
          if (CURSOR_MOVED_LEFT(event))
            menuVerticalOffset--;
          else
            menuVerticalOffset++;
        }
        break;
#endif

      case ITEM_MODEL_SWITCHES_WARNING:
#if defined(PCBTARANIS)
        {
          #define FIRSTSW_STR   STR_VSRCRAW+(MIXSRC_FIRST_SWITCH-MIXSRC_Rud+1)*length
          uint8_t switchWarningsCount = getSwitchWarningsCount();
          uint8_t length = STR_VSRCRAW[0];
          horzpos_t l_posHorz = menuHorizontalPosition;

          if (i>=NUM_BODY_LINES-2 && getSwitchWarningsCount() > 4*(NUM_BODY_LINES-i)) {
            if (CURSOR_MOVED_LEFT(event))
              menuVerticalOffset--;
            else
              menuVerticalOffset++;
            break;
          }

          swarnstate_t states = g_model.switchWarningState;
          char c;

          lcdDrawTextAlignedLeft(y, STR_SWITCHWARNING);
#if defined(PCBXLITE)
          lcdDrawText(LCD_W, y, "<]", RIGHT);
          if (attr) {
            if (menuHorizontalPosition > switchWarningsCount)
              menuHorizontalPosition = switchWarningsCount;
          }
          if (attr && menuHorizontalPosition == switchWarningsCount) {
#else
          if (attr) {
#endif
            s_editMode = 0;
            if (!READ_ONLY()) {
              switch (event) {
                case EVT_KEY_BREAK(KEY_ENTER):
                  break;

                case EVT_KEY_LONG(KEY_ENTER):
                  if (menuHorizontalPosition < 0 || menuHorizontalPosition >= switchWarningsCount) {
                    START_NO_HIGHLIGHT();
                    getMovedSwitch();
                    g_model.switchWarningState = switches_states;
                    AUDIO_WARNING1();
                    storageDirty(EE_MODEL);
                  }
                  killEvents(event);
                  break;
              }
            }
          }

          LcdFlags line = attr;

          int current = 0;
          for (int i=0; i<switchWarningsCount; i++) {
            if (SWITCH_WARNING_ALLOWED(i)) {
              div_t qr = div(current, 4);
              if (!READ_ONLY() && event==EVT_KEY_BREAK(KEY_ENTER) && line && l_posHorz==current && old_editMode) {
                g_model.switchWarningEnable ^= (1 << i);
                storageDirty(EE_MODEL);
#if defined(PCBXLITE)
                s_editMode = 0;
#endif
              }
              uint8_t swactive = !(g_model.switchWarningEnable & (1<<i));
              c = "\300-\301"[states & 0x03];
              //lcdDrawChar(MODEL_SETUP_2ND_COLUMN+qr.rem*(2*FW+1), y+FH*qr.quot, 'A'+i, line && (menuHorizontalPosition==current) ? INVERS : 0);
              lcdDrawSizedText(MODEL_SETUP_2ND_COLUMN + qr.rem*((2*FW)+1), y+FH*qr.quot, FIRSTSW_STR+(i*length)+3, 1, line && (menuHorizontalPosition==current) ? INVERS : 0);
              if (swactive) lcdDrawChar(lcdNextPos, y+FH*qr.quot, c);
              ++current;
            }
            states >>= 2;
          }
          if (attr && ((menuHorizontalPosition < 0) || menuHorizontalPosition >= switchWarningsCount)) {
            lcdDrawFilledRect(MODEL_SETUP_2ND_COLUMN-1, y-1, 8*(2*FW+1), 1+FH*((current+4)/5));
          }
#else // PCBTARANIS
      {
        lcdDrawTextAlignedLeft(y, STR_SWITCHWARNING);
        swarnstate_t states = g_model.switchWarningState;
        char c;
        if (attr) {
          s_editMode = 0;
          if (!READ_ONLY()) {
            switch (event) {
              case EVT_KEY_BREAK(KEY_ENTER):
                if (menuHorizontalPosition < NUM_SWITCHES-1) {
                  g_model.switchWarningEnable ^= (1 << menuHorizontalPosition);
                  storageDirty(EE_MODEL);
                }
                break;

              case EVT_KEY_LONG(KEY_ENTER):
                if (menuHorizontalPosition == NUM_SWITCHES-1) {
                  START_NO_HIGHLIGHT();
                  getMovedSwitch();
                  g_model.switchWarningState = switches_states;
                  AUDIO_WARNING1();
                  storageDirty(EE_MODEL);
                }
                killEvents(event);
                break;
            }
          }
        }
        LcdFlags line = attr;

        for (uint8_t i=0; i<NUM_SWITCHES-1/*not on TRN switch*/; i++) {
          uint8_t swactive = !(g_model.switchWarningEnable & 1 << i);
          attr = 0;

          if (IS_3POS(i)) {
            c = '0'+(states & 0x03);
            states >>= 2;
          }
          else {
            if ((states & 0x01) && swactive)
              attr = INVERS;
            c = *(STR_VSWITCHES - 2 + 9 + (3*(i+1)));
            states >>= 1;
          }
          if (line && (menuHorizontalPosition == i)) {
            attr = BLINK | INVERS;
          }
          lcdDrawChar(MODEL_SETUP_2ND_COLUMN+i*FW, y, (swactive) ? c : '-', attr);
          lcdDrawText(MODEL_SETUP_2ND_COLUMN+(NUM_SWITCHES*FW), y, "<]", (menuHorizontalPosition == NUM_SWITCHES-1 && !NO_HIGHLIGHT()) ? line : 0);
        }
#endif
        break;
      }

#if defined(PCBTARANIS)
      case ITEM_MODEL_POTS_WARNING:
        lcdDrawTextAlignedLeft(y, STR_POTWARNING);
        lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, "\004""OFF\0""Man\0""Auto", g_model.potsWarnMode, (menuHorizontalPosition == 0) ? attr : 0);
        if (attr && (menuHorizontalPosition == 0)) {
          CHECK_INCDEC_MODELVAR(event, g_model.potsWarnMode, POTS_WARN_OFF, POTS_WARN_AUTO);
          storageDirty(EE_MODEL);
        }

        if (attr) {
          if (menuHorizontalPosition > 0) s_editMode = 0;
          if (!READ_ONLY() && menuHorizontalPosition > 0) {
            switch (event) {
              case EVT_KEY_LONG(KEY_ENTER):
                killEvents(event);
                if (g_model.potsWarnMode == POTS_WARN_MANUAL) {
                  SAVE_POT_POSITION(menuHorizontalPosition-1);
                  AUDIO_WARNING1();
                  storageDirty(EE_MODEL);
                }
                break;
              case EVT_KEY_BREAK(KEY_ENTER):
                g_model.potsWarnEnabled ^= (1 << (menuHorizontalPosition-1));
                storageDirty(EE_MODEL);
                break;
            }
          }
        }
        if (g_model.potsWarnMode) {
          coord_t x = MODEL_SETUP_2ND_COLUMN+28;
          for (int i=0; i<NUM_POTS+NUM_SLIDERS; ++i) {
            if (i<NUM_XPOTS && !IS_POT_SLIDER_AVAILABLE(POT1+i)) {
              if (attr && (menuHorizontalPosition==i+1)) REPEAT_LAST_CURSOR_MOVE();
            }
            else {
              LcdFlags flags = ((menuHorizontalPosition==i+1) && attr) ? BLINK : 0;
              if ((!attr || menuHorizontalPosition >= 0) && !(g_model.potsWarnEnabled & (1 << i))) {
                flags |= INVERS;
              }

              // TODO add a new function
              lcdDrawSizedText(x, y, STR_VSRCRAW+2+STR_VSRCRAW[0]*(NUM_STICKS+1+i), STR_VSRCRAW[0]-1, flags & ~ZCHAR);
              x = lcdNextPos+3;
            }
          }
        }
        break;
#endif

      case ITEM_MODEL_BEEP_CENTER:
        lcdDrawTextAlignedLeft(y, STR_BEEPCTR);
        for (uint8_t i=0; i<NUM_STICKS+NUM_POTS+NUM_SLIDERS; i++) {
          // TODO flash saving, \001 not needed in STR_RETA123
          coord_t x = MODEL_SETUP_2ND_COLUMN+i*FW;
          lcdDrawTextAtIndex(x, y, STR_RETA123, i, ((menuHorizontalPosition==i) && attr) ? BLINK|INVERS : (((g_model.beepANACenter & ((BeepANACenter)1<<i)) || (attr && CURSOR_ON_LINE())) ? INVERS : 0 ) );
        }
        if (attr && CURSOR_ON_CELL) {
          if (event == EVT_KEY_BREAK(KEY_ENTER)) {
            if (READ_ONLY_UNLOCKED()) {
              s_editMode = 0;
              g_model.beepANACenter ^= ((BeepANACenter)1<<menuHorizontalPosition);
              storageDirty(EE_MODEL);
            }
          }
        }
        break;

      case ITEM_MODEL_USE_GLOBAL_FUNCTIONS:
        lcdDrawTextAlignedLeft(y, STR_USE_GLOBAL_FUNCS);
        drawCheckBox(MODEL_SETUP_2ND_COLUMN, y, !g_model.noGlobalFunctions, attr);
        if (attr) g_model.noGlobalFunctions = !checkIncDecModel(event, !g_model.noGlobalFunctions, 0, 1);
        break;

#if defined(HARDWARE_INTERNAL_MODULE)
      case ITEM_MODEL_INTERNAL_MODULE_LABEL:
        lcdDrawTextAlignedLeft(y, TR_INTERNALRF);
        break;

      case ITEM_MODEL_INTERNAL_MODULE_MODE:
        lcdDrawTextAlignedLeft(y, STR_MODE);
        lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_MODULE_PROTOCOLS, g_model.moduleData[INTERNAL_MODULE].type, menuHorizontalPosition==0 ? attr : 0);
        if (isModuleXJT(INTERNAL_MODULE))
          lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN+6*FW, y, STR_XJT_PROTOCOLS, 1+g_model.moduleData[INTERNAL_MODULE].rfProtocol, menuHorizontalPosition==1 ? attr : 0);
        if (attr) {
          if (menuHorizontalPosition == 0) {
            uint8_t moduleType = checkIncDec(event, g_model.moduleData[INTERNAL_MODULE].type, MODULE_TYPE_NONE, MODULE_TYPE_MAX, EE_MODEL, isInternalModuleAvailable);
            if (checkIncDec_Ret) {
              // TODO this code should be common, in module.h (X10_new_UI branch)
              memclear(&g_model.moduleData[INTERNAL_MODULE], sizeof(ModuleData));
              g_model.moduleData[INTERNAL_MODULE].type = moduleType;
              g_model.moduleData[INTERNAL_MODULE].channelsCount = defaultModuleChannels_M8(INTERNAL_MODULE);
            }
          }
          else if (isModuleXJT(INTERNAL_MODULE)) {
            g_model.moduleData[INTERNAL_MODULE].rfProtocol = checkIncDec(event, g_model.moduleData[INTERNAL_MODULE].rfProtocol, RF_PROTO_X16, RF_PROTO_LAST, EE_MODEL, isRfProtocolAvailable);
            if (checkIncDec_Ret) {
              g_model.moduleData[0].type = MODULE_TYPE_PXX_XJT;
              g_model.moduleData[0].channelsStart = 0;
              g_model.moduleData[0].channelsCount = defaultModuleChannels_M8(INTERNAL_MODULE);
            }
          }
        }
        break;
#endif

#if defined(PCBSKY9X)
      case ITEM_MODEL_EXTRA_MODULE_LABEL:
        lcdDrawTextAlignedLeft(y, "RF Port 2 (PPM)");
        break;
#endif

      case ITEM_MODEL_EXTERNAL_MODULE_LABEL:
        lcdDrawTextAlignedLeft(y, TR_EXTERNALRF);
        break;

      case ITEM_MODEL_EXTERNAL_MODULE_MODE:
        lcdDrawTextAlignedLeft(y, STR_MODE);
        lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_MODULE_PROTOCOLS, g_model.moduleData[EXTERNAL_MODULE].type, menuHorizontalPosition==0 ? attr : 0);
        if (isModuleXJT(EXTERNAL_MODULE))
          lcdDrawTextAtIndex(lcdNextPos + FW, y, STR_XJT_PROTOCOLS, 1+g_model.moduleData[EXTERNAL_MODULE].rfProtocol, menuHorizontalPosition==1 ? attr : 0);
        else if (isModuleDSM2(EXTERNAL_MODULE))
          lcdDrawTextAtIndex(lcdNextPos + FW, y, STR_DSM_PROTOCOLS, g_model.moduleData[EXTERNAL_MODULE].rfProtocol, menuHorizontalPosition==1 ? attr : 0);
        else if (isModuleR9M(EXTERNAL_MODULE))
          lcdDrawTextAtIndex(lcdNextPos + FW, y, STR_R9M_REGION, g_model.moduleData[EXTERNAL_MODULE].subType, (menuHorizontalPosition==1 ? attr : 0));
#if defined(MULTIMODULE)
        else if (isModuleMultimodule(EXTERNAL_MODULE)) {
          int multi_rfProto = g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(false);
          if (g_model.moduleData[EXTERNAL_MODULE].multi.customProto)
            lcdDrawText(lcdNextPos + FW, y, STR_MULTI_CUSTOM, menuHorizontalPosition==1 ? attr : 0);
          else
            lcdDrawTextAtIndex(lcdNextPos + FW, y, STR_MULTI_PROTOCOLS, multi_rfProto, menuHorizontalPosition==1 ? attr : 0);
        }
#endif
        if (attr && editMode > 0) {
          switch (menuHorizontalPosition) {
            case 0:
              {
                uint8_t moduleType = checkIncDec(event, g_model.moduleData[EXTERNAL_MODULE].type,
                                                 MODULE_TYPE_NONE,
                                                 IS_TRAINER_EXTERNAL_MODULE() ? MODULE_TYPE_NONE :
                                                 MODULE_TYPE_COUNT - 1, EE_MODEL,
                                                 isExternalModuleAvailable);
                if (checkIncDec_Ret) {
                  // TODO this code should be common, in module.h (X10_new_UI branch)
                  memclear(&g_model.moduleData[EXTERNAL_MODULE], sizeof(ModuleData));
                  g_model.moduleData[EXTERNAL_MODULE].type = moduleType;
                  g_model.moduleData[EXTERNAL_MODULE].channelsCount = defaultModuleChannels_M8(EXTERNAL_MODULE);
                  if (isModuleSBUS(EXTERNAL_MODULE))
                    g_model.moduleData[EXTERNAL_MODULE].sbus.refreshRate = -31;
                  if (isModulePPM(EXTERNAL_MODULE))
                    SET_DEFAULT_PPM_FRAME_LENGTH(EXTERNAL_MODULE);
                }
              }
              break;
            case 1:
              if (isModuleDSM2(EXTERNAL_MODULE))
                CHECK_INCDEC_MODELVAR(event, g_model.moduleData[EXTERNAL_MODULE].rfProtocol, DSM2_PROTO_LP45, DSM2_PROTO_DSMX);
              else if (isModuleR9M(EXTERNAL_MODULE)) {
                uint8_t newR9MType = checkIncDec(event, g_model.moduleData[EXTERNAL_MODULE].subType, MODULE_SUBTYPE_R9M_FCC, MODULE_SUBTYPE_R9M_LAST, EE_MODEL, isR9MModeAvailable);
#if !defined(DEBUG)
                if (newR9MType != g_model.moduleData[EXTERNAL_MODULE].subType && newR9MType > MODULE_SUBTYPE_R9M_EU) {
                  POPUP_WARNING(STR_R9MFLEXWARN1);
                  const char * w = STR_R9MFLEXWARN2;
                  SET_WARNING_INFO(w, strlen(w), 0);
                }
#endif
                g_model.moduleData[EXTERNAL_MODULE].subType = newR9MType;
              }

#if defined(MULTIMODULE)
              else if (isModuleMultimodule(EXTERNAL_MODULE)) {
                int multiRfProto = g_model.moduleData[EXTERNAL_MODULE].multi.customProto == 1 ? MM_RF_PROTO_CUSTOM : g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(false);
                CHECK_INCDEC_MODELVAR(event, multiRfProto, MM_RF_PROTO_FIRST, MM_RF_PROTO_LAST);
                if (checkIncDec_Ret) {
                  g_model.moduleData[EXTERNAL_MODULE].multi.customProto = (multiRfProto == MM_RF_PROTO_CUSTOM);
                  if (!g_model.moduleData[EXTERNAL_MODULE].multi.customProto)
                    g_model.moduleData[EXTERNAL_MODULE].setMultiProtocol(multiRfProto);
                  g_model.moduleData[EXTERNAL_MODULE].subType = 0;
                  // Sensible default for DSM2 (same as for ppm): 7ch@22ms + Autodetect settings enabled
                  if (g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(true) == MM_RF_PROTO_DSM2) {
                    g_model.moduleData[EXTERNAL_MODULE].multi.autoBindMode = 1;
                  }
                  else {
                    g_model.moduleData[EXTERNAL_MODULE].multi.autoBindMode = 0;
                  }
                  g_model.moduleData[EXTERNAL_MODULE].multi.optionValue = 0;
                }
              }
#endif
              else {
                CHECK_INCDEC_MODELVAR(event, g_model.moduleData[EXTERNAL_MODULE].rfProtocol, RF_PROTO_X16, RF_PROTO_LAST);
              }
              if (checkIncDec_Ret) {
                g_model.moduleData[EXTERNAL_MODULE].channelsStart = 0;
                g_model.moduleData[EXTERNAL_MODULE].channelsCount = defaultModuleChannels_M8(EXTERNAL_MODULE);
              }
          }
        }
        break;

#if defined(MULTIMODULE)
      case ITEM_MODEL_EXTERNAL_MODULE_SUBTYPE:
      {
        lcdDrawTextAlignedLeft(y, STR_SUBTYPE);
        uint8_t multi_rfProto = g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(true);
        const mm_protocol_definition * pdef = getMultiProtocolDefinition(multi_rfProto);

        if (multi_rfProto == MM_RF_CUSTOM_SELECTED) {
          lcdDrawNumber(MODEL_SETUP_2ND_COLUMN + 3 * FW, y, g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(false), RIGHT | (menuHorizontalPosition == 0 ? attr : 0), 2);
          lcdDrawNumber(MODEL_SETUP_2ND_COLUMN + 5 * FW, y, g_model.moduleData[EXTERNAL_MODULE].subType, RIGHT | (menuHorizontalPosition == 1 ? attr : 0), 2);
        }
        else {
          if (pdef->subTypeString != nullptr)
            lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, pdef->subTypeString, g_model.moduleData[EXTERNAL_MODULE].subType, attr);
        }
        if (attr && editMode > 0) {
          switch (menuHorizontalPosition) {
            case 0:
              if (multi_rfProto == MM_RF_CUSTOM_SELECTED)
                g_model.moduleData[EXTERNAL_MODULE].setMultiProtocol(checkIncDec(event, g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(false), 0, 63, EE_MODEL));
              else if (pdef->maxSubtype > 0)
                CHECK_INCDEC_MODELVAR(event, g_model.moduleData[EXTERNAL_MODULE].subType, 0, pdef->maxSubtype);
              break;
            case 1:
              // Custom protocol, third column is subtype
              CHECK_INCDEC_MODELVAR(event, g_model.moduleData[EXTERNAL_MODULE].subType, 0, 7);
              break;
          }
        }
      }
      break;
#endif

#if defined(PCBTARANIS)
      case ITEM_MODEL_TRAINER_LABEL:
        lcdDrawTextAlignedLeft(y, STR_TRAINER);
        break;

      case ITEM_MODEL_TRAINER_MODE:
        lcdDrawTextAlignedLeft(y, STR_MODE);
        lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_VTRAINERMODES, g_model.trainerData.mode, attr);
        if (attr) {
          g_model.trainerData.mode = checkIncDec(event, g_model.trainerData.mode, 0, TRAINER_MODE_MAX(), EE_MODEL, isTrainerModeAvailable);
#if defined(BLUETOOTH)
          if (checkIncDec_Ret) {
            bluetooth.state = BLUETOOTH_STATE_OFF;
            bluetooth.distantAddr[0] = 0;
          }
#endif
        }
        break;
#endif

#if defined(PCBTARANIS) && defined(BLUETOOTH)
      case ITEM_MODEL_TRAINER_BLUETOOTH:
        if (g_model.trainerData.mode == TRAINER_MODE_MASTER_BLUETOOTH) {
          if (attr) {
            s_editMode = 0;
          }
          if (bluetooth.distantAddr[0]) {
            lcdDrawText(INDENT_WIDTH, y+1, bluetooth.distantAddr, TINSIZE);
            lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, BUTTON(TR_CLEAR), attr);
            if (attr && event == EVT_KEY_BREAK(KEY_ENTER)) {
              bluetooth.state = BLUETOOTH_STATE_CLEAR_REQUESTED;
              memclear(bluetooth.distantAddr, sizeof(bluetooth.distantAddr));
            }
          }
          else {
            lcdDrawText(INDENT_WIDTH, y, "---");
            if (bluetooth.state < BLUETOOTH_STATE_IDLE)
              lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, STR_BUTTON_INIT, attr);
            else
              lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, STR_BUTTON_DISCOVER, attr);
            if (attr && event == EVT_KEY_BREAK(KEY_ENTER)) {
              if (bluetooth.state < BLUETOOTH_STATE_IDLE) {
                bluetooth.state = BLUETOOTH_STATE_OFF;
              }
              else {
                reusableBuffer.moduleSetup.bt.devicesCount = 0;
                bluetooth.state = BLUETOOTH_STATE_DISCOVER_REQUESTED;
              }
            }

            if (bluetooth.state == BLUETOOTH_STATE_DISCOVER_START && reusableBuffer.moduleSetup.bt.devicesCount > 0) {
              popupMenuItemsCount = min<uint8_t>(reusableBuffer.moduleSetup.bt.devicesCount, 6);
              for (uint8_t i=0; i<popupMenuItemsCount; i++) {
                popupMenuItems[i] = reusableBuffer.moduleSetup.bt.devices[i];
              }
              popupMenuTitle = STR_BT_SELECT_DEVICE;
              POPUP_MENU_START(onBluetoothConnectMenu);
            }
          }
        }
        else {
          if (bluetooth.distantAddr[0])
            lcdDrawText(INDENT_WIDTH, y+1, bluetooth.distantAddr, TINSIZE);
          else
            lcdDrawText(INDENT_WIDTH, y, "---");
          lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, bluetooth.state == BLUETOOTH_STATE_CONNECTED ? STR_CONNECTED : STR_NOT_CONNECTED);
        }
        break;
#endif

#if defined(PCBTARANIS)
      case ITEM_MODEL_TRAINER_CHANNELS:
        lcdDrawTextAlignedLeft(y, STR_CHANNELRANGE);
        lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, STR_CH, menuHorizontalPosition==0 ? attr : 0);
        lcdDrawNumber(lcdLastRightPos, y, g_model.trainerData.channelsStart+1, LEFT | (menuHorizontalPosition==0 ? attr : 0));
        lcdDrawChar(lcdLastRightPos, y, '-');
        lcdDrawNumber(lcdLastRightPos + FW+1, y, g_model.trainerData.channelsStart + 8 + g_model.trainerData.channelsCount, LEFT | (menuHorizontalPosition==1 ? attr : 0));
        if (attr && editMode > 0) {
          switch (menuHorizontalPosition) {
            case 0:
              CHECK_INCDEC_MODELVAR_ZERO(event, g_model.trainerData.channelsStart, 32-8-g_model.trainerData.channelsCount);
              break;
            case 1:
              CHECK_INCDEC_MODELVAR(event, g_model.trainerData.channelsCount, -4, min<int8_t>(MAX_TRAINER_CHANNELS_M8, 32-8-g_model.trainerData.channelsStart));
              break;
          }
        }
        break;
#endif

#if defined(PCBTARANIS)
      case ITEM_MODEL_INTERNAL_MODULE_CHANNELS:
#endif
#if defined(PCBSKY9X)
      case ITEM_MODEL_EXTRA_MODULE_CHANNELS:
#endif
      case ITEM_MODEL_EXTERNAL_MODULE_CHANNELS:
      {
        uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
        ModuleData & moduleData = g_model.moduleData[moduleIdx];
        lcdDrawTextAlignedLeft(y, STR_CHANNELRANGE);
        if ((int8_t)PORT_CHANNELS_ROWS(moduleIdx) >= 0) {
          lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, STR_CH, menuHorizontalPosition==0 ? attr : 0);
          lcdDrawNumber(lcdLastRightPos, y, moduleData.channelsStart+1, LEFT | (menuHorizontalPosition==0 ? attr : 0));
          lcdDrawChar(lcdLastRightPos, y, '-');
          lcdDrawNumber(lcdLastRightPos + FW+1, y, moduleData.channelsStart+sentModuleChannels(moduleIdx), LEFT | (menuHorizontalPosition==1 ? attr : 0));
          if (attr && editMode > 0) {
            switch (menuHorizontalPosition) {
              case 0:
                CHECK_INCDEC_MODELVAR_ZERO(event, moduleData.channelsStart, 32-8-moduleData.channelsCount);
                break;
              case 1:
                CHECK_INCDEC_MODELVAR_CHECK(event, moduleData.channelsCount, -4, min<int8_t>(maxModuleChannels_M8(moduleIdx), 32-8-moduleData.channelsStart), moduleData.type == MODULE_TYPE_ACCESS_ISRM ? isPXX2ChannelsCountAllowed : nullptr);
                if ((k == ITEM_MODEL_EXTERNAL_MODULE_CHANNELS && g_model.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_PPM)) {
                  SET_DEFAULT_PPM_FRAME_LENGTH(moduleIdx);
                }
                break;
            }
          }
        }
        break;
      }

#if defined(PCBX7) || defined(PCBX9LITE)
      case ITEM_MODEL_TRAINER_PARAMS:
        lcdDrawTextAlignedLeft(y, STR_PPMFRAME);
        lcdDrawText(MODEL_SETUP_2ND_COLUMN+3*FW, y, STR_MS);
        lcdDrawNumber(MODEL_SETUP_2ND_COLUMN, y, (int16_t)g_model.trainerData.frameLength*5 + 225, (menuHorizontalPosition<=0 ? attr : 0) | PREC1|LEFT);
        lcdDrawChar(MODEL_SETUP_2ND_COLUMN+8*FW+2, y, 'u');
        lcdDrawNumber(MODEL_SETUP_2ND_COLUMN+8*FW+2, y, (g_model.trainerData.delay*50)+300, RIGHT | ((CURSOR_ON_LINE() || menuHorizontalPosition==1) ? attr : 0));
        lcdDrawChar(MODEL_SETUP_2ND_COLUMN+10*FW, y, g_model.trainerData.pulsePol ? '+' : '-', (CURSOR_ON_LINE() || menuHorizontalPosition==2) ? attr : 0);
        if (attr && editMode > 0) {
          switch (menuHorizontalPosition) {
            case 0:
              CHECK_INCDEC_MODELVAR(event, g_model.trainerData.frameLength, -20, 35);
              break;
            case 1:
              CHECK_INCDEC_MODELVAR(event, g_model.trainerData.delay, -4, 10);
              break;
            case 2:
              CHECK_INCDEC_MODELVAR_ZERO(event, g_model.trainerData.pulsePol, 1);
              break;
          }
        }
        break;
#endif

#if defined(PXX2)
      case ITEM_MODEL_REGISTRATION_ID:
        lcdDrawTextAlignedLeft(y, STR_REG_ID);
        if (isDefaultModelRegistrationID())
          lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, STR_PXX2_DEFAULT);
        else
          lcdDrawSizedText(MODEL_SETUP_2ND_COLUMN, y, g_model.modelRegistrationID, PXX2_LEN_REGISTRATION_ID, ZCHAR);
        break;

      case ITEM_MODEL_INTERNAL_MODULE_PXX2_MODEL_NUM:
      case ITEM_MODEL_EXTERNAL_MODULE_PXX2_MODEL_NUM:
      {
        uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
        lcdDrawTextAlignedLeft(y, STR_RECEIVER_NUM);
        lcdDrawNumber(MODEL_SETUP_2ND_COLUMN, y, g_model.header.modelId[moduleIdx], attr | LEADING0 | LEFT, 2);
        if (attr) {
          CHECK_INCDEC_MODELVAR_ZERO(event, g_model.header.modelId[moduleIdx], MAX_RX_NUM(moduleIdx));
          if (checkIncDec_Ret) {
            modelHeaders[g_eeGeneral.currModel].modelId[moduleIdx] = g_model.header.modelId[moduleIdx];
          }
        }
      }
      break;

      case ITEM_MODEL_INTERNAL_MODULE_PXX2_REGISTER_RANGE:
      case ITEM_MODEL_EXTERNAL_MODULE_PXX2_REGISTER_RANGE:
      {
        uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
        lcdDrawTextAlignedLeft(y, STR_MODULE);
        lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, STR_REGISTER_BUTTON, (menuHorizontalPosition == 0 ? attr : 0));
        lcdDrawText(lcdLastRightPos + 3, y, STR_MODULE_RANGE, (menuHorizontalPosition == 1 ? attr : 0));
        if (attr) {
          if (moduleState[moduleIdx].mode == MODULE_MODE_NORMAL && s_editMode > 0) {
            if (menuHorizontalPosition == 0 && event == EVT_BUTTON_PRESSED()) {
              startRegisterDialog(moduleIdx);
            }
            else if (menuHorizontalPosition == 1) {
              moduleState[moduleIdx].mode = MODULE_MODE_RANGECHECK;
            }
          }
          if (s_editMode == 0 && !warningText) {
            moduleState[moduleIdx].mode = MODULE_MODE_NORMAL;
          }
          if (moduleState[moduleIdx].mode == MODULE_MODE_NORMAL) {
            // REGISTER finished
            s_editMode = 0;
          }
        }
      }
      break;

      case ITEM_MODEL_INTERNAL_MODULE_PXX2_OPTIONS:
      case ITEM_MODEL_EXTERNAL_MODULE_PXX2_OPTIONS:
        lcdDrawText(INDENT_WIDTH, y, STR_OPTIONS);
        lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, STR_SET, attr);
        if (event == EVT_KEY_BREAK(KEY_ENTER) && attr) {
          g_moduleIdx = CURRENT_MODULE_EDITED(k);
          pushMenu(menuModelModuleOptions);
        }
        break;

      case ITEM_MODEL_INTERNAL_MODULE_PXX2_RECEIVER_1:
      case ITEM_MODEL_INTERNAL_MODULE_PXX2_RECEIVER_2:
      case ITEM_MODEL_INTERNAL_MODULE_PXX2_RECEIVER_3:
      case ITEM_MODEL_EXTERNAL_MODULE_PXX2_RECEIVER_1:
      case ITEM_MODEL_EXTERNAL_MODULE_PXX2_RECEIVER_2:
      case ITEM_MODEL_EXTERNAL_MODULE_PXX2_RECEIVER_3:
      {
        uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
        uint8_t receiverIdx = CURRENT_RECEIVER_EDITED(k);
        ModuleInformation & moduleInformation = reusableBuffer.moduleSetup.pxx2.moduleInformation;

        drawStringWithIndex(INDENT_WIDTH, y, STR_RECEIVER, receiverIdx + 1);

        if (!(g_model.moduleData[moduleIdx].pxx2.receivers & (1 << receiverIdx))) {
          lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, STR_MODULE_BIND, attr);
          if (attr && s_editMode > 0) {
            s_editMode = 0;
            killEvents(event);
            g_model.moduleData[moduleIdx].pxx2.receivers |= (1 << receiverIdx);
            memclear(g_model.moduleData[moduleIdx].pxx2.receiverName[receiverIdx], PXX2_LEN_RX_NAME);
            storageDirty(EE_MODEL);
          }
          else {
            break;
          }
        }

        drawReceiverName(MODEL_SETUP_2ND_COLUMN, y, moduleIdx, receiverIdx, attr);

        if (s_editMode && isModuleR9M2(moduleIdx) && moduleState[moduleIdx].mode == MODULE_MODE_NORMAL && moduleInformation.information.modelID) {
          moduleInformation.information.modelID = 0;
          moduleState[moduleIdx].startBind(&reusableBuffer.moduleSetup.bindInformation);
        }

        if (attr && (moduleState[moduleIdx].mode == 0 || s_editMode == 0)) {
          if (moduleState[moduleIdx].mode) {
            moduleState[moduleIdx].mode = 0;
            removePXX2ReceiverIfEmpty(moduleIdx, receiverIdx);
            killEvents(event); // we stopped BIND / SHARE, we don't want to re-open the menu
            event = 0;
            CLEAR_POPUP();
          }
          s_editMode = 0;
        }

        if (moduleState[moduleIdx].mode == MODULE_MODE_BIND) {
          if (reusableBuffer.moduleSetup.bindInformation.step == BIND_INIT) {
            if (reusableBuffer.moduleSetup.bindInformation.candidateReceiversCount > 0) {
              popupMenuItemsCount = min<uint8_t>(reusableBuffer.moduleSetup.bindInformation.candidateReceiversCount, PXX2_MAX_RECEIVERS_PER_MODULE);
              for (uint8_t i = 0; i < popupMenuItemsCount; i++) {
                popupMenuItems[i] = reusableBuffer.moduleSetup.bindInformation.candidateReceiversNames[i];
              }
              popupMenuTitle = STR_PXX2_SELECT_RX;
              CLEAR_POPUP();
              POPUP_MENU_START(onPXX2BindMenu);
            }
            else {
              POPUP_WAIT(STR_WAITING_FOR_RX);
            }
          }
        }

        if (attr && EVT_KEY_MASK(event) == KEY_ENTER) {
          killEvents(event);
          if (!isSimu() && isPXX2ReceiverEmpty(moduleIdx, receiverIdx)) {
            onPXX2ReceiverMenu(STR_BIND);
          }
          else {
            POPUP_MENU_ADD_ITEM(STR_BIND);
            POPUP_MENU_ADD_ITEM(STR_OPTIONS);
            POPUP_MENU_ADD_ITEM(STR_SHARE);
            POPUP_MENU_ADD_ITEM(STR_DELETE);
            POPUP_MENU_ADD_ITEM(STR_RESET);
          }
          POPUP_MENU_START(onPXX2ReceiverMenu);
        }
      }
      break;
#endif

#if defined(PCBSKY9X)
      case ITEM_MODEL_EXTRA_MODULE_BIND:
#endif
#if defined(PCBTARANIS)
      case ITEM_MODEL_INTERNAL_MODULE_NPXX2_BIND:
      case ITEM_MODEL_EXTERNAL_MODULE_NPXX2_BIND:
#endif
      {
        uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
        ModuleData & moduleData = g_model.moduleData[moduleIdx];
        if (isModulePPM(moduleIdx)) {
          lcdDrawTextAlignedLeft(y, STR_PPMFRAME);
          lcdDrawText(MODEL_SETUP_2ND_COLUMN+3*FW, y, STR_MS);
          lcdDrawNumber(MODEL_SETUP_2ND_COLUMN, y, (int16_t)moduleData.ppm.frameLength*5 + 225, (menuHorizontalPosition<=0 ? attr : 0) | PREC1|LEFT);
          lcdDrawChar(MODEL_SETUP_2ND_COLUMN+8*FW+2, y, 'u');
          lcdDrawNumber(MODEL_SETUP_2ND_COLUMN+8*FW+2, y, (moduleData.ppm.delay*50)+300, RIGHT | ((CURSOR_ON_LINE() || menuHorizontalPosition==1) ? attr : 0));
          lcdDrawChar(MODEL_SETUP_2ND_COLUMN+10*FW, y, moduleData.ppm.pulsePol ? '+' : '-', (CURSOR_ON_LINE() || menuHorizontalPosition==2) ? attr : 0);
          if (attr && editMode > 0) {
            switch (menuHorizontalPosition) {
              case 0:
                CHECK_INCDEC_MODELVAR(event, moduleData.ppm.frameLength, -20, 35);
                break;
              case 1:
                CHECK_INCDEC_MODELVAR(event, moduleData.ppm.delay, -4, 10);
                break;
              case 2:
                CHECK_INCDEC_MODELVAR_ZERO(event, moduleData.ppm.pulsePol, 1);
                break;
            }
          }
        }
        else if (isModuleSBUS(moduleIdx)) {
          lcdDrawTextAlignedLeft(y, STR_REFRESHRATE);
          lcdDrawNumber(MODEL_SETUP_2ND_COLUMN, y, (int16_t)moduleData.ppm.frameLength*5 + 225, (menuHorizontalPosition<=0 ? attr : 0) | PREC1|LEFT);
          lcdDrawText(lcdLastRightPos, y, STR_MS);
          lcdDrawText(MODEL_SETUP_2ND_COLUMN+5*FW+2, y, moduleData.sbus.noninverted ? STR_NOT_INVERTED : STR_NORMAL, (CURSOR_ON_LINE() || menuHorizontalPosition==1) ? attr : 0);

          if (attr && s_editMode>0) {
            switch (menuHorizontalPosition) {
              case 0:
                CHECK_INCDEC_MODELVAR(event, moduleData.ppm.frameLength, -33, 35);
                break;
              case 1:
                CHECK_INCDEC_MODELVAR_ZERO(event, moduleData.sbus.noninverted, 1);
                break;
            }
          }
        }
        else {
          horzpos_t l_posHorz = menuHorizontalPosition;
          coord_t xOffsetBind = MODEL_SETUP_BIND_OFS;
          if (isModuleXJT(moduleIdx) && IS_D8_RX(moduleIdx)) {
            xOffsetBind = 0;
            lcdDrawText(INDENT_WIDTH, y, STR_RECEIVER);
            if (attr) l_posHorz += 1;
          }
          else {
            lcdDrawTextAlignedLeft(y, STR_RECEIVER_NUM);
          }
          if (isModulePXX(moduleIdx) || isModuleDSM2(moduleIdx) || isModuleMultimodule(moduleIdx)) {
            if (xOffsetBind)
              lcdDrawNumber(MODEL_SETUP_2ND_COLUMN, y, g_model.header.modelId[moduleIdx], (l_posHorz==0 ? attr : 0) | LEADING0|LEFT, 2);
            if (attr && l_posHorz == 0) {
              if (editMode > 0) {
                CHECK_INCDEC_MODELVAR_ZERO(event, g_model.header.modelId[moduleIdx], MAX_RX_NUM(moduleIdx));
                if (checkIncDec_Ret) {
                  modelHeaders[g_eeGeneral.currModel].modelId[moduleIdx] = g_model.header.modelId[moduleIdx];
                }
                else if (event == EVT_KEY_LONG(KEY_ENTER)) {
                  killEvents(event);
                  uint8_t newVal = findNextUnusedModelId(g_eeGeneral.currModel, moduleIdx);
                  if (newVal != g_model.header.modelId[moduleIdx]) {
                    modelHeaders[g_eeGeneral.currModel].modelId[moduleIdx] = g_model.header.modelId[moduleIdx] = newVal;
                    storageDirty(EE_MODEL);
                  }
                }
              }
            }
            lcdDrawText(MODEL_SETUP_2ND_COLUMN+xOffsetBind, y, STR_MODULE_BIND, l_posHorz==1 ? attr : 0);
            lcdDrawText(MODEL_SETUP_2ND_COLUMN+MODEL_SETUP_RANGE_OFS+xOffsetBind, y, STR_MODULE_RANGE, l_posHorz==2 ? attr : 0);
            uint8_t newFlag = 0;
#if defined(MULTIMODULE)
            if (multiBindStatus == MULTI_BIND_FINISHED) {
              multiBindStatus = MULTI_NORMAL_OPERATION;
              s_editMode = 0;
            }
#endif
#if defined(PCBTARANIS)
            if (attr && l_posHorz > 0) {
              if (s_editMode > 0) {
                if (l_posHorz == 1) {
                  if (isModuleR9M(moduleIdx) || (isModuleXJT(moduleIdx) && g_model.moduleData[moduleIdx].rfProtocol== RF_PROTO_X16)) {
#if defined(PCBXLITE)
                    if (EVT_KEY_MASK(event) == KEY_ENTER) {
#elif defined(PCBSKY9X) || defined(PCBAR9X)
                    if (event ==  EVT_KEY_FIRST(KEY_ENTER)) {
#else
                    if (event == EVT_KEY_BREAK(KEY_ENTER)) {
#endif
                      killEvents(event);
                      uint8_t default_selection = 0; // R9M_LBT should default to 0 as available options are variables
                      if (isModuleR9M_LBT(moduleIdx)) {
                        if (BIND_TELEM_ALLOWED(moduleIdx))
                          POPUP_MENU_ADD_ITEM(STR_BINDING_1_8_TELEM_ON);
                        POPUP_MENU_ADD_ITEM(STR_BINDING_1_8_TELEM_OFF);
                        if (BIND_TELEM_ALLOWED(moduleIdx) && BIND_CH9TO16_ALLOWED(moduleIdx))
                         POPUP_MENU_ADD_ITEM(STR_BINDING_9_16_TELEM_ON);
                        if (BIND_CH9TO16_ALLOWED(moduleIdx))
                          POPUP_MENU_ADD_ITEM(STR_BINDING_9_16_TELEM_OFF);
                      }
                      else {
                        if (BIND_TELEM_ALLOWED(moduleIdx))
                          POPUP_MENU_ADD_ITEM(STR_BINDING_1_8_TELEM_ON);
                        POPUP_MENU_ADD_ITEM(STR_BINDING_1_8_TELEM_OFF);
                        if (BIND_TELEM_ALLOWED(moduleIdx))
                          POPUP_MENU_ADD_ITEM(STR_BINDING_9_16_TELEM_ON);
                        POPUP_MENU_ADD_ITEM(STR_BINDING_9_16_TELEM_OFF);
                        default_selection = g_model.moduleData[moduleIdx].pxx.receiver_telem_off + (g_model.moduleData[moduleIdx].pxx.receiver_channel_9_16 << 1);
                      }
                      POPUP_MENU_SELECT_ITEM(default_selection);
                      POPUP_MENU_START(onBindMenu);
                      continue;
                    }
                    if (moduleState[moduleIdx].mode == MODULE_MODE_BIND) {
                      newFlag = MODULE_MODE_BIND;
                    }
                    else {
                      if (!popupMenuItemsCount) {
                        s_editMode = 0;  // this is when popup is exited before a choice is made
                      }
                    }
                  }
                  else {
                    newFlag = MODULE_MODE_BIND;
                  }
                }
                else if (l_posHorz == 2) {
                  newFlag = MODULE_MODE_RANGECHECK;
                }
              }
            }
#else
            if (attr && l_posHorz>0 && s_editMode>0) {
              if (l_posHorz == 1)
                newFlag = MODULE_MODE_BIND;
              else if (l_posHorz == 2)
                newFlag = MODULE_MODE_RANGECHECK;
            }
#endif
            moduleState[moduleIdx].mode = newFlag;

#if defined(MULTIMODULE)
            if (newFlag == MODULE_MODE_BIND) {
              multiBindStatus = MULTI_BIND_INITIATED;
            }
#endif

          }
        }
        break;
      }

#if defined(PCBSKY9X) && defined(REVX)
      case ITEM_MODEL_EXTERNAL_MODULE_OUTPUT_TYPE:
      {
        uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
        ModuleData & moduleData = g_model.moduleData[moduleIdx];
        moduleData.ppm.outputType = editChoice(MODEL_SETUP_2ND_COLUMN, y, STR_OUTPUT_TYPE, STR_VOUTPUT_TYPE, moduleData.ppm.outputType, 0, 1, attr, event);
        break;
      }
#endif

#if defined(PCBTARANIS)
      case ITEM_MODEL_INTERNAL_MODULE_FAILSAFE:
#endif
      case ITEM_MODEL_EXTERNAL_MODULE_FAILSAFE: {
        uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
        ModuleData &moduleData = g_model.moduleData[moduleIdx];
        lcdDrawTextAlignedLeft(y, STR_FAILSAFE);
        lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_VFAILSAFE, moduleData.failsafeMode, menuHorizontalPosition == 0 ? attr : 0);
        if (moduleData.failsafeMode == FAILSAFE_CUSTOM)
          lcdDrawText(MODEL_SETUP_2ND_COLUMN + MODEL_SETUP_SET_FAILSAFE_OFS, y, STR_SET, menuHorizontalPosition == 1 ? attr : 0);
        if (attr) {
          if (moduleData.failsafeMode != FAILSAFE_CUSTOM)
            menuHorizontalPosition = 0;
          if (menuHorizontalPosition == 0) {
            if (editMode > 0) {
              CHECK_INCDEC_MODELVAR_ZERO(event, moduleData.failsafeMode, FAILSAFE_LAST);
              if (checkIncDec_Ret) SEND_FAILSAFE_NOW(moduleIdx);
            }
          }
          else if (menuHorizontalPosition == 1) {
            s_editMode = 0;
            if (moduleData.failsafeMode == FAILSAFE_CUSTOM) {
              if (event == EVT_KEY_LONG(KEY_ENTER)) {
                killEvents(event);
                setCustomFailsafe(moduleIdx);
                storageDirty(EE_MODEL);
                AUDIO_WARNING1();
                SEND_FAILSAFE_NOW(moduleIdx);
              }
              else if (event == EVT_KEY_BREAK(KEY_ENTER) && attr) {
                g_moduleIdx = moduleIdx;
                pushMenu(menuModelFailsafe);
              }
            }
          }
          else {
            lcdDrawSolidFilledRect(MODEL_SETUP_2ND_COLUMN, y, LCD_W - MODEL_SETUP_2ND_COLUMN, 8);
          }
        }
      }
      break;

#if defined(PCBXLITE)
      case ITEM_MODEL_INTERNAL_MODULE_ANTENNA:
      {
        uint8_t newAntennaSel = editChoice(MODEL_SETUP_2ND_COLUMN, y, STR_ANTENNASELECTION, STR_VANTENNATYPES, g_model.moduleData[INTERNAL_MODULE].pxx.external_antenna, 0, 1, attr, event);
        if (newAntennaSel != g_model.moduleData[INTERNAL_MODULE].pxx.external_antenna && newAntennaSel == XJT_EXTERNAL_ANTENNA) {
          POPUP_CONFIRMATION(STR_ANTENNACONFIRM1, nullptr);
          const char * w = STR_ANTENNACONFIRM2;
          SET_WARNING_INFO(w, strlen(w), 0);
        }
        else {
          g_model.moduleData[INTERNAL_MODULE].pxx.external_antenna = newAntennaSel;
        }
        break;
      }
#endif
      case ITEM_MODEL_EXTERNAL_MODULE_OPTIONS:
      {
        uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
#if defined(MULTIMODULE)
        if (isModuleMultimodule(moduleIdx)) {
          int optionValue = g_model.moduleData[moduleIdx].multi.optionValue;

          const uint8_t multi_proto = g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(true);
          const mm_protocol_definition * pdef = getMultiProtocolDefinition(multi_proto);
          if (pdef->optionsstr)
            lcdDrawTextAlignedLeft(y, pdef->optionsstr);

          if (multi_proto == MM_RF_PROTO_FS_AFHDS2A)
            optionValue = 50 + 5 * optionValue;

          lcdDrawNumber(MODEL_SETUP_2ND_COLUMN, y, optionValue, LEFT | attr);
          if (attr) {
            if (multi_proto == MM_RF_PROTO_FS_AFHDS2A) {
              CHECK_INCDEC_MODELVAR(event, g_model.moduleData[moduleIdx].multi.optionValue, 0, 70);
            }
            else if (multi_proto == MM_RF_PROTO_OLRS) {
              CHECK_INCDEC_MODELVAR(event, g_model.moduleData[moduleIdx].multi.optionValue, -1, 7);
            }
            else {
              CHECK_INCDEC_MODELVAR(event, g_model.moduleData[moduleIdx].multi.optionValue, -128, 127);
            }
          }
        }
#endif
        if (isModuleR9M(moduleIdx)) {
          lcdDrawTextAlignedLeft(y, STR_MODULE_TELEMETRY);
          if (IS_TELEMETRY_INTERNAL_MODULE()) {
            lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, STR_DISABLE_INTERNAL);
          }
          else {
            lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, STR_MODULE_TELEM_ON);
          }
        }
        else if (isModuleSBUS(moduleIdx)) {
          lcdDrawTextAlignedLeft(y, STR_WARN_BATTVOLTAGE);
          putsVolts(lcdLastRightPos, y, getBatteryVoltage(), attr | PREC2 | LEFT);
        }
        break;
      }

      case ITEM_MODEL_EXTERNAL_MODULE_POWER:
      {
        uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
        if (isModuleR9M(moduleIdx)) {
          lcdDrawTextAlignedLeft(y, TR_MULTI_RFPOWER);
          if (isModuleR9M_FCC_VARIANT(moduleIdx)) {
            g_model.moduleData[moduleIdx].pxx.power = min((uint8_t)g_model.moduleData[moduleIdx].pxx.power, (uint8_t)R9M_FCC_POWER_MAX); // Lite FCC has only one setting
            if (g_model.moduleData[moduleIdx].type == MODULE_TYPE_PXX_R9M_LITE) { // R9M lite FCC has only one power value, so displayed for info only
              lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_R9M_LITE_FCC_POWER_VALUES, g_model.moduleData[moduleIdx].pxx.power, LEFT);
              if (attr)
                REPEAT_LAST_CURSOR_MOVE();
            }
            else {
              lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_R9M_FCC_POWER_VALUES, g_model.moduleData[moduleIdx].pxx.power, LEFT | attr);
              if (attr)
                CHECK_INCDEC_MODELVAR_ZERO(event, g_model.moduleData[moduleIdx].pxx.power, R9M_FCC_POWER_MAX);
            }
          }
          else if (g_model.moduleData[moduleIdx].type == MODULE_TYPE_PXX_R9M_LITE) {
            g_model.moduleData[moduleIdx].pxx.power = min((uint8_t)g_model.moduleData[moduleIdx].pxx.power, (uint8_t)R9M_LITE_LBT_POWER_MAX);
            lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_R9M_LITE_LBT_POWER_VALUES, g_model.moduleData[moduleIdx].pxx.power, LEFT | attr);
            if (attr) {
              CHECK_INCDEC_MODELVAR_ZERO(event, g_model.moduleData[moduleIdx].pxx.power, R9M_LITE_LBT_POWER_MAX);
            }
            if (attr && editMode == 0 && reusableBuffer.moduleSetup.r9mPower != g_model.moduleData[moduleIdx].pxx.power) {
              if((reusableBuffer.moduleSetup.r9mPower + g_model.moduleData[moduleIdx].pxx.power) < 5) { //switching between mode 2 and 3 does not require rebind
                POPUP_WARNING(STR_REBIND);
              }
              reusableBuffer.moduleSetup.r9mPower = g_model.moduleData[moduleIdx].pxx.power;
            }
          }
          else if (g_model.moduleData[moduleIdx].type == MODULE_TYPE_PXX_R9M) {
            g_model.moduleData[moduleIdx].pxx.power = min((uint8_t)g_model.moduleData[moduleIdx].pxx.power, (uint8_t)R9M_LBT_POWER_MAX);
            lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_R9M_LBT_POWER_VALUES, g_model.moduleData[moduleIdx].pxx.power, LEFT | attr);
            if (attr) {
              CHECK_INCDEC_MODELVAR_ZERO(event, g_model.moduleData[moduleIdx].pxx.power, R9M_LBT_POWER_MAX);
            }
            if (attr && editMode == 0 && reusableBuffer.moduleSetup.r9mPower != g_model.moduleData[moduleIdx].pxx.power) {
              if((reusableBuffer.moduleSetup.r9mPower + g_model.moduleData[moduleIdx].pxx.power) < 5) { //switching between mode 2 and 3 does not require rebind
                POPUP_WARNING(STR_REBIND);
              }
              reusableBuffer.moduleSetup.r9mPower = g_model.moduleData[moduleIdx].pxx.power;
            }
          }
        }
#if defined(MULTIMODULE)
        else if (isModuleMultimodule(moduleIdx)) {
          g_model.moduleData[EXTERNAL_MODULE].multi.lowPowerMode = editCheckBox(g_model.moduleData[EXTERNAL_MODULE].multi.lowPowerMode, MODEL_SETUP_2ND_COLUMN, y, STR_MULTI_LOWPOWER, attr, event);
        }
#endif

      }
      break;

#if defined(MULTIMODULE)
      case ITEM_MODEL_EXTERNAL_MODULE_AUTOBIND:
        if (g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(true) == MM_RF_PROTO_DSM2)
          g_model.moduleData[EXTERNAL_MODULE].multi.autoBindMode = editCheckBox(g_model.moduleData[EXTERNAL_MODULE].multi.autoBindMode, MODEL_SETUP_2ND_COLUMN, y, STR_MULTI_DSM_AUTODTECT, attr, event);
        else
          g_model.moduleData[EXTERNAL_MODULE].multi.autoBindMode = editCheckBox(g_model.moduleData[EXTERNAL_MODULE].multi.autoBindMode, MODEL_SETUP_2ND_COLUMN, y, STR_MULTI_AUTOBIND, attr, event);
        break;

      case ITEM_MODEL_EXTERNAL_MODULE_STATUS: {
        lcdDrawTextAlignedLeft(y, STR_MODULE_STATUS);

        char statusText[64];
        multiModuleStatus.getStatusString(statusText);
        lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, statusText);
        break;
      }

      case ITEM_MODEL_EXTERNAL_MODULE_SYNCSTATUS: {
        lcdDrawTextAlignedLeft(y, STR_MODULE_SYNC);

        char statusText[64];
        multiSyncStatus.getRefreshString(statusText);
        lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, statusText);
        break;
      }
#endif


#if 0
      case ITEM_MODEL_PPM2_PROTOCOL:
        lcdDrawTextAlignedLeft(y, "Port2");
        lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_VPROTOS, 0, 0);
        lcdDrawText(MODEL_SETUP_2ND_COLUMN+4*FW+3, y, STR_CH, menuHorizontalPosition<=0 ? attr : 0);
        lcdDrawNumber(lcdLastRightPos, y, g_model.moduleData[1].channelsStart+1, LEFT | (menuHorizontalPosition<=0 ? attr : 0));
        lcdDrawChar(lcdLastRightPos, y, '-');
        lcdDrawNumber(lcdLastRightPos + FW+1, y, g_model.moduleData[1].channelsStart+8+g_model.moduleData[1].channelsCount, LEFT | (menuHorizontalPosition!=0 ? attr : 0));
        if (attr && editMode > 0) {
          switch (menuHorizontalPosition) {
            case 0:
              CHECK_INCDEC_MODELVAR_ZERO(event, g_model.moduleData[1].channelsStart, 32-8-g_model.moduleData[1].channelsCount);
              SET_DEFAULT_PPM_FRAME_LENGTH(1);
              break;
            case 1:
              CHECK_INCDEC_MODELVAR(event, g_model.moduleData[1].channelsCount, -4, min<int8_t>(8, 32-8-g_model.moduleData[1].channelsStart));
              SET_DEFAULT_PPM_FRAME_LENGTH(1);
              break;
          }
        }
        break;

      case ITEM_MODEL_PPM2_PARAMS:
        lcdDrawTextAlignedLeft(y, STR_PPMFRAME);
        lcdDrawText(MODEL_SETUP_2ND_COLUMN+3*FW, y, STR_MS);
        lcdDrawNumber(MODEL_SETUP_2ND_COLUMN, y, (int16_t)g_model.moduleData[1].ppmFrameLength*5 + 225, (menuHorizontalPosition<=0 ? attr : 0) | PREC1 | LEFT);
        lcdDrawChar(MODEL_SETUP_2ND_COLUMN+8*FW+2, y, 'u');
        lcdDrawNumber(MODEL_SETUP_2ND_COLUMN+8*FW+2, y, (g_model.moduleData[1].ppmDelay*50)+300, RIGHT | ((menuHorizontalPosition < 0 || menuHorizontalPosition==1) ? attr : 0));
        lcdDrawChar(MODEL_SETUP_2ND_COLUMN+10*FW, y, g_model.moduleData[1].ppmPulsePol ? '+' : '-', (menuHorizontalPosition < 0 || menuHorizontalPosition==2) ? attr : 0);
        if (attr && editMode > 0) {
          switch (menuHorizontalPosition) {
            case 0:
              CHECK_INCDEC_MODELVAR(event, g_model.moduleData[1].ppmFrameLength, -20, 35);
              break;
            case 1:
              CHECK_INCDEC_MODELVAR(event, g_model.moduleData[1].ppmDelay, -4, 10);
              break;
            case 2:
              CHECK_INCDEC_MODELVAR_ZERO(event, g_model.moduleData[1].ppmPulsePol, 1);
              break;
          }
        }
        break;
#endif

    }
  }

#if defined(PXX)
  if (IS_RANGECHECK_ENABLE()) {
    showMessageBox("RSSI: ");
    lcdDrawNumber(WARNING_LINE_X, 5*FH, TELEMETRY_RSSI(), BOLD);
  }
#endif

  // some field just finished being edited
  if (old_editMode > 0 && s_editMode == 0) {
    switch(menuVerticalPosition) {
#if defined(PCBTARANIS)
      case ITEM_MODEL_INTERNAL_MODULE_NPXX2_BIND:
      case ITEM_MODEL_INTERNAL_MODULE_PXX2_MODEL_NUM:
        if (menuHorizontalPosition == 0)
          checkModelIdUnique(g_eeGeneral.currModel, INTERNAL_MODULE);
        break;
#endif
#if defined(PCBSKY9X)
      case ITEM_MODEL_EXTRA_MODULE_BIND:
        if (menuHorizontalPosition == 0)
          checkModelIdUnique(g_eeGeneral.currModel, EXTRA_MODULE);
        break;
#endif
      case ITEM_MODEL_EXTERNAL_MODULE_NPXX2_BIND:
      case ITEM_MODEL_EXTERNAL_MODULE_PXX2_MODEL_NUM:
        if (menuHorizontalPosition == 0)
          checkModelIdUnique(g_eeGeneral.currModel, EXTERNAL_MODULE);
        break;
    }
  }
}
