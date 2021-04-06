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
#include "storage/modelslist.h"

uint8_t g_moduleIdx;

enum MenuModelSetupItems {
  ITEM_MODEL_SETUP_NAME,
  ITEM_MODEL_SETUP_BITMAP,
  ITEM_MODEL_SETUP_TIMER1,
  ITEM_MODEL_SETUP_TIMER1_NAME,
  ITEM_MODEL_SETUP_TIMER1_PERSISTENT,
  ITEM_MODEL_SETUP_TIMER1_MINUTE_BEEP,
  ITEM_MODEL_SETUP_TIMER1_COUNTDOWN_BEEP,
#if TIMERS > 1
  ITEM_MODEL_SETUP_TIMER2,
  ITEM_MODEL_SETUP_TIMER2_NAME,
  ITEM_MODEL_SETUP_TIMER2_PERSISTENT,
  ITEM_MODEL_SETUP_TIMER2_MINUTE_BEEP,
  ITEM_MODEL_SETUP_TIMER2_COUNTDOWN_BEEP,
#endif
#if TIMERS > 2
  ITEM_MODEL_SETUP_TIMER3,
  ITEM_MODEL_SETUP_TIMER3_NAME,
  ITEM_MODEL_SETUP_TIMER3_PERSISTENT,
  ITEM_MODEL_SETUP_TIMER3_MINUTE_BEEP,
  ITEM_MODEL_SETUP_TIMER3_COUNTDOWN_BEEP,
#endif
  ITEM_MODEL_SETUP_EXTENDED_LIMITS,
  ITEM_MODEL_SETUP_EXTENDED_TRIMS,
  ITEM_MODEL_SETUP_DISPLAY_TRIMS,
  ITEM_MODEL_SETUP_TRIM_INC,
  ITEM_MODEL_SETUP_THROTTLE_LABEL,
  ITEM_MODEL_SETUP_THROTTLE_REVERSED,
  ITEM_MODEL_SETUP_THROTTLE_TRACE,
  ITEM_MODEL_SETUP_THROTTLE_TRIM,
  ITEM_MODEL_SETUP_THROTTLE_TRIM_SWITCH,
  ITEM_MODEL_SETUP_PREFLIGHT_LABEL,
  ITEM_MODEL_SETUP_CHECKLIST_DISPLAY,
  ITEM_MODEL_SETUP_THROTTLE_WARNING,
  ITEM_MODEL_SETUP_SWITCHES_WARNING,
  ITEM_MODEL_SETUP_POTS_SLIDERS_WARNING_STATE,
  ITEM_MODEL_SETUP_POTS_WARNING,
  ITEM_MODEL_SETUP_SLIDERS_WARNING,
  ITEM_MODEL_SETUP_BEEP_CENTER,
  ITEM_MODEL_SETUP_USE_GLOBAL_FUNCTIONS,

#if defined(PXX2)
  ITEM_MODEL_SETUP_REGISTRATION_ID,
#endif
#if defined(HARDWARE_INTERNAL_MODULE)
  ITEM_MODEL_SETUP_INTERNAL_MODULE_LABEL,
  ITEM_MODEL_SETUP_INTERNAL_MODULE_TYPE,
#if defined(MULTIMODULE)
  ITEM_MODEL_SETUP_INTERNAL_MODULE_STATUS,
  ITEM_MODEL_SETUP_INTERNAL_MODULE_SYNCSTATUS,
#endif
  ITEM_MODEL_SETUP_INTERNAL_MODULE_CHANNELS,
  ITEM_MODEL_SETUP_INTERNAL_MODULE_RACING_MODE,
  ITEM_MODEL_SETUP_INTERNAL_MODULE_NOT_ACCESS_RXNUM_BIND_RANGE,
  ITEM_MODEL_SETUP_INTERNAL_MODULE_PXX2_MODEL_NUM,
  ITEM_MODEL_SETUP_INTERNAL_MODULE_OPTIONS,
#if defined(MULTIMODULE)
  ITEM_MODEL_SETUP_INTERNAL_MODULE_AUTOBIND,
  ITEM_MODEL_SETUP_INTERNAL_MODULE_DISABLE_TELEM,
  ITEM_MODEL_SETUP_INTERNAL_MODULE_DISABLE_MAPPING,
#endif
#if defined(INTERNAL_MODULE_PXX1) && defined(EXTERNAL_ANTENNA)
  ITEM_MODEL_SETUP_INTERNAL_MODULE_ANTENNA,
#endif
  ITEM_MODEL_SETUP_INTERNAL_MODULE_POWER,
  ITEM_MODEL_SETUP_INTERNAL_MODULE_FAILSAFE,
  ITEM_MODEL_SETUP_INTERNAL_MODULE_PXX2_REGISTER_RANGE,
  ITEM_MODEL_SETUP_INTERNAL_MODULE_PXX2_OPTIONS,
  ITEM_MODEL_SETUP_INTERNAL_MODULE_PXX2_RECEIVER_1,
  ITEM_MODEL_SETUP_INTERNAL_MODULE_PXX2_RECEIVER_2,
  ITEM_MODEL_SETUP_INTERNAL_MODULE_PXX2_RECEIVER_3,
#endif
  ITEM_MODEL_SETUP_EXTERNAL_MODULE_LABEL,
  ITEM_MODEL_SETUP_EXTERNAL_MODULE_TYPE,
#if defined(MULTIMODULE)
  ITEM_MODEL_SETUP_EXTERNAL_MODULE_STATUS,
  ITEM_MODEL_SETUP_EXTERNAL_MODULE_SYNCSTATUS,
#endif
#if defined(AFHDS3)
  ITEM_MODEL_SETUP_EXTERNAL_MODULE_AFHDS3_MODE,
  ITEM_MODEL_SETUP_EXTERNAL_MODULE_AFHDS3_STATUS,
  ITEM_MODEL_SETUP_EXTERNAL_MODULE_AFHDS3_POWER_STATUS,
#endif
  ITEM_MODEL_SETUP_EXTERNAL_MODULE_CHANNELS,
  ITEM_MODEL_SETUP_EXTERNAL_MODULE_NOT_ACCESS_RXNUM_BIND_RANGE,
  ITEM_MODEL_SETUP_EXTERNAL_MODULE_PXX2_MODEL_NUM,
  ITEM_MODEL_SETUP_EXTERNAL_MODULE_OPTIONS,
#if defined(MULTIMODULE)
  ITEM_MODEL_SETUP_EXTERNAL_MODULE_AUTOBIND,
  ITEM_MODEL_SETUP_EXTERNAL_MODULE_DISABLE_TELEM,
  ITEM_MODEL_SETUP_EXTERNAL_MODULE_DISABLE_MAPPING,
#endif
#if defined(AFHDS3)
  ITEM_MODEL_SETUP_EXTERNAL_MODULE_AFHDS3_RX_FREQ,
  ITEM_MODEL_SETUP_EXTERNAL_MODULE_AFHDS3_ACTUAL_POWER,
#endif
  ITEM_MODEL_SETUP_EXTERNAL_MODULE_POWER,
  ITEM_MODEL_SETUP_EXTERNAL_MODULE_FAILSAFE,
  ITEM_MODEL_SETUP_EXTERNAL_MODULE_PXX2_REGISTER_RANGE,
  ITEM_MODEL_SETUP_EXTERNAL_MODULE_PXX2_OPTIONS,
  ITEM_MODEL_SETUP_EXTERNAL_MODULE_PXX2_RECEIVER_1,
  ITEM_MODEL_SETUP_EXTERNAL_MODULE_PXX2_RECEIVER_2,
  ITEM_MODEL_SETUP_EXTERNAL_MODULE_PXX2_RECEIVER_3,
  ITEM_MODEL_SETUP_TRAINER_LABEL,
  ITEM_MODEL_SETUP_TRAINER_MODE,
#if defined(BLUETOOTH)
  ITEM_MODEL_SETUP_TRAINER_BLUETOOTH,
#endif
  ITEM_MODEL_SETUP_TRAINER_CHANNELS,
  ITEM_MODEL_SETUP_TRAINER_PPM_PARAMS,
  ITEM_MODEL_SETUP_MAX
};

#define MODEL_SETUP_2ND_COLUMN         200
#define MODEL_SETUP_3RD_COLUMN         270
#define MODEL_SETUP_4TH_COLUMN         350
#define MODEL_SETUP_SET_FAILSAFE_OFS   100
#define MODEL_SETUP_SLIDPOT_SPACING    45

#define CURRENT_MODULE_EDITED(k)       (k >= ITEM_MODEL_SETUP_EXTERNAL_MODULE_LABEL ? EXTERNAL_MODULE : INTERNAL_MODULE)
#if defined(HARDWARE_INTERNAL_MODULE)
#define CURRENT_RECEIVER_EDITED(k)      (k - (k >= ITEM_MODEL_SETUP_EXTERNAL_MODULE_LABEL ? ITEM_MODEL_SETUP_EXTERNAL_MODULE_PXX2_RECEIVER_1 : ITEM_MODEL_SETUP_INTERNAL_MODULE_PXX2_RECEIVER_1))
#else
#define CURRENT_RECEIVER_EDITED(k)      (EXTERNAL_MODULE)
#endif

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

#include "common/stdlcd/model_setup_pxx1.cpp"

#if defined(AFHDS3)
#include "common/stdlcd/model_setup_afhds3.cpp"
#endif

void onPXX2R9MBindModeMenu(const char * result)
{
  if (result == STR_16CH_WITH_TELEMETRY) {
    reusableBuffer.moduleSetup.bindInformation.lbtMode = 1;
  }
  else if (result == STR_16CH_WITHOUT_TELEMETRY) {
    reusableBuffer.moduleSetup.bindInformation.lbtMode = 2;
  }
  else if (result == STR_FLEX_915) {
    reusableBuffer.moduleSetup.bindInformation.flexMode = 0;
  }
  else if (result == STR_FLEX_868) {
    reusableBuffer.moduleSetup.bindInformation.flexMode = 1;
  }
  else {
    // the user pressed [Exit]
    uint8_t moduleIdx = CURRENT_MODULE_EDITED(menuVerticalPosition);
    uint8_t receiverIdx = CURRENT_RECEIVER_EDITED(menuVerticalPosition);
    moduleState[moduleIdx].mode = MODULE_MODE_NORMAL;
    reusableBuffer.moduleSetup.bindInformation.step = 0;
    removePXX2ReceiverIfEmpty(moduleIdx, receiverIdx);
    return;
  }

#if defined(SIMU)
  uint8_t moduleIdx = CURRENT_MODULE_EDITED(menuVerticalPosition);
  uint8_t receiverIdx = CURRENT_RECEIVER_EDITED(menuVerticalPosition);
  memcpy(g_model.moduleData[moduleIdx].pxx2.receiverName[receiverIdx], reusableBuffer.moduleSetup.bindInformation.candidateReceiversNames[reusableBuffer.moduleSetup.bindInformation.selectedReceiverIndex], PXX2_LEN_RX_NAME);
  storageDirty(EE_MODEL);
  moduleState[moduleIdx].mode = MODULE_MODE_NORMAL;
  reusableBuffer.moduleSetup.bindInformation.step = BIND_OK;
  POPUP_INFORMATION(STR_BIND_OK);
#else
  reusableBuffer.moduleSetup.bindInformation.step = BIND_START;
#endif
}

// TODO code duplicated


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
    if (isModuleR9MAccess(moduleIdx)) {
      reusableBuffer.moduleSetup.bindInformation.step = BIND_MODULE_TX_INFORMATION_REQUEST;
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

// TODO code duplicated
void onPXX2BindMenu(const char * result)
{
  if (result != STR_EXIT) {
    uint8_t moduleIdx = CURRENT_MODULE_EDITED(menuVerticalPosition - HEADER_LINE);
    reusableBuffer.moduleSetup.bindInformation.selectedReceiverIndex = (result - reusableBuffer.moduleSetup.bindInformation.candidateReceiversNames[0]) / sizeof(reusableBuffer.moduleSetup.bindInformation.candidateReceiversNames[0]);
    if (isModuleR9MAccess(moduleIdx) && reusableBuffer.moduleSetup.pxx2.moduleInformation.information.variant == PXX2_VARIANT_EU) {
      reusableBuffer.moduleSetup.bindInformation.step = BIND_RX_NAME_SELECTED;
      POPUP_MENU_ADD_ITEM(STR_16CH_WITH_TELEMETRY);
      POPUP_MENU_ADD_ITEM(STR_16CH_WITHOUT_TELEMETRY);
      POPUP_MENU_START(onPXX2R9MBindModeMenu);
    }
    else if (isModuleR9MAccess(moduleIdx) && reusableBuffer.moduleSetup.pxx2.moduleInformation.information.variant == PXX2_VARIANT_FLEX) {
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
    s_editMode = 0;
  }
}

enum PopupRegisterItems {
  ITEM_REGISTER_PASSWORD,
  ITEM_REGISTER_MODULE_INDEX,
  ITEM_REGISTER_RECEIVER_NAME,
  ITEM_REGISTER_BUTTONS
};

constexpr coord_t REGISTER_TOP = POPUP_Y + 10;
constexpr coord_t REGISTER_H = 6 * FH - 4;
constexpr coord_t REGISTER_COLUMN_1 = POPUP_X + 10;
constexpr coord_t REGISTER_COLUMN_2 = POPUP_X + 150;

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

    case EVT_KEY_FIRST(KEY_EXIT):
      if (s_editMode <= 0) {
        warningText = nullptr;
      }
      break;
  }

  if (warningText) {
    const uint8_t dialogRows[] = { 0, 0, uint8_t(reusableBuffer.moduleSetup.pxx2.registerStep < REGISTER_RX_NAME_RECEIVED ? READONLY_ROW : 0), uint8_t(reusableBuffer.moduleSetup.pxx2.registerStep < REGISTER_RX_NAME_RECEIVED ? 0 : 1)};
    check(event, 0, nullptr, 0, dialogRows, 3, 4); // TODO add a comment for 3 - HEADER_LINE once understood

    drawPopupBackgroundAndBorder(POPUP_X, REGISTER_TOP, POPUP_W, REGISTER_H);
    // showMessageBox(warningText);

    // registration password
    lcdDrawText(REGISTER_COLUMN_1, REGISTER_TOP + 8, STR_REG_ID);
    editName(REGISTER_COLUMN_2, REGISTER_TOP + 8, g_model.modelRegistrationID, PXX2_LEN_REGISTRATION_ID, event, menuVerticalPosition == ITEM_REGISTER_PASSWORD);

    // loop index (will be removed in future)
    lcdDrawText(REGISTER_COLUMN_1, REGISTER_TOP + 8 + FH, "UID");
    if (menuVerticalPosition == ITEM_REGISTER_MODULE_INDEX && s_editMode) {
      CHECK_INCDEC_MODELVAR_ZERO(event, reusableBuffer.moduleSetup.pxx2.registerLoopIndex, 2);
    }
    lcdDrawNumber(REGISTER_COLUMN_2, REGISTER_TOP + 8 + FH, reusableBuffer.moduleSetup.pxx2.registerLoopIndex, menuVerticalPosition == ITEM_REGISTER_MODULE_INDEX ? (s_editMode ? INVERS : INVERS) : 0);

    // RX name
    if (reusableBuffer.moduleSetup.pxx2.registerStep < REGISTER_RX_NAME_RECEIVED) {
      lcdDrawText(REGISTER_COLUMN_1, REGISTER_TOP + 8 + 2 * FH, STR_WAITING);
      lcdDrawText(REGISTER_COLUMN_1, REGISTER_TOP + 8 + 4 * FH, TR_EXIT, menuVerticalPosition == ITEM_REGISTER_BUTTONS ? INVERS : 0);
    }
    else {
      lcdDrawText(REGISTER_COLUMN_1, REGISTER_TOP + 8 + 2 * FH, STR_RX_NAME);
      editName(REGISTER_COLUMN_2, REGISTER_TOP + 8 + 2 * FH, reusableBuffer.moduleSetup.pxx2.registerRxName, PXX2_LEN_RX_NAME, event, menuVerticalPosition == ITEM_REGISTER_RECEIVER_NAME);
      lcdDrawText(REGISTER_COLUMN_1, REGISTER_TOP + 8 + 4 * FH, TR_ENTER, menuVerticalPosition == ITEM_REGISTER_BUTTONS && menuHorizontalPosition == 0 ? INVERS : 0);
      lcdDrawText(REGISTER_COLUMN_2, REGISTER_TOP + 8 + 4 * FH, TR_EXIT, menuVerticalPosition == ITEM_REGISTER_BUTTONS && menuHorizontalPosition == 1 ? INVERS : 0);
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
  moduleState[module].mode = MODULE_MODE_REGISTER;
  s_editMode = 0;
  POPUP_INPUT("", runPopupRegister);
}

void checkModelIdUnique(uint8_t moduleIdx)
{
  if (isModuleXJTD8(moduleIdx))
    return;

  char * warn_buf = reusableBuffer.moduleSetup.msg;

  // cannot rely exactly on WARNING_LINE_LEN so using WARNING_LINE_LEN-2
  size_t warn_buf_len = sizeof(reusableBuffer.moduleSetup.msg) - WARNING_LINE_LEN - 2;
  if (!modelslist.isModelIdUnique(moduleIdx, warn_buf, warn_buf_len)) {
    if (warn_buf[0] != 0) {
      POPUP_WARNING(STR_MODELIDUSED);
      SET_WARNING_INFO(warn_buf, sizeof(reusableBuffer.moduleSetup.msg), 0);
    }
  }
}

void onModelSetupBitmapMenu(const char * result)
{
  if (result == STR_UPDATE_LIST) {
    if (!sdListFiles(BITMAPS_PATH, BITMAPS_EXT, sizeof(g_model.header.bitmap)-LEN_BITMAPS_EXT, NULL)) {
      POPUP_WARNING(STR_NO_BITMAPS_ON_SD);
    }
  }
  else if (result != STR_EXIT) {
    // The user choosed a bmp file in the list
    copySelection(g_model.header.bitmap, result, sizeof(g_model.header.bitmap));
    storageDirty(EE_MODEL);
    if (modelslist.getCurrentModel())
      modelslist.getCurrentModel()->resetBuffer();
  }
}

void editTimerMode(int timerIdx, coord_t y, LcdFlags attr, event_t event)
{
  TimerData & timer = g_model.timers[timerIdx];
  if (attr && menuHorizontalPosition < 0) {
    lcdDrawSolidFilledRect(MODEL_SETUP_2ND_COLUMN-INVERT_HORZ_MARGIN, y-INVERT_VERT_MARGIN+1, 115+2*INVERT_HORZ_MARGIN, INVERT_LINE_HEIGHT, TEXT_INVERTED_BGCOLOR);
  }
  drawStringWithIndex(MENUS_MARGIN_LEFT, y, STR_TIMER, timerIdx+1);
  drawTimerMode(MODEL_SETUP_2ND_COLUMN, y, timer.mode, (menuHorizontalPosition<=0 ? attr : 0));
  drawTimer(MODEL_SETUP_2ND_COLUMN+50, y, timer.start, (menuHorizontalPosition!=0 ? attr|TIMEHOUR : TIMEHOUR));
  if (attr && s_editMode>0) {
    switch (menuHorizontalPosition) {
      case 0:
      {
        int32_t timerMode = timer.mode;
        if (timerMode < 0) timerMode -= TMRMODE_COUNT-1;
        CHECK_INCDEC_MODELVAR_CHECK(event, timerMode, -TMRMODE_COUNT-SWSRC_LAST+1, TMRMODE_COUNT+SWSRC_LAST-1, isSwitchAvailableInTimers);
        if (timerMode < 0) timerMode += TMRMODE_COUNT-1;
        timer.mode = timerMode;
#if defined(AUTOSWITCH)
        if (s_editMode>0) {
          int8_t val = timer.mode - (TMRMODE_COUNT-1);
          int8_t switchVal = checkIncDecMovedSwitch(val);
          if (val != switchVal) {
            timer.mode = switchVal + (TMRMODE_COUNT-1);
            storageDirty(EE_MODEL);
          }
        }
#endif
        break;
      }
      case 1:
      {
        const int stopsMinutes[] = { 8, 60, 120, 180, 240, 300, 600, 900, 1200 };
        timer.start = checkIncDec(event, timer.start, 0, TIMER_MAX, EE_MODEL, NULL, (const CheckIncDecStops&)stopsMinutes);
        break;
      }
    }
  }
}

void editTimerCountdown(int timerIdx, coord_t y, LcdFlags attr, event_t event)
{
  TimerData & timer = g_model.timers[timerIdx];
  lcdDrawText(MENUS_MARGIN_LEFT, y, STR_BEEPCOUNTDOWN);
  lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_VBEEPCOUNTDOWN, timer.countdownBeep, (menuHorizontalPosition == 0 ? attr : 0));
  if (timer.countdownBeep != COUNTDOWN_SILENT) {
    lcdDrawNumber(MODEL_SETUP_3RD_COLUMN, y, TIMER_COUNTDOWN_START(timerIdx), (menuHorizontalPosition == 1 ? attr : 0) | LEFT, 0, NULL, "s");
  }
  if (attr && s_editMode > 0) {
    switch (menuHorizontalPosition) {
      case 0:
        CHECK_INCDEC_MODELVAR(event, timer.countdownBeep, COUNTDOWN_SILENT, COUNTDOWN_COUNT - 1);
        break;
      case 1:
        timer.countdownStart = -checkIncDecModel(event, -timer.countdownStart, -1, +2);
        break;
    }
  }
}

int getSwitchWarningsCount()
{
  int count = 0;
  for (int i=0; i<NUM_SWITCHES; ++i) {
    if (SWITCH_WARNING_ALLOWED(i)) {
      ++count;
    }
  }
  return count;
}

#define IF_INTERNAL_MODULE_ON(x)             (IS_INTERNAL_MODULE_ENABLED() ? (uint8_t)(x) : HIDDEN_ROW)
#define IF_EXTERNAL_MODULE_ON(x)             (IS_EXTERNAL_MODULE_ENABLED() ? (uint8_t)(x) : HIDDEN_ROW)

#define IF_PXX2_MODULE(module, xxx)          (isModulePXX2(module) ? (uint8_t)(xxx) : HIDDEN_ROW)
#define IF_NOT_PXX2_MODULE(module, xxx)      (isModulePXX2(module) ? HIDDEN_ROW : (uint8_t)(xxx))
#define IF_ACCESS_MODULE_RF(module, xxx)     (isModuleRFAccess(module) ? (uint8_t)(xxx) : HIDDEN_ROW)
#define IF_NOT_ACCESS_MODULE_RF(module, xxx) (isModuleRFAccess(module) ? HIDDEN_ROW : (uint8_t)(xxx))

#define TIMER_ROWS(x)                        NAVIGATION_LINE_BY_LINE|1, 0, 0, 0, g_model.timers[x].countdownBeep != COUNTDOWN_SILENT ? (uint8_t)1 : (uint8_t)0

inline uint8_t MODULE_TYPE_ROWS(int moduleIdx)
{
  if (isModuleXJT(moduleIdx) || isModuleR9MNonAccess(moduleIdx) || isModuleDSM2(moduleIdx) || isModuleISRM(moduleIdx) || isModuleAFHDS3(moduleIdx))
    return 1;
#if defined(MULTIMODULE)
  else if (isModuleMultimodule(moduleIdx)) {
    return 1 + MULTIMODULE_RFPROTO_COLUMNS(moduleIdx);
  }
#endif
  else
    return 0;
}

#if TIMERS == 1
#define TIMERS_ROWS                          TIMER_ROWS(0)
#elif TIMERS == 2
#define TIMERS_ROWS                          TIMER_ROWS(0), TIMER_ROWS(1)
#elif TIMERS == 3
#define TIMERS_ROWS                          TIMER_ROWS(0), TIMER_ROWS(1), TIMER_ROWS(2)
#endif

#if defined(BLUETOOTH)
#define TRAINER_CHANNELS_ROW                 (g_model.trainerData.mode == TRAINER_MODE_SLAVE ? (uint8_t)1 : (g_model.trainerData.mode == TRAINER_MODE_SLAVE_BLUETOOTH ? (uint8_t)0 : HIDDEN_ROW))
#define IF_BT_TRAINER_ON(x)                  (g_eeGeneral.bluetoothMode == BLUETOOTH_TRAINER ? (uint8_t)(x) : HIDDEN_ROW)
#define TRAINER_BLUETOOTH_M_ROW              ((bluetooth.distantAddr[0] == '\0' || bluetooth.state == BLUETOOTH_STATE_CONNECTED) ? (uint8_t)0 : (uint8_t)1)
#define TRAINER_BLUETOOTH_S_ROW              (bluetooth.distantAddr[0] == '\0' ? HIDDEN_ROW : LABEL())
#define TRAINER_BLUETOOTH_ROW                (g_model.trainerData.mode == TRAINER_MODE_MASTER_BLUETOOTH ? TRAINER_BLUETOOTH_M_ROW : (g_model.trainerData.mode == TRAINER_MODE_SLAVE_BLUETOOTH ? TRAINER_BLUETOOTH_S_ROW : HIDDEN_ROW))
#define TRAINER_ROWS                         LABEL(Trainer), 0, IF_BT_TRAINER_ON(TRAINER_BLUETOOTH_ROW), TRAINER_CHANNELS_ROW, TRAINER_PPM_PARAMS_ROW
#else
#define TRAINER_CHANNELS_ROW                 (g_model.trainerData.mode == TRAINER_MODE_SLAVE ? (uint8_t)1 :  HIDDEN_ROW)
#define TRAINER_ROWS                         LABEL(Trainer), 0, TRAINER_CHANNELS_ROW, TRAINER_PPM_PARAMS_ROW
#endif
#define TRAINER_PPM_PARAMS_ROW               (g_model.trainerData.mode == TRAINER_MODE_SLAVE ? (uint8_t)2 : HIDDEN_ROW)

#if defined(PXX2)
#define REGISTRATION_ID_ROWS                 uint8_t((isDefaultModelRegistrationID() || (warningText && popupFunc == runPopupRegister)) ? HIDDEN_ROW : READONLY_ROW),
#else
#define REGISTRATION_ID_ROWS
#endif

#if defined(INTERNAL_MODULE_PXX1) && defined(EXTERNAL_ANTENNA)
#define EXTERNAL_ANTENNA_ROW                 ((isModuleXJT(INTERNAL_MODULE) && g_eeGeneral.antennaMode == ANTENNA_MODE_PER_MODEL) ? (uint8_t)0 : HIDDEN_ROW),
void onModelAntennaSwitchConfirm(const char * result)
{
  if (result == STR_OK) {
    // Switch to external antenna confirmation
    g_model.moduleData[INTERNAL_MODULE].pxx.antennaMode = ANTENNA_MODE_EXTERNAL;
    globalData.externalAntennaEnabled = true;
    storageDirty(EE_MODEL);
  }
  else {
    reusableBuffer.moduleSetup.antennaMode = g_model.moduleData[INTERNAL_MODULE].pxx.antennaMode;
  }
}
#else
#define EXTERNAL_ANTENNA_ROW
#endif

#if defined(HARDWARE_INTERNAL_MODULE)
  #define INTERNAL_MODULE_ROWS \
         LABEL(InternalModule), \
         MODULE_TYPE_ROWS(INTERNAL_MODULE),         /* ITEM_MODEL_SETUP_INTERNAL_MODULE_TYPE*/ \
         MULTIMODULE_STATUS_ROWS(INTERNAL_MODULE)   /* ITEM_MODEL_SETUP_INTERNAL_MODULE_STATUS, ITEM_MODEL_SETUP_INTERNAL_MODULE_SYNCSTATUS */ \
         MODULE_CHANNELS_ROWS(INTERNAL_MODULE),     /* ITEM_MODEL_SETUP_INTERNAL_MODULE_CHANNELS*/ \
         IF_ALLOW_RACING_MODE(INTERNAL_MODULE),     /* ITEM_MODEL_SETUP_INTERNAL_MODULE_RACING_MODE */ \
         IF_NOT_ACCESS_MODULE_RF(INTERNAL_MODULE, MODULE_BIND_ROWS(INTERNAL_MODULE)), /* *ITEM_MODEL_SETUP_INTERNAL_MODULE_NOT_ACCESS_RXNUM_BIND_RANGE */\
         IF_ACCESS_MODULE_RF(INTERNAL_MODULE, 0),   /* ITEM_MODEL_SETUP_INTERNAL_MODULE_PXX2_MODEL_NUM*/ \
         MODULE_OPTION_ROW(INTERNAL_MODULE),        /* ITEM_MODEL_SETUP_INTERNAL_MODULE_OPTIONS */ \
         MULTIMODULE_MODULE_ROWS(INTERNAL_MODULE)   /* ITEM_MODEL_SETUP_INTERNAL_MODULE_AUTOBIND */  \
                                                    /* ITEM_MODEL_SETUP_INTERNAL_MODULE_DISABLE_TELEM */ \
                                                    /* ITEM_MODEL_SETUP_INTERNAL_MODULE_DISABLE_MAPPING */ \
         EXTERNAL_ANTENNA_ROW                       /* ITEM_MODEL_SETUP_INTERNAL_MODULE_ANTENNA */ \
         MODULE_POWER_ROW(INTERNAL_MODULE),         /* ITEM_MODEL_SETUP_INTERNAL_MODULE_POWER */ \
         IF_INTERNAL_MODULE_ON(FAILSAFE_ROWS(INTERNAL_MODULE)), /* ITEM_MODEL_SETUP_INTERNAL_MODULE_FAILSAFE */ \
         IF_ACCESS_MODULE_RF(INTERNAL_MODULE, 1),   /* ITEM_MODEL_SETUP_INTERNAL_MODULE_PXX2_REGISTER_RANGE */ \
         IF_PXX2_MODULE(INTERNAL_MODULE, 0),        /* ITEM_MODEL_SETUP_INTERNAL_MODULE_PXX2_OPTIONS*/ \
         IF_ACCESS_MODULE_RF(INTERNAL_MODULE, 0),   /* ITEM_MODEL_SETUP_INTERNAL_MODULE_PXX2_RECEIVER_1 */ \
         IF_ACCESS_MODULE_RF(INTERNAL_MODULE, 0),   /* ITEM_MODEL_SETUP_INTERNAL_MODULE_PXX2_RECEIVER_2 */ \
         IF_ACCESS_MODULE_RF(INTERNAL_MODULE, 0),   /* ITEM_MODEL_SETUP_INTERNAL_MODULE_PXX2_RECEIVER_3 */
#else
  #define INTERNAL_MODULE_ROWS
#endif


bool menuModelSetup(event_t event)
{
  bool CURSOR_ON_CELL = (menuHorizontalPosition >= 0);

  int8_t old_editMode = s_editMode;

  MENU(STR_MENUSETUP, MODEL_ICONS, menuTabModel, MENU_MODEL_SETUP, ITEM_MODEL_SETUP_MAX,
       { 0, // Model name
         0, // Model bitmap
         TIMERS_ROWS,
         0, // Extended limits
         1, // Extended trims
         0, // Show trims
         0, // Trims step

         LABEL(Throttle),
           0, // Throttle reverse
           0, // Throttle trace source
           0, // Throttle trim
           0, // Throttle trim switch

         LABEL(PreflightCheck),
           0, // Display checklist
           0, // Throttle state
           uint8_t(NAVIGATION_LINE_BY_LINE|(getSwitchWarningsCount()-1)), // Switches warnings
           0, // Pots & Sliders warning mode
           g_model.potsWarnMode ? uint8_t(NAVIGATION_LINE_BY_LINE|(NUM_POTS-1)) : HIDDEN_ROW, // Pots positions
           g_model.potsWarnMode ? uint8_t(NAVIGATION_LINE_BY_LINE|(NUM_SLIDERS-1)) : HIDDEN_ROW, // Sliders positions

         NAVIGATION_LINE_BY_LINE|(NUM_STICKS+NUM_POTS+NUM_SLIDERS-1), // Center beeps
         0, // Global functions

         REGISTRATION_ID_ROWS

         INTERNAL_MODULE_ROWS

         LABEL(ExternalModule),
           MODULE_TYPE_ROWS(EXTERNAL_MODULE),
           MULTIMODULE_STATUS_ROWS(EXTERNAL_MODULE)
           AFHDS3_MODE_ROWS(EXTERNAL_MODULE)
           MODULE_CHANNELS_ROWS(EXTERNAL_MODULE),
           IF_NOT_ACCESS_MODULE_RF(EXTERNAL_MODULE, MODULE_BIND_ROWS(EXTERNAL_MODULE)),
           IF_ACCESS_MODULE_RF(EXTERNAL_MODULE, 0),   // RxNum for ACCESS
           IF_NOT_PXX2_MODULE(EXTERNAL_MODULE, MODULE_OPTION_ROW(EXTERNAL_MODULE)),
           MULTIMODULE_MODULE_ROWS(EXTERNAL_MODULE)
           AFHDS3_MODULE_ROWS(EXTERNAL_MODULE)
           MODULE_POWER_ROW(EXTERNAL_MODULE),
           FAILSAFE_ROWS(EXTERNAL_MODULE),
           IF_ACCESS_MODULE_RF(EXTERNAL_MODULE, 1),   // Range check and Register buttons
           IF_PXX2_MODULE(EXTERNAL_MODULE, 0),        // Module options
           IF_ACCESS_MODULE_RF(EXTERNAL_MODULE, 0),   // Receiver 1
           IF_ACCESS_MODULE_RF(EXTERNAL_MODULE, 0),   // Receiver 2
           IF_ACCESS_MODULE_RF(EXTERNAL_MODULE, 0),   // Receiver 3

         TRAINER_ROWS
       });

  if (event == EVT_ENTRY || event == EVT_ENTRY_UP) {
    memclear(&reusableBuffer.moduleSetup, sizeof(reusableBuffer.moduleSetup));
    reusableBuffer.moduleSetup.r9mPower = g_model.moduleData[EXTERNAL_MODULE].pxx.power;
    reusableBuffer.moduleSetup.previousType = g_model.moduleData[EXTERNAL_MODULE].type;
    reusableBuffer.moduleSetup.newType = g_model.moduleData[EXTERNAL_MODULE].type;
#if defined(INTERNAL_MODULE_PXX1) && defined(EXTERNAL_ANTENNA)
    reusableBuffer.moduleSetup.antennaMode = g_model.moduleData[INTERNAL_MODULE].pxx.antennaMode;
#endif
  }

  if (menuEvent) {
    moduleState[0].mode = 0;
    moduleState[1].mode = 0;
  }

  int sub = menuVerticalPosition;

  for (uint8_t i = 0; i < NUM_BODY_LINES; ++i) {
    coord_t y = MENU_CONTENT_TOP + i*FH;
    uint8_t k = i + menuVerticalOffset;
    for (int j = 0; j <= k; j++) {
      if (mstate_tab[j] == HIDDEN_ROW)
        k++;
    }
    uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
    LcdFlags blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
    LcdFlags attr = (sub == k ? blink : 0);

    switch (k) {
      case ITEM_MODEL_SETUP_NAME:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MODELNAME);
        editName(MODEL_SETUP_2ND_COLUMN, y, g_model.header.name, sizeof(g_model.header.name), event, attr);
        break;

      case ITEM_MODEL_SETUP_BITMAP:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_BITMAP);
        if (ZEXIST(g_model.header.bitmap))
          lcdDrawSizedText(MODEL_SETUP_2ND_COLUMN, y, g_model.header.bitmap, sizeof(g_model.header.bitmap), attr);
        else
          lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_VCSWFUNC, 0, attr);
        if (attr && event==EVT_KEY_BREAK(KEY_ENTER) && READ_ONLY_UNLOCKED()) {
          s_editMode = 0;
          if (sdListFiles(BITMAPS_PATH, BITMAPS_EXT, sizeof(g_model.header.bitmap)-LEN_BITMAPS_EXT, g_model.header.bitmap, LIST_NONE_SD_FILE | LIST_SD_FILE_EXT)) {
            POPUP_MENU_START(onModelSetupBitmapMenu);
          }
          else {
            POPUP_WARNING(STR_NO_BITMAPS_ON_SD);
          }
        }
        break;

      case ITEM_MODEL_SETUP_TIMER1:
        editTimerMode(0, y, attr, event);
        break;

      case ITEM_MODEL_SETUP_TIMER1_NAME:
        lcdDrawText(MENUS_MARGIN_LEFT, y, INDENT TR_NAME);
        editName(MODEL_SETUP_2ND_COLUMN, y, g_model.timers[0].name, LEN_TIMER_NAME, event, attr);
        break;

      case ITEM_MODEL_SETUP_TIMER1_MINUTE_BEEP:
        lcdDrawText(MENUS_MARGIN_LEFT, y, INDENT TR_MINUTEBEEP);
        g_model.timers[0].minuteBeep = editCheckBox(g_model.timers[0].minuteBeep, MODEL_SETUP_2ND_COLUMN, y, attr, event);
        break;

      case ITEM_MODEL_SETUP_TIMER1_COUNTDOWN_BEEP:
        editTimerCountdown(0, y, attr, event);
        break;

      case ITEM_MODEL_SETUP_TIMER1_PERSISTENT:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_PERSISTENT);
        g_model.timers[0].persistent = editChoice(MODEL_SETUP_2ND_COLUMN, y, STR_VPERSISTENT, g_model.timers[0].persistent, 0, 2, attr, event);
        break;

#if TIMERS > 1
      case ITEM_MODEL_SETUP_TIMER2:
        editTimerMode(1, y, attr, event);
        break;

      case ITEM_MODEL_SETUP_TIMER2_NAME:
        lcdDrawText(MENUS_MARGIN_LEFT, y, INDENT TR_NAME);
        editName(MODEL_SETUP_2ND_COLUMN, y, g_model.timers[1].name, LEN_TIMER_NAME, event, attr);
        break;

      case ITEM_MODEL_SETUP_TIMER2_MINUTE_BEEP:
        lcdDrawText(MENUS_MARGIN_LEFT, y, INDENT TR_MINUTEBEEP);
        g_model.timers[1].minuteBeep = editCheckBox(g_model.timers[1].minuteBeep, MODEL_SETUP_2ND_COLUMN, y, attr, event);
        break;

      case ITEM_MODEL_SETUP_TIMER2_COUNTDOWN_BEEP:
        editTimerCountdown(1, y, attr, event);
        break;

      case ITEM_MODEL_SETUP_TIMER2_PERSISTENT:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_PERSISTENT);
        g_model.timers[1].persistent = editChoice(MODEL_SETUP_2ND_COLUMN, y, STR_VPERSISTENT, g_model.timers[1].persistent, 0, 2, attr, event);
        break;
#endif

#if TIMERS > 2
      case ITEM_MODEL_SETUP_TIMER3:
        editTimerMode(2, y, attr, event);
        break;

      case ITEM_MODEL_SETUP_TIMER3_NAME:
        lcdDrawText(MENUS_MARGIN_LEFT, y, INDENT TR_NAME);
        editName(MODEL_SETUP_2ND_COLUMN, y, g_model.timers[2].name, LEN_TIMER_NAME, event, attr);
        break;

      case ITEM_MODEL_SETUP_TIMER3_MINUTE_BEEP:
        lcdDrawText(MENUS_MARGIN_LEFT, y, INDENT TR_MINUTEBEEP);
        g_model.timers[2].minuteBeep = editCheckBox(g_model.timers[2].minuteBeep, MODEL_SETUP_2ND_COLUMN, y, attr, event);
        break;

      case ITEM_MODEL_SETUP_TIMER3_COUNTDOWN_BEEP:
        editTimerCountdown(2, y, attr, event);
        break;

      case ITEM_MODEL_SETUP_TIMER3_PERSISTENT:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_PERSISTENT);
        g_model.timers[2].persistent = editChoice(MODEL_SETUP_2ND_COLUMN, y, STR_VPERSISTENT, g_model.timers[2].persistent, 0, 2, attr, event);
        break;
#endif

      case ITEM_MODEL_SETUP_EXTENDED_LIMITS:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_ELIMITS);
        g_model.extendedLimits = editCheckBox(g_model.extendedLimits, MODEL_SETUP_2ND_COLUMN, y, attr, event);
        break;

      case ITEM_MODEL_SETUP_EXTENDED_TRIMS:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_ETRIMS);
        g_model.extendedTrims = editCheckBox(g_model.extendedTrims, MODEL_SETUP_2ND_COLUMN, y, menuHorizontalPosition<=0 ? attr : 0, event==EVT_KEY_BREAK(KEY_ENTER) ? event : 0);
        lcdDrawText(MODEL_SETUP_2ND_COLUMN+18, y, STR_RESET_BTN, menuHorizontalPosition>0  && !NO_HIGHLIGHT() ? attr : 0);
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

      case ITEM_MODEL_SETUP_DISPLAY_TRIMS:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_DISPLAY_TRIMS);
        g_model.displayTrims = editChoice(MODEL_SETUP_2ND_COLUMN, y, STR_VDISPLAYTRIMS, g_model.displayTrims, 0, 2, attr, event);
        break;

      case ITEM_MODEL_SETUP_TRIM_INC:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_TRIMINC);
        g_model.trimInc = editChoice(MODEL_SETUP_2ND_COLUMN, y, STR_VTRIMINC, g_model.trimInc, -2, 2, attr, event);
        break;

      case ITEM_MODEL_SETUP_THROTTLE_LABEL:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_THROTTLE_LABEL);
        break;

      case ITEM_MODEL_SETUP_THROTTLE_REVERSED:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_THROTTLEREVERSE);
        g_model.throttleReversed = editCheckBox(g_model.throttleReversed, MODEL_SETUP_2ND_COLUMN, y, attr, event);
        break;

      case ITEM_MODEL_SETUP_THROTTLE_TRACE:
      {
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_TTRACE);
        // check if source is available (EXT1 & EXT2 on X10)
        if (attr) CHECK_INCDEC_MODELVAR_ZERO_CHECK(event, g_model.thrTraceSrc, NUM_POTS+NUM_SLIDERS+MAX_OUTPUT_CHANNELS, isThrottleSourceAvailable);
        uint8_t idx = g_model.thrTraceSrc + MIXSRC_Thr;
        if (idx > MIXSRC_Thr)
          idx += 1;
        if (idx >= MIXSRC_FIRST_POT+NUM_POTS+NUM_SLIDERS)
          idx += MIXSRC_CH1 - MIXSRC_FIRST_POT - NUM_POTS - NUM_SLIDERS;
        drawSource(MODEL_SETUP_2ND_COLUMN, y, idx, attr);
        break;
      }

      case ITEM_MODEL_SETUP_THROTTLE_TRIM:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_TTRIM);
        g_model.thrTrim = editCheckBox(g_model.thrTrim, MODEL_SETUP_2ND_COLUMN, y, attr, event);
        break;

      case ITEM_MODEL_SETUP_THROTTLE_TRIM_SWITCH:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_TTRIM_SW);
        if (attr)
          CHECK_INCDEC_MODELVAR_ZERO(event, g_model.thrTrimSw, NUM_TRIMS - 1);
        drawSource(MODEL_SETUP_2ND_COLUMN, y, g_model.getThrottleStickTrimSource(), attr);
        break;

      case ITEM_MODEL_SETUP_PREFLIGHT_LABEL:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_PREFLIGHT);
        break;

      case ITEM_MODEL_SETUP_CHECKLIST_DISPLAY:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_CHECKLIST);
        g_model.displayChecklist = editCheckBox(g_model.displayChecklist, MODEL_SETUP_2ND_COLUMN, y, attr, event);
        break;

      case ITEM_MODEL_SETUP_THROTTLE_WARNING:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_THROTTLEWARNING);
        g_model.disableThrottleWarning = !editCheckBox(!g_model.disableThrottleWarning, MODEL_SETUP_2ND_COLUMN, y, attr, event);
        break;

      case ITEM_MODEL_SETUP_SWITCHES_WARNING:
      {
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_SWITCHWARNING);
        if (!READ_ONLY() && attr && menuHorizontalPosition<0 && event==EVT_KEY_LONG(KEY_ENTER)) {
          killEvents(event);
          START_NO_HIGHLIGHT();
          getMovedSwitch();
          for (int i=0; i<NUM_SWITCHES; i++) {
            bool enabled = ((g_model.switchWarningState >> (3*i)) & 0x07);
            if (enabled) {
              g_model.switchWarningState &= ~(0x07 << (3*i));
              unsigned int newState = (switches_states >> (2*i) & 0x03) + 1;
              g_model.switchWarningState |= (newState << (3*i));
            }
          }
          AUDIO_WARNING1();
          storageDirty(EE_MODEL);
        }

        if (attr && menuHorizontalPosition < 0) {
          lcdDrawSolidFilledRect(MODEL_SETUP_2ND_COLUMN-INVERT_HORZ_MARGIN, y-INVERT_VERT_MARGIN+1, (NUM_SWITCHES-1)*25+INVERT_HORZ_MARGIN, INVERT_LINE_HEIGHT, TEXT_INVERTED_BGCOLOR);
        }

        unsigned int newStates = 0;
        for (int i=0, current=0; i<NUM_SWITCHES; i++) {
          if (SWITCH_WARNING_ALLOWED(i)) {
            unsigned int state = ((g_model.switchWarningState >> (3*i)) & 0x07);
            LcdFlags color = (state > 0 ? TEXT_COLOR : TEXT_DISABLE_COLOR);
            if (attr && menuHorizontalPosition < 0) {
              color |= INVERS;
            }
            char s[3];
            s[0] = 'A' + i;
            s[1] = "x\300-\301"[state];
            s[2] = '\0';
            lcdDrawText(MODEL_SETUP_2ND_COLUMN+i*25, y, s, color|(menuHorizontalPosition==current ? attr : 0));
            if (!READ_ONLY() && attr && menuHorizontalPosition==current) {
              CHECK_INCDEC_MODELVAR_ZERO_CHECK(event, state, 3, IS_CONFIG_3POS(i) ? NULL : isSwitch2POSWarningStateAvailable);
            }
            newStates |= (state << (3*i));
            ++current;
          }
        }
        g_model.switchWarningState = newStates;
        break;
      }

      case ITEM_MODEL_SETUP_POTS_SLIDERS_WARNING_STATE:
        lcdDrawText(MENUS_MARGIN_LEFT, y,STR_POTWARNINGSTATE);
        lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, "\004""OFF\0""Man\0""Auto", g_model.potsWarnMode, attr);
        if (attr) {
          CHECK_INCDEC_MODELVAR(event, g_model.potsWarnMode, POTS_WARN_OFF, POTS_WARN_AUTO);
          storageDirty(EE_MODEL);
        }
        break;

      case ITEM_MODEL_SETUP_POTS_WARNING:
      {
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_POTWARNING);
        if (attr) {
          if (!READ_ONLY() && menuHorizontalPosition >= 0 && event==EVT_KEY_LONG(KEY_ENTER)) {
            killEvents(event);
            if (g_model.potsWarnMode == POTS_WARN_MANUAL) {
              SAVE_POT_POSITION(menuHorizontalPosition);
              AUDIO_WARNING1();
              storageDirty(EE_MODEL);
            }
          }

          if (!READ_ONLY() &&  menuHorizontalPosition >= 0 && s_editMode && event==EVT_KEY_BREAK(KEY_ENTER)) {
            s_editMode = 0;
            g_model.potsWarnEnabled ^= (1 << (menuHorizontalPosition));
            storageDirty(EE_MODEL);
          }
        }

        if (attr && menuHorizontalPosition < 0) {
          lcdDrawSolidFilledRect(MODEL_SETUP_2ND_COLUMN-INVERT_HORZ_MARGIN, y-INVERT_VERT_MARGIN+1, NUM_POTS*MODEL_SETUP_SLIDPOT_SPACING+INVERT_HORZ_MARGIN, INVERT_LINE_HEIGHT, TEXT_INVERTED_BGCOLOR);
        }

        if (g_model.potsWarnMode) {
          coord_t x = MODEL_SETUP_2ND_COLUMN;
          for (int i=0; i<NUM_POTS; ++i) {
            LcdFlags flags = (((menuHorizontalPosition==i) && attr) ? INVERS : 0);
            flags |= (g_model.potsWarnEnabled & (1 << i)) ? TEXT_DISABLE_COLOR : TEXT_COLOR;
            if (attr && menuHorizontalPosition < 0) {
              flags |= INVERS;
            }
            lcdDrawTextAtIndex(x, y, STR_VSRCRAW, NUM_STICKS+1+i, flags);
            x += MODEL_SETUP_SLIDPOT_SPACING;
          }
        }
        break;
      }

      case ITEM_MODEL_SETUP_SLIDERS_WARNING:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_SLIDERWARNING);
        if (attr) {
          if (!READ_ONLY() && menuHorizontalPosition+1 && event==EVT_KEY_LONG(KEY_ENTER)) {
            killEvents(event);
            if (g_model.potsWarnMode == POTS_WARN_MANUAL) {
              SAVE_POT_POSITION(menuHorizontalPosition+NUM_POTS);
              AUDIO_WARNING1();
              storageDirty(EE_MODEL);
            }
          }

          if (!READ_ONLY() && menuHorizontalPosition+1 && s_editMode && event==EVT_KEY_BREAK(KEY_ENTER)) {
            s_editMode = 0;
            g_model.potsWarnEnabled ^= (1 << (menuHorizontalPosition+NUM_POTS));
            storageDirty(EE_MODEL);
          }
        }

        if (attr && menuHorizontalPosition < 0) {
          lcdDrawSolidFilledRect(MODEL_SETUP_2ND_COLUMN-INVERT_HORZ_MARGIN, y-INVERT_VERT_MARGIN+1, NUM_SLIDERS*MODEL_SETUP_SLIDPOT_SPACING+INVERT_HORZ_MARGIN, INVERT_LINE_HEIGHT, TEXT_INVERTED_BGCOLOR);
        }

        if (g_model.potsWarnMode) {
          coord_t x = MODEL_SETUP_2ND_COLUMN;
          for (int i=NUM_POTS; i<NUM_POTS+NUM_SLIDERS; ++i) {
            LcdFlags flags = (((menuHorizontalPosition==i-NUM_POTS) && attr) ? INVERS : 0);
            flags |= (g_model.potsWarnEnabled & (1 << i)) ? TEXT_DISABLE_COLOR : TEXT_COLOR;
            if (attr && menuHorizontalPosition < 0) {
              flags |= INVERS;
            }
            lcdDrawTextAtIndex(x, y, STR_VSRCRAW, NUM_STICKS+1+i, flags);
            x += MODEL_SETUP_SLIDPOT_SPACING;
          }
        }
        break;

      case ITEM_MODEL_SETUP_BEEP_CENTER:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_BEEPCTR);
        lcdNextPos = MODEL_SETUP_2ND_COLUMN - 3;
        for (int i=0; i<NUM_STICKS+NUM_POTS+NUM_SLIDERS; i++) {
          if (i >= NUM_STICKS && (!IS_POT_SLIDER_AVAILABLE(i) || IS_POT_MULTIPOS(i))) {
            if (attr && menuHorizontalPosition == i) REPEAT_LAST_CURSOR_MOVE(NUM_STICKS+NUM_POTS+NUM_SLIDERS, true);
            continue;
          }
          LcdFlags flags = ((menuHorizontalPosition==i && attr) ? INVERS : 0);
          flags |= (g_model.beepANACenter & ((BeepANACenter)1<<i)) ? TEXT_COLOR : (TEXT_DISABLE_COLOR | NO_FONTCACHE);
          if (attr && menuHorizontalPosition < 0) flags |= INVERS;
          lcdDrawTextAtIndex(lcdNextPos+3, y, STR_RETA123, i, flags);
        }
        if (attr && CURSOR_ON_CELL) {
          if (event==EVT_KEY_BREAK(KEY_ENTER)) {
            if (READ_ONLY_UNLOCKED()) {
              s_editMode = 0;
              g_model.beepANACenter ^= ((BeepANACenter)1<<menuHorizontalPosition);
              storageDirty(EE_MODEL);
            }
          }
        }
        break;

      case ITEM_MODEL_SETUP_USE_GLOBAL_FUNCTIONS:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_USE_GLOBAL_FUNCS);
        drawCheckBox(MODEL_SETUP_2ND_COLUMN, y, !g_model.noGlobalFunctions, attr);
        if (attr) g_model.noGlobalFunctions = !checkIncDecModel(event, !g_model.noGlobalFunctions, 0, 1);
        break;

#if defined(HARDWARE_INTERNAL_MODULE)
      case ITEM_MODEL_SETUP_INTERNAL_MODULE_LABEL:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_INTERNALRF);
        break;

#if !defined(INTERNAL_MODULE_MULTI)
      case ITEM_MODEL_SETUP_INTERNAL_MODULE_TYPE:
        lcdDrawText(MENUS_MARGIN_LEFT + INDENT_WIDTH, y, STR_MODE);
        lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_INTERNAL_MODULE_PROTOCOLS, g_model.moduleData[INTERNAL_MODULE].type, menuHorizontalPosition==0 ? attr : 0);
        if (isModuleXJT(INTERNAL_MODULE))
          lcdDrawTextAtIndex(MODEL_SETUP_3RD_COLUMN, y, STR_XJT_ACCST_RF_PROTOCOLS, 1 + g_model.moduleData[INTERNAL_MODULE].subType, menuHorizontalPosition==1 ? attr : 0);
        else if (isModuleISRM(INTERNAL_MODULE))
          lcdDrawTextAtIndex(MODEL_SETUP_3RD_COLUMN, y, STR_ISRM_RF_PROTOCOLS, g_model.moduleData[INTERNAL_MODULE].subType, menuHorizontalPosition==1 ? attr : 0);
        if (attr) {
          if (menuHorizontalPosition == 0) {
            uint8_t moduleType = checkIncDec(event, g_model.moduleData[INTERNAL_MODULE].type, MODULE_TYPE_NONE, MODULE_TYPE_MAX, EE_MODEL, isInternalModuleAvailable);
            if (checkIncDec_Ret) {
              setModuleType(INTERNAL_MODULE, moduleType);
            }
          }
          else if (isModuleXJT(INTERNAL_MODULE)) {
            g_model.moduleData[INTERNAL_MODULE].subType = checkIncDec(event, g_model.moduleData[INTERNAL_MODULE].subType, 0, MODULE_SUBTYPE_PXX1_LAST, EE_MODEL, isRfProtocolAvailable);
            if (checkIncDec_Ret) {
              g_model.moduleData[0].type = MODULE_TYPE_XJT_PXX1;
              g_model.moduleData[0].channelsStart = 0;
              g_model.moduleData[0].channelsCount = defaultModuleChannels_M8(INTERNAL_MODULE);
            }
          }
          else if (isModulePXX2(INTERNAL_MODULE)) {
            g_model.moduleData[INTERNAL_MODULE].subType = checkIncDec(event, g_model.moduleData[INTERNAL_MODULE].subType, 0, MODULE_SUBTYPE_ISRM_PXX2_ACCST_D16, EE_MODEL);
            resetAccessAuthenticationCount();
          }
        }
        break;
#endif

#if defined(INTERNAL_MODULE_PXX1) && defined(EXTERNAL_ANTENNA)
      case ITEM_MODEL_SETUP_INTERNAL_MODULE_ANTENNA:
        lcdDrawText(MENUS_MARGIN_LEFT + INDENT_WIDTH, y, STR_ANTENNA);
        reusableBuffer.moduleSetup.antennaMode = editChoice(MODEL_SETUP_2ND_COLUMN, y, STR_ANTENNA_MODES, reusableBuffer.moduleSetup.antennaMode == ANTENNA_MODE_PER_MODEL ? ANTENNA_MODE_INTERNAL : reusableBuffer.moduleSetup.antennaMode, ANTENNA_MODE_INTERNAL, ANTENNA_MODE_EXTERNAL, attr, event, [](int value) { return value != ANTENNA_MODE_PER_MODEL; });
        if (event && !s_editMode && reusableBuffer.moduleSetup.antennaMode != g_model.moduleData[INTERNAL_MODULE].pxx.antennaMode) {
          if (reusableBuffer.moduleSetup.antennaMode == ANTENNA_MODE_EXTERNAL && !isExternalAntennaEnabled()) {
            POPUP_CONFIRMATION(STR_ANTENNACONFIRM1, onModelAntennaSwitchConfirm);
            SET_WARNING_INFO(STR_ANTENNACONFIRM2, sizeof(TR_ANTENNACONFIRM2), 0);
          }
          else {
            g_model.moduleData[INTERNAL_MODULE].pxx.antennaMode = reusableBuffer.moduleSetup.antennaMode;
            checkExternalAntenna();
          }
        }
        break;
#endif
#endif
      case ITEM_MODEL_SETUP_EXTERNAL_MODULE_LABEL:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_EXTERNALRF);
        break;

#if defined(INTERNAL_MODULE_MULTI)
      case ITEM_MODEL_SETUP_INTERNAL_MODULE_TYPE:
#endif
      case ITEM_MODEL_SETUP_EXTERNAL_MODULE_TYPE:
        lcdDrawText(MENUS_MARGIN_LEFT + INDENT_WIDTH, y, STR_MODE);
        lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_EXTERNAL_MODULE_PROTOCOLS, moduleIdx == EXTERNAL_MODULE ? reusableBuffer.moduleSetup.newType : g_model.moduleData[INTERNAL_MODULE].type, menuHorizontalPosition==0 ? attr : 0);
        if (isModuleXJT(moduleIdx))
          lcdDrawTextAtIndex(MODEL_SETUP_3RD_COLUMN, y, STR_XJT_ACCST_RF_PROTOCOLS, 1+g_model.moduleData[moduleIdx].subType, (menuHorizontalPosition==1 ? attr : 0));
        else if (isModuleDSM2(moduleIdx))
          lcdDrawTextAtIndex(MODEL_SETUP_3RD_COLUMN, y, STR_DSM_PROTOCOLS, g_model.moduleData[moduleIdx].rfProtocol, (menuHorizontalPosition==1 ? attr : 0));
        else if (isModuleR9MNonAccess(moduleIdx))
          lcdDrawTextAtIndex(MODEL_SETUP_3RD_COLUMN, y, STR_R9M_REGION, g_model.moduleData[moduleIdx].subType, (menuHorizontalPosition==1 ? attr : 0));
#if defined(MULTIMODULE)
        else if (isModuleMultimodule(moduleIdx)) {
          int multi_rfProto = g_model.moduleData[moduleIdx].getMultiProtocol();
          lcdDrawMultiProtocolString(MODEL_SETUP_3RD_COLUMN, y, moduleIdx, multi_rfProto, menuHorizontalPosition == 1 ? attr : 0);
          if (MULTIMODULE_HAS_SUBTYPE(moduleIdx))
            lcdDrawMultiSubProtocolString(MODEL_SETUP_4TH_COLUMN, y, moduleIdx, g_model.moduleData[moduleIdx].subType, menuHorizontalPosition==2 ? attr : 0);
        }
#endif
#if defined(AFHDS3)
        else if (isModuleAFHDS3(moduleIdx)) {
          lcdDrawTextAtIndex(lcdNextPos + 15, y, STR_AFHDS3_PROTOCOLS, g_model.moduleData[moduleIdx].subType, (menuHorizontalPosition==1 ? attr : 0));
        }
#endif
        if (attr && menuHorizontalPosition == 0 && moduleIdx == EXTERNAL_MODULE) {
          if (s_editMode > 0) {
            g_model.moduleData[EXTERNAL_MODULE].type = MODULE_TYPE_NONE;
          }
          else if (reusableBuffer.moduleSetup.newType != reusableBuffer.moduleSetup.previousType) {
            g_model.moduleData[EXTERNAL_MODULE].type = reusableBuffer.moduleSetup.newType;
            reusableBuffer.moduleSetup.previousType = reusableBuffer.moduleSetup.newType;
            setModuleType(EXTERNAL_MODULE, g_model.moduleData[EXTERNAL_MODULE].type);
          }
          else if (g_model.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_NONE) {
            g_model.moduleData[EXTERNAL_MODULE].type = reusableBuffer.moduleSetup.newType;
          }
        }
        if (attr) {
          if (s_editMode > 0) {
            switch (menuHorizontalPosition) {
              case 0: {
#if defined(HARDWARE_INTERNAL_MODULE)
                if (moduleIdx == INTERNAL_MODULE) {
                  uint8_t moduleType = checkIncDec(event, g_model.moduleData[moduleIdx].type, MODULE_TYPE_NONE, MODULE_TYPE_MAX, EE_MODEL,
                                                   isInternalModuleAvailable);
                  if (checkIncDec_Ret) {
                    setModuleType(moduleIdx, moduleType);
                  }
                }
                else
#endif
                  reusableBuffer.moduleSetup.newType = checkIncDec(event, reusableBuffer.moduleSetup.newType, MODULE_TYPE_NONE, MODULE_TYPE_MAX, EE_MODEL,
                                                                   isExternalModuleAvailable);
              }
              break;
              case 1:
                if (isModuleDSM2(moduleIdx))
                  CHECK_INCDEC_MODELVAR(event, g_model.moduleData[moduleIdx].rfProtocol, DSM2_PROTO_LP45, DSM2_PROTO_DSMX);
#if defined(MULTIMODULE)
                else if (isModuleMultimodule(moduleIdx)) {
                  int multiRfProto = g_model.moduleData[moduleIdx].getMultiProtocol();
                  MultiModuleStatus &status = getMultiModuleStatus(moduleIdx);
                  if (status.isValid()) {
                    int8_t direction = checkIncDec(event, 0, -1, 1);
                    if (direction == -1) {
                      if (multiRfProto == MODULE_SUBTYPE_MULTI_FRSKY)
                        multiRfProto = MODULE_SUBTYPE_MULTI_FRSKYX_RX;
                      else
                        multiRfProto = convertMultiToOtx(status.protocolPrev);
                    }
                    if (direction == 1) {
                      if (multiRfProto == MODULE_SUBTYPE_MULTI_FRSKY)
                        multiRfProto = MODULE_SUBTYPE_MULTI_FRSKYX2;
                      else
                        multiRfProto = convertMultiToOtx(status.protocolNext);
                    }
                  }
                  else {
                    CHECK_INCDEC_MODELVAR_CHECK(event, multiRfProto, MODULE_SUBTYPE_MULTI_FIRST, MULTI_MAX_PROTOCOLS, isMultiProtocolSelectable);
                  }
                  if (checkIncDec_Ret) {
                    g_model.moduleData[moduleIdx].setMultiProtocol(multiRfProto);
                    g_model.moduleData[moduleIdx].subType = 0;
                    resetMultiProtocolsOptions(moduleIdx);
                    storageDirty(EE_MODEL);
                  }
                }
#endif
                else if (isModuleR9MNonAccess(moduleIdx)) {
                  g_model.moduleData[moduleIdx].subType = checkIncDec(event, g_model.moduleData[moduleIdx].subType, MODULE_SUBTYPE_R9M_FCC,
                                                                            MODULE_SUBTYPE_R9M_LAST, EE_MODEL, isR9MModeAvailable);
                }
#if defined(AFHDS3)
                else if (isModuleAFHDS3(moduleIdx)) {
                  CHECK_INCDEC_MODELVAR(event, g_model.moduleData[moduleIdx].subType, AFHDS_SUBTYPE_FIRST, AFHDS_SUBTYPE_LAST);
                }
#endif
                else {
                  CHECK_INCDEC_MODELVAR(event, g_model.moduleData[moduleIdx].subType, MODULE_SUBTYPE_PXX1_ACCST_D16, MODULE_SUBTYPE_PXX1_LAST);
                }
                if (checkIncDec_Ret) {
                  g_model.moduleData[moduleIdx].channelsStart = 0;
                  g_model.moduleData[moduleIdx].channelsCount = defaultModuleChannels_M8(moduleIdx);
                }
                break;

#if defined(MULTIMODULE)
              case 2: {
                CHECK_INCDEC_MODELVAR(event, g_model.moduleData[moduleIdx].subType, 0, getMaxMultiSubtype(moduleIdx));
                if (checkIncDec_Ret) {
                  resetMultiProtocolsOptions(moduleIdx);
                }
                break;
              }
#endif
            }
          }
#if POPUP_LEVEL > 1
          else if (old_editMode > 0) {
            if (isModuleR9MNonAccess(moduleIdx)) {
              if (g_model.moduleData[moduleIdx].subType > MODULE_SUBTYPE_R9M_EU) {
                POPUP_WARNING(STR_MODULE_PROTOCOL_FLEX_WARN_LINE1);
                SET_WARNING_INFO(STR_MODULE_PROTOCOL_WARN_LINE2, sizeof(TR_MODULE_PROTOCOL_WARN_LINE2) - 1, 0);
              }
#if POPUP_LEVEL >= 3
              else if (g_model.moduleData[moduleIdx].subType == MODULE_SUBTYPE_R9M_EU) {
                POPUP_WARNING(STR_MODULE_PROTOCOL_EU_WARN_LINE1);
                SET_WARNING_INFO(STR_MODULE_PROTOCOL_WARN_LINE2, sizeof(TR_MODULE_PROTOCOL_WARN_LINE2) - 1, 0);
              }
              else {
                POPUP_WARNING(STR_MODULE_PROTOCOL_FCC_WARN_LINE1);
                SET_WARNING_INFO(STR_MODULE_PROTOCOL_WARN_LINE2, sizeof(TR_MODULE_PROTOCOL_WARN_LINE2) - 1, 0);
              }
#endif
            }
          }
#endif
        }
        break;
#if defined(HARDWARE_INTERNAL_MODULE)
      case ITEM_MODEL_SETUP_INTERNAL_MODULE_CHANNELS:
#endif
      case ITEM_MODEL_SETUP_EXTERNAL_MODULE_CHANNELS:
      {
        ModuleData & moduleData = g_model.moduleData[moduleIdx];
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_CHANNELRANGE);
        if ((int8_t)MODULE_CHANNELS_ROWS(moduleIdx) >= 0) {
          drawStringWithIndex(MODEL_SETUP_2ND_COLUMN, y, STR_CH, moduleData.channelsStart+1, menuHorizontalPosition==0 ? attr : 0);
          lcdDrawText(lcdNextPos+5, y, "-");
          drawStringWithIndex(lcdNextPos+5, y, STR_CH, moduleData.channelsStart+sentModuleChannels(moduleIdx), menuHorizontalPosition==1 ? attr : 0);
          const char * delay = getModuleDelay(moduleIdx);
          if (delay)
            lcdDrawText(lcdNextPos + 15, y, delay);
          if (attr && s_editMode>0) {
            switch (menuHorizontalPosition) {
              case 0:
                CHECK_INCDEC_MODELVAR_ZERO(event, moduleData.channelsStart, 32-8-moduleData.channelsCount);
                break;
              case 1:
                CHECK_INCDEC_MODELVAR_CHECK(event, moduleData.channelsCount, -4, min<int8_t>(maxModuleChannels_M8(moduleIdx), 32-8-moduleData.channelsStart), moduleData.type == MODULE_TYPE_ISRM_PXX2 ? isPxx2IsrmChannelsCountAllowed : nullptr);
                if (checkIncDec_Ret && moduleData.type == MODULE_TYPE_PPM) {
                  setDefaultPpmFrameLength(moduleIdx);
                }
                break;
            }
          }
        }
        break;
      }

#if defined(HARDWARE_INTERNAL_MODULE)
      case ITEM_MODEL_SETUP_INTERNAL_MODULE_RACING_MODE:
      {
        ModuleData & moduleData = g_model.moduleData[moduleIdx];
        lcdDrawText(MENUS_MARGIN_LEFT + INDENT_WIDTH, y, STR_RACING_MODE);
        moduleData.pxx2.racingMode = editCheckBox(moduleData.pxx2.racingMode, MODEL_SETUP_2ND_COLUMN, y, attr, event);
        break;
      }
#endif

      case ITEM_MODEL_SETUP_TRAINER_LABEL:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_TRAINER);
        break;

      case ITEM_MODEL_SETUP_TRAINER_MODE:
        lcdDrawText(MENUS_MARGIN_LEFT + INDENT_WIDTH, y, STR_MODE);
        g_model.trainerData.mode = editChoice(MODEL_SETUP_2ND_COLUMN, y, STR_VTRAINERMODES, g_model.trainerData.mode, 0, TRAINER_MODE_MAX(), attr, event, isTrainerModeAvailable);
#if defined(BLUETOOTH)
        if (attr && checkIncDec_Ret) {
          bluetooth.state = BLUETOOTH_STATE_OFF;
          bluetooth.distantAddr[0] = 0;
        }
#endif
        break;

#if defined(BLUETOOTH)
      case ITEM_MODEL_SETUP_TRAINER_BLUETOOTH:
        if (g_model.trainerData.mode == TRAINER_MODE_MASTER_BLUETOOTH) {
          if (attr) {
            s_editMode = 0;
          }
          if (bluetooth.distantAddr[0]) {
            lcdDrawText(MENUS_MARGIN_LEFT + INDENT_WIDTH, y, bluetooth.distantAddr);
            drawButton(MODEL_SETUP_2ND_COLUMN, y, STR_CLEAR, attr);
            if (attr && event == EVT_KEY_FIRST(KEY_ENTER)) {
              bluetooth.state = BLUETOOTH_STATE_CLEAR_REQUESTED;
              memclear(bluetooth.distantAddr, sizeof(bluetooth.distantAddr));
            }
          }
          else {
            lcdDrawText(MENUS_MARGIN_LEFT + INDENT_WIDTH, y, "---");
            if (bluetooth.state < BLUETOOTH_STATE_IDLE)
              drawButton(MODEL_SETUP_2ND_COLUMN, y, STR_BLUETOOTH_INIT, attr);
            else
              drawButton(MODEL_SETUP_2ND_COLUMN, y, STR_BLUETOOTH_DISC, attr);
            if (attr && event == EVT_KEY_FIRST(KEY_ENTER)) {
              if (bluetooth.state < BLUETOOTH_STATE_IDLE) {
                bluetooth.state = BLUETOOTH_STATE_OFF;
              }
              else {
                reusableBuffer.moduleSetup.bt.devicesCount = 0;
                bluetooth.state = BLUETOOTH_STATE_DISCOVER_REQUESTED;
              }
            }
          }

          if (bluetooth.state == BLUETOOTH_STATE_DISCOVER_END && reusableBuffer.moduleSetup.bt.devicesCount > 0) {
            popupMenuItemsCount = min<uint8_t>(reusableBuffer.moduleSetup.bt.devicesCount, MAX_BLUETOOTH_DISTANT_ADDR);
            for (uint8_t i = 0; i < popupMenuItemsCount; i++) {
              popupMenuItems[i] = reusableBuffer.moduleSetup.bt.devices[i];
            }
            POPUP_MENU_START(onBluetoothConnectMenu);
          }
        }
        else {
          if (bluetooth.distantAddr[0])
            lcdDrawText(MENUS_MARGIN_LEFT + INDENT_WIDTH, y+1, bluetooth.distantAddr);
          else
            lcdDrawText(MENUS_MARGIN_LEFT + INDENT_WIDTH, y, "---");
          lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, bluetooth.state == BLUETOOTH_STATE_CONNECTED ? STR_CONNECTED : STR_NOT_CONNECTED);
        }
        break;
#endif

      case ITEM_MODEL_SETUP_TRAINER_CHANNELS:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_CHANNELRANGE);
        drawStringWithIndex(MODEL_SETUP_2ND_COLUMN, y, STR_CH, g_model.trainerData.channelsStart+1, menuHorizontalPosition==0 ? attr : 0);
        lcdDrawText(lcdNextPos+5, y, "-");
        drawStringWithIndex(lcdNextPos+5, y, STR_CH, g_model.trainerData.channelsStart + 8 + g_model.trainerData.channelsCount, menuHorizontalPosition==1 ? attr : 0);
        if (attr && s_editMode > 0) {
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

      case ITEM_MODEL_SETUP_TRAINER_PPM_PARAMS:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_PPMFRAME);
        lcdDrawNumber(MODEL_SETUP_2ND_COLUMN, y, (int16_t)g_model.trainerData.frameLength*5 + 225, (menuHorizontalPosition<=0 ? attr : 0) | PREC1|LEFT, 0, NULL, STR_MS);
        lcdDrawNumber(MODEL_SETUP_2ND_COLUMN+80, y, (g_model.trainerData.delay*50)+300, (CURSOR_ON_LINE() || menuHorizontalPosition==1) ? attr|LEFT : LEFT, 0, NULL, "us");
        lcdDrawText(MODEL_SETUP_2ND_COLUMN+160, y, g_model.trainerData.pulsePol ? "+" : "-", (CURSOR_ON_LINE() || menuHorizontalPosition==2) ? attr : 0);
        if (attr && s_editMode>0) {
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

#if defined(PXX2)
      case ITEM_MODEL_SETUP_REGISTRATION_ID:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_REG_ID);
        if (isDefaultModelRegistrationID())
          lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, STR_PXX2_DEFAULT);
        else
          lcdDrawSizedText(MODEL_SETUP_2ND_COLUMN, y, g_model.modelRegistrationID, PXX2_LEN_REGISTRATION_ID, ZCHAR);
        break;
#endif

#if defined(HARDWARE_INTERNAL_MODULE)
      case ITEM_MODEL_SETUP_INTERNAL_MODULE_PXX2_MODEL_NUM:
#endif
      case ITEM_MODEL_SETUP_EXTERNAL_MODULE_PXX2_MODEL_NUM:
      {
        lcdDrawText(MENUS_MARGIN_LEFT + INDENT_WIDTH, y, STR_RECEIVER_NUM);
        lcdDrawNumber(MODEL_SETUP_2ND_COLUMN, y, g_model.header.modelId[moduleIdx], attr | LEADING0 | LEFT, 2);
        if (attr) {
          CHECK_INCDEC_MODELVAR_ZERO(event, g_model.header.modelId[moduleIdx], getMaxRxNum(moduleIdx));
          if (event == EVT_KEY_LONG(KEY_ENTER)) {
            killEvents(event);
            uint8_t newVal = modelslist.findNextUnusedModelId(moduleIdx);
            if (newVal != g_model.header.modelId[moduleIdx]) {
              g_model.header.modelId[moduleIdx] = newVal;
              storageDirty(EE_MODEL);
            }
          }
        }
      }
      break;

#if defined(HARDWARE_INTERNAL_MODULE)
      case ITEM_MODEL_SETUP_INTERNAL_MODULE_PXX2_REGISTER_RANGE:
#endif
      case ITEM_MODEL_SETUP_EXTERNAL_MODULE_PXX2_REGISTER_RANGE:
      {
        lcdDrawText(MENUS_MARGIN_LEFT + INDENT_WIDTH, y, STR_MODULE);
        drawButton(MODEL_SETUP_2ND_COLUMN, y, STR_REGISTER, (menuHorizontalPosition == 0 ? attr : 0));
        if (isModuleRangeAvailable(moduleIdx))
          drawButton(MODEL_SETUP_2ND_COLUMN + MODEL_SETUP_SET_FAILSAFE_OFS, y, STR_MODULE_RANGE, (menuHorizontalPosition == 1 ? attr : 0));
        if (attr) {
          if (moduleState[moduleIdx].mode == MODULE_MODE_NORMAL && s_editMode > 0) {
            if (menuHorizontalPosition == 0 && event == EVT_KEY_BREAK(KEY_ENTER)) {
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
        break;
      }

#if defined(HARDWARE_INTERNAL_MODULE)
      case ITEM_MODEL_SETUP_INTERNAL_MODULE_PXX2_OPTIONS:
#endif
      case ITEM_MODEL_SETUP_EXTERNAL_MODULE_PXX2_OPTIONS:
        lcdDrawText(MENUS_MARGIN_LEFT + INDENT_WIDTH, y, STR_OPTIONS);
        drawButton(MODEL_SETUP_2ND_COLUMN, y, STR_SET, attr);
        if (event == EVT_KEY_BREAK(KEY_ENTER) && attr) {
          g_moduleIdx = CURRENT_MODULE_EDITED(k);
          pushMenu(menuModelModuleOptions);
        }
        break;

#if defined(HARDWARE_INTERNAL_MODULE)
      case ITEM_MODEL_SETUP_INTERNAL_MODULE_PXX2_RECEIVER_1:
      case ITEM_MODEL_SETUP_INTERNAL_MODULE_PXX2_RECEIVER_2:
      case ITEM_MODEL_SETUP_INTERNAL_MODULE_PXX2_RECEIVER_3:
#endif
      case ITEM_MODEL_SETUP_EXTERNAL_MODULE_PXX2_RECEIVER_1:
      case ITEM_MODEL_SETUP_EXTERNAL_MODULE_PXX2_RECEIVER_2:
      case ITEM_MODEL_SETUP_EXTERNAL_MODULE_PXX2_RECEIVER_3:
      {
        uint8_t receiverIdx = CURRENT_RECEIVER_EDITED(k);
        ModuleInformation & moduleInformation = reusableBuffer.moduleSetup.pxx2.moduleInformation;

        drawStringWithIndex(MENUS_MARGIN_LEFT + INDENT_WIDTH, y, STR_RECEIVER, receiverIdx + 1);

        if (!isPXX2ReceiverUsed(moduleIdx, receiverIdx)) {
          drawButton(MODEL_SETUP_2ND_COLUMN, y, STR_MODULE_BIND, attr);
          if (attr && event == EVT_KEY_BREAK(KEY_ENTER)) {
            setPXX2ReceiverUsed(moduleIdx, receiverIdx);
            memclear(g_model.moduleData[moduleIdx].pxx2.receiverName[receiverIdx], PXX2_LEN_RX_NAME);
            onPXX2ReceiverMenu(STR_BIND);
          }
          break;
        }

        drawReceiverName(MODEL_SETUP_2ND_COLUMN, y, moduleIdx, receiverIdx, attr);

        if (s_editMode && isModuleR9MAccess(moduleIdx) && moduleState[moduleIdx].mode == MODULE_MODE_NORMAL && reusableBuffer.moduleSetup.bindInformation.step < 0) {
          if (reusableBuffer.moduleSetup.bindInformation.step == BIND_MODULE_TX_INFORMATION_REQUEST && moduleInformation.information.modelID) {
            // For R9M ACCESS the module information has been requested to know if we are in EU mode. We just receive it here and continue
            if (moduleInformation.information.variant == PXX2_VARIANT_EU) {
              // In EU mode we will need the power of the module to know if telemetry can be proposed
              reusableBuffer.moduleSetup.bindInformation.step = BIND_MODULE_TX_SETTINGS_REQUEST;
#if defined(SIMU)
              reusableBuffer.moduleSetup.pxx2.moduleSettings.txPower = 14;
              break;
#else
              moduleState[moduleIdx].readModuleSettings(&reusableBuffer.moduleSetup.pxx2.moduleSettings);
#endif
            }
            else {
              reusableBuffer.moduleSetup.bindInformation.step = 0;
              moduleState[moduleIdx].startBind(&reusableBuffer.moduleSetup.bindInformation);
            }
          }
          else if (reusableBuffer.moduleSetup.bindInformation.step == BIND_MODULE_TX_SETTINGS_REQUEST && reusableBuffer.moduleSetup.pxx2.moduleSettings.txPower > 0) {
            // We just receive the module settings (for TX power)
            reusableBuffer.moduleSetup.bindInformation.step = 0;
            moduleState[moduleIdx].startBind(&reusableBuffer.moduleSetup.bindInformation);
          }
        }
        else if (attr && (moduleState[moduleIdx].mode == MODULE_MODE_NORMAL || s_editMode == 0)) {
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
              for (auto rx = 0; rx < popupMenuItemsCount; rx++) {
                popupMenuItems[rx] = reusableBuffer.moduleSetup.bindInformation.candidateReceiversNames[rx];
              }
              POPUP_MENU_TITLE(STR_PXX2_SELECT_RX);
              POPUP_MENU_START(onPXX2BindMenu);
            }
          }
        }

        if (attr && event == EVT_KEY_BREAK(KEY_ENTER)) {
          POPUP_MENU_ADD_ITEM(STR_BIND);
          POPUP_MENU_ADD_ITEM(STR_OPTIONS);
          POPUP_MENU_ADD_ITEM(STR_SHARE);
          POPUP_MENU_ADD_ITEM(STR_DELETE);
          POPUP_MENU_ADD_ITEM(STR_RESET);
          POPUP_MENU_START(onPXX2ReceiverMenu);
        }
      }
      break;

#if defined(HARDWARE_INTERNAL_MODULE)
      case ITEM_MODEL_SETUP_INTERNAL_MODULE_NOT_ACCESS_RXNUM_BIND_RANGE:
#endif
      case ITEM_MODEL_SETUP_EXTERNAL_MODULE_NOT_ACCESS_RXNUM_BIND_RANGE:
      {
        ModuleData & moduleData = g_model.moduleData[moduleIdx];
        if (isModulePPM(moduleIdx)) {
          lcdDrawText(MENUS_MARGIN_LEFT, y, STR_PPMFRAME);
          lcdDrawNumber(MODEL_SETUP_2ND_COLUMN, y, (int16_t)moduleData.ppm.frameLength*5 + 225, (menuHorizontalPosition<=0 ? attr : 0) | PREC1|LEFT, 0, NULL, STR_MS);
          lcdDrawNumber(MODEL_SETUP_2ND_COLUMN+80, y, (moduleData.ppm.delay*50)+300, (CURSOR_ON_LINE() || menuHorizontalPosition==1) ? attr|LEFT : LEFT, 0, NULL, "us");
          lcdDrawText(MODEL_SETUP_2ND_COLUMN+160, y, moduleData.ppm.pulsePol ? "+" : "-", (CURSOR_ON_LINE() || menuHorizontalPosition==2) ? attr : 0);
          if (attr && s_editMode>0) {
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
          lcdDrawText(MENUS_MARGIN_LEFT, y, STR_REFRESHRATE);
          lcdDrawNumber(MODEL_SETUP_2ND_COLUMN, y, (int16_t)moduleData.ppm.frameLength*5 + 225, (menuHorizontalPosition<=0 ? attr : 0) | PREC1|LEFT, 0, NULL, STR_MS);
          lcdDrawText(MODEL_SETUP_3RD_COLUMN, y, moduleData.sbus.noninverted ? "not inverted" : "normal", (CURSOR_ON_LINE() || menuHorizontalPosition==1) ? attr : 0);
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
          lcdDrawText(MENUS_MARGIN_LEFT + INDENT_WIDTH, y, STR_RECEIVER);
          int l_posHorz = menuHorizontalPosition;
          coord_t bindButtonPos = MODEL_SETUP_2ND_COLUMN;
          // RXNUM
          if (isModuleRxNumAvailable(moduleIdx)) {
            lcdDrawNumber(MODEL_SETUP_2ND_COLUMN, y, g_model.header.modelId[moduleIdx], (l_posHorz==0 ? attr : 0) | LEADING0 | LEFT, 2);
            bindButtonPos += 40;
            if (attr && l_posHorz == 0) {
              if (s_editMode > 0) {
                CHECK_INCDEC_MODELVAR_ZERO(event, g_model.header.modelId[moduleIdx], getMaxRxNum(moduleIdx));
                if (checkIncDec_Ret && isModuleCrossfire(moduleIdx)) {
                  moduleState[EXTERNAL_MODULE].counter = CRSF_FRAME_MODELID;
                }
                if (event == EVT_KEY_LONG(KEY_ENTER)) {
                  killEvents(event);
                  uint8_t newVal = modelslist.findNextUnusedModelId(moduleIdx);
                  if (newVal != g_model.header.modelId[moduleIdx]) {
                    g_model.header.modelId[moduleIdx] = newVal;
                    storageDirty(EE_MODEL);
                  }
                }
              }
            }
          }
          else if (attr) {
            l_posHorz += 1;
          }
          // BIND - RANGE
          if (isModuleBindRangeAvailable(moduleIdx)) {
            drawButton(bindButtonPos, y, STR_MODULE_BIND, (moduleState[moduleIdx].mode == MODULE_MODE_BIND ? BUTTON_ON : BUTTON_OFF) | (l_posHorz==1 ? attr : 0));
            if (isModuleRangeAvailable(moduleIdx)) {
              drawButton(bindButtonPos + 80, y, STR_MODULE_RANGE, (moduleState[moduleIdx].mode == MODULE_MODE_RANGECHECK ? BUTTON_ON : BUTTON_OFF) | (l_posHorz==2 ? attr : 0));
            }
            uint8_t newFlag = MODULE_MODE_NORMAL;
#if defined(MULTIMODULE)
            if (getMultiBindStatus(moduleIdx) == MULTI_BIND_FINISHED) {
              setMultiBindStatus(moduleIdx, MULTI_NORMAL_OPERATION);
              s_editMode = 0;
            }
#endif
            if (attr && l_posHorz>0) {
              if (s_editMode>0) {
                if (l_posHorz == 1) {
                  if (isModuleR9MNonAccess(moduleIdx) || isModuleD16(moduleIdx) || isModuleAFHDS3(moduleIdx) || IS_R9_MULTI(moduleIdx)) {
                    if (event == EVT_KEY_BREAK(KEY_ENTER)) {
#if defined(AFHDS3)
                      if (isModuleAFHDS3(moduleIdx)) {
                        startBindMenuAfhds3(moduleIdx);
                        continue;
                      }
#endif
                      startBindMenu(moduleIdx);
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
            moduleState[moduleIdx].mode = newFlag;
#if defined(MULTIMODULE)
            if (newFlag == MODULE_MODE_BIND)
              setMultiBindStatus(moduleIdx, MULTI_BIND_INITIATED);
#endif
          }
        }
        break;
      }
#if defined(HARDWARE_INTERNAL_MODULE)
      case ITEM_MODEL_SETUP_INTERNAL_MODULE_FAILSAFE:
#endif
      case ITEM_MODEL_SETUP_EXTERNAL_MODULE_FAILSAFE:
      {
        ModuleData & moduleData = g_model.moduleData[moduleIdx];
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_FAILSAFE);
        lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_VFAILSAFE, moduleData.failsafeMode, menuHorizontalPosition==0 ? attr : 0);
        if (moduleData.failsafeMode == FAILSAFE_CUSTOM) {
          drawButton(MODEL_SETUP_2ND_COLUMN + MODEL_SETUP_SET_FAILSAFE_OFS, y, STR_SET, menuHorizontalPosition==1 ? attr : 0);
        }
        if (attr) {
          if (moduleData.failsafeMode != FAILSAFE_CUSTOM)
            menuHorizontalPosition = 0;
          if (menuHorizontalPosition==0) {
            if (s_editMode>0) {
              CHECK_INCDEC_MODELVAR_ZERO(event, moduleData.failsafeMode, isModuleR9M(moduleIdx) ? FAILSAFE_NOPULSES : FAILSAFE_LAST);
              if (checkIncDec_Ret) SEND_FAILSAFE_NOW(moduleIdx);
            }
          }
          else if (menuHorizontalPosition==1) {
            s_editMode = 0;
            if (moduleData.failsafeMode == FAILSAFE_CUSTOM) {
              if (event == EVT_KEY_LONG(KEY_ENTER)) {
                killEvents(event);
                setCustomFailsafe(moduleIdx);
                AUDIO_WARNING1();
                SEND_FAILSAFE_NOW(moduleIdx);
              }
              else if (event == EVT_KEY_BREAK(KEY_ENTER)) {
                g_moduleIdx = moduleIdx;
                pushMenu(menuModelFailsafe);
              }
            }
          }
          else {
            lcdDrawSolidFilledRect(MODEL_SETUP_2ND_COLUMN, y, LCD_W - MODEL_SETUP_2ND_COLUMN - 2, 8, TEXT_COLOR);
          }
        }
        break;
      }
#if defined(HARDWARE_INTERNAL_MODULE)
      case ITEM_MODEL_SETUP_INTERNAL_MODULE_OPTIONS:
#endif
      case ITEM_MODEL_SETUP_EXTERNAL_MODULE_OPTIONS:
      {
#if defined(MULTIMODULE)
        if (MULTIMODULE_PROTOCOL_KNOWN(moduleIdx)) {
          const char * title = getMultiOptionTitle(moduleIdx);

          lcdDrawText(MENUS_MARGIN_LEFT + INDENT_WIDTH, y, title);
          if (title == STR_MULTI_RFTUNE) {
            lcdDrawNumber(LCD_W - 10, y, TELEMETRY_RSSI(), RIGHT, 0, "RSSI(", ")");
          }

          int optionValue = g_model.moduleData[moduleIdx].multi.optionValue;
          const uint8_t multi_proto = g_model.moduleData[moduleIdx].getMultiProtocol();
          int8_t min, max;
          getMultiOptionValues(multi_proto, min, max);

          if (title == STR_MULTI_RFPOWER) {
            lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_MULTI_POWER, optionValue, LEFT | attr);
            min = 0;
            max = 15;
          }
          else if (title == STR_MULTI_TELEMETRY) {
            lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_MULTI_TELEMETRY_MODE, optionValue, LEFT | attr);
          }
          else if (title == STR_MULTI_WBUS) {
            lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_MULTI_WBUS_MODE, optionValue, LEFT | attr);
            min = 0;
            max = 1;
          }
          else if (multi_proto == MODULE_SUBTYPE_MULTI_FS_AFHDS2A) {
            lcdDrawNumber(MODEL_SETUP_2ND_COLUMN, y, 50 + 5 * optionValue, LEFT | attr);
          }
          else if (multi_proto == MODULE_SUBTYPE_MULTI_DSM2) {
            optionValue = optionValue & 0x01;
            editCheckBox(optionValue, MODEL_SETUP_2ND_COLUMN, y, LEFT | attr, event);
          }
          else {
            if (min == 0 && max == 1)
              editCheckBox(optionValue, MODEL_SETUP_2ND_COLUMN, y, LEFT | attr, event);
            else
              lcdDrawNumber(MODEL_SETUP_2ND_COLUMN, y, optionValue, LEFT | attr);
          }

          if (attr) {
            CHECK_INCDEC_MODELVAR(event, optionValue, min, max);
            if (checkIncDec_Ret) {
              if (multi_proto == MODULE_SUBTYPE_MULTI_DSM2) {
                g_model.moduleData[moduleIdx].multi.optionValue = (g_model.moduleData[moduleIdx].multi.optionValue & 0xFE) + optionValue;
              }
              else {
                g_model.moduleData[moduleIdx].multi.optionValue = optionValue;
              }
            }
          }
        }
#endif
        if (isModuleR9MNonAccess(moduleIdx)) {
          lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MODULE_TELEMETRY);
          if (isSportLineUsedByInternalModule())
            lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, STR_DISABLE_INTERNAL);
          else
            lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, STR_MODULE_TELEM_ON);
        }
        else if (isModuleSBUS(moduleIdx)) {
#if defined(RADIO_TX16S)  // TX16S has 5v output on CPPM pin
          lcdDrawText(MENUS_MARGIN_LEFT + INDENT_WIDTH, y, STR_WARN_5VOLTS);
#else
          lcdDrawText(MENUS_MARGIN_LEFT, y, STR_WARN_BATTVOLTAGE);
          drawValueWithUnit(MODEL_SETUP_4TH_COLUMN, y, getBatteryVoltage(), UNIT_VOLTS, attr|PREC2|LEFT);
#endif
        }
        break;
      }
#if defined(HARDWARE_INTERNAL_MODULE)
      case ITEM_MODEL_SETUP_INTERNAL_MODULE_POWER:
#endif
      case ITEM_MODEL_SETUP_EXTERNAL_MODULE_POWER:
      {
        if (isModuleR9MNonAccess(moduleIdx)) {
          lcdDrawText(MENUS_MARGIN_LEFT + INDENT_WIDTH, y, STR_RFPOWER);
          if (isModuleR9M_FCC_VARIANT(moduleIdx)) {
            lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_R9M_FCC_POWER_VALUES, g_model.moduleData[moduleIdx].pxx.power, LEFT | attr);
            if (attr)
              CHECK_INCDEC_MODELVAR(event, g_model.moduleData[moduleIdx].pxx.power, 0, R9M_FCC_POWER_MAX);
          }
          else {
            lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_R9M_LBT_POWER_VALUES, g_model.moduleData[moduleIdx].pxx.power, LEFT | attr);
            if (attr)
              CHECK_INCDEC_MODELVAR(event, g_model.moduleData[moduleIdx].pxx.power, 0, R9M_LBT_POWER_MAX);
            if (attr && s_editMode == 0 && reusableBuffer.moduleSetup.r9mPower != g_model.moduleData[moduleIdx].pxx.power) {
              if ((reusableBuffer.moduleSetup.r9mPower + g_model.moduleData[moduleIdx].pxx.power) < 5) { //switching between mode 2 and 3 does not require rebind
                POPUP_WARNING(STR_WARNING);
                SET_WARNING_INFO(STR_REBIND, sizeof(TR_REBIND), 0);
              }
              reusableBuffer.moduleSetup.r9mPower = g_model.moduleData[moduleIdx].pxx.power;
            }
          }
        }
#if defined(MULTIMODULE)
        else if (isModuleMultimodule(moduleIdx)) {
          lcdDrawText(MENUS_MARGIN_LEFT, y, IS_RX_MULTI(moduleIdx) ? STR_MULTI_LNA_DISABLE : STR_MULTI_LOWPOWER);
          g_model.moduleData[moduleIdx].multi.lowPowerMode = editCheckBox(g_model.moduleData[moduleIdx].multi.lowPowerMode, MODEL_SETUP_2ND_COLUMN, y, attr, event);
        }
#endif
#if defined(AFHDS3)
        else if (isModuleAFHDS3(moduleIdx)) {
          lcdDrawText(MENUS_MARGIN_LEFT + INDENT_WIDTH, y, STR_RFPOWER);
          lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_AFHDS3_POWERS, g_model.moduleData[moduleIdx].afhds3.runPower, LEFT | attr);
          if (attr)
            CHECK_INCDEC_MODELVAR(event, g_model.moduleData[moduleIdx].afhds3.runPower, afhds3::RUN_POWER::RUN_POWER_FIRST, afhds3::RUN_POWER::RUN_POWER_LAST);
        }
#endif
      }
      break;

#if defined(MULTIMODULE)
#if defined(HARDWARE_INTERNAL_MODULE)
    case ITEM_MODEL_SETUP_INTERNAL_MODULE_AUTOBIND:
#endif
    case ITEM_MODEL_SETUP_EXTERNAL_MODULE_AUTOBIND:
      if (g_model.moduleData[moduleIdx].getMultiProtocol() == MODULE_SUBTYPE_MULTI_DSM2) {
        int8_t value = (g_model.moduleData[moduleIdx].multi.optionValue & 0x02) >> 1;
        lcdDrawText(MENUS_MARGIN_LEFT + INDENT_WIDTH, y, STR_MULTI_SERVOFREQ);
        lcdDrawNumber(MODEL_SETUP_2ND_COLUMN, y, value ? 11 : 22, attr, 0, "", "ms");
        if (attr) {
          CHECK_INCDEC_MODELVAR(event, value, 0, 1);
          if (checkIncDec_Ret) {
            g_model.moduleData[moduleIdx].multi.optionValue = (g_model.moduleData[moduleIdx].multi.optionValue & 0xFD) + (value << 1);
          }
        }
      }
      else {
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MULTI_AUTOBIND);
        g_model.moduleData[moduleIdx].multi.autoBindMode = editCheckBox(g_model.moduleData[moduleIdx].multi.autoBindMode, MODEL_SETUP_2ND_COLUMN, y, attr, event);
      }
      break;
#if defined(HARDWARE_INTERNAL_MODULE)
      case ITEM_MODEL_SETUP_INTERNAL_MODULE_DISABLE_TELEM:
#endif
      case ITEM_MODEL_SETUP_EXTERNAL_MODULE_DISABLE_TELEM:
        lcdDrawText(MENUS_MARGIN_LEFT + INDENT_WIDTH, y, STR_DISABLE_TELEM);
        g_model.moduleData[moduleIdx].multi.disableTelemetry = editCheckBox(g_model.moduleData[moduleIdx].multi.disableTelemetry, MODEL_SETUP_2ND_COLUMN, y, attr, event);
        break;
#if defined(HARDWARE_INTERNAL_MODULE)
      case ITEM_MODEL_SETUP_INTERNAL_MODULE_DISABLE_MAPPING:
#endif
      case ITEM_MODEL_SETUP_EXTERNAL_MODULE_DISABLE_MAPPING:
        lcdDrawText(MENUS_MARGIN_LEFT + INDENT_WIDTH, y, STR_DISABLE_CH_MAP);
        g_model.moduleData[moduleIdx].multi.disableMapping = editCheckBox(g_model.moduleData[moduleIdx].multi.disableMapping, MODEL_SETUP_2ND_COLUMN, y, attr, event);
        break;
#endif

#if defined(AFHDS3)
      case ITEM_MODEL_SETUP_EXTERNAL_MODULE_AFHDS3_MODE:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_TYPE);
        lcdDrawText(MODEL_SETUP_2ND_COLUMN, y,
            g_model.moduleData[EXTERNAL_MODULE].afhds3.telemetry ? STR_AFHDS3_ONE_TO_ONE_TELEMETRY : TR_AFHDS3_ONE_TO_MANY);
        break;
      case ITEM_MODEL_SETUP_EXTERNAL_MODULE_AFHDS3_RX_FREQ:
        lcdDrawText(MENUS_MARGIN_LEFT + INDENT_WIDTH, y, STR_AFHDS3_RX_FREQ);
        lcdDrawNumber(MODEL_SETUP_2ND_COLUMN, y, g_model.moduleData[moduleIdx].afhds3.rxFreq(), attr | LEFT, 0, NULL, "Hz");
        if (attr) {
          uint16_t rxFreq = g_model.moduleData[moduleIdx].afhds3.rxFreq();
          CHECK_INCDEC_MODELVAR(event, rxFreq, MIN_FREQ, MAX_FREQ);
          g_model.moduleData[moduleIdx].afhds3.setRxFreq(rxFreq);
        }
        break;
      case ITEM_MODEL_SETUP_EXTERNAL_MODULE_AFHDS3_ACTUAL_POWER:
        lcdDrawText(MENUS_MARGIN_LEFT + INDENT_WIDTH, y, STR_AFHDS3_ACTUAL_POWER);
        lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_AFHDS3_POWERS, actualAfhdsRunPower(moduleIdx), LEFT);
        break;
#endif

#if defined(MULTIMODULE) || defined(AFHDS3)
#if defined(MULTIMODULE) && defined(HARDWARE_INTERNAL_MODULE)
    case ITEM_MODEL_SETUP_INTERNAL_MODULE_STATUS:
#endif
#if defined(AFHDS3)
    case ITEM_MODEL_SETUP_EXTERNAL_MODULE_AFHDS3_STATUS:
#endif
#if defined(MULTIMODULE)
    case ITEM_MODEL_SETUP_EXTERNAL_MODULE_STATUS:
#endif
    {
      lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MODULE_STATUS);

      char statusText[64];
      getModuleStatusString(moduleIdx, statusText);
      lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, statusText);
      break;
    }

#if defined(MULTIMODULE) && defined(HARDWARE_INTERNAL_MODULE)
    case ITEM_MODEL_SETUP_INTERNAL_MODULE_SYNCSTATUS:
#endif
#if defined(AFHDS3)
    case ITEM_MODEL_SETUP_EXTERNAL_MODULE_AFHDS3_POWER_STATUS:
#endif
#if defined(MULTIMODULE)
    case ITEM_MODEL_SETUP_EXTERNAL_MODULE_SYNCSTATUS:
#endif
    {
#if defined(AFHDS3)
      if (isModuleAFHDS3(moduleIdx)) {
        lcdDrawText(MENUS_MARGIN_LEFT + INDENT_WIDTH, y, STR_AFHDS3_POWER_SOURCE);
      }
      else
#endif
      {
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MODULE_SYNC);
      }
      char statusText[64];
      getModuleSyncStatusString(moduleIdx, statusText);
      lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, statusText);
      break;
    }
#endif

    }
  }

  if (isModuleInRangeCheckMode()) {
    theme->drawMessageBox("RSSI :", NULL, NULL, WARNING_TYPE_INFO);
    lcdDrawNumber(WARNING_LINE_X, WARNING_INFOLINE_Y, TELEMETRY_RSSI(), DBLSIZE|LEFT);
  }

  // some field just finished being edited
  if (old_editMode > 0 && s_editMode == 0) {
    ModelCell * mod_cell = modelslist.getCurrentModel();
    if (mod_cell) {
      switch(menuVerticalPosition) {
        case ITEM_MODEL_SETUP_NAME:
          mod_cell->setModelName(g_model.header.name);
          break;
#if defined(HARDWARE_INTERNAL_MODULE)
        case ITEM_MODEL_SETUP_INTERNAL_MODULE_NOT_ACCESS_RXNUM_BIND_RANGE:
          if (menuHorizontalPosition != 0)
            break;
        case ITEM_MODEL_SETUP_INTERNAL_MODULE_TYPE:
          mod_cell->setRfData(&g_model);
          checkModelIdUnique(INTERNAL_MODULE);
          break;
#endif
        case ITEM_MODEL_SETUP_EXTERNAL_MODULE_NOT_ACCESS_RXNUM_BIND_RANGE:
          if (menuHorizontalPosition != 0)
            break;
        case ITEM_MODEL_SETUP_EXTERNAL_MODULE_TYPE:
          mod_cell->setRfData(&g_model);
          if (g_model.moduleData[EXTERNAL_MODULE].type != MODULE_TYPE_NONE)
            checkModelIdUnique(EXTERNAL_MODULE);
          break;
      }
    }
  }

  return true;
}
