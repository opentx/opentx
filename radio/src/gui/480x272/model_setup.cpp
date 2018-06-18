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
  ITEM_MODEL_NAME,
  ITEM_MODEL_BITMAP,
  ITEM_MODEL_TIMER1,
  ITEM_MODEL_TIMER1_NAME,
  ITEM_MODEL_TIMER1_PERSISTENT,
  ITEM_MODEL_TIMER1_MINUTE_BEEP,
  ITEM_MODEL_TIMER1_COUNTDOWN_BEEP,
#if TIMERS > 1
  ITEM_MODEL_TIMER2,
  ITEM_MODEL_TIMER2_NAME,
  ITEM_MODEL_TIMER2_PERSISTENT,
  ITEM_MODEL_TIMER2_MINUTE_BEEP,
  ITEM_MODEL_TIMER2_COUNTDOWN_BEEP,
#endif
#if TIMERS > 2
  ITEM_MODEL_TIMER3,
  ITEM_MODEL_TIMER3_NAME,
  ITEM_MODEL_TIMER3_PERSISTENT,
  ITEM_MODEL_TIMER3_MINUTE_BEEP,
  ITEM_MODEL_TIMER3_COUNTDOWN_BEEP,
#endif
  ITEM_MODEL_EXTENDED_LIMITS,
  ITEM_MODEL_EXTENDED_TRIMS,
  ITEM_MODEL_DISPLAY_TRIMS,
  ITEM_MODEL_TRIM_INC,
  ITEM_MODEL_THROTTLE_LABEL,
  ITEM_MODEL_THROTTLE_REVERSED,
  ITEM_MODEL_THROTTLE_TRACE,
  ITEM_MODEL_THROTTLE_TRIM,
  ITEM_MODEL_PREFLIGHT_LABEL,
  ITEM_MODEL_CHECKLIST_DISPLAY,
  ITEM_MODEL_THROTTLE_WARNING,
  ITEM_MODEL_SWITCHES_WARNING,
  ITEM_MODEL_SLIDPOT_WARNING_STATE,
  ITEM_MODEL_POTS_WARNING,
  ITEM_MODEL_SLIDERS_WARNING,
  ITEM_MODEL_BEEP_CENTER,
  ITEM_MODEL_USE_GLOBAL_FUNCTIONS,
  ITEM_MODEL_INTERNAL_MODULE_LABEL,
  ITEM_MODEL_INTERNAL_MODULE_MODE,
  ITEM_MODEL_INTERNAL_MODULE_CHANNELS,
  ITEM_MODEL_INTERNAL_MODULE_BIND,
  ITEM_MODEL_INTERNAL_MODULE_FAILSAFE,
  ITEM_MODEL_INTERNAL_MODULE_ANTENNA,
  ITEM_MODEL_EXTERNAL_MODULE_LABEL,
  ITEM_MODEL_EXTERNAL_MODULE_MODE,
#if defined(MULTIMODULE)
  ITEM_MODEL_EXTERNAL_MODULE_STATUS,
  ITEM_MODEL_EXTERNAL_MODULE_SYNCSTATUS,
#endif
  ITEM_MODEL_EXTERNAL_MODULE_CHANNELS,
  ITEM_MODEL_EXTERNAL_MODULE_BIND,
  ITEM_MODEL_EXTERNAL_MODULE_FAILSAFE,
  ITEM_MODEL_EXTERNAL_MODULE_OPTIONS,
#if defined(MULTIMODULE)
  ITEM_MODEL_EXTERNAL_MODULE_AUTOBIND,
#endif
  ITEM_MODEL_EXTERNAL_MODULE_POWER,
  ITEM_MODEL_TRAINER_LABEL,
  ITEM_MODEL_TRAINER_MODE,
  ITEM_MODEL_TRAINER_LINE1,
  ITEM_MODEL_TRAINER_LINE2,
  ITEM_MODEL_SETUP_MAX
};

#define MODEL_SETUP_2ND_COLUMN         200
#define MODEL_SETUP_3RD_COLUMN         270
#define MODEL_SETUP_4TH_COLUMN         350
#define MODEL_SETUP_BIND_OFS           40
#define MODEL_SETUP_RANGE_OFS          80
#define MODEL_SETUP_SET_FAILSAFE_OFS   100
#define MODEL_SETUP_SLIDPOT_SPACING    45

#define CURRENT_MODULE_EDITED(k)       (k>=ITEM_MODEL_TRAINER_LABEL ? TRAINER_MODULE : (k>=ITEM_MODEL_EXTERNAL_MODULE_LABEL ? EXTERNAL_MODULE : INTERNAL_MODULE))

void checkModelIdUnique(uint8_t moduleIdx)
{
  char* warn_buf = reusableBuffer.modelsetup.msg;

  // cannot rely exactly on WARNING_LINE_LEN so using WARNING_LINE_LEN-2
  size_t warn_buf_len = sizeof(reusableBuffer.modelsetup.msg) - WARNING_LINE_LEN - 2;
  if (!modelslist.isModelIdUnique(moduleIdx,warn_buf,warn_buf_len)) {
    if (warn_buf[0] != 0) {
      POPUP_WARNING(STR_MODELIDUSED);
      SET_WARNING_INFO(warn_buf, sizeof(reusableBuffer.modelsetup.msg), 0);
    }
  }
}

void onBindMenu(const char * result)
{
  uint8_t moduleIdx = (menuVerticalPosition >= ITEM_MODEL_EXTERNAL_MODULE_LABEL ? EXTERNAL_MODULE : INTERNAL_MODULE);

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

  moduleFlag[moduleIdx] = MODULE_BIND;
}

void onModelSetupBitmapMenu(const char * result)
{
  if (result == STR_UPDATE_LIST) {
    if (!sdListFiles(BITMAPS_PATH, BITMAPS_EXT, sizeof(g_model.header.bitmap)-LEN_BITMAPS_EXT, NULL)) {
      POPUP_WARNING(STR_NO_BITMAPS_ON_SD);
    }
  }
  else {
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

#define IF_INTERNAL_MODULE_ON(x)          (IS_INTERNAL_MODULE_ENABLED() ? (uint8_t)(x) : HIDDEN_ROW)
#define IF_EXTERNAL_MODULE_ON(x)          (IS_EXTERNAL_MODULE_ENABLED() ? (uint8_t)(x) : HIDDEN_ROW)

#define INTERNAL_MODULE_MODE_ROWS         (uint8_t)0
#define INTERNAL_MODULE_CHANNELS_ROWS     IF_INTERNAL_MODULE_ON(1)
#define PORT_CHANNELS_ROWS(x)             (x==INTERNAL_MODULE ? INTERNAL_MODULE_CHANNELS_ROWS : (x==EXTERNAL_MODULE ? EXTERNAL_MODULE_CHANNELS_ROWS : 1))

#define TIMER_ROWS(x)                     NAVIGATION_LINE_BY_LINE|1, 0, 0, 0, g_model.timers[x].countdownBeep != COUNTDOWN_SILENT ? (uint8_t)1 : (uint8_t)0

#define EXTERNAL_MODULE_MODE_ROWS         (IS_MODULE_PXX(EXTERNAL_MODULE) || IS_MODULE_DSM2(EXTERNAL_MODULE)) ? (uint8_t)1 : IS_MODULE_MULTIMODULE(EXTERNAL_MODULE) ? MULTIMODULE_MODE_ROWS(EXTERNAL_MODULE) : (uint8_t)0

#if TIMERS == 1
  #define TIMERS_ROWS                     TIMER_ROWS(0)
#elif TIMERS == 2
  #define TIMERS_ROWS                     TIMER_ROWS(0), TIMER_ROWS(1)
#elif TIMERS == 3
  #define TIMERS_ROWS                     TIMER_ROWS(0), TIMER_ROWS(1), TIMER_ROWS(2)
#endif

#define SW_WARN_ITEMS()                   uint8_t(NAVIGATION_LINE_BY_LINE|(getSwitchWarningsCount()-1))
#define POT_WARN_ROWS                     (uint8_t)0
#define POT_WARN_ITEMS()                  ((g_model.potsWarnMode) ? uint8_t(NAVIGATION_LINE_BY_LINE|(NUM_POTS-1)) : (uint8_t)0)
#define SLIDER_WARN_ITEMS()               ((g_model.potsWarnMode) ? uint8_t(NAVIGATION_LINE_BY_LINE|(NUM_SLIDERS-1)) : (uint8_t)0)

#define TRAINER_LINE1_BLUETOOTH_M_ROWS    ((bluetoothDistantAddr[0] == 0 || bluetoothState == BLUETOOTH_STATE_CONNECTED) ? (uint8_t)0 : (uint8_t)1)
#define TRAINER_LINE1_ROWS                (g_model.trainerMode == TRAINER_MODE_SLAVE ? (uint8_t)1 : (g_model.trainerMode == TRAINER_MODE_MASTER_BLUETOOTH ? TRAINER_LINE1_BLUETOOTH_M_ROWS : (g_model.trainerMode == TRAINER_MODE_SLAVE_BLUETOOTH ? (uint8_t)1 : HIDDEN_ROW)))
#define TRAINER_LINE2_ROWS                (g_model.trainerMode == TRAINER_MODE_SLAVE ? (uint8_t)2 : HIDDEN_ROW)

bool menuModelSetup(event_t event)
{
  bool CURSOR_ON_CELL = (menuHorizontalPosition >= 0);

  // Switch to external antenna confirmation
  if (warningResult) {
    warningResult = 0;
    g_model.moduleData[INTERNAL_MODULE].pxx.external_antenna = XJT_EXTERNAL_ANTENNA;
  }

  int8_t old_editMode = s_editMode;
  MENU(STR_MENUSETUP, MODEL_ICONS, menuTabModel, MENU_MODEL_SETUP, ITEM_MODEL_SETUP_MAX,
       { 0, 0, TIMERS_ROWS, 0, 1, 0, 0,
         LABEL(Throttle), 0, 0, 0,
         LABEL(PreflightCheck), 0, 0, SW_WARN_ITEMS(), POT_WARN_ROWS, (g_model.potsWarnMode ? POT_WARN_ITEMS() : HIDDEN_ROW), (g_model.potsWarnMode ? SLIDER_WARN_ITEMS() : HIDDEN_ROW), NAVIGATION_LINE_BY_LINE|(NUM_STICKS+NUM_POTS+NUM_SLIDERS+NUM_ROTARY_ENCODERS-1), 0,
         LABEL(InternalModule),
         INTERNAL_MODULE_MODE_ROWS,
         INTERNAL_MODULE_CHANNELS_ROWS,
         IF_INTERNAL_MODULE_ON(IS_MODULE_XJT(INTERNAL_MODULE) ? (HAS_RF_PROTOCOL_MODELINDEX(g_model.moduleData[INTERNAL_MODULE].rfProtocol) ? (uint8_t)2 : (uint8_t)1) : (IS_MODULE_PPM(INTERNAL_MODULE) ? (uint8_t)1 : HIDDEN_ROW)),
         IF_INTERNAL_MODULE_ON((IS_MODULE_XJT(INTERNAL_MODULE)) ? FAILSAFE_ROWS(INTERNAL_MODULE) : HIDDEN_ROW),
         IF_INTERNAL_MODULE_ON(0),
         LABEL(ExternalModule),
         EXTERNAL_MODULE_MODE_ROWS,
         MULTIMODULE_STATUS_ROWS
         EXTERNAL_MODULE_CHANNELS_ROWS,
         ((IS_MODULE_XJT(EXTERNAL_MODULE) && !HAS_RF_PROTOCOL_MODELINDEX(g_model.moduleData[EXTERNAL_MODULE].rfProtocol)) || IS_MODULE_SBUS(EXTERNAL_MODULE)) ? (uint8_t)1 : (IS_MODULE_PPM(EXTERNAL_MODULE) || IS_MODULE_PXX(EXTERNAL_MODULE) || IS_MODULE_DSM2(EXTERNAL_MODULE) || IS_MODULE_MULTIMODULE(EXTERNAL_MODULE)) ? (uint8_t)2 : HIDDEN_ROW,
         FAILSAFE_ROWS(EXTERNAL_MODULE),
         EXTERNAL_MODULE_OPTION_ROW,
         MULTIMODULE_MODULE_ROWS
         EXTERNAL_MODULE_POWER_ROW,
         LABEL(Trainer),
         0,
         TRAINER_LINE1_ROWS,
         TRAINER_LINE2_ROWS });

  if (event == EVT_ENTRY) {
    reusableBuffer.modelsetup.r9mPower = g_model.moduleData[EXTERNAL_MODULE].pxx.power;
  }
  
  if (menuEvent) {
    moduleFlag[0] = 0;
    moduleFlag[1] = 0;
  }

  int sub = menuVerticalPosition;

  for (uint8_t i=0; i<NUM_BODY_LINES; ++i) {
    coord_t y = MENU_CONTENT_TOP + i*FH;
    uint8_t k = i + menuVerticalOffset;
    for (int j=0; j<=k; j++) {
      if (mstate_tab[j] == HIDDEN_ROW)
        k++;
    }

    LcdFlags blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
    LcdFlags attr = (sub == k ? blink : 0);

    switch(k) {
      case ITEM_MODEL_NAME:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MODELNAME);
        editName(MODEL_SETUP_2ND_COLUMN, y, g_model.header.name, sizeof(g_model.header.name), event, attr);
        break;

      case ITEM_MODEL_BITMAP:
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

      case ITEM_MODEL_TIMER1:
        editTimerMode(0, y, attr, event);
        break;

      case ITEM_MODEL_TIMER1_NAME:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_TIMER_NAME);
        editName(MODEL_SETUP_2ND_COLUMN, y, g_model.timers[0].name, LEN_TIMER_NAME, event, attr);
        break;

      case ITEM_MODEL_TIMER1_MINUTE_BEEP:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MINUTEBEEP);
        g_model.timers[0].minuteBeep = editCheckBox(g_model.timers[0].minuteBeep, MODEL_SETUP_2ND_COLUMN, y, attr, event);
        break;

      case ITEM_MODEL_TIMER1_COUNTDOWN_BEEP:
        editTimerCountdown(0, y, attr, event);
        break;

      case ITEM_MODEL_TIMER1_PERSISTENT:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_PERSISTENT);
        g_model.timers[0].persistent = editChoice(MODEL_SETUP_2ND_COLUMN, y, STR_VPERSISTENT, g_model.timers[0].persistent, 0, 2, attr, event);
        break;

#if TIMERS > 1
      case ITEM_MODEL_TIMER2:
        editTimerMode(1, y, attr, event);
        break;

      case ITEM_MODEL_TIMER2_NAME:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_TIMER_NAME);
        editName(MODEL_SETUP_2ND_COLUMN, y, g_model.timers[1].name, LEN_TIMER_NAME, event, attr);
        break;

      case ITEM_MODEL_TIMER2_MINUTE_BEEP:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MINUTEBEEP);
        g_model.timers[1].minuteBeep = editCheckBox(g_model.timers[1].minuteBeep, MODEL_SETUP_2ND_COLUMN, y, attr, event);
        break;

      case ITEM_MODEL_TIMER2_COUNTDOWN_BEEP:
        editTimerCountdown(1, y, attr, event);
        break;

      case ITEM_MODEL_TIMER2_PERSISTENT:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_PERSISTENT);
        g_model.timers[1].persistent = editChoice(MODEL_SETUP_2ND_COLUMN, y, STR_VPERSISTENT, g_model.timers[1].persistent, 0, 2, attr, event);
        break;
#endif

#if TIMERS > 2
      case ITEM_MODEL_TIMER3:
        editTimerMode(2, y, attr, event);
        break;

      case ITEM_MODEL_TIMER3_NAME:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_TIMER_NAME);
        editName(MODEL_SETUP_2ND_COLUMN, y, g_model.timers[2].name, LEN_TIMER_NAME, event, attr);
        break;

      case ITEM_MODEL_TIMER3_MINUTE_BEEP:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MINUTEBEEP);
        g_model.timers[2].minuteBeep = editCheckBox(g_model.timers[2].minuteBeep, MODEL_SETUP_2ND_COLUMN, y, attr, event);
        break;

      case ITEM_MODEL_TIMER3_COUNTDOWN_BEEP:
        editTimerCountdown(2, y, attr, event);
        break;

      case ITEM_MODEL_TIMER3_PERSISTENT:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_PERSISTENT);
        g_model.timers[2].persistent = editChoice(MODEL_SETUP_2ND_COLUMN, y, STR_VPERSISTENT, g_model.timers[2].persistent, 0, 2, attr, event);
        break;
#endif

      case ITEM_MODEL_EXTENDED_LIMITS:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_ELIMITS);
        g_model.extendedLimits = editCheckBox(g_model.extendedLimits, MODEL_SETUP_2ND_COLUMN, y, attr, event);
        break;

      case ITEM_MODEL_EXTENDED_TRIMS:
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

      case ITEM_MODEL_DISPLAY_TRIMS:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_DISPLAY_TRIMS);
        g_model.displayTrims = editChoice(MODEL_SETUP_2ND_COLUMN, y, "\006No\0   ChangeYes", g_model.displayTrims, 0, 2, attr, event);
        break;

      case ITEM_MODEL_TRIM_INC:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_TRIMINC);
        g_model.trimInc = editChoice(MODEL_SETUP_2ND_COLUMN, y, STR_VTRIMINC, g_model.trimInc, -2, 2, attr, event);
        break;

      case ITEM_MODEL_THROTTLE_LABEL:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_THROTTLE_LABEL);
        break;

      case ITEM_MODEL_THROTTLE_REVERSED:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_THROTTLEREVERSE);
        g_model.throttleReversed = editCheckBox(g_model.throttleReversed, MODEL_SETUP_2ND_COLUMN, y, attr, event);
        break;

      case ITEM_MODEL_THROTTLE_TRACE:
      {
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_TTRACE);
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
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_TTRIM);
        g_model.thrTrim = editCheckBox(g_model.thrTrim, MODEL_SETUP_2ND_COLUMN, y, attr, event);
        break;

      case ITEM_MODEL_PREFLIGHT_LABEL:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_PREFLIGHT);
        break;

      case ITEM_MODEL_CHECKLIST_DISPLAY:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_CHECKLIST);
        g_model.displayChecklist = editCheckBox(g_model.displayChecklist, MODEL_SETUP_2ND_COLUMN, y, attr, event);
        break;

      case ITEM_MODEL_THROTTLE_WARNING:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_THROTTLEWARNING);
        g_model.disableThrottleWarning = !editCheckBox(!g_model.disableThrottleWarning, MODEL_SETUP_2ND_COLUMN, y, attr, event);
        break;

      case ITEM_MODEL_SWITCHES_WARNING:
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

      case ITEM_MODEL_SLIDPOT_WARNING_STATE:
        lcdDrawText(MENUS_MARGIN_LEFT, y,STR_POTWARNINGSTATE);
        lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, PSTR("\004""OFF\0""Man\0""Auto"), g_model.potsWarnMode, attr);
        if (attr) {
          CHECK_INCDEC_MODELVAR(event, g_model.potsWarnMode, POTS_WARN_OFF, POTS_WARN_AUTO);
          storageDirty(EE_MODEL);
        }
        break;

      case ITEM_MODEL_POTS_WARNING:
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

      case ITEM_MODEL_SLIDERS_WARNING:
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

      case ITEM_MODEL_BEEP_CENTER:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_BEEPCTR);
        lcdNextPos = MODEL_SETUP_2ND_COLUMN - 3;
        for (int i=0; i<NUM_STICKS+NUM_POTS+NUM_SLIDERS; i++) {
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

      case ITEM_MODEL_USE_GLOBAL_FUNCTIONS:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_USE_GLOBAL_FUNCS);
        drawCheckBox(MODEL_SETUP_2ND_COLUMN, y, !g_model.noGlobalFunctions, attr);
        if (attr) g_model.noGlobalFunctions = !checkIncDecModel(event, !g_model.noGlobalFunctions, 0, 1);
        break;

      case ITEM_MODEL_INTERNAL_MODULE_LABEL:
        lcdDrawText(MENUS_MARGIN_LEFT, y, TR_INTERNALRF);
        break;

      case ITEM_MODEL_INTERNAL_MODULE_MODE:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MODE);
        lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_XJT_PROTOCOLS, 1+g_model.moduleData[0].rfProtocol, attr);
        if (attr) {
          g_model.moduleData[INTERNAL_MODULE].rfProtocol = checkIncDec(event, g_model.moduleData[INTERNAL_MODULE].rfProtocol, RF_PROTO_OFF, RF_PROTO_LAST, EE_MODEL, isRfProtocolAvailable);
          if (checkIncDec_Ret) {
            g_model.moduleData[0].type = MODULE_TYPE_XJT;
            g_model.moduleData[0].channelsStart = 0;
            g_model.moduleData[0].channelsCount = DEFAULT_CHANNELS(INTERNAL_MODULE);
            if (g_model.moduleData[INTERNAL_MODULE].rfProtocol == RF_PROTO_OFF)
              g_model.moduleData[INTERNAL_MODULE].type = MODULE_TYPE_NONE;
          }
        }
        break;

      case ITEM_MODEL_INTERNAL_MODULE_ANTENNA:
      {
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_ANTENNASELECTION);
        uint8_t newAntennaSel = editChoice(MODEL_SETUP_2ND_COLUMN, y, STR_VANTENNATYPES, g_model.moduleData[INTERNAL_MODULE].pxx.external_antenna, 0, 1, attr, event);
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

      case ITEM_MODEL_TRAINER_MODE:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MODE);
        g_model.trainerMode = editChoice(MODEL_SETUP_2ND_COLUMN, y, STR_VTRAINERMODES, g_model.trainerMode, 0, TRAINER_MODE_MAX(), attr, event);
        if (attr && checkIncDec_Ret) {
          bluetoothState = BLUETOOTH_STATE_OFF;
          bluetoothDistantAddr[0] = 0;
        }
        break;

      case ITEM_MODEL_EXTERNAL_MODULE_LABEL:
        lcdDrawText(MENUS_MARGIN_LEFT, y, TR_EXTERNALRF);
        break;

      case ITEM_MODEL_EXTERNAL_MODULE_MODE:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MODE);
        lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_TARANIS_PROTOCOLS, g_model.moduleData[EXTERNAL_MODULE].type, menuHorizontalPosition==0 ? attr : 0);
        if (IS_MODULE_XJT(EXTERNAL_MODULE))
          lcdDrawTextAtIndex(MODEL_SETUP_3RD_COLUMN, y, STR_XJT_PROTOCOLS, 1+g_model.moduleData[EXTERNAL_MODULE].rfProtocol, (menuHorizontalPosition==1 ? attr : 0));
        else if (IS_MODULE_DSM2(EXTERNAL_MODULE))
          lcdDrawTextAtIndex(MODEL_SETUP_3RD_COLUMN, y, STR_DSM_PROTOCOLS, g_model.moduleData[EXTERNAL_MODULE].rfProtocol, (menuHorizontalPosition==1 ? attr : 0));
        else if (IS_MODULE_R9M(EXTERNAL_MODULE))
          lcdDrawTextAtIndex(MODEL_SETUP_3RD_COLUMN, y, STR_R9M_REGION, g_model.moduleData[EXTERNAL_MODULE].subType, (menuHorizontalPosition==1 ? attr : 0));
#if defined(MULTIMODULE)
        else if (IS_MODULE_MULTIMODULE(EXTERNAL_MODULE)) {
          int multi_rfProto = g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(false);
          if (g_model.moduleData[EXTERNAL_MODULE].multi.customProto) {
            lcdDrawText(MODEL_SETUP_3RD_COLUMN, y, STR_MULTI_CUSTOM, menuHorizontalPosition == 1 ? attr : 0);
            lcdDrawNumber(MODEL_SETUP_4TH_COLUMN, y, multi_rfProto, menuHorizontalPosition==2 ? attr : 0, 2);
            lcdDrawNumber(MODEL_SETUP_4TH_COLUMN + MODEL_SETUP_BIND_OFS, y, g_model.moduleData[EXTERNAL_MODULE].subType, menuHorizontalPosition==3 ? attr : 0, 2);
          }
          else {
            const mm_protocol_definition * pdef = getMultiProtocolDefinition(multi_rfProto);
            lcdDrawTextAtIndex(MODEL_SETUP_3RD_COLUMN, y, STR_MULTI_PROTOCOLS, multi_rfProto, menuHorizontalPosition == 1 ? attr : 0);
            if (pdef->subTypeString != nullptr)
              lcdDrawTextAtIndex(MODEL_SETUP_4TH_COLUMN, y, pdef->subTypeString, g_model.moduleData[EXTERNAL_MODULE].subType, menuHorizontalPosition==2 ? attr : 0);
          }
        }
#endif
        if (attr && s_editMode>0) {
          switch (menuHorizontalPosition) {
            case 0:
              g_model.moduleData[EXTERNAL_MODULE].type = checkIncDec(event, g_model.moduleData[EXTERNAL_MODULE].type, MODULE_TYPE_NONE, MODULE_TYPE_COUNT-1, EE_MODEL, isModuleAvailable);
              if (checkIncDec_Ret) {
                g_model.moduleData[EXTERNAL_MODULE].rfProtocol = 0;
                g_model.moduleData[EXTERNAL_MODULE].channelsStart = 0;
                g_model.moduleData[EXTERNAL_MODULE].channelsCount = DEFAULT_CHANNELS(EXTERNAL_MODULE);
                if (IS_MODULE_SBUS(EXTERNAL_MODULE))
                  g_model.moduleData[EXTERNAL_MODULE].sbus.refreshRate = -31;
                if(IS_MODULE_PPM(EXTERNAL_MODULE))
                  SET_DEFAULT_PPM_FRAME_LENGTH(EXTERNAL_MODULE);
              }
              break;
            case 1:
              if (IS_MODULE_DSM2(EXTERNAL_MODULE))
                CHECK_INCDEC_MODELVAR(event, g_model.moduleData[EXTERNAL_MODULE].rfProtocol, DSM2_PROTO_LP45, DSM2_PROTO_DSMX);
              else if (IS_MODULE_R9M(EXTERNAL_MODULE))
                CHECK_INCDEC_MODELVAR(event, g_model.moduleData[EXTERNAL_MODULE].subType, MODULE_SUBTYPE_R9M_FCC, MODULE_SUBTYPE_R9M_LBT);
#if defined(MULTIMODULE)
              else if (IS_MODULE_MULTIMODULE(EXTERNAL_MODULE)) {
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
                g_model.moduleData[EXTERNAL_MODULE].channelsCount = DEFAULT_CHANNELS(EXTERNAL_MODULE);
                g_model.moduleData[EXTERNAL_MODULE].channelsCount = MAX_EXTERNAL_MODULE_CHANNELS();
              }
              break;
#if defined(MULTIMODULE)
            case 2:
              if (g_model.moduleData[EXTERNAL_MODULE].multi.customProto) {
                g_model.moduleData[EXTERNAL_MODULE].setMultiProtocol(checkIncDec(event, g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(false), 0, 63, EE_MODEL));
                break;
              } else {
                const mm_protocol_definition *pdef = getMultiProtocolDefinition(g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(false));
                if (pdef->maxSubtype > 0)
                  CHECK_INCDEC_MODELVAR(event, g_model.moduleData[EXTERNAL_MODULE].subType, 0, pdef->maxSubtype);
              }
              break;
            case 3:
              // Custom protocol, third column is subtype
              CHECK_INCDEC_MODELVAR(event, g_model.moduleData[EXTERNAL_MODULE].subType, 0, 7);
              break;
#endif
          }
        }
        break;

      case ITEM_MODEL_TRAINER_LABEL:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_TRAINER);
        break;

      case ITEM_MODEL_TRAINER_LINE1:
        if (g_model.trainerMode == TRAINER_MODE_MASTER_BLUETOOTH) {
          if (attr) {
            s_editMode = 0;
          }
          if (bluetoothDistantAddr[0]) {
            lcdDrawText(MENUS_MARGIN_LEFT + INDENT_WIDTH, y, bluetoothDistantAddr);
            if (bluetoothState != BLUETOOTH_STATE_CONNECTED) {
              drawButton(MODEL_SETUP_2ND_COLUMN, y, "Bind", menuHorizontalPosition == 0 ? attr : 0);
              drawButton(MODEL_SETUP_2ND_COLUMN+60, y, "Clear", menuHorizontalPosition == 1 ? attr : 0);
            }
            else {
              drawButton(MODEL_SETUP_2ND_COLUMN, y, "Clear", attr);
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
            lcdDrawText(MENUS_MARGIN_LEFT + INDENT_WIDTH, y, "---");
            if (bluetoothState < BLUETOOTH_STATE_IDLE)
              drawButton(MODEL_SETUP_2ND_COLUMN, y, STR_BLUETOOTH_INIT, attr);
            else
              drawButton(MODEL_SETUP_2ND_COLUMN, y, STR_BLUETOOTH_DISC, attr);
            if (attr && event == EVT_KEY_FIRST(KEY_ENTER)) {
              if (bluetoothState < BLUETOOTH_STATE_IDLE)
                bluetoothState = BLUETOOTH_STATE_OFF;
              else
                bluetoothState = BLUETOOTH_STATE_DISCOVER_REQUESTED;
            }
          }
          break;
        }
        // no break

      case ITEM_MODEL_INTERNAL_MODULE_CHANNELS:
      case ITEM_MODEL_EXTERNAL_MODULE_CHANNELS:
      {
        uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
        ModuleData & moduleData = g_model.moduleData[moduleIdx];
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_CHANNELRANGE);
        if ((int8_t)PORT_CHANNELS_ROWS(moduleIdx) >= 0) {
          drawStringWithIndex(MODEL_SETUP_2ND_COLUMN, y, STR_CH, moduleData.channelsStart+1, menuHorizontalPosition==0 ? attr : 0);
          lcdDrawText(lcdNextPos+5, y, "-");
          drawStringWithIndex(lcdNextPos+5, y, STR_CH, moduleData.channelsStart+NUM_CHANNELS(moduleIdx), menuHorizontalPosition==1 ? attr : 0);
          if (IS_R9M_OR_XJTD16(moduleIdx)) {
            if (NUM_CHANNELS(moduleIdx) > 8)
              lcdDrawText(lcdNextPos + 15, y, "(18ms)");
            else
              lcdDrawText(lcdNextPos + 15, y, "(9ms)");
          }
          if (attr && s_editMode>0) {
            switch (menuHorizontalPosition) {
              case 0:
                CHECK_INCDEC_MODELVAR_ZERO(event, moduleData.channelsStart, 32-8-moduleData.channelsCount);
                break;
              case 1:
                CHECK_INCDEC_MODELVAR(event, moduleData.channelsCount, -4, min<int8_t>(MAX_CHANNELS(moduleIdx), 32-8-moduleData.channelsStart));
                if ((k == ITEM_MODEL_EXTERNAL_MODULE_CHANNELS && g_model.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_PPM)
                    || (k == ITEM_MODEL_TRAINER_LINE1)
                    )
                  SET_DEFAULT_PPM_FRAME_LENGTH(moduleIdx);
                break;
            }
          }
        }
        break;
      }

      case ITEM_MODEL_INTERNAL_MODULE_BIND:
      case ITEM_MODEL_EXTERNAL_MODULE_BIND:
      case ITEM_MODEL_TRAINER_LINE2:
      {
        uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
        ModuleData & moduleData = g_model.moduleData[moduleIdx];
        if (IS_MODULE_PPM(moduleIdx)) {
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
        else if (IS_MODULE_SBUS(moduleIdx)) {
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
          int l_posHorz = menuHorizontalPosition;
          coord_t xOffsetBind = MODEL_SETUP_BIND_OFS;
          if (IS_MODULE_XJT(moduleIdx) && IS_D8_RX(moduleIdx)) {
            xOffsetBind = 0;
            lcdDrawText(MENUS_MARGIN_LEFT, y, STR_RECEIVER);
            if (attr) l_posHorz += 1;
          }
          else {
            lcdDrawText(MENUS_MARGIN_LEFT, y, STR_RECEIVER_NUM);
          }
          if (IS_MODULE_PXX(moduleIdx) || IS_MODULE_DSM2(moduleIdx) || IS_MODULE_MULTIMODULE(moduleIdx)) {
            if (xOffsetBind)
              lcdDrawNumber(MODEL_SETUP_2ND_COLUMN, y, g_model.header.modelId[moduleIdx], (l_posHorz==0 ? attr : 0) | LEADING0 | LEFT, 2);
            if (attr && l_posHorz==0) {
              if (s_editMode>0) {
                CHECK_INCDEC_MODELVAR_ZERO(event, g_model.header.modelId[moduleIdx], MAX_RX_NUM(moduleIdx));
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
            drawButton(MODEL_SETUP_2ND_COLUMN+xOffsetBind, y, STR_MODULE_BIND, (moduleFlag[moduleIdx] == MODULE_BIND ? BUTTON_ON : BUTTON_OFF) | (l_posHorz==1 ? attr : 0));
            drawButton(MODEL_SETUP_2ND_COLUMN+MODEL_SETUP_RANGE_OFS+xOffsetBind, y, STR_MODULE_RANGE, (moduleFlag[moduleIdx] == MODULE_RANGECHECK ? BUTTON_ON : BUTTON_OFF) | (l_posHorz==2 ? attr : 0));
            uint8_t newFlag = 0;
#if defined(MULTIMODULE)
            if (multiBindStatus == MULTI_BIND_FINISHED) {
              multiBindStatus = MULTI_NORMAL_OPERATION;
              s_editMode = 0;
            }
#endif
            if (attr && l_posHorz>0) {
              if (s_editMode>0) {
                if (l_posHorz == 1) {
                  if (IS_MODULE_R9M(moduleIdx) || (IS_MODULE_XJT(moduleIdx) && g_model.moduleData[moduleIdx].rfProtocol == RF_PROTO_X16)) {
                    if (event == EVT_KEY_BREAK(KEY_ENTER)) {
                      uint8_t default_selection = 0; // R9M_LBT should default to 0 as available options are variables
                      if (IS_MODULE_R9M_LBT(moduleIdx)) {
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
                    if (moduleFlag[moduleIdx] == MODULE_BIND) {
                      newFlag = MODULE_BIND;
                    }
                    else {
                      if (!popupMenuNoItems) {
                        s_editMode = 0;  // this is when popup is exited before a choice is made
                      }
                    }
                  }
                  else {
                    newFlag = MODULE_BIND;
                  }
                }
                else if (l_posHorz == 2) {
                  newFlag = MODULE_RANGECHECK;
                }
              }
            }
            moduleFlag[moduleIdx] = newFlag;
#if defined(MULTIMODULE)
            if (newFlag == MODULE_BIND)
              multiBindStatus = MULTI_BIND_INITIATED;
#endif
          }
        }
        break;
      }

      case ITEM_MODEL_INTERNAL_MODULE_FAILSAFE:
      case ITEM_MODEL_EXTERNAL_MODULE_FAILSAFE:
      {
        uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
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
              CHECK_INCDEC_MODELVAR_ZERO(event, moduleData.failsafeMode, FAILSAFE_LAST);
              if (checkIncDec_Ret) SEND_FAILSAFE_NOW(moduleIdx);
            }
          }
          else if (menuHorizontalPosition==1) {
            s_editMode = 0;
            if (moduleData.failsafeMode == FAILSAFE_CUSTOM) {
              if (event == EVT_KEY_LONG(KEY_ENTER)) {
                killEvents(event);
                setCustomFailsafe(moduleIdx);
                storageDirty(EE_MODEL);
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

      case ITEM_MODEL_EXTERNAL_MODULE_OPTIONS:
      {
        uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
#if defined(MULTIMODULE)
        if (IS_MODULE_MULTIMODULE(moduleIdx)) {
          int optionValue = g_model.moduleData[moduleIdx].multi.optionValue;

          const uint8_t multi_proto = g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(true);
          const mm_protocol_definition *pdef = getMultiProtocolDefinition(multi_proto);
          if (pdef->optionsstr)
            lcdDrawText(MENUS_MARGIN_LEFT, y, pdef->optionsstr);

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
        if (IS_MODULE_R9M(moduleIdx)) {
          lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MODULE_TELEMETRY);
          if (IS_TELEMETRY_INTERNAL_MODULE()) {
            lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, STR_DISABLE_INTERNAL);
          }
          else {
            lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, STR_MODULE_TELEM_ON);
          }
        }
        else if (IS_MODULE_SBUS(moduleIdx)) {
          lcdDrawText(MENUS_MARGIN_LEFT, y, STR_WARN_BATTVOLTAGE);
          drawValueWithUnit(MODEL_SETUP_4TH_COLUMN, y, getBatteryVoltage(), UNIT_VOLTS, attr|PREC2|LEFT);
        }
      }
      break;

      case ITEM_MODEL_EXTERNAL_MODULE_POWER:
      {
        uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
        if (IS_MODULE_R9M(moduleIdx)) {
          lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MULTI_RFPOWER);
          if(IS_MODULE_R9M_FCC(moduleIdx)) {
            lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_R9M_FCC_POWER_VALUES, g_model.moduleData[moduleIdx].pxx.power, LEFT | attr);
            if (attr)
              CHECK_INCDEC_MODELVAR(event, g_model.moduleData[moduleIdx].pxx.power, 0, R9M_FCC_POWER_MAX);
          }
          else {
            lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_R9M_LBT_POWER_VALUES, g_model.moduleData[moduleIdx].pxx.power, LEFT | attr);
            if (attr)
              CHECK_INCDEC_MODELVAR(event, g_model.moduleData[moduleIdx].pxx.power, 0, R9M_LBT_POWER_MAX);
            if (attr && s_editMode == 0 && reusableBuffer.modelsetup.r9mPower != g_model.moduleData[moduleIdx].pxx.power) {
              if((reusableBuffer.modelsetup.r9mPower + g_model.moduleData[moduleIdx].pxx.power) < 5) { //switching between mode 2 and 3 does not require rebind
                POPUP_WARNING(STR_WARNING);
                SET_WARNING_INFO(STR_REBIND, sizeof(TR_REBIND), 0);
              }
              reusableBuffer.modelsetup.r9mPower = g_model.moduleData[moduleIdx].pxx.power;
            }
          }
        }
#if defined(MULTIMODULE)
        else if (IS_MODULE_MULTIMODULE(moduleIdx)) {
          lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MULTI_LOWPOWER);
          g_model.moduleData[EXTERNAL_MODULE].multi.lowPowerMode = editCheckBox(g_model.moduleData[EXTERNAL_MODULE].multi.lowPowerMode, MODEL_SETUP_2ND_COLUMN, y, attr, event);
        }
#endif
      }
      break;

#if defined(MULTIMODULE)
    case ITEM_MODEL_EXTERNAL_MODULE_AUTOBIND:
      if (g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(true) == MM_RF_PROTO_DSM2)
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MULTI_DSM_AUTODTECT);
      else
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MULTI_AUTOBIND);
      g_model.moduleData[EXTERNAL_MODULE].multi.autoBindMode = editCheckBox(g_model.moduleData[EXTERNAL_MODULE].multi.autoBindMode, MODEL_SETUP_2ND_COLUMN, y, attr, event);
      break;
    case ITEM_MODEL_EXTERNAL_MODULE_STATUS: {
      lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MODULE_STATUS);

      char statusText[64];
      multiModuleStatus.getStatusString(statusText);
      lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, statusText);
      break;
    case ITEM_MODEL_EXTERNAL_MODULE_SYNCSTATUS: {
      lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MODULE_SYNC);

      char statusText[64];
      multiSyncStatus.getRefreshString(statusText);
      lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, statusText);
      break;
      }
    }
#endif
    }
  }

  if (IS_RANGECHECK_ENABLE()) {
    theme->drawMessageBox("RSSI :", NULL, NULL, WARNING_TYPE_INFO);
    lcdDrawNumber(WARNING_LINE_X, WARNING_INFOLINE_Y, TELEMETRY_RSSI(), DBLSIZE|LEFT);
  }

  // some field just finished being edited
  if (old_editMode > 0 && s_editMode == 0) {
    ModelCell* mod_cell = modelslist.getCurrentModel();
    if (mod_cell) {

      switch(menuVerticalPosition) {
      case ITEM_MODEL_NAME:
        mod_cell->setModelName(g_model.header.name);
        break;

      case ITEM_MODEL_INTERNAL_MODULE_BIND:
        if (menuHorizontalPosition != 0)
          break;
      case ITEM_MODEL_INTERNAL_MODULE_MODE:
        mod_cell->setRfData(&g_model);
        checkModelIdUnique(INTERNAL_MODULE);
        break;

      case ITEM_MODEL_EXTERNAL_MODULE_BIND:
        if (menuHorizontalPosition != 0)
          break;
      case ITEM_MODEL_EXTERNAL_MODULE_MODE:
        mod_cell->setRfData(&g_model);
        if (g_model.moduleData[EXTERNAL_MODULE].type != MODULE_TYPE_NONE)
          checkModelIdUnique(EXTERNAL_MODULE);
      }
    }
  }
  
  return true;
}

bool menuModelFailsafe(event_t event)
{
  uint8_t ch = 0;
  const uint8_t channelStart = g_model.moduleData[g_moduleIdx].channelsStart;
  const int lim = (g_model.extendedLimits ? (512 * LIMIT_EXT_PERCENT / 100) : 512) * 2;
  const uint8_t SLIDER_W = 128;
  const uint8_t cols = NUM_CHANNELS(g_moduleIdx) > 8 ? 2 : 1;

  if (event == EVT_KEY_LONG(KEY_ENTER)) {
    killEvents(event);
    event = 0;

    if (menuVerticalPosition < NUM_CHANNELS(g_moduleIdx)) {
      if (s_editMode) {
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

  SIMPLE_SUBMENU_WITH_OPTIONS("FAILSAFE", ICON_STATS_ANALOGS, NUM_CHANNELS(g_moduleIdx)+1, OPTION_MENU_NO_SCROLLBAR);
  drawStringWithIndex(50, 3+FH, "Module", g_moduleIdx+1, MENU_TITLE_COLOR);

  for (uint8_t col=0; col < cols; col++) {
    for (uint8_t line=0; line < 8; line++) {
      coord_t x = col*(LCD_W/2);
      const coord_t y = MENU_CONTENT_TOP - FH + line*(FH+2);
      const int32_t channelValue = channelOutputs[ch+channelStart];
      int32_t failsafeValue = g_model.moduleData[g_moduleIdx].failsafeChannels[8*col+line];

      // Channel name if present, number if not
      if (g_model.limitData[ch+channelStart].name[0] != '\0') {
        putsChn(x+MENUS_MARGIN_LEFT, y-3, ch+1, TINSIZE);
        lcdDrawSizedText(x+MENUS_MARGIN_LEFT, y+5, g_model.limitData[ch+channelStart].name, sizeof(g_model.limitData[ch+channelStart].name), ZCHAR|SMLSIZE);
      }
      else {
        putsChn(x+MENUS_MARGIN_LEFT, y, ch+1, 0);
      }

      // Value
      LcdFlags flags = RIGHT;
      if (menuVerticalPosition == ch) {
        flags |= INVERS;
        if (s_editMode) {
          if (failsafeValue == FAILSAFE_CHANNEL_HOLD || failsafeValue == FAILSAFE_CHANNEL_NOPULSE) {
            s_editMode = 0;
          }
          else {
            flags |= BLINK;
            CHECK_INCDEC_MODELVAR(event, g_model.moduleData[g_moduleIdx].failsafeChannels[8*col+line], -lim, +lim);
          }
        }
      }

      x += (LCD_W/2)-4-MENUS_MARGIN_LEFT-SLIDER_W;

      if (failsafeValue == FAILSAFE_CHANNEL_HOLD) {
        lcdDrawText(x, y+2, "HOLD", flags|SMLSIZE);
        failsafeValue = 0;
      }
      else if (failsafeValue == FAILSAFE_CHANNEL_NOPULSE) {
        lcdDrawText(x, y+2, "NONE", flags|SMLSIZE);
        failsafeValue = 0;
      }
      else {
#if defined(PPM_UNIT_US)
        lcdDrawNumber(x, y, PPM_CH_CENTER(ch)+failsafeValue/2, flags);
#elif defined(PPM_UNIT_PERCENT_PREC1)
        lcdDrawNumber(x, y, calcRESXto1000(failsafeValue), PREC1|flags);
#else
        lcdDrawNumber(x, y, calcRESXto1000(failsafeValue)/10, flags);
#endif
      }

      // Gauge
      x += 4;
      lcdDrawRect(x, y+3, SLIDER_W+1, 12);
      const coord_t lenChannel = limit((uint8_t)1, uint8_t((abs(channelValue) * SLIDER_W/2 + lim/2) / lim), uint8_t(SLIDER_W/2));
      const coord_t lenFailsafe = limit((uint8_t)1, uint8_t((abs(failsafeValue) * SLIDER_W/2 + lim/2) / lim), uint8_t(SLIDER_W/2));
      x += SLIDER_W/2;
      const coord_t xChannel = (channelValue>0) ? x : x+1-lenChannel;
      const coord_t xFailsafe = (failsafeValue>0) ? x : x+1-lenFailsafe;
      lcdDrawSolidFilledRect(xChannel, y+4, lenChannel, 5, TEXT_COLOR);
      lcdDrawSolidFilledRect(xFailsafe, y+9, lenFailsafe, 5, ALARM_COLOR);

      if (++ch >= NUM_CHANNELS(g_moduleIdx)) {

        // draw that menu at the bottom center
        flags = CENTERED | (menuVerticalPosition == ch ? INVERS : 0);
        lcdDrawSizedText(LCD_W/2, MENU_CONTENT_TOP - FH + 8*(FH+2), STR_OUTPUTS2FAILSAFE, 20, flags);
        break;
      }
    }
  }

  return true;
}
