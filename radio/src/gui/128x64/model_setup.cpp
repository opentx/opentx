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

uint8_t g_moduleIdx, g_receiverIdx;
void menuModelFailsafe(event_t event);
void menuModelPinmap(event_t event);

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
  ITEM_MODEL_INTERNAL_MODULE_PXX2_RANGE_REGISTER,
  ITEM_MODEL_INTERNAL_MODULE_PXX2_RECEIVER_1_ID,
  ITEM_MODEL_INTERNAL_MODULE_PXX2_RECEIVER_1_PINMAP,
  ITEM_MODEL_INTERNAL_MODULE_PXX2_RECEIVER_1_TELEM,
  ITEM_MODEL_INTERNAL_MODULE_PXX2_RECEIVER_1_BIND_DEL,
  ITEM_MODEL_INTERNAL_MODULE_PXX2_RECEIVER_2_ID,
  ITEM_MODEL_INTERNAL_MODULE_PXX2_RECEIVER_2_PINMAP,
  ITEM_MODEL_INTERNAL_MODULE_PXX2_RECEIVER_2_TELEM,
  ITEM_MODEL_INTERNAL_MODULE_PXX2_RECEIVER_2_BIND_DEL,
  ITEM_MODEL_INTERNAL_MODULE_PXX2_ADD_RECEIVER,
#endif
  ITEM_MODEL_EXTERNAL_MODULE_LABEL,
  ITEM_MODEL_EXTERNAL_MODULE_MODE,
#if defined(MULTIMODULE)
  ITEM_MODEL_EXTERNAL_MODULE_SUBTYPE,
  ITEM_MODEL_EXTERNAL_MODULE_STATUS,
  ITEM_MODEL_EXTERNAL_MODULE_SYNCSTATUS,
#endif
  ITEM_MODEL_EXTERNAL_MODULE_CHANNELS,
  ITEM_MODEL_EXTERNAL_MODULE_BIND,
#if defined(PCBSKY9X) && defined(REVX)
  ITEM_MODEL_EXTERNAL_MODULE_OUTPUT_TYPE,
#endif
  ITEM_MODEL_EXTERNAL_MODULE_OPTIONS,
#if defined(MULTIMODULE)
  ITEM_MODEL_EXTERNAL_MODULE_AUTOBIND,
#endif
  ITEM_MODEL_EXTERNAL_MODULE_POWER,
#if !defined(PCBXLITE)
  ITEM_MODEL_EXTERNAL_MODULE_FREQ,
#endif
  ITEM_MODEL_EXTERNAL_MODULE_FAILSAFE,
#if defined(PCBSKY9X) && !defined(REVA)
  ITEM_MODEL_EXTRA_MODULE_LABEL,
  ITEM_MODEL_EXTRA_MODULE_CHANNELS,
  ITEM_MODEL_EXTRA_MODULE_BIND,
#endif
#if defined(PCBX7)
  ITEM_MODEL_TRAINER_LABEL,
  ITEM_MODEL_TRAINER_MODE,
  #if defined(BLUETOOTH)
    ITEM_MODEL_TRAINER_BLUETOOTH,
  #endif
  ITEM_MODEL_TRAINER_CHANNELS,
  ITEM_MODEL_TRAINER_PARAMS,
#elif defined(PCBXLITE)
  ITEM_MODEL_TRAINER_LABEL,
  ITEM_MODEL_TRAINER_MODE,
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

#define IF_PXX2(xxx)                     (IS_PXX2_ENABLED() ? (uint8_t)(xxx) : HIDDEN_ROW)
#define IF_NOT_PXX2(xxx)                 (IS_PXX2_ENABLED() ? HIDDEN_ROW : (uint8_t)(xxx))
#define IF_PXX2_RECEIVER_DISPLAYED(module, idx, xxx)   ((IS_PXX2_ENABLED() && g_model.moduleData[module].pxx2.receivers[idx].enabled) ? (uint8_t)(xxx) : HIDDEN_ROW)

#if defined(PCBTARANIS)
  #define CURRENT_MODULE_EDITED(k)        (k >= ITEM_MODEL_EXTERNAL_MODULE_LABEL ? EXTERNAL_MODULE : INTERNAL_MODULE)
  #define CURRENT_RECEIVER_EDITED(k)      (k >= ITEM_MODEL_INTERNAL_MODULE_PXX2_RECEIVER_2_ID ? 1 : 0)
  #define INTERNAL_MODULE_ADD_RECEIVER_ROW       (g_model.moduleData[INTERNAL_MODULE].pxx2.receivers[MAX_RECEIVERS_PER_MODULE-1].enabled ? HIDDEN_ROW : (uint8_t)0)
#elif defined(PCBSKY9X) && !defined(REVA)
  #define CURRENT_MODULE_EDITED(k)       (k>=ITEM_MODEL_EXTRA_MODULE_LABEL ? EXTRA_MODULE : EXTERNAL_MODULE)
#else
  #define CURRENT_MODULE_EDITED(k)       (EXTERNAL_MODULE)
#endif

#if defined(PCBXLITE)
  #define SW_WARN_ROWS                    uint8_t(NAVIGATION_LINE_BY_LINE|getSwitchWarningsCount()), uint8_t(getSwitchWarningsCount() > 5 ? TITLE_ROW : HIDDEN_ROW) // X-Lite needs an additional column for full line selection (<])
#else
  #define SW_WARN_ROWS                    uint8_t(NAVIGATION_LINE_BY_LINE|(getSwitchWarningsCount()-1)), uint8_t(getSwitchWarningsCount() > 5 ? TITLE_ROW : HIDDEN_ROW)
#endif

#if defined(TARANIS_INTERNAL_PPM)
  #define INTERNAL_MODULE_MODE_ROWS       (isModuleXJT(INTERNAL_MODULE) ? (uint8_t)1 : (uint8_t)0) // Module type + RF protocols
#else
  #define INTERNAL_MODULE_MODE_ROWS       0 // (OFF / RF protocols)
#endif

#define IF_INTERNAL_MODULE_ON(x)       (IS_INTERNAL_MODULE_ENABLED()? (uint8_t)(x) : HIDDEN_ROW )
#define IF_EXTERNAL_MODULE_ON(x)       (IS_EXTERNAL_MODULE_ENABLED()? (uint8_t)(x) : HIDDEN_ROW)
#define EXTERNAL_MODULE_BIND_ROWS()    ((isModuleXJT(EXTERNAL_MODULE) && IS_D8_RX(EXTERNAL_MODULE)) || isModuleSBUS(EXTERNAL_MODULE)) ? (uint8_t)1 : (isModulePPM(EXTERNAL_MODULE) || isModulePXX(EXTERNAL_MODULE) || isModuleDSM2(EXTERNAL_MODULE) || isModuleMultimodule(EXTERNAL_MODULE)) ? (uint8_t)2 : HIDDEN_ROW

#if defined(PCBSKY9X) && defined(REVX)
  #define OUTPUT_TYPE_ROWS()             (isModulePPM(EXTERNAL_MODULE) ? (uint8_t)0 : HIDDEN_ROW) ,
#else
  #define OUTPUT_TYPE_ROWS()
#endif

#define PORT_CHANNELS_ROWS(x)          (x==EXTERNAL_MODULE ? EXTERNAL_MODULE_CHANNELS_ROWS : 0)

#if defined(PCBXLITE)
  #define EXTERNAL_MODULE_MODE_ROWS      (isModulePXX(EXTERNAL_MODULE) || isModuleDSM2(EXTERNAL_MODULE) || isModuleMultimodule(EXTERNAL_MODULE)) ? (uint8_t)1 : (uint8_t)0
  #define EXTERNAL_MODULE_FREQ_ROW
#else
  #define EXTERNAL_MODULE_MODE_ROWS      (isModuleXJT(EXTERNAL_MODULE) || isModuleDSM2(EXTERNAL_MODULE) || isModuleMultimodule(EXTERNAL_MODULE)) ? (uint8_t)1 : (uint8_t)0
  #define EXTERNAL_MODULE_FREQ_ROW              isR9MMFlex(EXTERNAL_MODULE) ? (uint8_t)1: (uint8_t)HIDDEN_ROW,
#endif

#define CURSOR_ON_CELL                 (true)
#define MODEL_SETUP_MAX_LINES          (HEADER_LINE+ITEM_MODEL_SETUP_MAX)
#define POT_WARN_ITEMS()               ((g_model.potsWarnMode) ? (uint8_t)(NUM_POTS+NUM_SLIDERS) : (uint8_t)0)
#define TIMER_ROWS                     2, 0, 0, 0, 0

#if defined(PCBSKY9X) && !defined(REVA)
  #define EXTRA_MODULE_ROWS              LABEL(ExtraModule), 1, 2,
#else
  #define EXTRA_MODULE_ROWS
#endif

#if defined(PCBX7)
  #define ANTENNA_ROW
  #if defined(BLUETOOTH)
    #define TRAINER_BLUETOOTH_M_ROW        ((bluetoothDistantAddr[0] == '\0' || bluetoothState == BLUETOOTH_STATE_CONNECTED) ? (uint8_t)0 : (uint8_t)1)
    #define TRAINER_BLUETOOTH_S_ROW        (bluetoothDistantAddr[0] == '\0' ? HIDDEN_ROW : LABEL())
    #define TRAINER_BLUETOOTH_ROW          (g_model.trainerData.mode == TRAINER_MODE_MASTER_BLUETOOTH ? TRAINER_BLUETOOTH_M_ROW : (g_model.trainerData.mode == TRAINER_MODE_SLAVE_BLUETOOTH ? TRAINER_BLUETOOTH_S_ROW : HIDDEN_ROW)),
  #else
    #define TRAINER_BLUETOOTH_ROW
  #endif
  #define TRAINER_CHANNELS_ROW             (IS_SLAVE_TRAINER() ? (uint8_t)1 : HIDDEN_ROW)
  #define TRAINER_PARAMS_ROW               (IS_SLAVE_TRAINER() ? (uint8_t)2 : HIDDEN_ROW)
  #define TRAINER_ROWS                     LABEL(Trainer), 0, TRAINER_BLUETOOTH_ROW TRAINER_CHANNELS_ROW, TRAINER_PARAMS_ROW
#elif defined(PCBXLITES)
  #define ANTENNA_ROW                    IF_INTERNAL_MODULE_ON(0),
  #define IF_BT_TRAINER_ON(x)            (g_eeGeneral.bluetoothMode == BLUETOOTH_TRAINER ? (uint8_t)(x) : HIDDEN_ROW)
  #define TRAINER_BLUETOOTH_M_ROW        ((bluetoothDistantAddr[0] == '\0' || bluetoothState == BLUETOOTH_STATE_CONNECTED) ? (uint8_t)0 : (uint8_t)1)
  #define TRAINER_BLUETOOTH_S_ROW        (bluetoothDistantAddr[0] == '\0' ? HIDDEN_ROW : LABEL())
  #define TRAINER_BLUETOOTH_ROW          (g_model.trainerData.mode == TRAINER_MODE_MASTER_BLUETOOTH ? TRAINER_BLUETOOTH_M_ROW : (g_model.trainerData.mode == TRAINER_MODE_SLAVE_BLUETOOTH ? TRAINER_BLUETOOTH_S_ROW : HIDDEN_ROW))
  #define TRAINER_CHANNELS_ROW           (IS_SLAVE_TRAINER() ? (uint8_t)1 : HIDDEN_ROW)
  #define TRAINER_PARAMS_ROW               (IS_SLAVE_TRAINER() ? (uint8_t)2 : HIDDEN_ROW)
  #define TRAINER_ROWS                   LABEL(Trainer), 0, IF_BT_TRAINER_ON(TRAINER_BLUETOOTH_ROW), TRAINER_CHANNELS_ROW, TRAINER_PARAMS_ROW
#elif defined(PCBXLITE)
  #define ANTENNA_ROW                    IF_INTERNAL_MODULE_ON(0),
  #define IF_BT_TRAINER_ON(x)            (g_eeGeneral.bluetoothMode == BLUETOOTH_TRAINER ? (uint8_t)(x) : HIDDEN_ROW)
  #define TRAINER_BLUETOOTH_M_ROW        ((bluetoothDistantAddr[0] == '\0' || bluetoothState == BLUETOOTH_STATE_CONNECTED) ? (uint8_t)0 : (uint8_t)1)
  #define TRAINER_BLUETOOTH_S_ROW        (bluetoothDistantAddr[0] == '\0' ? HIDDEN_ROW : LABEL())
  #define TRAINER_BLUETOOTH_ROW          (g_model.trainerData.mode == TRAINER_MODE_MASTER_BLUETOOTH ? TRAINER_BLUETOOTH_M_ROW : (g_model.trainerData.mode == TRAINER_MODE_SLAVE_BLUETOOTH ? TRAINER_BLUETOOTH_S_ROW : HIDDEN_ROW))
  #define TRAINER_CHANNELS_ROW           (IS_SLAVE_TRAINER() ? (uint8_t)1 : HIDDEN_ROW)
  #define TRAINER_ROWS                   IF_BT_TRAINER_ON(LABEL(Trainer)), IF_BT_TRAINER_ON(0), IF_BT_TRAINER_ON(TRAINER_BLUETOOTH_ROW), IF_BT_TRAINER_ON(TRAINER_CHANNELS_ROW)
#else
  #define TRAINER_ROWS
#endif

void onPXX2BindMenu(const char * result)
{
  reusableBuffer.modelSetup.pxx2.bindSelectedReceiverIndex = (result - reusableBuffer.modelSetup.pxx2.bindCandidateReceiversNames[0]) / sizeof(reusableBuffer.modelSetup.pxx2.bindCandidateReceiversNames[0]);
  reusableBuffer.modelSetup.pxx2.bindStep = BIND_RX_NAME_SELECTED;
}

void onBindMenu(const char * result)
{
  uint8_t moduleIdx = CURRENT_MODULE_EDITED(menuVerticalPosition);

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

  moduleSettings[moduleIdx].mode = MODULE_MODE_BIND;
}

void runPopupRegister(event_t event)
{
  uint8_t backupVerticalOffset = menuVerticalOffset;
  int8_t backupEditMode = s_editMode;

  menuVerticalPosition = reusableBuffer.modelSetup.pxx2.registerPopupVerticalPosition;
  menuHorizontalPosition = reusableBuffer.modelSetup.pxx2.registerPopupHorizontalPosition;
  s_editMode = reusableBuffer.modelSetup.pxx2.registerPopupEditMode;

  switch (event) {
    case EVT_KEY_BREAK(KEY_ENTER):
      if (menuVerticalPosition != 2) {
        break;
      }
      else if (menuHorizontalPosition == 0) {
        // [Enter] pressed
        moduleSettings[INTERNAL_MODULE].mode = MODULE_MODE_REGISTER;
      }
      // no break

    case EVT_KEY_LONG(KEY_EXIT):
      s_editMode = 0;
      // no break;

    case EVT_KEY_BREAK(KEY_EXIT):
      if (s_editMode <= 0) {
        warningText = NULL;
      }
      break;
  }

  if (warningText) {
    const uint8_t dialogRows[] = { 0, 0, 1};
    check(event, 0, nullptr, 0, dialogRows, 2, 2);

    drawMessageBox();

    lcdDrawText(WARNING_LINE_X, WARNING_LINE_Y - 3, "Reg. ID");
    editName(WARNING_LINE_X + 8*FW, WARNING_LINE_Y - 3, g_model.modelRegistrationID, PXX2_LEN_REGISTRATION_ID, event, menuVerticalPosition == 0);

    lcdDrawText(WARNING_LINE_X, WARNING_LINE_Y - 2 + FH, "Rx Name");
    editName(WARNING_LINE_X + 8*FW, WARNING_LINE_Y - 2 + FH, reusableBuffer.modelSetup.pxx2.registerRxName, PXX2_LEN_RX_NAME, event, menuVerticalPosition == 1);

    lcdDrawText(WARNING_LINE_X, WARNING_LINE_Y+2*FH + 2, TR_ENTER, menuVerticalPosition == 2 && menuHorizontalPosition == 0 ? INVERS : 0);
    lcdDrawText(WARNING_LINE_X + 8*FW, WARNING_LINE_Y+2*FH + 2, TR_EXIT, menuVerticalPosition == 2 && menuHorizontalPosition == 1 ? INVERS : 0);

    reusableBuffer.modelSetup.pxx2.registerPopupVerticalPosition = menuVerticalPosition;
    reusableBuffer.modelSetup.pxx2.registerPopupHorizontalPosition = menuHorizontalPosition;
    reusableBuffer.modelSetup.pxx2.registerPopupEditMode = s_editMode;
  }

  menuVerticalPosition = ITEM_MODEL_INTERNAL_MODULE_PXX2_RANGE_REGISTER + HEADER_LINE;
  menuHorizontalPosition = 1;
  menuVerticalOffset = backupVerticalOffset;
  s_editMode = backupEditMode;
}

inline bool isDefaultModelRegistrationID()
{
  return memcmp(g_model.modelRegistrationID, g_eeGeneral.ownerRegistrationID, PXX2_LEN_REGISTRATION_ID) == 0;
}

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

    NUM_STICKS + NUM_POTS + NUM_SLIDERS + NUM_ROTARY_ENCODERS - 1, // Center beeps
    0, // Global functions
    IF_PXX2(isDefaultModelRegistrationID() ? HIDDEN_ROW : READONLY_ROW), // Registration ID

    LABEL(InternalModule),
      INTERNAL_MODULE_MODE_ROWS, // module mode (PXX(2) / None)
      INTERNAL_MODULE_CHANNELS_ROWS, // Channels min and count
      IF_NOT_PXX2(IF_INTERNAL_MODULE_ON(HAS_RF_PROTOCOL_MODELINDEX(g_model.moduleData[INTERNAL_MODULE].rfProtocol) ? (uint8_t)2 : (uint8_t)1)),
      IF_PXX2(0),    // Model Number
      ANTENNA_ROW
      IF_INTERNAL_MODULE_ON(FAILSAFE_ROWS(INTERNAL_MODULE)),       // Module start channel (pxx2 always send 16)
      IF_PXX2_RECEIVER_DISPLAYED(INTERNAL_MODULE, 0, 1),           // Range check and Register buttons
      IF_PXX2_RECEIVER_DISPLAYED(INTERNAL_MODULE, 0, (uint8_t)-1), // Receiver Number
      IF_PXX2_RECEIVER_DISPLAYED(INTERNAL_MODULE, 0, 0),           // Receiver Range
      IF_PXX2_RECEIVER_DISPLAYED(INTERNAL_MODULE, 0, 0),           // Receiver Telemetry
      IF_PXX2_RECEIVER_DISPLAYED(INTERNAL_MODULE, 0, 1),           // Receiver Bind/Delete
      IF_PXX2_RECEIVER_DISPLAYED(INTERNAL_MODULE, 1, (uint8_t)-1), // Receiver Number
      IF_PXX2_RECEIVER_DISPLAYED(INTERNAL_MODULE, 1, 0),           // Receiver Range
      IF_PXX2_RECEIVER_DISPLAYED(INTERNAL_MODULE, 1, 0),           // Receiver Telemetry
      IF_PXX2_RECEIVER_DISPLAYED(INTERNAL_MODULE, 1, 1),           // Receiver Bind/Delete
      IF_PXX2(INTERNAL_MODULE_ADD_RECEIVER_ROW),

    LABEL(ExternalModule),
      EXTERNAL_MODULE_MODE_ROWS,
      MULTIMODULE_SUBTYPE_ROWS(EXTERNAL_MODULE)
      MULTIMODULE_STATUS_ROWS
      EXTERNAL_MODULE_CHANNELS_ROWS,
      EXTERNAL_MODULE_BIND_ROWS(), // line reused for PPM: PPM settings
      OUTPUT_TYPE_ROWS()
      EXTERNAL_MODULE_OPTION_ROW,
      MULTIMODULE_MODULE_ROWS
      EXTERNAL_MODULE_POWER_ROW,
      EXTERNAL_MODULE_FREQ_ROW
      FAILSAFE_ROWS(EXTERNAL_MODULE),

    TRAINER_ROWS
  });
#else
  MENU_TAB({ HEADER_LINE_COLUMNS 0, TIMER_ROWS, TIMER_ROWS, TIMER_ROWS, 0, 1, 0, 0, 0, 0, 0, LABEL(PreflightCheck), 0, 0, NUM_SWITCHES-1, NUM_STICKS+NUM_POTS+NUM_SLIDERS+NUM_ROTARY_ENCODERS-1, 0,
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
    moduleSettings[0].mode = 0;
#if NUM_MODULES > 1
    moduleSettings[1].mode = 0;
#endif
  }
#endif

  TITLE(STR_MENUSETUP);

  if (event == EVT_ENTRY) {
    reusableBuffer.modelSetup.r9mPower = g_model.moduleData[EXTERNAL_MODULE].pxx.power;
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
          uint8_t length = STR_VSRCRAW[0];
          horzpos_t l_posHorz = menuHorizontalPosition;

          if (i>=NUM_BODY_LINES-2 && getSwitchWarningsCount() > 5*(NUM_BODY_LINES-i)) {
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
            if (menuHorizontalPosition > NUM_SWITCHES)
              menuHorizontalPosition = NUM_SWITCHES;
          }
          if (attr && menuHorizontalPosition == NUM_SWITCHES) {
#else
          if (attr) {
#endif
            s_editMode = 0;
            if (!READ_ONLY()) {
              switch (event) {
                case EVT_KEY_BREAK(KEY_ENTER):
                  break;

                case EVT_KEY_LONG(KEY_ENTER):
                  if (menuHorizontalPosition < 0 || menuHorizontalPosition >= NUM_SWITCHES) {
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
          for (int i=0; i<NUM_SWITCHES; i++) {
            if (SWITCH_WARNING_ALLOWED(i)) {
              div_t qr = div(current, 5);
              if (!READ_ONLY() && event==EVT_KEY_BREAK(KEY_ENTER) && line && l_posHorz==current) {
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
          if (attr && ((menuHorizontalPosition < 0) || menuHorizontalPosition >= NUM_SWITCHES)) {
            lcdDrawFilledRect(MODEL_SETUP_2ND_COLUMN-1, y-1, 8*(2*FW+1), 1+FH*((current+4)/5));
          }
#else //PCBTARANIS
      {
        lcdDrawTextAlignedLeft(y, STR_SWITCHWARNING);
        swarnstate_t states = g_model.switchWarningState;
        char c;
        if (attr) {
          s_editMode = 0;
          if (!READ_ONLY()) {
            switch (event) {
              CASE_EVT_ROTARY_BREAK
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
        for (uint8_t i=0; i<NUM_STICKS+NUM_POTS+NUM_SLIDERS+NUM_ROTARY_ENCODERS; i++) {
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

#if defined(PCBTARANIS)
      case ITEM_MODEL_INTERNAL_MODULE_LABEL:
        lcdDrawTextAlignedLeft(y, TR_INTERNALRF);
        break;

      case ITEM_MODEL_INTERNAL_MODULE_MODE:
        lcdDrawTextAlignedLeft(y, STR_MODE);
        lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_TARANIS_PROTOCOLS, g_model.moduleData[INTERNAL_MODULE].type, attr);
        if (attr) {
          g_model.moduleData[INTERNAL_MODULE].type = checkIncDec(event, g_model.moduleData[INTERNAL_MODULE].type, MODULE_TYPE_NONE, MODULE_TYPE_MAX, EE_MODEL, isInternalModuleAvailable);
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
        lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_TARANIS_PROTOCOLS, g_model.moduleData[EXTERNAL_MODULE].type, menuHorizontalPosition==0 ? attr : 0);
        if (isModuleXJT(EXTERNAL_MODULE))
          lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN+5*FW, y, STR_XJT_PROTOCOLS, 1+g_model.moduleData[EXTERNAL_MODULE].rfProtocol, menuHorizontalPosition==1 ? attr : 0);
        else if (isModuleDSM2(EXTERNAL_MODULE))
          lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN+5*FW, y, STR_DSM_PROTOCOLS, g_model.moduleData[EXTERNAL_MODULE].rfProtocol, menuHorizontalPosition==1 ? attr : 0);
#if defined(PCBXLITE)
        else if (isModuleR9M(EXTERNAL_MODULE))
          lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN+5*FW, y, STR_R9M_REGION, g_model.moduleData[EXTERNAL_MODULE].subType, (menuHorizontalPosition==1 ? attr : 0));
#else
        else if (isR9ModuleRunning(EXTERNAL_MODULE))
          lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN+5*FW, y, STR_R9M_REGION, g_model.moduleData[EXTERNAL_MODULE].r9m.region, (menuHorizontalPosition==1 ? attr : 0));
#endif
#if defined(MULTIMODULE)
        else if (isModuleMultimodule(EXTERNAL_MODULE)) {
          int multi_rfProto = g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(false);
          if (g_model.moduleData[EXTERNAL_MODULE].multi.customProto)
            lcdDrawText(MODEL_SETUP_2ND_COLUMN+5*FW, y, STR_MULTI_CUSTOM, menuHorizontalPosition==1 ? attr : 0);
          else
            lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN+5*FW, y, STR_MULTI_PROTOCOLS, multi_rfProto, menuHorizontalPosition==1 ? attr : 0);
        }
#endif
        if (attr && editMode > 0) {
          switch (menuHorizontalPosition) {
            case 0:
              g_model.moduleData[EXTERNAL_MODULE].type = checkIncDec(event, g_model.moduleData[EXTERNAL_MODULE].type,
                                                                     MODULE_TYPE_NONE,
                                                                     IS_TRAINER_EXTERNAL_MODULE() ? MODULE_TYPE_NONE :
                                                                     MODULE_TYPE_COUNT - 1, EE_MODEL,
                                                                     isExternalModuleAvailable);
              if (checkIncDec_Ret) {
                g_model.moduleData[EXTERNAL_MODULE].rfProtocol = 0;
                g_model.moduleData[EXTERNAL_MODULE].channelsStart = 0;
                g_model.moduleData[EXTERNAL_MODULE].channelsCount = defaultModuleChannels_M8(EXTERNAL_MODULE);
                if (isModuleSBUS(EXTERNAL_MODULE))
                  g_model.moduleData[EXTERNAL_MODULE].sbus.refreshRate = -31;
                if(isModulePPM(EXTERNAL_MODULE))
                  SET_DEFAULT_PPM_FRAME_LENGTH(EXTERNAL_MODULE);
              }
              break;
            case 1:
              if (isModuleDSM2(EXTERNAL_MODULE))
                CHECK_INCDEC_MODELVAR(event, g_model.moduleData[EXTERNAL_MODULE].rfProtocol, DSM2_PROTO_LP45, DSM2_PROTO_DSMX);
#if defined(PCBXLITE)
              else if (isModuleR9M(EXTERNAL_MODULE)) {
                uint8_t newR9MType = checkIncDec(event, g_model.moduleData[EXTERNAL_MODULE].subType, MODULE_SUBTYPE_R9M_FCC, MODULE_SUBTYPE_R9M_LAST, EE_MODEL, isR9MModeAvailable);
                if (newR9MType != g_model.moduleData[EXTERNAL_MODULE].subType && newR9MType > MODULE_SUBTYPE_R9M_EU) {
                  POPUP_WARNING(STR_R9MFLEXWARN1);
                  const char * w = STR_R9MFLEXWARN2;
                  SET_WARNING_INFO(w, strlen(w), 0);
                }
                g_model.moduleData[EXTERNAL_MODULE].subType = newR9MType;
              }
#endif
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
            bluetoothState = BLUETOOTH_STATE_OFF;
            bluetoothDistantAddr[0] = 0;
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
          if (bluetoothDistantAddr[0]) {
            lcdDrawText(INDENT_WIDTH, y+1, bluetoothDistantAddr, TINSIZE);
            if (bluetoothState != BLUETOOTH_STATE_CONNECTED) {
              lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, BUTTON("Bind"), menuHorizontalPosition == 0 ? attr : 0);
              lcdDrawText(MODEL_SETUP_2ND_COLUMN+5*FW, y, BUTTON("Clear"), menuHorizontalPosition == 1 ? attr : 0);
            }
            else {
              lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, BUTTON("Clear"), attr);
            }
            if (attr && event == EVT_KEY_FIRST(KEY_ENTER)) {
              if (bluetoothState == BLUETOOTH_STATE_CONNECTED || menuHorizontalPosition == 1) {
                bluetoothState = BLUETOOTH_STATE_OFF;
                bluetoothDistantAddr[0] = 0;
              }
              else {
                bluetoothState = BLUETOOTH_STATE_BIND_REQUESTED;
              }
            }
          }
          else {
            lcdDrawText(INDENT_WIDTH, y, "---");
            if (bluetoothState < BLUETOOTH_STATE_IDLE)
              lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, BUTTON("Init"), attr);
            else
              lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, BUTTON("Discover"), attr);
            if (attr && event == EVT_KEY_FIRST(KEY_ENTER)) {
              if (bluetoothState < BLUETOOTH_STATE_IDLE)
                bluetoothState = BLUETOOTH_STATE_OFF;
              else
                bluetoothState = BLUETOOTH_STATE_DISCOVER_REQUESTED;
            }
          }
        }
        else {
          if (bluetoothDistantAddr[0])
            lcdDrawText(INDENT_WIDTH, y+1, bluetoothDistantAddr, TINSIZE);
          else
            lcdDrawText(INDENT_WIDTH, y, "---");
          lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, bluetoothState == BLUETOOTH_STATE_CONNECTED ? "Connected" : "!Connected");
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
                CHECK_INCDEC_MODELVAR(event, moduleData.channelsCount, -4, min<int8_t>(maxModuleChannels_M8(moduleIdx), 32-8-moduleData.channelsStart));
                if ((k == ITEM_MODEL_EXTERNAL_MODULE_CHANNELS && g_model.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_PPM)) {
                  SET_DEFAULT_PPM_FRAME_LENGTH(moduleIdx);
                }
                break;
            }
          }
        }
        break;
      }

#if defined(PCBX7)
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

#if defined(PCBTARANIS)
      case ITEM_MODEL_REGISTRATION_ID:
        editSingleName(MODEL_SETUP_2ND_COLUMN, y, STR_REG_ID, g_model.modelRegistrationID, sizeof(g_model.modelRegistrationID), event, attr);
        break;

      case ITEM_MODEL_INTERNAL_MODULE_PXX2_MODEL_NUM:
        lcdDrawTextAlignedLeft(y, STR_RECEIVER_NUM);
        lcdDrawNumber(MODEL_SETUP_2ND_COLUMN, y, g_model.header.modelId[INTERNAL_MODULE], attr | LEADING0|LEFT, 2);
        if (attr) {
          CHECK_INCDEC_MODELVAR_ZERO(event, g_model.header.modelId[INTERNAL_MODULE], MAX_RX_NUM(INTERNAL_MODULE));
          if (checkIncDec_Ret) {
            modelHeaders[g_eeGeneral.currModel].modelId[INTERNAL_MODULE] = g_model.header.modelId[INTERNAL_MODULE];
          }
        }
      break;

      case ITEM_MODEL_INTERNAL_MODULE_PXX2_RANGE_REGISTER:
        lcdDrawTextAlignedLeft(y, INDENT "Module");
        lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, STR_MODULE_RANGE, (menuHorizontalPosition==0 ? attr : 0) );
        lcdDrawText(lcdLastRightPos, y, "[Reg]", (menuHorizontalPosition==1 ? attr : 0) );
        if (attr) {
          if (moduleSettings[INTERNAL_MODULE].mode == MODULE_MODE_NORMAL && s_editMode > 0) {
            if (menuHorizontalPosition == 1) {
              reusableBuffer.modelSetup.pxx2.registerPopupVerticalPosition = 0;
              reusableBuffer.modelSetup.pxx2.registerPopupHorizontalPosition = 0;
              reusableBuffer.modelSetup.pxx2.registerPopupEditMode = 0;
              s_editMode = 0;
              POPUP_INPUT("", runPopupRegister);
            }
            else {
              moduleSettings[INTERNAL_MODULE].mode = MODULE_MODE_RANGECHECK;
            }
          }
          if (s_editMode == 0) {
            moduleSettings[INTERNAL_MODULE].mode = MODULE_MODE_NORMAL;
          }
        }
      break;

      case ITEM_MODEL_INTERNAL_MODULE_PXX2_ADD_RECEIVER:
        lcdDrawText(INDENT_WIDTH, y, "Receiver");
        lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, "[Add]", attr);
        if (attr && s_editMode>0) {
          if (g_model.moduleData[INTERNAL_MODULE].pxx2.receivers[0].enabled) {
            g_model.moduleData[INTERNAL_MODULE].pxx2.receivers[1].enabled = 0x01;
            memcpy(&g_model.moduleData[INTERNAL_MODULE].pxx2.receivers[1].channelMapping, DEFAULT_CHANNEL_MAPPING, sizeof(uint64_t));
            menuVerticalPosition = ITEM_MODEL_INTERNAL_MODULE_PXX2_RECEIVER_2_BIND_DEL + 1;
          }
          else {
            g_model.moduleData[INTERNAL_MODULE].pxx2.receivers[0].enabled = 0x01;
            memcpy(&g_model.moduleData[INTERNAL_MODULE].pxx2.receivers[0].channelMapping, DEFAULT_CHANNEL_MAPPING, sizeof(uint64_t));
            menuVerticalPosition = ITEM_MODEL_INTERNAL_MODULE_PXX2_RECEIVER_1_BIND_DEL + 1;
          }
          killEvents(event);
          s_editMode = 0;
        }
        break;

      case ITEM_MODEL_INTERNAL_MODULE_PXX2_RECEIVER_1_ID:
      case ITEM_MODEL_INTERNAL_MODULE_PXX2_RECEIVER_2_ID:
      {
        uint8_t receiverIdx = CURRENT_RECEIVER_EDITED(k);
        uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);

        lcdDrawTextAlignedLeft(y, STR_RECEIVER);
        lcdDrawSizedText(MODEL_SETUP_2ND_COLUMN, y, g_model.moduleData[moduleIdx].pxx2.receivers[receiverIdx].rxName, PXX2_LEN_RX_NAME, 0);
      }
      break;

      case ITEM_MODEL_INTERNAL_MODULE_PXX2_RECEIVER_1_PINMAP:
      case ITEM_MODEL_INTERNAL_MODULE_PXX2_RECEIVER_2_PINMAP:
      {
        uint8_t receiverIdx = CURRENT_RECEIVER_EDITED(k);
        uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);

        lcdDrawTextAlignedLeft(y, INDENT INDENT "Pinmap");
        lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, STR_SET, attr);
        if (event == EVT_KEY_BREAK(KEY_ENTER) && attr) {
          g_receiverIdx = receiverIdx;
          g_moduleIdx = moduleIdx;
          pushMenu(menuModelPinmap);
        }
      }
      break;

      case ITEM_MODEL_INTERNAL_MODULE_PXX2_RECEIVER_1_TELEM:
      case ITEM_MODEL_INTERNAL_MODULE_PXX2_RECEIVER_2_TELEM:
      {
        uint8_t receiverIdx = CURRENT_RECEIVER_EDITED(k);
        g_model.moduleData[INTERNAL_MODULE].pxx2.receivers[receiverIdx].telemetry = editCheckBox(g_model.moduleData[INTERNAL_MODULE].pxx2.receivers[receiverIdx].telemetry, MODEL_SETUP_2ND_COLUMN, y, INDENT INDENT "Telemetry", attr, event);
      }
      break;

      case ITEM_MODEL_INTERNAL_MODULE_PXX2_RECEIVER_1_BIND_DEL:
      case ITEM_MODEL_INTERNAL_MODULE_PXX2_RECEIVER_2_BIND_DEL:
      {
        uint8_t receiverIdx = CURRENT_RECEIVER_EDITED(k);
        uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);

        lcdDrawSizedText(6, y, g_model.moduleData[moduleIdx].pxx2.receivers[receiverIdx].rxName, PXX2_LEN_RX_NAME, 0);
        lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, STR_MODULE_BIND, menuHorizontalPosition==0 ? attr : 0);
        lcdDrawText(MODEL_SETUP_2ND_COLUMN+MODEL_SETUP_RANGE_OFS, y, STR_DEL_BUTTON, menuHorizontalPosition==1 ? attr : 0);
        if (attr) {
          if (menuHorizontalPosition == 0) {
            if (event == EVT_KEY_BREAK(KEY_ENTER)) {
              reusableBuffer.modelSetup.pxx2.bindStep = BIND_START;
              reusableBuffer.modelSetup.pxx2.bindCandidateReceiversCount = 0;
              moduleSettings[INTERNAL_MODULE].mode ^= MODULE_MODE_BIND;
            }
            if (moduleSettings[INTERNAL_MODULE].mode == MODULE_MODE_BIND) {
              s_editMode = 1;
              if (reusableBuffer.modelSetup.pxx2.bindStep == BIND_RX_NAME_RECEIVED) {
                popupMenuItemsCount = min<uint8_t>(reusableBuffer.modelSetup.pxx2.bindCandidateReceiversCount, PXX2_MAX_RECEIVERS_PER_MODULE);
                for (uint8_t i=0; i<popupMenuItemsCount; i++) {
                  popupMenuItems[i] = reusableBuffer.modelSetup.pxx2.bindCandidateReceiversNames[i];
                }
                POPUP_MENU_START(onPXX2BindMenu);
              }
            }
            else {
              s_editMode = 0;
            }
            if (reusableBuffer.modelSetup.pxx2.bindStep == BIND_OK) {
              memcpy(g_model.moduleData[INTERNAL_MODULE].pxx2.receivers[receiverIdx].rxName, reusableBuffer.modelSetup.pxx2.bindCandidateReceiversIds[reusableBuffer.modelSetup.pxx2.bindSelectedReceiverIndex], PXX2_LEN_RX_NAME);
            }
          }
          else if (menuHorizontalPosition == 1 && s_editMode > 0) {
            if (receiverIdx == 0) {
              memcpy(&g_model.moduleData[INTERNAL_MODULE].pxx2.receivers[0], &g_model.moduleData[INTERNAL_MODULE].pxx2.receivers[1], sizeof(ReceiverData));
              menuVerticalPosition = ITEM_MODEL_INTERNAL_MODULE_PXX2_ADD_RECEIVER + 1;
            }
            memclear(&g_model.moduleData[INTERNAL_MODULE].pxx2.receivers[receiverIdx], sizeof(ReceiverData));
            s_editMode = 0;
            killEvents(event);
          }
        }
      }
      break;

#endif

#if defined(PCBTARANIS)
      case ITEM_MODEL_INTERNAL_MODULE_NPXX2_BIND:
#endif
#if defined(PCBSKY9X)
      case ITEM_MODEL_EXTRA_MODULE_BIND:
#endif
      case ITEM_MODEL_EXTERNAL_MODULE_BIND:
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
          lcdDrawText(MODEL_SETUP_2ND_COLUMN+5*FW+2, y, moduleData.sbus.noninverted ? "no inv" : "normal", (CURSOR_ON_LINE() || menuHorizontalPosition==1) ? attr : 0);

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
            lcdDrawTextAlignedLeft(y, STR_RECEIVER);
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
#elif defined(PCBSKY9X) || defined(AR9X)
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
                    if (moduleSettings[moduleIdx].mode == MODULE_MODE_BIND) {
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
            moduleSettings[moduleIdx].mode = newFlag;

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
          POPUP_CONFIRMATION(STR_ANTENNACONFIRM1);
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
#if defined(PCBXLITE)
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
#else
        if (isModuleSBUS(moduleIdx)) {
#endif
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
#if defined(PCBXLITE) && !defined(MODULE_R9M_FULLSIZE)    // R9M lite FCC has only one power value, so displayed for info only
            lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_R9M_FCC_POWER_VALUES, g_model.moduleData[moduleIdx].pxx.power, LEFT);
            if (attr)
              REPEAT_LAST_CURSOR_MOVE();
#else
            lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_R9M_FCC_POWER_VALUES, g_model.moduleData[moduleIdx].pxx.power, LEFT | attr);
            if (attr)
              CHECK_INCDEC_MODELVAR_ZERO(event, g_model.moduleData[moduleIdx].pxx.power, R9M_FCC_POWER_MAX);
#endif
          }
          else {
            g_model.moduleData[moduleIdx].pxx.power = min((uint8_t)g_model.moduleData[moduleIdx].pxx.power, (uint8_t)R9M_LBT_POWER_MAX);
            lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_R9M_LBT_POWER_VALUES, g_model.moduleData[moduleIdx].pxx.power, LEFT | attr);
            if (attr) {
              CHECK_INCDEC_MODELVAR_ZERO(event, g_model.moduleData[moduleIdx].pxx.power, R9M_LBT_POWER_MAX);
            }
            if (attr && editMode == 0 && reusableBuffer.modelSetup.r9mPower != g_model.moduleData[moduleIdx].pxx.power) {
              if((reusableBuffer.modelSetup.r9mPower + g_model.moduleData[moduleIdx].pxx.power) < 5) { //switching between mode 2 and 3 does not require rebind
                POPUP_WARNING(STR_REBIND);
              }
              reusableBuffer.modelSetup.r9mPower = g_model.moduleData[moduleIdx].pxx.power;
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
#if !defined(PCBXLITE)
      case ITEM_MODEL_EXTERNAL_MODULE_FREQ:
      {
        uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
        lcdDrawTextAlignedLeft(y, STR_FREQUENCY);
        lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_R9MFLEX_FREQ, g_model.moduleData[moduleIdx].r9m.freq, LEFT | attr);
        if (attr)
        {
          CHECK_INCDEC_MODELVAR_ZERO(event, g_model.moduleData[moduleIdx].r9m.freq, 1);
        }
      }
      break;
#endif

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
    lcdDrawNumber(16+4*FW, 5*FH, TELEMETRY_RSSI(), BOLD);
  }
#endif

  // some field just finished being edited
  if (old_editMode > 0 && s_editMode == 0) {
    switch(menuVerticalPosition) {
#if defined(PCBTARANIS)
    case ITEM_MODEL_INTERNAL_MODULE_NPXX2_BIND:
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
      case ITEM_MODEL_EXTERNAL_MODULE_BIND:
      if (menuHorizontalPosition == 0)
        checkModelIdUnique(g_eeGeneral.currModel, EXTERNAL_MODULE);
      break;
    }
  }
}

void menuModelFailsafe(event_t event)
{
  const uint8_t channelStart = g_model.moduleData[g_moduleIdx].channelsStart;
  const int lim = (g_model.extendedLimits ? (512 * LIMIT_EXT_PERCENT / 100) : 512) * 2;
  uint8_t wbar = LCD_W - FW * 4 - FWNUM * 4;
#if defined(PPM_UNIT_PERCENT_PREC1)
  wbar -= 6;
#endif

  if (event == EVT_KEY_LONG(KEY_ENTER)) {
    killEvents(event);
    event = 0;

    if (menuVerticalPosition < sentModuleChannels(g_moduleIdx)) {
      if (s_editMode > 0) {
        g_model.moduleData[g_moduleIdx].failsafeChannels[menuVerticalPosition] = channelOutputs[menuVerticalPosition+channelStart];
        s_editMode = 0;
      }
      else {
        int16_t & failsafe = g_model.moduleData[g_moduleIdx].failsafeChannels[menuVerticalPosition];
        if (failsafe < FAILSAFE_CHANNEL_HOLD)
          failsafe = FAILSAFE_CHANNEL_HOLD;
        else if (failsafe == FAILSAFE_CHANNEL_HOLD)
          failsafe = FAILSAFE_CHANNEL_NOPULSE;
        else
          failsafe = 0;
      }
    }
    else {
      // "Outputs => Failsafe" menu item
      setCustomFailsafe(g_moduleIdx);
    }

    storageDirty(EE_MODEL);
    AUDIO_WARNING1();
    SEND_FAILSAFE_NOW(g_moduleIdx);
  }

  SIMPLE_SUBMENU_NOTITLE(sentModuleChannels(g_moduleIdx) + 1);

  lcdDrawTextAlignedCenter(0, FAILSAFESET);
  lcdInvertLine(0);

  const coord_t x = 1;
  coord_t y = FH + 1;
  uint8_t line = (menuVerticalPosition >= sentModuleChannels(g_moduleIdx) ? 2 : 0);
  uint8_t ch = (menuVerticalPosition >= 8 ? 8 : 0) + line;

  // Channels
  for (; line < 8; line++) {
    const int32_t channelValue = channelOutputs[ch+channelStart];
    int32_t failsafeValue = g_model.moduleData[g_moduleIdx].failsafeChannels[ch];

    //Channel
    putsChn(x+1, y, ch+1, SMLSIZE);

    // Value
    LcdFlags flags = TINSIZE;
    if (menuVerticalPosition == ch) {
      flags |= INVERS;
      if (s_editMode > 0) {
        if (failsafeValue == FAILSAFE_CHANNEL_HOLD || failsafeValue == FAILSAFE_CHANNEL_NOPULSE) {
          s_editMode = 0;
        }
        else {
          flags |= BLINK;
          CHECK_INCDEC_MODELVAR(event, g_model.moduleData[g_moduleIdx].failsafeChannels[ch], -lim, +lim);
        }
      }
    }

    uint8_t xValue = x+LCD_W-4-wbar;
    if (failsafeValue == FAILSAFE_CHANNEL_HOLD) {
      lcdDrawText(xValue, y, STR_HOLD, RIGHT|flags);
      failsafeValue = 0;
    }
    else if (failsafeValue == FAILSAFE_CHANNEL_NOPULSE) {
      lcdDrawText(xValue, y, STR_NONE, RIGHT|flags);
      failsafeValue = 0;
    }
    else {
#if defined(PPM_UNIT_US)
      lcdDrawNumber(xValue, y, PPM_CH_CENTER(ch)+failsafeValue/2, RIGHT|flags);
#elif defined(PPM_UNIT_PERCENT_PREC1)
      lcdDrawNumber(xValue, y, calcRESXto1000(failsafeValue), RIGHT|PREC1|flags);
#else
      lcdDrawNumber(xValue, y, calcRESXto1000(failsafeValue)/10, RIGHT|flags);
#endif
    }

    // Gauge
#if !defined(PCBX7)  // X7 LCD doesn't like too many horizontal lines
    lcdDrawRect(x+LCD_W-3-wbar, y, wbar+1, 6);
#endif
    const uint8_t lenChannel = limit<uint8_t>(1, (abs(channelValue) * wbar/2 + lim/2) / lim, wbar/2);
    const uint8_t lenFailsafe = limit<uint8_t>(1, (abs(failsafeValue) * wbar/2 + lim/2) / lim, wbar/2);
    const coord_t xChannel = (channelValue>0) ? x+LCD_W-3-wbar/2 : x+LCD_W-2-wbar/2-lenChannel;
    const coord_t xFailsafe = (failsafeValue>0) ? x+LCD_W-3-wbar/2 : x+LCD_W-2-wbar/2-lenFailsafe;
    lcdDrawHorizontalLine(xChannel, y+1, lenChannel, DOTTED, 0);
    lcdDrawHorizontalLine(xChannel, y+2, lenChannel, DOTTED, 0);
    lcdDrawSolidHorizontalLine(xFailsafe, y+3, lenFailsafe);
    lcdDrawSolidHorizontalLine(xFailsafe, y+4, lenFailsafe);

    y += FH - 1;

    if (++ch >= sentModuleChannels(g_moduleIdx))
      break;
  }

  if (menuVerticalPosition >= sentModuleChannels(g_moduleIdx)) {
    // Outputs => Failsafe
    lcdDrawText(CENTER_OFS, LCD_H - (FH + 1), STR_OUTPUTS2FAILSAFE, INVERS);
  }
}

void menuModelPinmap(event_t event)
{
  const int lim = (g_model.extendedLimits ? (512 * LIMIT_EXT_PERCENT / 100) : 512) * 2;
  uint8_t wbar = LCD_W / 2 - 20;
  SIMPLE_SUBMENU_NOTITLE(sentModuleChannels(g_moduleIdx));

  lcdDrawTextAlignedLeft(0, STR_PINMAPSET);
  for (uint8_t pos=0; pos<PXX2_LEN_RX_NAME; pos++) {
    lcdDrawHexChar(50 + pos*FW*2, 0, g_model.moduleData[INTERNAL_MODULE].pxx2.receivers[g_receiverIdx].rxName[pos], 0);
  }
  lcdInvertLine(0);

  const coord_t x = 1;
  coord_t y = FH + 1;
  uint8_t pin = (menuVerticalPosition >= 8 ? 8 : 0);

  for (uint8_t line = 0; line < 8; line++) {
    const int32_t channelValue = channelOutputs[getPinOuput(g_receiverIdx, g_moduleIdx, pin)];

    // Pin
    lcdDrawText(0, y, "Pin", SMLSIZE);
    lcdDrawNumber(lcdLastRightPos + 1, y, pin+1, SMLSIZE);

    // Channel
    LcdFlags flags = SMLSIZE;
    if (menuVerticalPosition == pin) {
      flags |= INVERS;
      if (s_editMode > 0) {
        uint8_t channel = getPinOuput(g_receiverIdx, g_moduleIdx, pin);
        flags |= BLINK;
        CHECK_INCDEC_MODELVAR(event, channel, 0, sentModuleChannels(g_moduleIdx));
        setPinOuput(g_receiverIdx, g_moduleIdx, pin, channel);
      }
    }
    putsChn(8*FW, y, getPinOuput(g_receiverIdx, g_moduleIdx, pin)+1, flags);

    // Bargraph
    const uint8_t lenChannel = limit<uint8_t>(1, (abs(channelValue) * wbar/2 + lim/2) / lim, wbar/2);
    const coord_t xChannel = (channelValue>0) ? x+LCD_W-3-wbar/2 : x+LCD_W-2-wbar/2-lenChannel;
    lcdDrawHorizontalLine(xChannel, y+1, lenChannel, DOTTED, 0);
    lcdDrawHorizontalLine(xChannel, y+2, lenChannel, DOTTED, 0);

    y += FH-1;
    if (++pin > sentModuleChannels(g_moduleIdx))
      break;
  }
}